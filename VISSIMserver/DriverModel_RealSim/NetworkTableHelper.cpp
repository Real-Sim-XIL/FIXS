#include "NetworkTableHelper.h"

using namespace std;

NetworkTableHelper::NetworkTableHelper() {

}

int NetworkTableHelper::readSignalTable(string SignalTableFullName) {

	// Open an existing file
	//ifstream SignalTableFile(".\\SignalTable.csv");
	ifstream SignalTableFile(SignalTableFullName);

	if (!SignalTableFile) {
		
		return -1;

	}

	string curLine;
	getline(SignalTableFile, curLine); // ignore the first line
	while (getline(SignalTableFile, curLine)) {

		istringstream lineString(curLine);

		string element;

		SignalHead_t sigHead;

		// 'SignalHeadIndex' : int,
		getline(lineString, element, ',');
		sigHead.signalHeadIndex = stoi(element);

		//'SignalHeadName' : str,
		getline(lineString, element, ',');
		sigHead.signalHeadName = element;

		//'SignalGroupIndex' : int,
		getline(lineString, element, ',');
		sigHead.signalGroupIndex = stoi(element);

		//'SignalGroupName' : str,
		getline(lineString, element, ',');
		sigHead.signalGroupName = element;

		//'SignalControllerIndex' : int,
		getline(lineString, element, ',');
		sigHead.signalControllerIndex = stoi(element);

		//'SignalControllerName' : str,
		getline(lineString, element, ',');
		sigHead.signalControllerName = element;

		//'NumberOfSignalGroup' : int
		getline(lineString, element, ',');
		int nSigGroup = stoi(element);

		// save to SignalInfo
		SignalHeadInfo_um[sigHead.signalHeadIndex] = sigHead;

		//
		string sigState = "";

		for (int i = 0; i < nSigGroup; i++) {
			sigState = sigState + "z";
		}

		TrafficLightData_t Sig{ sigHead.signalControllerIndex, sigHead.signalControllerName, sigState };

		//
		SignalData[sigHead.signalControllerIndex] = Sig;
	}

	return 0;
}


int NetworkTableHelper::readLinkTable(std::string LinkTableFullName) {

	// Open an existing file
	ifstream LinkFile(LinkTableFullName);

	// if could not find such file, return -1
	if (!LinkFile) {
		return -1;
	}

	// read line by line
	string curLine;
	getline(LinkFile, curLine); // ignore the first line
	while (getline(LinkFile, curLine)) {

		istringstream lineString(curLine);

		string element;

		string index, name, fromLink, toLink;
		int numLanes, isConnector;
		double length, fromPos, toPos;

		// 'index' : string,
		getline(lineString, element, ',');
		index = element;

		//'name' : string,
		getline(lineString, element, ',');
		name = (element);

		//'length' : double,
		getline(lineString, element, ',');
		length = stod(element);

		//'numLanes' : int,
		getline(lineString, element, ',');
		numLanes = stoi(element);

		//'isConnector' : int,
		getline(lineString, element, ',');
		isConnector = stoi(element);

		//'fromLink' : string,
		getline(lineString, element, ',');
		fromLink = (element);

		//'fromPos' : double,
		getline(lineString, element, ',');
		if (element.empty()) {
			fromPos = -1;
		}
		else {
			fromPos = stod(element);
		}

		//'toLink' : string,
		getline(lineString, element, ',');
		toLink = element;

		//'toPos' : double,
		getline(lineString, element, ',');
		if (element.empty()) {
			toPos = -1;
		}
		else {
			toPos = stod(element);
		}


		LinkInfo_um[index] = make_pair(length, numLanes);

		if (isConnector) {
			ConnectorInfo_um[index] = make_tuple(fromLink, fromPos, toLink, toPos);
		}
	}


	return 0;
}



