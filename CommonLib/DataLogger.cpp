//============================================================================
//  DataLogger -- CSV time-series logging of the FIXS vehicle-data stream.
//  See DataLogger.h for the (SUMO/VISSIM) coordinate/heading convention.
//============================================================================
#include "DataLogger.h"

#include <windows.h>     // CreateDirectoryA (CommonLib stays C++14: no std::filesystem)
#include <cstdio>

namespace fixs {

// Best-effort: create every ancestor directory of 'path' (Win32, C++14-safe).
static void ensureParentDir(const std::string& path) {
    std::string cur;
    for (char c : path) {
        if ((c == '/' || c == '\\') && !cur.empty() && cur.back() != ':')
            CreateDirectoryA(cur.c_str(), NULL);
        cur.push_back(c);
    }
}

bool DataLogger::open(const std::string& path, const std::vector<std::string>& fields) {
    path_   = path;
    fields_ = fields;
    if (fields_.empty())
        fields_ = { "positionX", "positionY", "positionZ", "heading", "speed" };

    ensureParentDir(path_);
    ofs_.open(path_, std::ios::out | std::ios::trunc);
    if (!ofs_.is_open()) return false;

    // Self-documenting convention line; CSV readers skip lines starting with '#'.
    ofs_ << "# FIXS DataLogger -- FIXS wire convention (== SUMO/VISSIM): position = "
            "front-of-vehicle anchor [m], heading = navigational degrees (0=N, CW), "
            "speed [m/s]. Backend-native frames (Carla/CarMaker) normalized upstream.\n";
    ofs_ << "simTime,id";
    for (const std::string& f : fields_) ofs_ << "," << f;
    ofs_ << "\n";
    ofs_.flush();
    return true;
}

void DataLogger::logVehicle(double simTime, const VehFullData_t& v) {
    if (!ofs_.is_open()) return;
    char t[32];
    std::snprintf(t, sizeof(t), "%.3f", simTime);
    ofs_ << t << "," << v.id;
    for (const std::string& f : fields_) ofs_ << "," << cell(f, v);
    ofs_ << "\n";
    ofs_.flush();   // 10 Hz feed rate -> cheap; survives a hard kill of the demo
}

std::string DataLogger::cell(const std::string& f, const VehFullData_t& v) {
    char b[48];
    auto num = [&](double x) { std::snprintf(b, sizeof(b), "%.5f", x); return std::string(b); };
    if (f == "positionX")     return num(v.positionX);
    if (f == "positionY")     return num(v.positionY);
    if (f == "positionZ")     return num(v.positionZ);
    if (f == "heading")       return num(v.heading);
    if (f == "speed")         return num(v.speed);
    if (f == "speedDesired")  return num(v.speedDesired);
    if (f == "acceleration")  return num(v.acceleration);
    if (f == "grade")         return num(v.grade);
    if (f == "length")        return num(v.length);
    if (f == "width")         return num(v.width);
    if (f == "height")        return num(v.height);
    if (f == "type")          return v.type;
    if (f == "vehicleClass")  return v.vehicleClass;
    if (f == "linkId")        return v.linkId;
    // The actuation channel. Logging these is how a run separates "was the
    // vehicle told to move?" from "did it move?" -- indistinguishable in a
    // position/speed trace, and with completely different causes.
    if (f == "acceleratorPedalDesired") return num(v.acceleratorPedalDesired);
    if (f == "brakePedalDesired")       return num(v.brakePedalDesired);
    if (f == "steerAngleDesired")       return num(v.steerAngleDesired);
    return "";                // unknown field -> empty cell (safe)
}

void DataLogger::close() {
    if (ofs_.is_open()) ofs_.close();
}

} // namespace fixs
