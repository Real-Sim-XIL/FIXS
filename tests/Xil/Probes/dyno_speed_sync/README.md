# Dyno / CARLA forced-speed-match study

Companion to `FIXS_Applications#24`. Standalone — no CARLA server, no dyno, no
hardware. Pure Python plus an optional matplotlib for the figure.

## The question

The dyno holds the real vehicle against a road load given as EPA coastdown
coefficients, `F_road = A + B*v + C*v^2`. CARLA meanwhile runs its own vehicle,
with its own mass and its own resistance. We tie the two together by overwriting
CARLA's speed with the dyno's speed every sync interval — what
`set_target_velocity` does.

**How much does that paper over, and what does it cost?**

## Running it

Use the `realsim` conda env. The simulator itself needs only the standard
library; matplotlib is optional and only draws the figure, so if you want the
plot use `fixs_applications`, which has it.

```bash
# conda run does not work in every shell here, so resolve the interpreter directly
PY=~/miniconda3/envs/realsim/python.exe

$PY -m pytest tests/Xil/Probes/dyno_speed_sync/ -q
$PY tests/Xil/Probes/dyno_speed_sync/dyno_sync_sim.py --sync-sweep 0.002,0.005,0.02,0.05,0.10
```

Verified identical results under `realsim` (3.10.20) and `fixs_applications`
(3.10.20).

Outputs land in `out/`: one `trace_<coupling>.csv` per coupling, a `summary.json`
with every parameter and every metric, and `dyno_sync_overview.png` if matplotlib
is importable. Every physical value is a CLI option — `--help` lists them.

## The four couplings

Reported together so the numbers can be compared against something rather than
read in isolation.

| coupling | what CARLA is doing | why it is here |
|---|---|---|
| `free` | its own driver, never synced | how far the two vehicles drift apart on their own |
| `track` | its own driver, closing the loop on the dyno speed | the alternative to overwriting |
| `forced_coast` | coasting, speed overwritten | pessimistic bound: the sync ends up supplying the whole tractive effort |
| `forced_driven` | its own driver **and** speed overwritten | **the architecture under test** |

## What it found

**1. The model disagreement changes sign at about 20 m/s.**

```
   v [m/s]     road load - CARLA resistance [N]
         0                   -106.1
        10                    -74.8
        20                     -0.6
        30                    116.5
        40                    276.5
```

CARLA's rolling resistance dominates at low speed, the dyno's `C` dominates at
high speed. Below ~20 m/s the sync has to push CARLA *forward*; above it, hold it
*back*. This is not a bias that one constant correction removes.

**2. Syncing faster buys accuracy, not quietness.**

```
  sync [s]   speed err rms [m/s]   F_sync rms [N]
     0.002              0.0003              408.8
     0.005              0.0007              408.4
     0.010              0.0013              407.6
     0.020              0.0025              406.0
     0.050              0.0061              400.9
     0.100              0.0119              390.9
```

A 50x range of sync interval moves the speed error by 40x and the injected force
by 4%. That follows directly from `F_sync = m*dv/T` with `dv` growing linearly in
`T` — the two cancel. **The disturbance is set by the model mismatch, not by the
sync rate.** If ~400 N rms of unaccounted force is too much, a faster sync will
not help; only reconciling the two resistance models will.

**3. Whether CARLA drives itself matters enormously.**

```
  coupling        err rms [m/s]   F_sync rms [N]   F_sync max [N]
  free                   0.1762              n/a              n/a
  track                  1.2717              n/a              n/a
  forced_coast           0.0093           2896.2           6676.5
  forced_driven          0.0013            407.6            819.1
```

If CARLA coasts, the overwrite silently supplies the entire tractive effort —
2896 N rms, comparable to the real thing. With CARLA's own agent driving, the
sync supplies only the residual, 408 N rms with a near-zero mean (−3.7 N). Over
the cycle the overwrite does −67 kJ against 4343 kJ of tractive work, **−1.55 %**.

**4. Forced sync tracks far better than closed-loop tracking**, 0.0013 m/s rms
against 1.27 m/s. That is not a fair fight — one is an overwrite and the other a
PI loop through an actuator — but it does answer whether a well-tuned tracking
controller could substitute. At these gains, no.

**5. Matching the resistance models does not zero the sync force.** With
identical resistance curves and equal masses, ~55 N rms remains, because the dyno
delivers its effort through a second-order actuator and CARLA's driver does not.
A 10x faster actuator shrinks it. Two independent error sources, and both tests
pin them.

## What is second order, and why

The dyno's force loop is not instantaneous:

```
Fdd + 2*zeta*wn*Fd + wn^2*F = wn^2*F_cmd        default 5 Hz, zeta 0.7
```

Without it the dyno speed would be a kinematic replay of the cycle and the sync
error would be a static offset. With it the error has dynamics of its own, which
is the realistic case and the reason finding (5) exists.

## Parameters, and which are trustworthy

| value | source |
|---|---|
| CARLA `aero_CdA = 0.377 m^2`, `rho = 1.25` | CARLA source, **confirmed in motion**: `drag/v^2` constant to 3.2 % over 941 samples |
| CARLA `mass = 1845 kg` | `get_physics_control()` on the stock Tesla Model 3 |
| CARLA `roll_coeff = 0.012` | **not measured.** CARLA's rolling resistance lives inside the tire model and only appears lumped into `sum(long_force)`. This is the weakest number here and it sets the low-speed end of the gap in finding (1) |
| dyno mass, A/B/C | representative EV6 AWD values, not measured from our dyno |
| actuator 5 Hz / zeta 0.7 | placeholder for the real force loop |

**The first thing to replace with measurement is `roll_coeff`**, because it
decides where the sign change in finding (1) sits, and that crossing is the whole
character of the mismatch.

## Feeding it a real cycle

```bash
python dyno_sync_sim.py --cycle-csv mycycle.csv --sync-dt 0.01
```

Two columns, `time [s], speed [m/s]`, header optional, resampled onto `--dt`.
The EV6 drive-cycle logs used elsewhere in this work can be exported this way.
