//============================================================================
//  EgoDriver -- pure-pursuit steering + proportional speed hold (SDK-free).
//  Behavior is a 1:1 port of the driver that used to live inline in
//  CarlaBackend::driveEgo; the only change is that pose/route are plain doubles
//  in a caller-chosen planar frame instead of carla::geom types.
//============================================================================
#include "EgoDriver.h"

#include <algorithm>
#include <cmath>

namespace virenv {

namespace { constexpr double kPi = 3.14159265358979323846; }

void EgoDriver::setRoute(const std::vector<std::pair<double, double>>& pts, bool closed) {
    route_.clear();
    closed_ = closed;
    cursor_ = 0;
    if (pts.size() < 2) {
        if (pts.size() == 1) route_.push_back(pts.front());
        return;
    }
    // densify the polyline to ~3 m spacing (frame-neutral; caller pre-converts
    // to its own frame). Closed loops wrap the last->first segment.
    const std::size_t segs = closed ? pts.size() : pts.size() - 1;
    for (std::size_t i = 0; i < segs; i++) {
        const std::pair<double, double>& a = pts[i];
        const std::pair<double, double>& b = pts[(i + 1) % pts.size()];
        const double dx = b.first - a.first, dy = b.second - a.second;
        const int n = std::max(1, (int)(std::sqrt(dx * dx + dy * dy) / 3.0));
        for (int k = 0; k < n; k++)
            route_.emplace_back(a.first + dx * k / n, a.second + dy * k / n);
    }
    if (!closed) route_.push_back(pts.back());
}

DriveCommand EgoDriver::computeControl(double x, double y, double headingRad,
                                       double speed, double targetSpeed, double dtSec) {
    DriveCommand c;
    const std::size_t n = route_.size();
    if (n < 2) return c;

    // advance the cursor to the nearest point ahead within a local window
    // (closed loop: wrap; open: stop at the end).
    double best = 1e18;
    std::size_t bestIx = cursor_;
    for (std::size_t k = 0; k < p_.searchWindow; k++) {
        std::size_t ix = cursor_ + k;
        if (ix >= n) { if (closed_) ix %= n; else break; }
        const double dx = route_[ix].first - x, dy = route_[ix].second - y;
        const double d = dx * dx + dy * dy;
        if (d < best) { best = d; bestIx = ix; }
    }
    cursor_ = bestIx;

    // lookahead point: ~max(6 m, 1.2 s of travel)
    const double lookahead = std::max(p_.lookaheadMin, p_.lookaheadTime * speed);
    std::size_t tgtIx = cursor_;
    double acc = 0.0;
    while (acc < lookahead) {
        std::size_t nxt = tgtIx + 1;
        if (nxt >= n) { if (closed_) nxt = 0; else { tgtIx = n - 1; break; } }
        const double dx = route_[nxt].first - route_[tgtIx].first;
        const double dy = route_[nxt].second - route_[tgtIx].second;
        acc += std::sqrt(dx * dx + dy * dy);
        tgtIx = nxt;
    }

    const double tx = route_[tgtIx].first - x;
    const double ty = route_[tgtIx].second - y;
    double alpha = std::atan2(ty, tx) - headingRad;         // heading error to lookahead
    while (alpha >  kPi) alpha -= 2.0 * kPi;
    while (alpha < -kPi) alpha += 2.0 * kPi;
    const double delta = std::atan2(2.0 * p_.wheelbase * std::sin(alpha), lookahead);

    c.steer = std::max(-1.0, std::min(1.0, delta / p_.maxSteerRad));
    // curvature-aware speed: slow toward corners so pure pursuit tracks the arc
    const double curveSlow = std::max(p_.curveSlowMin, 1.0 - p_.curveSlowGain * std::fabs(alpha));
    const double dv = targetSpeed * curveSlow - speed;
    if (dv >= 0) { c.throttle = std::min(p_.throttleMax, p_.throttleGain * dv + p_.throttleBias); c.brake = 0.0; }
    else         { c.throttle = 0.0; c.brake = std::min(1.0, -p_.brakeGain * dv); }

    // Integral term. A purely proportional law is a TRACKING law: it assumes the
    // vehicle already moves and only needs trimming. It cannot start a stopped
    // car when the requested speed is small, which is exactly what a controller
    // pulling away from a stop bar asks for -- ~0.15 m/s on the first tick, i.e.
    // 0.25*0.15 + 0.15 = 0.19 throttle. Measured on a graded arterial with a
    // 1845 kg vehicle: 0.19 and even 0.45 throttle produce a crawl (0.34 m/s
    // after 4 s), while 0.75 accelerates normally (2.98 m/s) -- the response is
    // sharply nonlinear, so no fixed floor is right for every vehicle or grade.
    //
    // Integrating the deficit fixes that without a magic number: throttle winds
    // up until the vehicle actually responds, wherever its knee happens to be,
    // and unwinds as soon as it is tracking. Clamped so it cannot exceed what the
    // throttle can express, and dropped on braking so it cannot fight the brake.
    if (dv > 0.0) {
        integral_ = std::min(integral_ + dv * dtSec, p_.throttleMax / p_.throttleIntegral);
        c.throttle = std::min(p_.throttleMax, c.throttle + p_.throttleIntegral * integral_);
    }
    else {
        integral_ = 0.0;
    }
    return c;
}

} // namespace virenv