int NetworkTableHelper::readSpeedLimitTable(string SpeedLimitFullName) {
	
	int readSpeedLimitStatus = 0;

	// Open an existing file
	ifstream SpeedLimitFile(SpeedLimitFullName);

	// if could not find such file, return -1
	if (!SpeedLimitFile) {
		return -1;
	}

	fstream fLog;
	fLog.open("DriverModelLog.txt", fstream::in | fstream::out | fstream::app);

	// read line by line
	string curLine;
	getline(SpeedLimitFile, curLine); // ignore the first line
	while (getline(SpeedLimitFile, curLine)) {

		istringstream lineString(curLine);

		string element;

		string link, vehicleType;
		int lane, speedLimitType;
		double position, speedLimitValue, timeFrom, timeTo;

		// 'Link' : string,
		getline(lineString, element, ',');
		if (element.empty()) {
			fLog << "SpeedLimitTable.csv has empty link item, check SpeedLimitTable.csv!" << endl;
			link = -1;
			readSpeedLimitStatus = 10000;
		}
		else {
			link = element;
		}

		//'lane' : int,
		getline(lineString, element, ',');
		if (element.empty()) {
			fLog << "SpeedLimitTable.csv link" << link << " has empty lane item, check SpeedLimitTable.csv!" << endl;
			lane = -1;
			readSpeedLimitStatus = 10000;
		}
		else {
			lane = stoi(element);
		}

		//'position' : double,
		getline(lineString, element, ',');
		if (element.empty()) {
			fLog << "SpeedLimitTable.csv link" << link << " lane " << lane << " has empty position item, check SpeedLimitTable.csv!" << endl;
			position = -1;
			readSpeedLimitStatus = 10000;
		}
		else {
			position = stod(element);
		}

		//'vehicleType' : string,
		getline(lineString, element, ',');
		if (element.empty()) {
			fLog << "SpeedLimitTable.csv link" << link << " lane " << lane << " position " << position << " has empty vehicleType item, check SpeedLimitTable.csv!" << endl;
			vehicleType = -1;
			readSpeedLimitStatus = 10000;
		}
		else {
			vehicleType = element;
		}

		//'speedLimitType' : int,
		getline(lineString, element, ',');
		if (element.empty()) {
			fLog << "SpeedLimitTable.csv link" << link << " lane " << lane << " position " << position << " has empty speedLimitType item, check SpeedLimitTable.csv!" << endl;
			speedLimitType = -1;
			readSpeedLimitStatus = 10000;
		}
		else {
			speedLimitType = stoi(element);
		}

		//'speedLimitValue' : double,
		getline(lineString, element, ',');
		if (element.empty()) {
			fLog << "SpeedLimitTable.csv link" << link << " lane " << lane << " position " << position << " has empty speedLimitValue item, check SpeedLimitTable.csv!" << endl;
			speedLimitValue = -1;
			readSpeedLimitStatus = 10000;
		}
		else {
			speedLimitValue = stod(element);
		}

		//'timeFrom' : double,
		getline(lineString, element, ',');
		if (element.empty()) {
			fLog << "SpeedLimitTable.csv link" << link << " lane " << lane << " position " << position << " has empty timeFrom item, check SpeedLimitTable.csv!" << endl;
			timeFrom = -1;
			readSpeedLimitStatus = 10000;
		}
		else {
			timeFrom = stod(element);
		}

		//'timeTo' : double,
		getline(lineString, element, ',');
		if (element.empty()) {
			fLog << "SpeedLimitTable.csv link" << link << " lane " << lane << " position " << position << " has empty timeTo item, check SpeedLimitTable.csv!" << endl;
			timeTo = -1;
			readSpeedLimitStatus = 10000;
		}
		else {
			timeTo = stod(element);
		}
		
		// if no current link, lane, vehicleType in the um
		tuple <string, int, string> linklanetype_tuple = make_tuple(link, lane, vehicleType);
		SpeedLimit_t speedLimit{ position, speedLimitType, speedLimitValue, timeFrom, timeTo }; // position, speedlimittype, speedlimitvalue, timefrome, timeto
		if (SpeedLimitInfo_um.find(linklanetype_tuple) == SpeedLimitInfo_um.end()) {
			vector <SpeedLimit_t> temp{ speedLimit };
			SpeedLimitInfo_um[linklanetype_tuple] = temp;
		}
		else {
			auto temp = SpeedLimitInfo_um[linklanetype_tuple];
			temp.push_back(speedLimit);
			SpeedLimitInfo_um[linklanetype_tuple] = temp;
		}

	}

	for (auto it : SpeedLimitInfo_um) {
		std::sort(it.second.begin(), it.second.end(),
			[](const auto& i, const auto& j) { return i.position < j.position; });
		SpeedLimitInfo_um[it.first] = it.second;
	}

	fLog.close();

	return readSpeedLimitStatus;
}

