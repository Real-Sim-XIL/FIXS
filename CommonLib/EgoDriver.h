#pragma once
//============================================================================
//  EgoDriver  (#174 -- backend-agnostic geometric ego driver)
//----------------------------------------------------------------------------
//  A standalone fallback driver: pure-pursuit steering + proportional speed
//  hold. Deliberately NOT fused with any simulator SDK -- it knows nothing of
//  Carla/CarMaker types. The caller supplies the ego pose and the route in ONE
//  consistent planar frame (x, y, heading in radians) and applies the returned
//  DriveCommand through whatever actuation the backend owns (Carla ApplyControl,
//  a CarMaker steer/pedal channel, ...).
//
//  This is the map-agnostic L0 fallback used when the simulator's native driver
//  is unavailable or undesirable (e.g. Carla Traffic Manager on a map whose
//  OpenDRIVE topology it cannot route). When the native driver works, prefer it
//  (see CarlaBackend::enableEgoTM); this module stays selectable via config.
//============================================================================

#include <vector>
#include <utility>
#include <cstddef>

namespace virenv {

// Neutral actuation command. throttle/brake in [0,1]; steer in [-1,1]
// (-1 full left, +1 full right), backend maps it to the sim's own convention.
struct DriveCommand {
    double throttle = 0.0;
    double brake    = 0.0;
    double steer    = 0.0;
};

class EgoDriver {
public:
    // Tunables (defaults reproduce the original bridge driver exactly).
    struct Params {
        double wheelbase     = 2.9;   // m (~tesla model3)
        double maxSteerRad   = 0.7;   // wheel angle at |steer| = 1 (~40 deg)
        double lookaheadMin  = 6.0;   // m
        double lookaheadTime = 1.2;   // s   (lookahead = max(min, time * speed))
        double curveSlowGain = 1.2;   // target-speed reduction vs |heading error|
        double curveSlowMin  = 0.4;
        double throttleGain  = 0.25;  // per (m/s) speed deficit
        double throttleBias  = 0.15;
        double throttleMax   = 0.75;
        double brakeGain     = 0.30;  // per (m/s) speed excess
        // Integral gain on the speed deficit (throttle per m/s per second). The
        // proportional terms above are a TRACKING law and cannot start a stopped
        // car when the requested speed is small; this winds the throttle up until
        // the vehicle responds, wherever its torque knee is, and unwinds again
        // once it tracks. 3.0 takes a persistent 0.15 m/s deficit from 0.19 to
        // full throttle in about a second.
        double throttleIntegral = 3.0;
        std::size_t searchWindow = 40;// nearest-point local window (points)
    };

    // Route in the caller's planar frame. Densified internally to ~3 m spacing.
    // closed=true wraps the path endlessly (loop scenarios).
    void setRoute(const std::vector<std::pair<double, double>>& pts, bool closed);
    bool hasRoute() const { return route_.size() >= 2; }
    void reset() { cursor_ = 0; integral_ = 0.0; }

    // One control step. (x, y) and headingRad are the ego pose in the route
    // frame; speed & targetSpeed in m/s; dtSec is the interval this command will
    // be held for (the integral term's step). Advances the internal path cursor.
    DriveCommand computeControl(double x, double y, double headingRad,
                                double speed, double targetSpeed, double dtSec = 0.05);

    const Params& params() const { return p_; }
    Params&       params()       { return p_; }
    std::size_t   routeSize() const { return route_.size(); }

private:
    std::vector<std::pair<double, double>> route_;   // densified planar path
    bool        closed_ = true;
    std::size_t cursor_ = 0;                          // current nearest index
    double      integral_ = 0.0;                      // wound-up speed deficit (m/s * s)
    Params      p_;
};

} // namespace virenv
