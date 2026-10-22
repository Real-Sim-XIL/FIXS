"""ViewSetup camera views for the CARLA bridge.

Each enabled carla view of Type bev | chase | driver is a ``sensor.camera.rgb``
attached to its Target vehicle. Its frames go to a window titled
``FIXS view: <Name>`` (Display) and/or to a video file (Record). The spectator
view is not handled here: it is the CARLA window itself, which mainVirCarla
snaps from the CarlaSetup keys.

The cameras never tick the world. The sensor callback only stores a reference to
the frame; one OpenCV thread draws the windows and one thread per recording
encodes, so the bridge loop pays a dict lookup per view per tick until a target
appears, changes actor, or leaves.
"""

import collections
import os
import sys
import threading
import time

import numpy as np

CAMERA_TYPES = ('bev', 'chase', 'driver')
ROLE_PREFIX = 'fixs_view:'
#: Frame gaps up to this long are filled with the last frame, so video time stays sim time.
kMaxFillS = 2.0
_FOURCC = {'.mp4': 'mp4v', '.avi': 'MJPG'}


def windowTitle(name):
    return 'FIXS view: %s' % name


def cameraViews(views):
    """The enabled carla camera views that show or record something, in list order."""
    return [v for v in (views or [])
            if v['Simulator'] == 'carla' and v['Type'] in CAMERA_TYPES and v['Enable']
            and (v['Display'] or v['Record'])]


def cameraMount(view):
    """(x, y, z, pitch) of the camera in its target's frame: x forward, y right, z up."""
    x, y, z = view['Offset']
    return float(x), float(y), float(z), float(view['Pitch'])


def bgraOf(image):
    """A carla.Image's pixels as an (h, w, 4) BGRA array, without copying."""
    return np.frombuffer(image.raw_data, dtype=np.uint8).reshape(image.height, image.width, 4)


def framesToWrite(prevFrame, frame, step):
    """How many times to write a frame arriving at world ``frame``: gaps up to kMaxFillS repeat it."""
    if prevFrame is None or frame <= prevFrame:
        return 1
    gap = frame - prevFrame
    return gap if gap * step <= kMaxFillS else 1


class _Cost:
    """Per-frame wall time of one stage, summarised at close."""

    def __init__(self):
        self.n = 0
        self.total = 0.0
        self.worst = 0.0

    def add(self, sec):
        self.n += 1
        self.total += sec
        self.worst = max(self.worst, sec)

    def text(self):
        if not self.n:
            return 'no frames'
        return '%d frames, mean %.2f ms, max %.1f ms' % (self.n, 1000 * self.total / self.n, 1000 * self.worst)


class _Slot:
    """Latest frame of one view; written by CARLA's sensor thread."""

    def __init__(self, view):
        self.name = view['Name']
        self.target = view['Target']
        self.size = tuple(view['Size'])
        self.display = view['Display']
        self.image = None
        self.seq = 0
        self.closed = False
        self.recorder = None
        self.callbackCost = _Cost()

    def push(self, image):
        t = time.perf_counter()
        self.image = image
        self.seq += 1
        if self.recorder is not None:
            self.recorder.push(image)
        self.callbackCost.add(time.perf_counter() - t)