void NetworkTableHelper::parserStringVector(YAML::Node node, string name, vector<string>& outStringVector) {
	YAML::Node subnode = node[name];

	for (int i = 0; i < subnode.size(); i++) {
		outStringVector.push_back(subnode[i].as<std::string>());

	}

}

int NetworkTableHelper::readRoutes(string routeFileName) {
	YAML::Node config = YAML::LoadFile(routeFileName);

	YAML::Node node = config["Routes"];

	
	// iterate over each route
	for (int i = 0; i < node.size(); i++) {
		YAML::Node subnode = node[i];

		YAML::Node typenode;
		YAML::Node idnode;
		YAML::Node lanenode;
		YAML::Node linknode;
		vector<string> type_v;
		vector<string> id_v;
		vector<string> link_v;
		vector<string> lane_v;

		vector<pair<string, int>> linklane_v;
		double initialSpeedLimit = -1;
		for (YAML::const_iterator it = subnode.begin(); it != subnode.end(); ++it) {
			const std::string field = it->first.as<std::string>();

			if (field.compare("vehicleType") == 0) {
				parserStringVector(subnode, "vehicleType", type_v);

			}
			if (field.compare("id") == 0) {
				parserStringVector(subnode, "id", id_v);
			}
			if (field.compare("link") == 0) {
				parserStringVector(subnode, "link", link_v);

			}
			if (field.compare("lane") == 0) {
				parserStringVector(subnode, "lane", lane_v);

			}
			if (field.compare("initialSpeedLimit") == 0) {
				initialSpeedLimit = stof(subnode["initialSpeedLimit"][0].as<std::string>());
			}

			int aa = 1;
		}


		// storage
		// sanity check 			
		if (lane_v.size() != link_v.size() && lane_v.size() != 1) {
			return -10001;
		}
		for (int i = 0; i < link_v.size(); i++) {
			if (lane_v.size() != link_v.size() && lane_v.size() == 1) {
				linklane_v.push_back(make_pair(link_v[i], stod(lane_v[0])));
			}
			else {
				linklane_v.push_back(make_pair(link_v[i], stod(lane_v[i])));
			}
		}
		// if define as id
		if (type_v.size() > 0 && id_v.size() > 0 && link_v.size() > 0) {
			// sanity check
			if (type_v.size() != id_v.size()) {
				return -10002;
			}

			// save to map
			for (int i = 0; i < type_v.size(); i++) {
				VehIdVehType2linklaneRoute[make_pair(id_v[i], type_v[i])] = linklane_v;
			}

		}
		// if define as a vehicle type
		else if (type_v.size() > 0 && id_v.size() == 0 && link_v.size() > 0) {

			// save to map
			for (int i = 0; i < type_v.size(); i++) {
				VehicleType2linklaneRoute[type_v[i]] = linklane_v;
			}
		}

		// add initial speed limit information
		SpeedLimit_t speedLimit{ 0, 0, initialSpeedLimit, 0, 99999 };
		auto linklanetype_tuple = make_tuple(link_v[0], stod(lane_v[0]), type_v[0]);
		if (SpeedLimitInfo_um.find(linklanetype_tuple) != SpeedLimitInfo_um.end()) {
			// if there is a speed limit of the same position, ignore
			bool ignoreSpeedLimit = false;
			for (auto curSpdLim : SpeedLimitInfo_um[linklanetype_tuple]) {
				if (abs(curSpdLim.position - speedLimit.position)<1e-5) {
					ignoreSpeedLimit = true;
					break;
				}
			}
			if (! ignoreSpeedLimit) {
				SpeedLimitInfo_um[linklanetype_tuple].push_back(speedLimit);
				auto speedLimit_v = SpeedLimitInfo_um[linklanetype_tuple];
				std::sort(speedLimit_v.begin(), speedLimit_v.end(),
					[](const auto& i, const auto& j) { return i.position < j.position; });
				SpeedLimitInfo_um[linklanetype_tuple] = speedLimit_v;
			}	
		}
		else {
			SpeedLimitInfo_um[linklanetype_tuple] = { speedLimit };
		}


		int aa = 1;

	}


	// make lookup tables
	for (auto itRoute : VehIdVehType2linklaneRoute) {

		auto type = itRoute.first.second;
		auto linklane_v = itRoute.second;

		// for each link lane on the route, assign a speed limit
		// need to go back ward
		vector <double> nextSpeedLimit_v(linklane_v.size());
		vector <double> nextSpeedLimitDistance_v(linklane_v.size());
		parserRouteSpeedLimit(type, linklane_v, nextSpeedLimit_v, nextSpeedLimitDistance_v);

		auto id = itRoute.first.first;

		// then cumulative sum all link length, then save to unordered_map
		for (int i = 0; i < linklane_v.size(); i++) {
			auto link = linklane_v[i].first;
			auto lane = linklane_v[i].second;

			RouteVehIdSpeedLimit_um[make_tuple(link, lane, type, id)] = make_pair(nextSpeedLimit_v[i], nextSpeedLimitDistance_v[i]);

		}

	}

	// make lookup tables
	for (auto itRoute : VehicleType2linklaneRoute) {

		auto type = itRoute.first;
		auto linklane_v = itRoute.second;

		// for each link lane on the route, assign a speed limit
		// need to go back ward
		vector <double> nextSpeedLimit_v(linklane_v.size());
		vector <double> nextSpeedLimitDistance_v(linklane_v.size());
		parserRouteSpeedLimit(type, linklane_v, nextSpeedLimit_v, nextSpeedLimitDistance_v);

		// then cumulative sum all link length, then save to unordered_map
		for (int i = 0; i < linklane_v.size(); i++) {
			auto link = linklane_v[i].first;
			auto lane = linklane_v[i].second;

			RouteVehTypeSpeedLimit_um[make_tuple(link, lane, type)] = make_pair(nextSpeedLimit_v[i], nextSpeedLimitDistance_v[i]);

		}
	}

	int aa = 1;

	return 1;

}

