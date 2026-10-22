# Configuration Reference

The Real-Sim interface is configured through YAML files (for example `config.yaml`). Each top-level section maps directly to structures parsed in `CommonLib/ConfigHelper`. This document describes the supported keys, their defaults, and any special behaviour such as derived paths.

> **Path handling.** Unless stated otherwise, relative paths are resolved against the directory that contains the config file. The interface also accepts absolute paths.

## SimulationSetup

| Key | Type | Default | Notes |
| --- | --- | --- | --- |
| EnableRealSim | bool | true | Master switch for the interface. |
| EnableVerboseLog | bool | false | Enables detailed logging in TrafficLayer and companion tools. |
| SimulationEndTime | double | 90000 | End time (seconds). Large default keeps the session open until external shutdown. |
| EnableExternalDynamics | bool | false | When true, SUMO speed updates use `setPreviousSpeed` so acceleration limits are respected. |
| VehicleMessageField | string list | All supported fields | Controls which vehicle fields are exchanged. Provide a subset if bandwidth is a concern. Must include `id`, `speed`, and one of `speedDesired` or `accelerationDesired`. |
| SelectedTrafficSimulator | string | `"SUMO"` | Use `"SUMO"` or `"VISSIM"`. |
| TrafficSimulatorIP | string | `"127.0.0.1"` | Host for TraCI/VISSIM connections. |
| TrafficSimulatorPort | int | 1337 | Port for the selected simulator. |
| WarmUpUntilEgoEntry | bool | false | Run the traffic simulator with the FIXS boundary closed until the first subscribed ego is in the network. SUMO only. |
| WarmUpTime | double | 0 | Run the traffic simulator with the FIXS boundary closed until this **absolute** simulation time, in one batch step. SUMO only. |
| TrafficLayerIP | string | inferred | Defaults to the first vehicle subscription IP if not provided. |
| TrafficLayerPort | int | inferred | Defaults to the first vehicle subscription port if not provided. |

### Warm-up

XIL testing usually cares about the simulation only from the moment the ego is on the
road. A warm-up runs the traffic simulator up to that moment with the **FIXS boundary
closed**: no message reaches any client, and no client is accepted yet, so CARLA's map
load and CarMaker's start-up overlap the warm-up instead of queueing ahead of it. The
sockets are bound and listening the whole time, so a client's `connect()` succeeds
exactly as it always did — it simply gets no data until the warm-up ends.

Pick the trigger by who knows the entry time:

- `WarmUpUntilEgoEntry: true` — the warm-up ends when the first ego appears. Use this
  when a controller inserts the ego: the controller keeps owning the entry time and no
  number is duplicated in this file. The ids watched are the **union** of the by-id
  vehicle subscriptions in `ApplicationSetup` and `XilSetup`; with several egos the
  *first* to arrive ends the warm-up, because an ego on the road while the boundary is
  still closed would be driven by the traffic model rather than by its XIL.
- `WarmUpTime: <absolute simulation time>` — the warm-up ends at that time, reached in a
  single batch step. Use this when nothing else inserts the ego, i.e. the CarMaker path
  where TrafficLayer injects it and has to be told when. Note it is absolute simulation
  time, not a duration: a scenario that begins at 28800 warms up to, say, 28985.

The two are mutually exclusive: a batch step observes nothing on the way, so it cannot
also watch for an ego. Setting both warns and uses ego entry. Setting neither means no
warm-up — the interface syncs from the first step.

Both are SUMO-only today; TrafficLayer does not own the VISSIM clock (the DriverModel
DLL does), and a VISSIM config that sets either key is rejected at startup rather than
silently ignored.