class _Display(threading.Thread):
    """One OpenCV thread owns every window: create, draw, pump, destroy."""

    def __init__(self, slots):
        super().__init__(name='fixs-views', daemon=True)
        self._slots = slots
        self._stopping = False
        self.ready = threading.Event()
        self.error = None
        self.cost = {s.name: _Cost() for s in slots}

    def run(self):
        try:
            import cv2
            self._cv2 = cv2
            for i, s in enumerate(self._slots):
                title = windowTitle(s.name)
                cv2.namedWindow(title, cv2.WINDOW_AUTOSIZE)
                cv2.moveWindow(title, 40 + 60 * i, 40 + 60 * i)
                blank = np.zeros((s.size[1], s.size[0], 3), np.uint8)
                cv2.putText(blank, '%s - waiting for %r' % (title, s.target), (20, s.size[1] // 2),
                            cv2.FONT_HERSHEY_SIMPLEX, 0.7, (180, 180, 180), 1, cv2.LINE_AA)
                cv2.imshow(title, blank)
            cv2.waitKey(1)
        except Exception as e:
            self.error = e
            self.ready.set()
            return
        self.ready.set()
        seen = {s.name: 0 for s in self._slots}
        shown = set()
        while not self._stopping:
            drew = False
            try:
                for s in self._slots:
                    if s.closed:
                        continue
                    image = s.image
                    if image is None or s.seq == seen[s.name]:
                        continue
                    seen[s.name] = s.seq
                    t = time.perf_counter()
                    cv2.imshow(windowTitle(s.name), cv2.cvtColor(bgraOf(image), cv2.COLOR_BGRA2BGR))
                    self.cost[s.name].add(time.perf_counter() - t)
                    drew = True
                cv2.waitKey(1 if drew else 5)
                for s in self._slots:
                    if s.closed:
                        continue
                    # Closed only once it has been seen open, so a window still being created is not.
                    visible = cv2.getWindowProperty(windowTitle(s.name), cv2.WND_PROP_VISIBLE) >= 1
                    if visible:
                        shown.add(s.name)
                    elif s.name in shown:
                        s.closed = True
            except Exception as e:
                if self.error is None:
                    self.error = e
                    print('ViewSetup: display error (%s)' % e, file=sys.stderr)
                time.sleep(0.05)
        cv2.destroyAllWindows()
        cv2.waitKey(1)

    def stop(self):
        self._stopping = True
        self.join(timeout=2.0)


class _Recorder(threading.Thread):
    """Encodes every frame of one camera to a video at one frame per world tick."""

    def __init__(self, name, path, step):
        super().__init__(name='fixs-record-' + name, daemon=True)
        self.name = name
        self.path = os.path.abspath(path)
        self.step = step
        self._queue = collections.deque()
        self._wake = threading.Event()
        self._stopping = False
        self.written = 0
        self.filled = 0
        self.maxQueue = 0
        self.error = None
        self.cost = _Cost()

    def push(self, image):
        if self.error is not None:
            return
        self._queue.append(image)
        self.maxQueue = max(self.maxQueue, len(self._queue))
        self._wake.set()

    def run(self):
        import cv2
        writer = None
        prev = None
        while True:
            if not self._queue:
                if self._stopping:
                    break
                self._wake.wait(0.05)
                self._wake.clear()
                continue
            image = self._queue.popleft()
            t = time.perf_counter()
            if writer is None:
                os.makedirs(os.path.dirname(self.path), exist_ok=True)
                ext = os.path.splitext(self.path)[1].lower()
                writer = cv2.VideoWriter(self.path, cv2.VideoWriter_fourcc(*_FOURCC[ext]),
                                         1.0 / self.step, (image.width, image.height))
                if not writer.isOpened():
                    self.error = 'cannot open %s for writing' % self.path
                    print("ViewSetup '%s': %s; not recording" % (self.name, self.error), file=sys.stderr)
                    self._queue.clear()
                    return
            bgr = cv2.cvtColor(bgraOf(image), cv2.COLOR_BGRA2BGR)
            n = framesToWrite(prev, image.frame, self.step)
            for _ in range(n):
                writer.write(bgr)
            self.written += n
            self.filled += n - 1
            prev = image.frame
            self.cost.add(time.perf_counter() - t)
        if writer is not None:
            writer.release()

    def stop(self):
        self._stopping = True
        self._wake.set()
        self.join()


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
    """Spawns, re-attaches, shows and records the carla camera views of ``ViewSetup.Views``.

    ``worldStep`` is the CARLA tick (s), the frame period of a recording.
    """

    def __init__(self, world, views, worldStep):
        self._world = world
        self._bp = None
        self._views = [_View(v) for v in cameraViews(views)]
        self._display = None
        self._recorders = []
        for v in views or []:
            if (v['Simulator'] == 'carla' and v['Type'] in CAMERA_TYPES and v['Enable']
                    and not v['Display'] and not v['Record']):
                print("ViewSetup '%s': Display is false and nothing to Record; no camera" % v['Name'])
        if not self._views:
            return
        shown = [v.slot for v in self._views if v.view['Display']]
        if shown:
            self._display = _Display(shown)
            self._display.start()
            self._display.ready.wait(10.0)
            if self._display.error is not None or not self._display.ready.is_set():
                print('ViewSetup: no window for the camera views (%s); %s not shown'
                      % (self._display.error or 'the OpenCV window thread did not start',
                         ', '.join(repr(s.name) for s in shown)), file=sys.stderr)
                for s in shown:
                    s.display = False
                self._display = None
        for v in self._views:
            if v.view['Record']:
                rec = _Recorder(v.slot.name, v.view['Record'], worldStep)
                rec.start()
                v.slot.recorder = rec
                self._recorders.append(rec)
        self._views = [v for v in self._views if v.slot.display or v.slot.recorder is not None]
        for v in self._views:
            d = v.view
            out = []
            if v.slot.display:
                out.append("window '%s'" % windowTitle(d['Name']))
            if v.slot.recorder is not None:
                out.append("video %s at %g fps" % (v.slot.recorder.path, 1.0 / worldStep))
            print("ViewSetup '%s': carla %s camera on '%s', %dx%d, fov %g, mount %s pitch %g -> %s"
                  % (d['Name'], d['Type'], d['Target'], d['Size'][0], d['Size'][1], d['Fov'],
                     list(d['Offset']), d['Pitch'], ' + '.join(out)))

    def active(self):
        return bool(self._views)

    def update(self, resolve, simTime):
        """Attach each camera to the actor ``resolve(target)`` returns; call once per tick, pre-tick."""
        for v in self._views:
            if v.closed:
                continue
            if v.slot.closed and v.slot.display:
                v.slot.display = False
                if v.slot.recorder is None:
                    self._drop(v)
                    v.closed = True
                    print("ViewSetup '%s': window closed; camera removed" % v.slot.name)
                    continue
                print("ViewSetup '%s': window closed; still recording" % v.slot.name)
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
        """Remove the cameras, finish the videos, close the windows, and report each stage's cost."""
        if not self._views:
            return
        for v in self._views:
            self._drop(v)
        for rec in self._recorders:
            rec.stop()
            if rec.error is None:
                print("ViewSetup '%s': recorded %d frames (%d repeated to fill gaps, queue peak %d) "
                      "to %s; encode %s" % (rec.name, rec.written, rec.filled, rec.maxQueue,
                                            rec.path, rec.cost.text()))
        for v in self._views:
            parts = ['callback %s' % v.slot.callbackCost.text()]
            if self._display is not None and v.slot.name in self._display.cost:
                parts.append('draw %s' % self._display.cost[v.slot.name].text())
            print("ViewSetup '%s': %s" % (v.slot.name, '; '.join(parts)))
        if self._display is not None:
            self._display.stop()
            self._display = None
        self._recorders = []
        self._views = []
