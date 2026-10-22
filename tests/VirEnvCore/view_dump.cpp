// Prints the ViewSetup.Views ConfigHelper.cpp resolves from a yaml, one line per
// view plus the back-filled legacy keys, for tests/VirEnv/test_view_setup_parity.py.
//
//   view_dump.exe <config.yaml>

#include "ConfigHelper.h"

#include <cstdio>
#include <string>

static std::string nums(const std::vector<double>& v) {
	std::string out;
	char buf[64];
	for (size_t i = 0; i < v.size(); i++) {
		snprintf(buf, sizeof(buf), "%s%.6g", i ? "," : "", v[i]);
		out += buf;
	}
	return out;
}

int main(int argc, char** argv) {
	if (argc < 2) {
		fprintf(stderr, "usage: view_dump <config.yaml>\n");
		return 2;
	}
	ConfigHelper c;
	c.getConfig(argv[1]);
	for (const ViewDef_t& v : c.ViewSetup.Views) {
		printf("VIEW|%s|%s|%s|%s|%d|%d|%dx%d|%.6g|%.6g|%s|%.6g|%.6g|%d|%.6g|%s\n",
			v.Name.c_str(), v.Simulator.c_str(), v.Type.c_str(), v.Target.c_str(),
			(int)v.Enable, (int)v.Implied, v.Size[0], v.Size[1], v.Fov, v.SensorTick,
			nums(v.Offset).c_str(), v.Pitch, v.Height, (int)v.AlignYaw, v.Zoom,
			nums(v.Boundary).c_str());
	}
	printf("VIEW|legacy-carla|%d|%s|%.6g|%d\n", (int)c.CarlaSetup.EnableSpectatorFollow,
		c.CarlaSetup.CenteredViewId.c_str(), c.CarlaSetup.SpectatorHeight, (int)c.CarlaSetup.SpectatorAlignYaw);
	printf("VIEW|legacy-sumo|%d|%s|%.6g\n", (int)c.SumoSetup.EnableGuiFollow,
		c.SumoSetup.GuiCenteredViewId.c_str(), c.SumoSetup.GuiZoom);
	return 0;
}
