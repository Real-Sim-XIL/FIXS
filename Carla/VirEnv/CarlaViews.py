"""ViewSetup camera views for the CARLA bridge.

Each enabled carla view of Type bev | chase | driver is a ``sensor.camera.rgb``
attached to its Target vehicle and shown in its own window titled
``FIXS view: <Name>``. The spectator view is not handled here: it is the CARLA
window itself, which mainVirCarla snaps from the CarlaSetup keys.

The cameras never tick the world. Frames arrive on CARLA's sensor thread and are
drawn by one tkinter thread, so the bridge loop only pays a dict lookup per view
per tick until a target appears, changes actor, or leaves.
"""

import sys
import threading

import numpy as np

CAMERA_TYPES = ('bev', 'chase', 'driver')
ROLE_PREFIX = 'fixs_view:'


def windowTitle(name):
    return 'FIXS view: %s' % name


def cameraViews(views):
    """The enabled carla camera views, in list order."""
    return [v for v in (views or [])
            if v['Simulator'] == 'carla' and v['Type'] in CAMERA_TYPES and v['Enable']]


def cameraMount(view):
    """(x, y, z, pitch) of the camera in its target's frame: x forward, y right, z up."""
    x, y, z = view['Offset']
    return float(x), float(y), float(z), float(view['Pitch'])


def bgraToPpm(raw, width, height):
    """CARLA's BGRA frame -> binary PPM bytes, which tkinter.PhotoImage reads directly."""
    bgra = np.frombuffer(raw, dtype=np.uint8).reshape(height, width, 4)
    header = b'P6 %d %d 255\n' % (width, height)
    out = np.empty(len(header) + width * height * 3, dtype=np.uint8)
    out[:len(header)] = np.frombuffer(header, dtype=np.uint8)
    rgb = out[len(header):].reshape(height, width, 3)
    # Per-channel copies: several times faster than one strided BGR->RGB slice.
    rgb[..., 0] = bgra[..., 2]
    rgb[..., 1] = bgra[..., 1]
    rgb[..., 2] = bgra[..., 0]
    return out.tobytes()


class _Slot:
    """Latest frame of one view; written by the sensor thread, read by the display thread."""

    def __init__(self, view):
        self.name = view['Name']
        self.target = view['Target']
        self.size = tuple(view['Size'])
        self.image = None
        self.seq = 0
        self.closed = False

    def push(self, image):
        # Only keep the latest; the display thread converts the frames it actually draws.
        self.image = image
        self.seq += 1


class _Display(threading.Thread):
    """One Tk interpreter, one Toplevel per view; every Tk call stays on this thread."""

    def __init__(self, slots):
        super().__init__(name='fixs-views', daemon=True)
        self._slots = slots
        self._stopping = False
        self.ready = threading.Event()
        self.error = None

    def run(self):
        try:
            import tkinter as tk
            self._tk = tk
            root = tk.Tk()
            root.withdraw()
            self._wins = []
            for s in self._slots:
                top = tk.Toplevel(root)
                top.title(windowTitle(s.name))
                top.configure(bg='black')
                top.resizable(False, False)
                blank = tk.PhotoImage(width=s.size[0], height=s.size[1])
                label = tk.Label(top, image=blank, text='%s\nwaiting for %r' % (windowTitle(s.name), s.target),
                                 compound='center', fg='gray70', bg='black', bd=0)
                label.pack()
                top.protocol('WM_DELETE_WINDOW', lambda s=s, top=top: (setattr(s, 'closed', True), top.withdraw()))
                self._wins.append({'slot': s, 'label': label, 'photo': blank, 'seq': 0})
            self._root = root
        except Exception as e:
            self.error = e
            self.ready.set()
            return
        self.ready.set()
        root.after(10, self._poll)
        root.mainloop()
        try:
            root.destroy()
        except Exception:
            pass

    def _poll(self):
        if self._stopping:
            self._root.quit()
            return
        try:
            for w in self._wins:
                s = w['slot']
                image = s.image
                if s.seq != w['seq'] and image is not None:
                    w['seq'] = s.seq
                    ppm = bgraToPpm(image.raw_data, image.width, image.height)
                    photo = self._tk.PhotoImage(data=ppm, format='PPM')
                    w['label'].configure(image=photo, text='')
                    w['photo'] = photo
        except Exception as e:
            if self.error is None:
                self.error = e
                print('ViewSetup: display error (%s)' % e, file=sys.stderr)
        self._root.after(10, self._poll)

    def stop(self):
        self._stopping = True
        self.join(timeout=2.0)