void NetworkTableHelper::parserRouteSpeedLimit(std::string type, std::vector<std::pair<std::string, int>> linklane_v, std::vector <double>& nextSpeedLimit_v, std::vector <double>& nextSpeedLimitDistance_v) {
	double nextSpeedLimit = -1;
	double nextSpeedLimitDistance = -1;
	// last link on route has no next speed limit
	nextSpeedLimit_v[nextSpeedLimit_v.size() - 1] = -1;
	nextSpeedLimitDistance_v[nextSpeedLimit_v.size() - 1] = -1;
	for (int i = linklane_v.size() - 2; i >= 0; i--) {
		// use next link lane to check speed limit
		auto linkNext = linklane_v[i + 1].first;
		auto laneNext = linklane_v[i + 1].second;

		int temp = stod(linkNext);

		auto linklanetype_tuple = make_tuple(linkNext, laneNext, type);
		// if current link has speed limit information then update it
		if (SpeedLimitInfo_um.find(linklanetype_tuple) != SpeedLimitInfo_um.end()) {
			// current speed limit is first speed limit value
			nextSpeedLimit = SpeedLimitInfo_um[linklanetype_tuple][0].speedLimitValue;

			// if is connector 
			if (ConnectorInfo_um.find(linkNext) != ConnectorInfo_um.end()) {
				// connector length - (from link length - from position)
				nextSpeedLimitDistance = SpeedLimitInfo_um[linklanetype_tuple][0].position - (LinkInfo_um[get<0>(ConnectorInfo_um[linkNext])].first - get<1>(ConnectorInfo_um[linkNext]));
			}
			else {
				nextSpeedLimitDistance = SpeedLimitInfo_um[linklanetype_tuple][0].position;
			}
		}
		else {
			// if not -1
			if (nextSpeedLimit > 0) {
				// if is connector 
				if (ConnectorInfo_um.find(linkNext) != ConnectorInfo_um.end()) {
					// connector length - (from link length - from position) - to link position
					nextSpeedLimitDistance += LinkInfo_um[linkNext].first - (LinkInfo_um[get<0>(ConnectorInfo_um[linkNext])].first - get<1>(ConnectorInfo_um[linkNext])) - get<3>(ConnectorInfo_um[linkNext]);
				}
				else {
					nextSpeedLimitDistance += LinkInfo_um[linkNext].first;
				}
			}
		}

		nextSpeedLimit_v[i] = nextSpeedLimit;
		nextSpeedLimitDistance_v[i] = nextSpeedLimitDistance;
	}

	
}


