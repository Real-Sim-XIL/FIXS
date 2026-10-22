"""The CARLA host for the Python VirEnvCore (#325).

The CARLA-specific half of the bridge -- the peer of ``VirCarlaEnv/VirCarlaEnv/``:

* ``BridgeHelper``  -- SUMO <-> CARLA frames, blueprints, signal-state mappings
* ``CarlaBackend``  -- the IVirEnvBackend verbs; the ONLY module importing ``carla``
* ``CarlaViews``    -- ViewSetup camera views: an RGB camera per view, each in its own window
* ``mainVirCarla``  -- the tick driver and CLI entry point

The backend-agnostic core it drives lives in ``CommonLib/VirEnv`` and imports no
simulator SDK, so a future host (CarMaker) adds a sibling package and changes
nothing there.
"""