> **Renamed in v0.9.0 (#86).** `SimulationMode` and `SimulationModeParameter` are gone —
> the "bitfield" never behaved like one (`5` was a duplicate of `4`, and `3`/`6`/`7`
> silently meant `0`). `SimulationMode: 1` becomes `WarmUpUntilEgoEntry: true`,
> `SimulationMode: 4` + `SimulationModeParameter: T` becomes `WarmUpTime: T`, and
> `SimulationMode: 0` means removing the key. No config in either FIXS or
> FIXS_Applications set them, so they are simply gone: an old config that still does is
> ignored like any other unknown key, and runs with no warm-up.

## SumoSetup

| Key | Type | Default | Notes |
| --- | --- | --- | --- |
| SpeedMode | int | 0 | Passed to `SUMO_TRACI_NAMESPACE::Vehicle::setSpeedMode`. Common values: 0 (default safety), 31 (default mode), 32 (all checks off). |
| ExecutionOrder | int | 1 | Order passed to `Simulation::setOrder`. |
| EnableAutoLaunch | bool | false | When true, TrafficLayer starts SUMO automatically; otherwise it expects a running instance. |
| SumoConfigFile | string | `""` | Path to `.sumocfg` used when auto-launching. Relative paths resolved against config file directory. |
| NumClients | int | 1 | Number of SUMO clients negotiated during auto-launch. |
| RuntimeLibraryPath | string | `""` | Optional override for the SUMO runtime directory (DLL/.so path). |
| EnableGuiFollow | bool | false | sumo-gui tracks `GuiCenteredViewId` whenever it is in the network, the counterpart of `CarlaSetup.EnableSpectatorFollow`. No effect on headless `sumo`; TrafficLayer prints one line and carries on. |
| GuiCenteredViewId | string | `"ego"` | Vehicle id sumo-gui follows. |
| GuiZoom | double | 600 | sumo-gui zoom (%) set when following starts; changing it in the GUI afterwards is kept. |

These three keys are the short form of one sumo view; see [ViewSetup](#viewsetup).

When `RuntimeLibraryPath` is omitted the executable searches for the libsumo runtime in the following order:

1. `libsumo\bin` located next to `TrafficLayer.exe`.
2. `CommonLib\libsumo\bin` relative to the executable, walking up to four parent directories (covers `TrafficLayer/x64/<Config>` and `build`).
3. `CommonLib\libsumo\bin` relative to the current working directory.

On Windows the resolved folder is supplied to `SetDllDirectory`. On Linux the folder is prepended to `LD_LIBRARY_PATH`. Leave the field commented out unless you intentionally repackage the SUMO binaries elsewhere.

## ApplicationSetup

| Key | Type | Default | Notes |
| --- | --- | --- | --- |
| EnableApplicationLayer | bool | false | Enables the application message bus. |
| ApplicationPort | int list | [] | Ports that receive messages (alternative to subscription-based port definition). |
| VehicleSubscription | list of subscription blocks | [] | Each block has `type`, an `attribute` map, `ip`, and `port`. |
| DetectorSubscription | list of subscription blocks | [] | Detector data subscriptions. |
| SignalSubscription | list of subscription blocks | [] | Traffic signal subscriptions. |

### Subscription Types

**VehicleSubscription** supports the following types:
- `ego`: Subscribe by vehicle ID. Attributes: `id` (list), `radius` (list), or `all` (bool).
- `link`: Subscribe by edge/link ID. Attributes: `id` (list).
- `point`: Subscribe by geographic point. Attributes: `x`, `y`, `z`, `radius` (all lists).
- `vehicleType`: Subscribe by vehicle type. Attributes: `id` (list), `radius` (list).

**SignalSubscription** supports:
- `intersection`: Subscribe by signal controller. Attributes: `name` or `id` (list).

**DetectorSubscription** supports:
- `detector`: Subscribe by detector pattern. Attributes: `pattern`, `name`, or `id` (list).

Example subscription block:

```yaml
VehicleSubscription:
-   type: ego
    attribute: {id: ['ego'], radius: [0]}
    ip: ['127.0.0.1']
    port: [2444]
```

## XilSetup

| Key | Type | Default | Notes |
| --- | --- | --- | --- |
| EnableXil | bool | false | Enables the co-simulation bridge. |
| AsServer | bool | false | When true, TrafficLayer listens for XIL connections; otherwise acts as client. |
| VehicleSubscription | list | [] | Same schema as the application-layer subscriptions. |
| SignalSubscription | list | [] | Signal subscriptions for XIL. |
| DetectorSubscription | list | [] | Detector subscriptions for XIL. |
| Transport | string | inprocess | `inprocess`: FIXS's simulated dyno (`fixs.xil.dynosim()`). `udp`: the application talks to its own rig at the first VehicleSubscription's ip and port; FIXS opens no socket. `tcp`: reserved. |

When the application layer is disabled but XIL is enabled, the interface automatically reuses the XIL vehicle subscriptions to seed outbound traffic.

A controller reads this section with `fixs.config.get('xil')`: keys in snake_case (`transport`, `enable_xil`), plus `ip` and `port` from the first VehicleSubscription.

## CarMakerSetup

| Key | Type | Default | Notes |
| --- | --- | --- | --- |
| EnableCosimulation | bool | false | Turn on integration with IPG CarMaker. |
| EnableEgoSimulink | bool | false | When true, ego state comes from Simulink; when false, from User.cpp. |
| CarMakerIP | string | inferred or `"127.0.0.1"` | Connection IP for CarMaker. Inferred from subscription if single vehicle subscribed. |
| CarMakerPort | int | inferred or 7331 | Connection port for CarMaker. Inferred from subscription if single vehicle subscribed. |
| TrafficRefreshRate | double | 0.001 | How often traffic poses are re-applied to CarMaker, i.e. CarMaker's own step. **Not** the FIXS exchange period, which is a fixed 0.1 s — the core interpolates each sample (position, heading, grade) across the sub-steps in between, so 0.001 means smooth 1 kHz traffic motion off a 10 Hz feed. |
| EgoId | string | inferred or `"egoCm"` | Identifier for the ego vehicle in SUMO. Inferred from subscription if single vehicle subscribed. |
| EgoType | string | `""` | SUMO vehicle type for ego. Empty uses SUMO default. |
| SynchronizeTrafficSignal | bool | false | If enabled, subscribes to all signal controllers for synchronization. |
| TrafficSignalPort | int | 2444 or inferred | Port for signal data. Inferred from SignalSubscription if available. |

## CarlaSetup

| Key | Type | Default | Notes |
| --- | --- | --- | --- |
| EnableVerboseLog | bool | false | Verbose logging for the CARLA bridge. |
| EnableCosimulation | bool | false | Enables CARLA co-simulation. |
| EnableExternalControl | bool | false | When true, SUMO vehicles updated from CARLA state; when false, CARLA only visualizes. |
| UseVehicleTypeAsBlueprint | bool | false | Interpret SUMO vehicle types as CARLA blueprints. |
| CarlaServerIP | string | `"127.0.0.1"` | CARLA server endpoint IP. |
| CarlaServerPort | int | 2000 | CARLA server endpoint port. |
| CarlaClientIP | string | `"127.0.0.1"` | **Not CARLA.** Where VirCarlaEnv reaches **TrafficLayer** — `mainVirCarla` assigns it to `core.trafficLayerIP_`. Keep it local even when the CARLA server is on another machine: TrafficLayer runs beside VirCarlaEnv, not beside CARLA. |
| CarlaClientPort | int | 430 | TrafficLayer's bridge port for this client (`core.vehDataPort_`); pair it with `ApplicationSetup.VehicleSubscription[].port`. Nothing to do with CARLA's streaming port. |
| CarlaMapName | string | `"Town01"` | Desired CARLA map name. |
| CenteredViewId | string | `"ego"` | Actor id used for camera centering. With `EnableSpectatorFollow` (true), `SpectatorHeight` (50) and `SpectatorAlignYaw` (false) it is the short form of the carla spectator view; see [ViewSetup](#viewsetup). |
| CarlaTimeStep | double | 0 (= the FIXS feed) | CARLA world step, i.e. `fixed_delta_seconds`. Must be the FIXS feed period or an exact divisor of it (0.1 / 0.05 / 0.025 / 0.02 / 0.01): the exchange boundary is tested on the feed grid, so a tick that does not divide it never lands on one and the bridge would exchange nothing. Finer than the feed makes the bridge interpolate traffic — position **and** heading — across the sub-steps, exactly as the CarMaker host does at 0.001 s. |
| TrafficRefreshRate | double | 0 (= every tick) | How often traffic poses are **re-applied**. Same meaning as the CarMakerSetup key above; **not** the feed period. Must be ≥ `CarlaTimeStep` and have a whole reciprocal. Coarser than the tick = fewer `ApplyBatch` calls on a heavy scene while physics and sensors still run at the tick rate. |
| InterestedIds | string list | `["ego"]` | Vehicle IDs that receive mirrored CARLA updates. Should be subset of VehicleSubscription. |

> **The FIXS feed period is not configurable.** It is 0.1 s (`fixs::kFeedPeriodS` in
> `CommonLib/FixsProtocol.h`): every VirEnvCore host tests its send/recv boundary
> against that grid on its own sim clock, and TrafficLayer steps the traffic simulator
> exactly once per exchange. The traffic simulator's step length therefore **is** that
> value — `run_cosim` passes `--step-length 0.1` to both bridges rather than offering
> it as a choice. Give SUMO a different step and the two clocks run at different rates:
> at 0.05 s, CARLA advances two ticks per SUMO step, so every sample is drawn twice and
> the scene plays back at half speed.
>
> Before #219, `CarlaSetup.TrafficRefreshRate` doubled as the feed period *and* as the
> tick whenever `CarlaTimeStep` was unset — so setting it to 0.05 silently turned
> interpolation off. They are separate keys now and neither falls back to the other. An
> older yaml carrying a feed-period value in it is migrated (generated yamls) or
> reported (app-owned ones) by `run_cosim`.

## ViewSetup

A list of co-sim views, for CARLA and sumo-gui. Each camera view opens its own
window titled `FIXS view: <Name>`, so a screen recorder can capture it by title.

```yaml
ViewSetup:
  Views:
    - Name: pov            # unique; camera window title "FIXS view: pov"
      Simulator: carla     # carla | sumo
      Type: chase          # carla: spectator | bev | chase | driver; sumo: follow | fixed
      Target: ego          # vehicle id
      Size: [960, 540]
    - Name: junction
      Simulator: sumo
      Type: fixed
      Boundary: [100, 200, 400, 350]
```

| Key | Applies to | Default | Notes |
| --- | --- | --- | --- |
| Name | all | the Type | Unique across the list. Camera windows and added sumo-gui views are titled `FIXS view: <Name>`. |
| Simulator | all | `carla` | `carla` or `sumo`. |
| Type | all | carla `chase`, sumo `follow` | carla `spectator`: the CARLA window itself, a top-down snap (at most one). carla `bev` / `chase` / `driver`: an RGB camera rigidly attached to Target, in its own window. sumo `follow`: tracks Target. sumo `fixed`: shows Boundary. |
| Target | all but sumo `fixed` | `"ego"` | Vehicle id. A camera attaches when the vehicle exists, re-attaches if it is respawned, and waits while it is gone. |
| Enable | all | true | |
| Height | carla `spectator`, `bev` | 50 | Metres above the target. |
| AlignYaw | carla `spectator` | false | Rotate the view with the target's heading. A `bev` camera always turns with the target. |
| Offset | carla `bev`, `chase`, `driver` | bev `[0, 0, Height]`, chase `[-6.5, 0, 2.8]`, driver `[0.5, 0, 1.5]` | Camera position in the target's frame, metres: x forward, y right, z up. |
| Pitch | carla `bev`, `chase`, `driver` | bev -90, chase -12, driver -5 | Degrees; negative looks down. |
| Size | carla cameras | `[960, 540]` | Image and window size, pixels. |
| Fov | carla cameras | 90 | Horizontal field of view, degrees. |
| SensorTick | carla cameras | 0 | Seconds between camera frames; 0 is every world tick. A larger value renders less often and is the cheapest way to cut a camera's cost. |
| Display | carla cameras | true | Show the camera in its window. |
| Record | carla cameras | `""` (off) | Write the camera to this `.mp4` or `.avi` file (relative to where the bridge was started). Exactly one video frame per world tick, ordered by the frame's world frame number rather than its arrival, so video time is sim time: a world frame with no camera image (`SensorTick`, gaps up to 2 s) repeats the previous image, and longer gaps are left out. CARLA delivers images in bursts, so a frame is only called missing after waiting 2 s of sim time for it (wider if worse lateness is seen), holding at most 256 MB of frames per recording (129 frames at 960x540) while it waits. The video starts at the camera's first frame. An `.mp4` is only playable once the bridge exits normally (end of run or Ctrl+C); if the run may be killed, record `.avi` (MJPG, larger). |
| Zoom | sumo `follow` | 600 | Zoom (%) set when following starts; a zoom changed in the GUI afterwards is kept. |
| Boundary | sumo `fixed` | required | `[xmin, ymin, xmax, ymax]` in SUMO coordinates. |

**Legacy keys.** The CARLA window and sumo-gui's own view are each one slot:

- If the list has no carla `spectator` entry, one is implied from
  `CarlaSetup.EnableSpectatorFollow / CenteredViewId / SpectatorHeight / SpectatorAlignYaw`.
- If the list has no sumo entry, one sumo `follow` is implied from
  `SumoSetup.EnableGuiFollow / GuiCenteredViewId / GuiZoom`.
- A declared entry replaces the implied one, and the parser writes it back into those
  legacy keys. A scenario without `ViewSetup` behaves exactly as before.

The first sumo entry drives sumo-gui's own view. Each further sumo entry is a view
added with `GUI::addView`, named `FIXS view: <Name>`; these are panes inside the
sumo-gui window, not separate OS windows. sumo-gui creates an added view
asynchronously, so TrafficLayer waits up to 10 s of sim time for it before giving up.
Headless `sumo` ignores the sumo views.

CARLA camera views need the Python engine (`CarlaSetup.EnablePythonBackend: true`)
and `opencv-python`; the cpp engine says so once and shows only the spectator. A
camera never ticks the world, and the bridge thread only hands its frames to an
OpenCV window thread and, with `Record`, an encoder thread. The cost that remains is
the extra render pass on the CARLA server, which shows up in the bridge's
`world.tick` timing row; lower it with `SensorTick`, a smaller `Size`, or fewer
cameras. At shutdown the bridge prints each camera's callback, draw and encode cost.
Closing a camera window removes that camera unless it is also recording.

## Vehicle Message Field Specifications

This is the full list of available vehicle message fields that can be specified in `VehicleMessageField`:

| Field | Type | Description |
| --- | --- | --- |
| id | string | Vehicle ID (VISSIM integer converted to string, e.g., 8 → "8"). **Required.** |
| type | string | Vehicle type (VISSIM integer converted to string, e.g., 100 → "100") |
| vehicleClass | string | Vehicle class (car, truck in VISSIM; private, passenger, truck in SUMO) |
| speed | float | Current speed (m/s). **Required.** |
| acceleration | float | Current acceleration (m/s²) |
| positionX | float | X position (m) in simulator coordinates |
| positionY | float | Y position (m) in simulator coordinates |
| positionZ | float | Z position (m) in simulator coordinates |
| heading | float | Heading in degrees (north = 0°, clockwise, east = 90°) |
| color | uint32 | Combined RGBA bits (8-bit each, leftmost = R) |
| linkId | string | Link ID (VISSIM integer converted to string) |
| laneId | int32 | Lane ID (rightmost lane = 1, consistent with VISSIM) |
| distanceTravel | float | Cumulative driving distance since entering network (m) |
| speedDesired | float | Desired speed (m/s). **Required if accelerationDesired not specified.** |
| accelerationDesired | float | Desired acceleration (m/s²). **Required if speedDesired not specified.** |
| hasPrecedingVehicle | int8 | 1 = has preceding vehicle, 0 = none |
| precedingVehicleId | string | ID of preceding vehicle |
| precedingVehicleDistance | float | Distance to preceding vehicle (m), -1 if none |
| precedingVehicleSpeed | float | Speed of preceding vehicle (m/s), -1 if none |
| signalLightId | string | Signal controller ID |
| signalLightHeadId | int32 | Signal head index |
| signalLightDistance | float | Distance to next signal (m), -1 if none |
| signalLightColor | int8 | 1=red, 2=amber, 3=green, 4=red/amber, 5=amber flashing, 6=off, 0=other, -1=none |
| speedLimit | float | Current speed limit (m/s), -1 if none |
| speedLimitNext | float | Next speed limit ahead (m/s), -1 if none |
| speedLimitChangeDistance | float | Distance to speed limit change (m), -1 if none |
| linkIdNext | string | Next link ID |
| grade | float | Road grade angle (radians), positive = uphill |
| activeLaneChange | int8 | 1 = left, -1 = right, 0 = stay |
| lightIndicators | - | Turn signal indicators |
| length | float | Vehicle length (m) |
| width | float | Vehicle width (m) |
| height | float | Vehicle height (m) |

## Example Configuration

Below is a comprehensive example configuration demonstrating SimulationSetup, SumoSetup, ApplicationSetup, XilSetup, CarMakerSetup, and CarlaSetup:

```yaml
# NOTE: YAML files do not allow tabs. use 2 or 4 spaces for indentation
#
# Reserved TCP/IP port by RealSim: 1337, 1338
# DO NOT use these ports when setting up clients to RealSim
#
# TCP/IP connection port config
#               1337/1338                 ApplicationPort                                   XilPort
#   VISSIM/SUMO ---------> Traffic Layer -----------------> Application Layer (controller) -----------> XIL Clients

# =============================================================================
# Global Simulation Setup
# =============================================================================
SimulationSetup:
    # Master Switch to turn on/off RealSim interface
    EnableRealSim: true

    # Whether or not to save verbose log during the simulation
    EnableVerboseLog: false

    # Simulation end time in seconds
    SimulationEndTime: 300

    # Specify which traffic simulator: 'SUMO' or 'VISSIM'
    SelectedTrafficSimulator: 'SUMO'

    # Traffic simulator connection parameters
    TrafficSimulatorIP: "127.0.0.1"
    TrafficSimulatorPort: 1337

    # Vehicle message fields to exchange
    # Must include: id, speed, and one of speedDesired or accelerationDesired
    VehicleMessageField: [id, type, vehicleClass, speed, acceleration, positionX,
        positionY, positionZ, heading, color, linkId, laneId, distanceTravel,
        speedDesired, grade, length, width, height]

    # Use setPreviousSpeed instead of setSpeed to respect SUMO's acceleration dynamics
    EnableExternalDynamics: true

# =============================================================================
# SUMO Setup
# =============================================================================
SumoSetup:
    # Speed mode bitmask: 0 = default safety, 31 = default mode, 32 = all checks off
    SpeedMode: 32

    # Auto-launch SUMO directly from TrafficLayer
    EnableAutoLaunch: true

    # Path to SUMO configuration file (.sumocfg)
    # Relative paths are resolved against the config file directory
    SumoConfigFile: "./network.sumocfg"

    # Number of SUMO client instances
    NumClients: 1

    # Optional override for SUMO runtime library directory
    # RuntimeLibraryPath: '../../../CommonLib/libsumo/bin'

# =============================================================================
# Application Layer Setup
# =============================================================================
ApplicationSetup:
    # Enable application layer to receive vehicle data
    EnableApplicationLayer: true

    # Vehicle subscriptions
    VehicleSubscription:
    # Subscribe to ego vehicle with 50m radius for surrounding vehicles
    -   type: ego
        attribute: {id: ['ego'], radius: [50]}
        ip: ['127.0.0.1']
        port: [440]

    # Subscribe to vehicles by geographic point
    -   type: point
        attribute: {radius: [200], x: [100.0], y: [500.0], z: [0]}
        ip: ['127.0.0.1']
        port: [441]

    # Signal subscriptions for traffic light data
    SignalSubscription:
    -   type: intersection
        attribute: {name: ['signal_1', 'signal_2', 'signal_3']}
        ip: ['127.0.0.1']
        port: [440]

    # Detector subscriptions
    DetectorSubscription:
    -   type: detector
        attribute: {pattern: ['det_*']}
        ip: ['127.0.0.1']
        port: [442]

# =============================================================================
# XIL Setup (Simulink / Hardware-in-the-Loop)
# =============================================================================
XilSetup:
    # Enable/disable XIL
    EnableXil: true

    # Set to true if TrafficLayer should act as server
    AsServer: false

    # Vehicle subscription for XIL client
    # Typically ego vehicle with radius 0 (only ego data sent to XIL)
    VehicleSubscription:
    -   type: ego
        attribute: {id: ['ego'], radius: [0]}
        ip: ['127.0.0.1']
        port: [420]

    # Signal subscription for XIL
    SignalSubscription:
    -   type: intersection
        attribute: {name: ['signal_1']}
        ip: ['127.0.0.1']
        port: [420]

# =============================================================================
# CarMaker Setup (IPG CarMaker Co-simulation)
# =============================================================================
CarMakerSetup:
    # Enable CarMaker co-simulation
    EnableCosimulation: true

    # If true, ego state comes from Simulink; if false, from User.cpp
    EnableEgoSimulink: true

    # CarMaker connection settings
    CarMakerIP: 127.0.0.1
    CarMakerPort: 7890

    # Traffic objects update rate (seconds)
    TrafficRefreshRate: 0.001

    # Ego vehicle settings
    EgoId: ego
    EgoType: passenger

    # Synchronize traffic signals with CarMaker
    SynchronizeTrafficSignal: true
    TrafficSignalPort: 2444

# =============================================================================
# CARLA Setup (Virtual Environment Co-simulation)
# =============================================================================
CarlaSetup:
    # Enable verbose logging for CARLA bridge
    EnableVerboseLog: false

    # Enable CARLA co-simulation
    EnableCosimulation: true

    # If true, SUMO vehicles updated from CARLA state
    # If false, CARLA only visualizes SUMO vehicles
    EnableExternalControl: true

    # Use vehicle type as CARLA blueprint
    UseVehicleTypeAsBlueprint: true

    # CARLA server connection
    CarlaServerIP: 127.0.0.1
    CarlaServerPort: 2000

    # CARLA client binding
    CarlaClientIP: 127.0.0.1
    CarlaClientPort: 440

    # CARLA map name
    CarlaMapName: Town01

    # Traffic objects update rate (seconds)
    TrafficRefreshRate: 0.1

    # Vehicle IDs to mirror in CARLA (subset of VehicleSubscription)
    InterestedIds: ['ego']

    # Center camera view on this vehicle
    CenteredViewId: ego
```

## Updating the YAML

1. Copy an existing configuration (for example `tests/Python/SimpleEchoClient/config.yaml` or `tests/SumoCarla/test_scenarios/Town01_with_ego_type_as_blueprint/defaultConfig.yaml`).
2. Adjust the sections described above.
3. If SUMO auto-launch is enabled, ensure the `.sumocfg` path is valid and that either `RuntimeLibraryPath` is set or the default `CommonLib/libsumo/bin` directory remains beside the executable.
4. Keep indentation spaces (YAML does not support tabs).

For quick reference, see the inline comments in the test configuration files, which demonstrate various setup combinations.
