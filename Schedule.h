#pragma once

class Schedule
{
	int schID;
	Vehicle vehicleObj;
	Commuter commuterObj;
public:
	Schedule(int schID, Vehicle vehicle, Commuter commuter);
	void displaySchedule();
};

