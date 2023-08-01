// tmptraci.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

#include <libsumo\libtraci.h>
#include <windows.h>
using namespace std;

using namespace libtraci;

int main()
{
	string trafficIp = "127.0.0.1";
	int trafficPort = 1337;

	Simulation::init(trafficPort, 60, trafficIp);
	Simulation::setOrder(1);


	while (1) {
		//try {
			Simulation::step();

			vector <string> vehIdList = Vehicle::getIDList();
			for (int i = 0; i < vehIdList.size(); i++) {
				string vehId = vehIdList[i];
				vector <string> edgeList = Vehicle::getRoute(vehId);
				vector <libsumo::TraCIConnection> nextLinkList = Vehicle::getNextLinks(vehId);

				//int a = 1;
			}
		//}
		//catch (const std::exception& e) {
		//	std::cout << e.what();
		//	return -1;
		//}
	}

    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