class _View:
    def __init__(self, view):
        self.view = view
        self.slot = _Slot(view)
        self.camera = None
        self.parentId = None
        self.seqAtAttach = 0
        self.frameSeen = False
        self.closed = False


class CarlaViews:
    """Spawns, re-attaches and shows the carla camera views of ``ViewSetup.Views``."""

    def __init__(self, world, views):
        self._world = world
        self._bp = None
        self._views = [_View(v) for v in cameraViews(views)]
        self._display = None
        if not self._views:
            return
        self._display = _Display([v.slot for v in self._views])
        self._display.start()
        self._display.ready.wait(10.0)
        if self._display.error is not None or not self._display.ready.is_set():
            print('ViewSetup: no window for the camera views (%s); %s not shown'
                  % (self._display.error or 'tkinter did not start',
                     ', '.join(repr(v.slot.name) for v in self._views)), file=sys.stderr)
            self._views = []
            self._display = None
            return
        for v in self._views:
            d = v.view
            print("ViewSetup '%s': carla %s camera on '%s', %dx%d, fov %g, mount %s pitch %g -> window '%s'"
                  % (d['Name'], d['Type'], d['Target'], d['Size'][0], d['Size'][1], d['Fov'],
                     list(d['Offset']), d['Pitch'], windowTitle(d['Name'])))

    def active(self):
        return bool(self._views)

    def update(self, resolve, simTime):
        """Attach each camera to the actor ``resolve(target)`` returns; call once per tick, pre-tick."""
        for v in self._views:
            if v.closed:
                continue
            if v.slot.closed:
                self._drop(v)
                v.closed = True
                print("ViewSetup '%s': window closed; camera removed" % v.slot.name)
                continue
            if v.camera is not None and not v.frameSeen and v.slot.seq > v.seqAtAttach:
                v.frameSeen = True
                print("ViewSetup '%s': first frame at t=%.2f" % (v.slot.name, simTime))
            actor = resolve(v.slot.target)
            if actor is None:
                if v.camera is not None:
                    self._drop(v)
                    print("ViewSetup '%s': '%s' left at t=%.2f; waiting for it to return"
                          % (v.slot.name, v.slot.target, simTime))
                continue
            if v.camera is not None and v.parentId == actor.id:
                continue
            self._drop(v)
            self._attach(v, actor, simTime)

    def _attach(self, v, actor, simTime):
        import carla
        if self._bp is None:
            self._bp = self._world.get_blueprint_library().find('sensor.camera.rgb')
        d = v.view
        bp = self._bp
        bp.set_attribute('image_size_x', str(d['Size'][0]))
        bp.set_attribute('image_size_y', str(d['Size'][1]))
        bp.set_attribute('fov', str(d['Fov']))
        bp.set_attribute('sensor_tick', str(d['SensorTick']))
        if bp.has_attribute('role_name'):
            bp.set_attribute('role_name', ROLE_PREFIX + d['Name'])
        x, y, z, pitch = cameraMount(d)
        try:
            cam = self._world.spawn_actor(
                bp, carla.Transform(carla.Location(x, y, z), carla.Rotation(pitch=pitch)),
                attach_to=actor, attachment_type=carla.AttachmentType.Rigid)
        except RuntimeError as e:
            print("ViewSetup '%s': camera spawn failed (%s)" % (d['Name'], e), file=sys.stderr)
            return
        cam.listen(v.slot.push)
        v.camera = cam
        v.parentId = actor.id
        v.seqAtAttach = v.slot.seq
        v.frameSeen = False
        print("ViewSetup '%s': camera attached to '%s' (actor %d) at t=%.2f"
              % (d['Name'], d['Target'], actor.id, simTime))

    @staticmethod
    def _drop(v):
        cam, v.camera, v.parentId = v.camera, None, None
        if cam is None:
            return
        try:
            cam.stop()
            cam.destroy()
        except RuntimeError:
            pass

    def close(self):
        for v in self._views:
            self._drop(v)
        if self._display is not None:
            self._display.stop()
            self._display = None