int NetworkTableHelper::getSpeedLimit(double simTime, VehicleDataAuxiliary_t VehicleDataAuxiliary, VehFullData_t& VehicleData) {

	// each step, check if has SpeedLimit, if not, try to get one
	// if no available
	//		keep the previous one
	// else
	//		update
	// 
	// if has SpeedLimit 
	//		check if int reaches end of vector <SpeedLimit_t>
	//		if so, need to update vector <SpeedLimit_t>
	//		else, check if position goes into next SpeedLimit_t
	// else
	//		use the previous SpeedLimit

	// INPUTS
	// get short names
	string id = VehicleData.id;
	string link = VehicleData.linkId;
	int lane = VehicleData.laneId;
	string type = VehicleData.type;
	string linkNext = VehicleData.linkIdNext;

	double position = VehicleDataAuxiliary.positionOnLink;

	// OUTPUTS
	// 
	double speedLimitValue = -1;
	double speedLimitNextValue = -1;
	double speedLimitChangeDistanceValue = -1;

	auto linklanetype_tuple = make_tuple(link, lane, type);

	int idInt = stod(id);
	int linkInt = stod(link);

	int isNextSpeedLimitFound = 0;
	double nextSpeedLimit = -1.0;
	double nextSpeedLimitDistanceCalc = -1.0;

	vector <SpeedLimit_t> SpeedLimit_v;

	if (SpeedLimitInfo_um.find(linklanetype_tuple) != SpeedLimitInfo_um.end()) {
		// retreive speed limit vector
		SpeedLimit_v = SpeedLimitInfo_um[linklanetype_tuple];
	}

	// if current link has speed limit
	// use distance to end of link as initial distance to next link
	// will have current speed limit only when on one of link 
	if (SpeedLimit_v.size() > 0) {


		//// very initial time step, need to track speed limit of this vehicle
		//if (VehicleId2SpeedLimitIntervals.find(id) == VehicleId2SpeedLimitIntervals.end()) {	
		//	// if already beyond the first speed limit point, or has initial speed limit at position 0
		//	if (VehicleDataAuxiliary.positionOnLink >= SpeedLimit_v[0].position || abs(SpeedLimit_v[0].position) < 1e-5) {
		//		VehicleId2SpeedLimitIntervals[id] = 0;
		//	}
		//	else {
		//		// assign a placeholder 
		//		VehicleId2SpeedLimitIntervals[id] = -1;
		//	}
		//}
		
		// if already beyond the first speed limit point, or has initial speed limit at position 0
		if (VehicleDataAuxiliary.positionOnLink >= SpeedLimit_v[0].position || abs(SpeedLimit_v[0].position) < 1e-5) {
			VehicleId2SpeedLimitIntervals[id] = 0;
		}
		else {
			// assign a placeholder 
			VehicleId2SpeedLimitIntervals[id] = -1;
		}

		// check if reaches next interval
		// should continuously check until reach the last SpeedLimit with position smaller than vehicle current position
		if (int(VehicleId2SpeedLimitIntervals[id] + 2) <= SpeedLimit_v.size()) {
			int iIntvl = VehicleId2SpeedLimitIntervals[id] + 1;
			while (iIntvl+1 <= SpeedLimit_v.size()) {
				double positionNext = SpeedLimit_v[iIntvl].position;
				if (position >= positionNext) {
					VehicleId2SpeedLimitIntervals[id]++;
					iIntvl++;
				}
				else {
					break;
				}
			}
			//double positionNext = SpeedLimit_v[VehicleId2SpeedLimitIntervals[id] + 1].position;
			//if (position >= positionNext) {
			//	VehicleId2SpeedLimitIntervals[id]++;
			//}
		}

		// if there is at least one more speed limit on current link, then we can find next speed limit
		if (int(VehicleId2SpeedLimitIntervals[id] + 2) <= SpeedLimit_v.size()) {
			isNextSpeedLimitFound = 1;

			nextSpeedLimit = SpeedLimit_v[VehicleId2SpeedLimitIntervals[id] + 1].speedLimitValue;
			nextSpeedLimitDistanceCalc = SpeedLimit_v[VehicleId2SpeedLimitIntervals[id] + 1].position - position;
		}
		// else, need to loop over all next links to find correct next speed limit, if any
		else {
			
			isNextSpeedLimitFound = 0;

		}

	}
	// else current link has no speed limit
	else {
		//// very initial time step, need to track speed limit of this vehicle
		//if (VehicleId2SpeedLimitIntervals.find(id) == VehicleId2SpeedLimitIntervals.end()) {
		//	// assign a placeholder 
		//	VehicleId2SpeedLimitIntervals[id] = -1;
		//}
		VehicleId2SpeedLimitIntervals[id] = -1;
	}

	// if have not found next speed limit yet (except for case wheren there's one more speed limit for current link)
	if (isNextSpeedLimitFound <= 0) {

		// if we have a route look up table, then can speed up
		// check generic vehicle class lookup table first
		if (RouteVehTypeSpeedLimit_um.find(make_tuple(link, lane, type)) != RouteVehTypeSpeedLimit_um.end()) {
			if (RouteVehTypeSpeedLimit_um[make_tuple(link, lane, type)].first > 0) {
				nextSpeedLimit = RouteVehTypeSpeedLimit_um[make_tuple(link, lane, type)].first;
				// if connector
				if (ConnectorInfo_um.find(link) != ConnectorInfo_um.end()) {
					// remaining length on link - to link length + distance to speed limit from next link
					nextSpeedLimitDistanceCalc = LinkInfo_um[link].first - VehicleDataAuxiliary.positionOnLink - get<3>(ConnectorInfo_um[link]) + RouteVehTypeSpeedLimit_um[make_tuple(link, lane, type)].second;
				}
				else {
					nextSpeedLimitDistanceCalc = LinkInfo_um[link].first - VehicleDataAuxiliary.positionOnLink + RouteVehTypeSpeedLimit_um[make_tuple(link, lane, type)].second;
				}
				isNextSpeedLimitFound = 1;
			}
			else {
				nextSpeedLimit = -1;
				nextSpeedLimitDistanceCalc = -1;
				isNextSpeedLimitFound = 0;
			}

		}
		// otherwise check specific vehicle id lookup table
		else if (RouteVehIdSpeedLimit_um.find(make_tuple(link, lane, type, id)) != RouteVehIdSpeedLimit_um.end()) {
			if (RouteVehIdSpeedLimit_um[make_tuple(link, lane, type, id)].first > 0) {
				nextSpeedLimit = RouteVehIdSpeedLimit_um[make_tuple(link, lane, type, id)].first;
				// if connector
				if (ConnectorInfo_um.find(link) != ConnectorInfo_um.end()) {
					// remaining length on link - to link length + distance to speed limit from next link
					nextSpeedLimitDistanceCalc = LinkInfo_um[link].first - VehicleDataAuxiliary.positionOnLink - get<3>(ConnectorInfo_um[link]) + RouteVehIdSpeedLimit_um[make_tuple(link, lane, type, id)].second;
				}
				else {
					nextSpeedLimitDistanceCalc = LinkInfo_um[link].first - VehicleDataAuxiliary.positionOnLink + RouteVehIdSpeedLimit_um[make_tuple(link, lane, type, id)].second;
				}
				isNextSpeedLimitFound = 1;
			}
			else {
				nextSpeedLimit = -1;
				nextSpeedLimitDistanceCalc = -1;
				isNextSpeedLimitFound = 0;
			}
			
		}
		// else, have to guess
		else {
			// get remaining distance of current link as initial next speed limit distance
			if (ConnectorInfo_um.find(link) != ConnectorInfo_um.end()) {
				nextSpeedLimitDistanceCalc = LinkInfo_um[link].first - VehicleDataAuxiliary.positionOnLink - get<3>(ConnectorInfo_um[link]);
			}
			else {
				nextSpeedLimitDistanceCalc = LinkInfo_um[link].first - VehicleDataAuxiliary.positionOnLink;
			}

			// loop over all next link
			for (auto nextLink : VehicleDataAuxiliary.nextLink_v) {
				// try to find a speed limit for current next line, and just any lane...
				for (int i = 1; i <= LinkInfo_um[nextLink].second; i++) {
					// if the current lane of next link, there is a speed limit, then stops
					if (SpeedLimitInfo_um.find(make_tuple(nextLink, i, type)) != SpeedLimitInfo_um.end()) {
						isNextSpeedLimitFound = 1;

						if (ConnectorInfo_um.find(nextLink) != ConnectorInfo_um.end()) {
							nextSpeedLimitDistanceCalc += SpeedLimitInfo_um[make_tuple(nextLink, i, type)][0].position - (LinkInfo_um[get<0>(ConnectorInfo_um[nextLink])].first - get<1>(ConnectorInfo_um[nextLink]));
						}
						else {
							nextSpeedLimitDistanceCalc += SpeedLimitInfo_um[make_tuple(nextLink, i, type)][0].position;
						}

						nextSpeedLimit = SpeedLimitInfo_um[make_tuple(nextLink, i, type)][0].speedLimitValue;

						break;
					}
				}

				if (isNextSpeedLimitFound > 0) {
					break;
				}

				// if not found, accumulate the entire distance				
				if (ConnectorInfo_um.find(nextLink) != ConnectorInfo_um.end()) {
					nextSpeedLimitDistanceCalc += LinkInfo_um[nextLink].first - (LinkInfo_um[get<0>(ConnectorInfo_um[nextLink])].first - get<1>(ConnectorInfo_um[nextLink])) - get<3>(ConnectorInfo_um[nextLink]);
				}
				else {
					nextSpeedLimitDistanceCalc += LinkInfo_um[nextLink].first;
				}

			}

			// if not found for all following links, then set to -1
			if (isNextSpeedLimitFound <= 0) {
				nextSpeedLimit = -1;
				nextSpeedLimitDistanceCalc = -1;
			}


			//// to speed up, only check the first two links (one connector, one normal link)
			//int iLink = 0;
			//for (auto nextLink : VehicleDataAuxiliary.nextLink_v) {
			//	// only check next link same lane
			//	int i = lane;
			//	if (SpeedLimitInfo_um.find(make_tuple(nextLink, i, type)) != SpeedLimitInfo_um.end()) {
			//		VehicleDataAuxiliary.isNextSpeedLimitFound = 1;

			//		if (ConnectorInfo_um.find(nextLink) != ConnectorInfo_um.end()) {
			//			VehicleDataAuxiliary.nextSpeedLimitDistanceCalc += SpeedLimitInfo_um[make_tuple(nextLink, i, type)][0].position - (LinkInfo_um[get<0>(ConnectorInfo_um[nextLink])].first - get<1>(ConnectorInfo_um[nextLink]));
			//		}
			//		else {
			//			VehicleDataAuxiliary.nextSpeedLimitDistanceCalc += SpeedLimitInfo_um[make_tuple(nextLink, i, type)][0].position;
			//		}

			//		VehicleDataAuxiliary.nextSpeedLimit = SpeedLimitInfo_um[make_tuple(nextLink, i, type)][0].speedLimitValue;

			//		break;
			//	}

			//	// at most check two links
			//	if (VehicleDataAuxiliary.isNextSpeedLimitFound > 0) {
			//		break;
			//	}

			//	// if not found, accumulate the entire distance				
			//	if (ConnectorInfo_um.find(nextLink) != ConnectorInfo_um.end()) {
			//		VehicleDataAuxiliary.nextSpeedLimitDistanceCalc += LinkInfo_um[nextLink].first - (LinkInfo_um[get<0>(ConnectorInfo_um[nextLink])].first - get<1>(ConnectorInfo_um[nextLink])) - get<3>(ConnectorInfo_um[nextLink]);
			//	}
			//	else {
			//		VehicleDataAuxiliary.nextSpeedLimitDistanceCalc += LinkInfo_um[nextLink].first;
			//	}

			//	iLink++;
			//}
		}
		
	}


	// if no current speed limit information, use previous one
	if (SpeedLimit_v.size() == 0 || VehicleId2SpeedLimitIntervals[id] < 0) {
		if (VehicleId2SpeedLimitCurrent.find(id) != VehicleId2SpeedLimitCurrent.end()) {
			//speedLimitValue = VehicleData.speedLimit;
			speedLimitValue = VehicleId2SpeedLimitCurrent[id];
		}
		else {
			speedLimitValue = -1;
		}
		
	}
	else {
		// !!!!!! check time interval as well
		speedLimitValue = SpeedLimit_v[VehicleId2SpeedLimitIntervals[id]].speedLimitValue; 
	}

	// if next one has no speed limit informaiton, then nextSpeedLimit, nextSpeedLimitDistanceCalc should be -1
	if (isNextSpeedLimitFound <= 0) {
		//speedLimitNextValue = VehicleData.speedLimitNext;
		//speedLimitChangeDistanceValue = VehicleData.speedLimitChangeDistance;
		speedLimitNextValue = nextSpeedLimit;
		speedLimitChangeDistanceValue = nextSpeedLimitDistanceCalc;
	}
	else {
		speedLimitNextValue = nextSpeedLimit; 
		speedLimitChangeDistanceValue = nextSpeedLimitDistanceCalc;
	}
	
	VehicleId2SpeedLimitCurrent[id] = speedLimitValue;

	VehicleData.speedLimit = speedLimitValue; 
	VehicleData.speedLimitNext = speedLimitNextValue; 
	VehicleData.speedLimitChangeDistance = speedLimitChangeDistanceValue;

	return 1;





	////// also get speed limit for next link
	////if (SpeedLimitInfo_um.find(linknextlanetype_tuple) != SpeedLimitInfo_um.end()) {
	////	// !!!!!!!! if next link have different number of lanes
	////	SpeedLimitNext_v = SpeedLimitInfo_um[linknextlanetype_tuple];
	////}	



	//// if the vehicle has been in the network
	////else {
	//	// if no speed limit, then move forward, use previous value
	//	if (SpeedLimit_v.size() == 0) {

	//	}
	//	else {
	//		// check if reaches next interval
	//		// !!!!!! need to add UDA 
	//		int intvlId = VehicleId2SpeedLimitIntervals[id];
	//		if ( (int)(intvlId + 2) <= SpeedLimit_v.size()) {
	//			double positionNext = SpeedLimit_v[intvlId + 1].position;
	//			if (position >= positionNext) {
	//				//// make sure simTime is within current time interval
	//				//while (intvlId + 1 <= SpeedLimit_v.size() - 1) {
	//				//	if (simTime >= SpeedLimit_v[intvlId + 1].timeFrom && simTime <= SpeedLimit_v[intvlId + 1].timeTo) {
	//				//		intvlId++;
	//				//		break;
	//				//	}
	//				//	else {
	//				//		intvlId++;
	//				//	}
	//				//}
	//				// !!!!!! check time interval as well
	//				VehicleId2SpeedLimitIntervals[id] = intvlId + 1;
	//			}
	//		}

	//	}


	////}



	//// if no speed limit information, use previous one
	//if (SpeedLimit_v.size() == 0 || VehicleId2SpeedLimitIntervals[id] < 0) {
	//	speedLimitValue = VehicleData.speedLimit;
	//}
	//else {
	//	// !!!!!! check time interval as well
	//	speedLimitValue = SpeedLimit_v[VehicleId2SpeedLimitIntervals[id]].speedLimitValue;
	//}

	//// if next one has no speed limit informaiton, use previous one
	//if (VehicleDataAuxiliary.isNextSpeedLimitFound <= 0) {
	//	speedLimitNextValue = VehicleData.speedLimitNext;
	//	speedLimitChangeDistanceValue = VehicleData.speedLimitChangeDistance;
	//}
	//else {
	//	// !!!!!! check time interval as well


	//	// if current link still has more speed limit
	//	//if ( int (VehicleId2SpeedLimitIntervals[id] + 2) <= SpeedLimit_v.size() ) {
	//	//	speedLimitNextValue = SpeedLimit_v[VehicleId2SpeedLimitIntervals[id] + 1].speedLimitValue;
	//	//	speedLimitChangeDistanceValue = SpeedLimit_v[VehicleId2SpeedLimitIntervals[id] + 1].position - position;
	//	//}
	//	//else {



	//		// next speed limit always will be the first one
	//		speedLimitNextValue = VehicleDataAuxiliary.nextSpeedLimit;
	//		// !!!!!! need map of length of each link to calculate distance of change
	//		// NOTE: next link cannot be empty, otherwise SpeedLimitNext_v will be size of 0

	//		// if next one is connector, current is nonconnector
	//		// (link position - position) - (link position - next link from position) + speed limit next position
	//		// need to calculate from main dll
	//		// need an auxiliary variable
	//		//if (ConnectorInfo_um.find(linkNext) != ConnectorInfo_um.end() && \
	//		//	ConnectorInfo_um.find(link) == ConnectorInfo_um.end()) {

	//		//}
	//		
	//		speedLimitChangeDistanceValue = VehicleDataAuxiliary.nextSpeedLimitDistanceCalc;
	//	//}
	//}


	//VehicleData.speedLimit = speedLimitValue;
	//VehicleData.speedLimitNext = speedLimitNextValue;
	//VehicleData.speedLimitChangeDistance = speedLimitChangeDistanceValue;

	
}
