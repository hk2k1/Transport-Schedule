#include <iostream>
#include <string>
#include <iomanip>			//For set() and setfill()
#include <vector>
#include <fstream>			//For file access
#include <sstream>			//For stringstream
#include <thread>			//For thread control - sleep
#include <algorithm>		//For sorting
#include<conio.h>
#include<iterator>
#include <windows.h> 

using namespace std;

#include "Vehicle.h"
#include "Commuter.h"
#include "Schedule.h"

/****************** Constants*************************************************/
static constexpr int VEHICLE_SMALL = 2;
static constexpr int VEHICLE_LARGE = 4;
static constexpr int VEHICLE_SUPER = 6;

/****************** class static constant ************************************/
const int Vehicle::V_SMALL = VEHICLE_SMALL;
const int Vehicle::V_LARGE = VEHICLE_LARGE;
const int Vehicle::V_SUPER = VEHICLE_SUPER;

/****************** Enumeration ***********************************/
enum {
	M_MAIN,
	M_DISP_VEHICLES,
	M_DISP_COMMUTERS,
	M_SCH_ARRIVAL_TIME,
	M_SCH_MIN_VEHICLE,
	M_DISP_VEHICLE_NOT_FULL,
	M_DISP_REMAINING_COMMUTERS,
	M_DISP_SCH_PLAN,
	M_SAVE_SCH_PLAN,
	M_MODIFY_VEHICLES,
	M_MODIFY_COMMUTERS,
	M_MAX,
	M_EXIT = -1
}Menu;

/****************** Function Prototypes ***************************/
void m_welcome();
void m_readvhfile(vector <Vehicle>& vhList);
void m_readcmfile(vector <Commuter>& cmList);
int m_main();
int m_disp_vehicle(vector <Vehicle>& vhList);
int m_schByCommuter(vector <Schedule>& shList, vector <Vehicle>& vhList, vector <Commuter>& cmList);
int m_schByMinVeh(vector <Schedule>& shList, vector <Vehicle>& vhList, vector <Commuter>& cmList);
bool compVehDestDescendCap(Vehicle v1, Vehicle v2);
bool compComDestTime(Commuter c1, Commuter c2);

int m_disp_commuters(vector <Commuter>& cmList);
int m_disp_schedulingPlan(vector<Schedule>& shList);
int m_save_SchedulingPlan(vector<Schedule>& shList);
bool compVehDestAscTime(Vehicle v1, Vehicle v2);
int modify_menu();
int modifyList(vector<Vehicle>& vhList, vector<Commuter>& cmList, int x);
int modifyAddVehicle(vector<Vehicle>& vhList);
int modifyDeleteVehicle(vector<Vehicle>& vhList);
int modifyEditVehicle(vector<Vehicle>& vhList);
int modifyAddCommuter(vector<Commuter>& cmList);
int modifyDeleteCommuter(vector<Commuter>& cmList);
int modifyEditCommuter(vector<Commuter>& cmList);
void setTime(string &enteredTime, int& hour, int& min);
string pathf_vehicle;
string pathf_commuter;


/****************** Main ******************************************/
int main() {
	vector <Vehicle> vehList;				//original vehicle list
	vector <Vehicle> vehUnderCap;			//vehicles still under capacity
	vector <Commuter> comList;				//original commuter list
	vector <Commuter> comLeft;				//commuters left behind with no vehicle 
	vector <Schedule> schList;				//schedule list

	bool exit_menu = false;
	int choice = M_MAIN, x = 0;

	m_welcome();
	system("CLS");
	m_readvhfile(vehList);					//read vehicle file and update vector list
	m_readcmfile(comList);					//read commuter file and update vector list

	while (!exit_menu)
	{
		switch (choice) {
		case M_MAIN:
			choice = m_main();				//Go to main choice
			break;
		case M_DISP_VEHICLES:
			choice = m_disp_vehicle(vehList);
			break;
		case M_DISP_COMMUTERS:
			choice = m_disp_commuters(comList);
			break;
		case M_SCH_ARRIVAL_TIME:
			vehUnderCap = vehList;			//clone a copy of vehicle vector list
			comLeft = comList;				//clone a copy of commuter vector list
			schList.clear();				//empty previous schedule plan
			choice = m_schByCommuter(schList, vehUnderCap, comLeft);
			break;
		case M_SCH_MIN_VEHICLE:
			vehUnderCap = vehList;			//clone a copy of vehicle vector list
			comLeft = comList;				//clone a copy of commuter vector list
			schList.clear();				//empty previous schedule plan
			choice = m_schByMinVeh(schList, vehUnderCap, comLeft);		//start schedule action
			break;
		case M_DISP_VEHICLE_NOT_FULL:
			choice = m_disp_vehicle(vehUnderCap);
			break;
		case M_DISP_REMAINING_COMMUTERS:
			choice = m_disp_commuters(comLeft);
			break;
		case M_DISP_SCH_PLAN:
			choice = m_disp_schedulingPlan(schList);
			break;
		case M_SAVE_SCH_PLAN:
			choice = m_save_SchedulingPlan(schList);
			break;
		case M_MODIFY_VEHICLES:
			x = 0;
			choice = modifyList(vehList, comList, x);
			break;
		case M_MODIFY_COMMUTERS:
			x = 1;
			choice = modifyList(vehList, comList, x);
			break;
		case M_EXIT:			//exit path
		default:
			exit_menu = true;
			break;
		}
	}

	return 0;
}

//**********************************************************//
// Welcome
//**********************************************************//
HANDLE hout = GetStdHandle(STD_OUTPUT_HANDLE);
void m_welcome()
{
	//system("CLS");
	SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE);
	cout << "*****************************************************************" << endl;
	cout << "*****************************************************************" << endl;
	cout << "**                                                             **" << endl;
	cout << "**                       WELCOME                               **" << endl;
	cout << "**                                                             **" << endl;
	cout << "**                                                             **" << endl;
	cout << "*****************************************************************" << endl;
	cout << "*****************************************************************" << endl;
	SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
	//this_thread::sleep_for(chrono::seconds(2));		//Add delay to show on screen
	cout << "\n Enter folder/path-name for available Vehicles(press Enter for default) :" << "\n>";
	getline(cin, pathf_vehicle);
	cout << "\n Enter folder/path-name for available Commuters(press Enter for default) :" << "\n>";
	getline(cin, pathf_commuter);
	cout << "\n Enter any number key to return to main menu:" << endl << ">";
	cin.get();
}


//**********************************************************//
// Read Files
//**********************************************************//
void m_readvhfile(vector <Vehicle>& vhList) {
	ifstream inFile;
	string row[6];								//array of string
	string line, word;
	if (pathf_vehicle.length() == 0) {
		inFile.open("vehicle.txt");						//Read file - Using text file with 1 whitespace
	}
	else {
		inFile.open(pathf_vehicle);
		if (!inFile.is_open()) {
			SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_RED);
			cout << "Invalid file" << endl;
			SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
			return;
		}
	}
	//if (!inFile.is_open()) {
	//	cout << "File does not exist" << endl;
	//}
	//else if ((inFile >> ch).eof()) {
	//	cout << "File is empty";
	//}
	if (inFile.is_open()) {
		while (getline(inFile, line)) {				//check if end of file, and store into line
			stringstream s(line);					//break entire line up and store into stringstream s
			int i = 0;
			while (getline(s, word, ' ')) {			//check each stringstream s, and store into word, avoid blank
				row[i] = word;						//backup word into array of row
				i++;
			}
			stringstream time(row[3]);				//repeat to split time at row[3] with ':' delimiter
			while (getline(time, word, ':')) {
				row[i] = word;
				i++;
			}

			Vehicle tmpVeh;							//temporary class buffer
			tmpVeh.setName(row[0]);					//store Freight ID
			tmpVeh.setCapacity(stoi(row[1]));		//convert string to integer and store Capacity
			tmpVeh.setDestination(row[2]);			//store Destination
			tmpVeh.setHour(stoi(row[4]));			//convert string to integer and store
			tmpVeh.setMin(stoi(row[5]));			//convert string to integer and store
			vhList.push_back(tmpVeh);				//save temp buffer into vector list
		}
	}
	else {
		SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_RED);
		cout << "Invalid file please enter again." << endl;
		SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
		return;
	}

}

void m_readcmfile(vector <Commuter>& cmList) {
	ifstream inFile;
	string row[6];									//array of string
	string line, word;
	if (pathf_commuter.length() == 0) {
		inFile.open("commuter.txt");					//Read file - Using text file with 1 whitespace
	}
	else {
		inFile.open(pathf_commuter);
	}
	if (inFile.is_open()) {
		while (getline(inFile, line)) {				//check if end of file, and store into line
			stringstream s(line);					//break entire line up and store into stringstream s
			int i = 0;
			while (getline(s, word, ' ')) {			//check each stringstream s, and store into word, avoid blank
				row[i] = word;						//backup word into array of row
				i++;
			}
			stringstream time(row[2]);				//repeat to split time at row[3] with ':' delimiter
			while (getline(time, word, ':')) {
				row[i] = word;
				i++;
			}

			Commuter tmpCom;						//temporary class buffer
			tmpCom.setName(row[0]);					//store Freight ID
			tmpCom.setDestination(row[1]);			//store Destination
			tmpCom.setHour(stoi(row[3]));			//convert string to integer and store
			tmpCom.setMin(stoi(row[4]));			//convert string to integer and store
			cmList.push_back(tmpCom);				//save temp buffer into vector list
		}
	}
}

//**********************************************************//
// Main Menu
//**********************************************************//
int m_main() {
	int choice = M_MAIN;

	system("CLS");
	SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE);
	cout << "MAIN MENU:" << endl;
	SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
	cout << "1. Display all vehicles" << endl;
	cout << "2. Display all commuters" << endl;
	cout << "3. Schedule commuter by arrival time" << endl;
	cout << "4. Schedule commuter by minimum vehicles" << endl;
	cout << "5. Display vehicle that is under capacity" << endl;
	cout << "6. Display remaining commuters" << endl;
	cout << "7. Display scheduling plan" << endl;
	cout << "8. Save scheduling plan" << endl;
	cout << "9. Modify vehicle list" << endl;
	cout << "10. Modify commuter list" << endl << endl;
	cout << "Key -1 To End Program" << endl << endl;
	cout << "Please enter your option:\n>";

	cin >> choice;
	if ((choice > M_MAIN) && (choice < M_MAX)) {
		//do nothing, valid keep data
	}
	else if (choice == M_EXIT) {
		//do nothing, valid exit
	}
	else {
		SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_RED);
		cout << "Invalid options! Please try again!" << endl;
		SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
		choice = M_MAIN;
		this_thread::sleep_for(chrono::seconds(1));  // stay for 1 sec to show warning
	}

	cin.ignore();		//flush cin buffer
	return choice;
}

//**********************************************************//
// Menu Display Vehicle
//**********************************************************//
int m_disp_vehicle(vector <Vehicle>& vhList) {
	int choice = M_MAIN;

	system("CLS");
	SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE);
	cout << "Display All Available Vehicles:" << endl;
	cout << "-----------------------------------------------------------------------" << endl;
	SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);

	/* Title */
	cout << left << setw(15) << "Name";
	cout << left << setw(15) << "Destination";
	cout << left << setw(15) << "Time";
	cout << left << setw(15) << "Capacity" << endl;

	/* Display vehicle list */
	for (auto& v : vhList) {
		v.displayVehicle();
		cout << endl;
	}

	/* Ask User to return to Main Menu */
	cout << endl;
	cout << "Please enter any number key to return to main menu:\n>";

	if (cin.peek() != '\n') {
		cin >> choice;
	}
	cin.ignore();		//flush cin buffer
	choice = M_MAIN;	//overwrite choice 

	return choice;
}

int m_disp_commuters(vector <Commuter>& cmList) {
	int choice = M_MAIN;
	system("CLS");
	SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE);
	cout << "Display All Available Commuters:" << endl;
	cout << "-----------------------------------------------------------------------" << endl;
	SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);

	cout << left << setw(15) << "Name";
	cout << left << setw(15) << "Destination";
	cout << left << setw(15) << "Time" << endl;
	for (auto& v : cmList) {
		v.displayCommuter();
		cout << endl;
	}

	cout << endl;
	cout << "Please enter any number key to return to main menu:\n>";

	if (cin.peek() != '\n') {
		cin >> choice;
	}
	cin.ignore();		//flush cin buffer
	choice = M_MAIN;	//overwrite choice 

	return choice;
}

int m_disp_schedulingPlan(vector<Schedule>& shList) {
	int choice = M_MAIN;

	system("CLS");
	SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE);
	cout << "Display Schedule Plan:" << endl;
	cout << "---------------------------------------------------------------------------------------------------------------" << endl;
	SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);

	cout << left << setw(15) << "ID";
	cout << left << setw(15) << "Vehicle";
	cout << left << setw(15) << "Destination";
	cout << left << setw(15) << "Time";
	cout << left << setw(15) << "Capacity";
	cout << left << setw(15) << "Commuter";
	cout << left << setw(15) << "Destination";
	cout << left << setw(15) << "Time" << endl;

	for (auto& v : shList) {
		v.displaySchedule();
	}

	cout << endl;
	cout << "Please enter any number key to return to main menu:\n>";

	if (cin.peek() != '\n') {
		cin >> choice;
	}
	cin.ignore();		//flush cin buffer
	choice = M_MAIN;	//overwrite choice 

	return choice;
}

//**********************************************************//
// Menu Schedule by Arrival Time
//**********************************************************//

int m_schByCommuter(vector <Schedule>& shList, vector <Vehicle>& vhList, vector <Commuter>& cmList) {
	system("CLS");
	SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE);
	cout << "Display Schedule By Commuter:" << endl;
	cout << "-----------------------------------------------------------------------" << endl;
	SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);

	int choice = M_MAIN;
	int sid = 1;
	size_t i = 0, j = 0;

	sort(vhList.begin(), vhList.end(), compVehDestAscTime);
	sort(cmList.begin(), cmList.end(), compComDestTime);

	while (i < vhList.size()) {												// Loop vehicle list
		j = 0;																// reset back to beginning of commuter list
		while (j < cmList.size()) {											// Loop commuter list
			int c_hr = cmList[j].getHour();
			int c_min = cmList[j].getMin();
			if ((vhList[i].getDestination() == cmList[j].getDestination()) &&	// Destination matched,
				(vhList[i].getGrace(c_hr, c_min) == true) &&					// Within Grace period, 
				(vhList[i].getCapacity() > 0)) 									// Within vehicle Capacity
			{
				Schedule tmpSch(sid, vhList[i], cmList[j]);						// store tmp object data, call Schedule constructor			
				shList.push_back(tmpSch);										// save into schedule list

				sid++;
				vhList[i].decCapacity();										// decrement capacity
				cmList.erase(cmList.begin() + j);								// erase commuter element by index
				// cannot increase j counter, 
				// stay in the same position to do next check
			}
			else
				j++;															// go for next commuter element					
		}

		if (vhList[i].getCapacity() == 0) {										// check capacity reached zero
			SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_GREEN);
			cout << "Vehicle: " << vhList[i].getName() << " Capacity Full!" << endl;;
			SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
			vhList.erase(vhList.begin() + i);									// erase vehicle element by index
			// cannot increase i counter, 
			// stay in the same position to do next check
		}
		else
			i++;															// go for the next freight element
	}

	/* Display Schedule Plan */
	cout << endl;
	cout << "---------------------------------------------------------------------------------------------------------------" << endl;
	cout << left << setw(15) << "ID";
	cout << left << setw(15) << "Vehicle";
	cout << left << setw(15) << "Destination";
	cout << left << setw(15) << "Time";
	cout << left << setw(15) << "Capacity";
	cout << left << setw(15) << "Commuter";
	cout << left << setw(15) << "Destination";
	cout << left << setw(15) << "Time" << endl;

	for (auto& v : shList) {
		v.displaySchedule();
	}

	/* Ask User to return to Main Menu */
	cout << endl;
	cout << "Please enter any number key to return to main menu:\n>";

	if (cin.peek() != '\n') {
		cin >> choice;
	}
	cin.ignore();		//flush cin buffer
	choice = M_MAIN;	//overwrite choice 

	return choice;



}

//**********************************************************//
// Menu Schedule by Minimum Vehicle
//**********************************************************//
int m_schByMinVeh(vector <Schedule>& shList, vector <Vehicle>& vhList, vector <Commuter>& cmList) {

	system("CLS");
	SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE);
	cout << "Display Schedule By Minimum Vehicle:" << endl;
	cout << "-----------------------------------------------------------------------" << endl;
	SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);

	int choice = M_MAIN;
	int sid = 1;
	size_t i = 0, j = 0;

	sort(vhList.begin(), vhList.end(), compVehDestDescendCap);				// sort Vehicle, same destination, Descending capacity
	sort(cmList.begin(), cmList.end(), compComDestTime);					// sort Commuter, same destination, Ascending time

	while (i < vhList.size()) {												// Loop vehicle list
		j = 0;																// reset back to beginning of commuter list
		while (j < cmList.size()) {											// Loop commuter list
			int c_hr = cmList[j].getHour();
			int c_min = cmList[j].getMin();
			if ((vhList[i].getDestination() == cmList[j].getDestination()) &&	// Destination matched,
				(vhList[i].getGrace(c_hr, c_min) == true) &&					// Within Grace period, 
				(vhList[i].getCapacity() > 0)) 									// Within vehicle Capacity
			{
				Schedule tmpSch(sid, vhList[i], cmList[j]);						// store tmp object data, call Schedule constructor			
				shList.push_back(tmpSch);										// save into schedule list

				sid++;
				vhList[i].decCapacity();										// decrement capacity
				cmList.erase(cmList.begin() + j);								// erase commuter element by index
				// cannot increase j counter, 
				// stay in the same position to do next check
			}
			else
				j++;															// go for next commuter element					
		}

		if (vhList[i].getCapacity() == 0) {										// check capacity reached zero
			SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_GREEN);
			cout << "Vehicle: " << vhList[i].getName() << " Capacity Full!" << endl;;
			SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
			vhList.erase(vhList.begin() + i);									// erase vehicle element by index
			// cannot increase i counter, 
			// stay in the same position to do next check
		}
		else
			i++;															// go for the next freight element
	}

	/* Display Schedule Plan */
	cout << endl;
	cout << "---------------------------------------------------------------------------------------------------------------" << endl;
	cout << left << setw(15) << "ID";
	cout << left << setw(15) << "Vehicle";
	cout << left << setw(15) << "Destination";
	cout << left << setw(15) << "Time";
	cout << left << setw(15) << "Capacity";
	cout << left << setw(15) << "Commuter";
	cout << left << setw(15) << "Destination";
	cout << left << setw(15) << "Time" << endl;

	for (auto& v : shList) {
		v.displaySchedule();
	}

	/* Ask User to return to Main Menu */
	cout << endl;
	cout << "Please enter any number key to return to main menu:\n>";

	if (cin.peek() != '\n') {
		cin >> choice;
	}
	cin.ignore();		//flush cin buffer
	choice = M_MAIN;	//overwrite choice 

	return choice;


}
//**********************************************************//
// Menu Modify Vehicle
//**********************************************************//

int modifyList(vector<Vehicle>& vhList, vector<Commuter>& cmList, int x) {
	int choice = M_MAIN, choice2;
	cout << "Modify List " << endl;
	choice2 = modify_menu();
	if (choice2 == -1) {
		return choice2;
	}
	else if (x == 0) {
		//for Vehicle
		switch (choice2) {
		case 1:
			choice2 = modifyAddVehicle(vhList);
			break;
		case 2:
			choice2 = modifyDeleteVehicle(vhList);
			break;
		case 3:
			choice2 = modifyEditVehicle(vhList);
			break;
		case 4:
			choice2 = 0;
			break;
		default:
			break;
		}

	}
	else if (x == 1) {
		//for Commuter
		switch (choice2) {
		case 1:
			choice2 = modifyAddCommuter(cmList);
			break;
		case 2:
			choice2 = modifyDeleteCommuter(cmList);
			break;
		case 3:
			choice2 = modifyEditCommuter(cmList);
			break;
		case 4:
			choice2 = 0;
			break;
		default:
			break;
		}
	}
	return choice2;
}

int modifyAddVehicle(vector<Vehicle>& vhList) {
	system("CLS");
	string name, dest, line, vehTime, word;
	string row[3];								//array of string
	int hour, min, choice;
	int newCap = 0;
	int check = 0;
	string key;
	int i = 0;


	SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE);
	cout << "Add Vehicle: " << endl;
	cout << "-----------------------------------------------------------------------" << endl;
	SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);

	cout << "\n Vehicle name (eg. V01 : V36) : ";
	cin >> name;
	for (size_t x = 0; x < vhList.size() + 1; x++)
	{
		if (x == vhList.size())
		{
			break;
			
		}
		if (name == vhList[x].getName())
		{
			cout << "Vehicle already exist,type in a new vehicle name" << endl;
			cout << "\n Vehicle name (eg. V01 : V36) : ";
			cin >> name;
			x = 0;
		}


	}
	while (check == 0)
	{
		cout << "\nEnter vehicle capacity in number, small(2) , large(4), super(6) : ";
		cin >> key;
		if ((key == "2") || (key == "4") || (key == "6"))
		{
			newCap = stoi(key);
			check++;
		}
		if ((key == "small") || (key == "Small") || (key == "SMALL") || (newCap == VEHICLE_SMALL))
		{
			newCap = VEHICLE_SMALL;
			check++;
		}
		else if ((key == "large") || (key == "Large") || (key == "LARGE") || (newCap == VEHICLE_LARGE))
		{
			newCap = VEHICLE_LARGE;
			check++;
		}
		else if ((key == "super") || (key == "Super") || (key == "SUPER") || (newCap == VEHICLE_SUPER))
		{
			newCap = VEHICLE_SUPER;
			check++;
		}
		if (check == 0)
		{
			cout << "\nInvalid capacity key!" << endl;
		}
	}

	cout << "\nAvailable Destination: " << endl;
	cout << "Aljunied\n" << "Bedok\n" << "Bishan\n"
		<< "Changi\n" << "ChuaChuKang\n" << "Holland\n"
		<< "Jurong\n" << "Marine Parade\n" << "Punggol\n"
		<< "ToaPayoh\n" << "Tampines\n" << "Yishun\n" << "WestCoast" << endl;
	cout << "\nEnter vehicle destination in string : ";
	cin >> dest;
	cout << "\nEnter Vehicle time in hh:mm : ";
	cin >> vehTime;
	setTime(vehTime, hour, min);
	Vehicle tmpVeh;							//temporary class buffer
	tmpVeh.setName(name);					//store Vehicle ID
	tmpVeh.setCapacity(newCap);				//convert string to integer and store Capacity
	tmpVeh.setDestination(dest);			//store Destination
	tmpVeh.setHour(hour);					//convert string to integer and store
	tmpVeh.setMin(min);						//convert string to integer and store
	vhList.push_back(tmpVeh);				//save temp buffer into vector list

	SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_GREEN);
	cout << "\nSuccesfully added Vehicle!" << endl;
	SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);

	cout << "========================================================================" << endl;
	cout << left << setw(15) << "Vehicle ID";
	cout << left << setw(15) << "Destination";
	cout << left << setw(15) << "Time";
	cout << left << setw(15) << "Capacity" << endl;

	cout << left << setw(15) << name;
	cout << left << setw(15) << dest;
	cout << left << hour << ":" << setw(20) << min;
	cout << left << setw(15) << newCap << endl;
	cout << "========================================================================" << endl;

	cout << "\n\nRETURN" << endl;
	cout << "-----------------------------------------------------------------------" << endl;
	cout << "1.Back to Previous Menu" << endl;
	cout << "2.Back to Main Menu" << endl;
	cin >> choice;
	if (choice == 1) {
		return 9;
	}
	else if (choice == 2) {
		return 1;
	}
	else {
		SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_RED);
		cout << "\nInvalid option Returning to Main Menu";
		SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
		this_thread::sleep_for(chrono::seconds(1));  // stay for 1 sec to show warning
		return 1;
	}
	return choice;

}

int modifyDeleteVehicle(vector<Vehicle>& vhList) {
	system("CLS");
	string name, dest;
	int hour, min, cap;
	int choice, flag = 1;
	size_t i = 0;

	SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE);
	cout << "Delete Vehicle: " << endl;
	cout << "-----------------------------------------------------------------------" << endl;
	SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);

	cout << "\n Please enter the Vehicle name to be deleted (eg. V01 : V36) : ";
	cin >> name;

	while (i < vhList.size()) {
		if (vhList[i].getName() == name) {
			SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_GREEN);
			cout << "\nVehicle Found" << endl;
			SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
			dest = vhList[i].getDestination();
			hour = vhList[i].getHour();
			min = vhList[i].getMin();
			cap = vhList[i].getCapacity();
			vhList.erase(vhList.begin() + i);

			SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_RED);
			cout << "\nVehicle Name: " << name << " deleted from the current vehicle list!" << endl;
			SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
			cout << "========================================================================" << endl;

			cout << left << setw(15) << "Vehicle ID";
			cout << left << setw(15) << "Destination";
			cout << left << setw(15) << "Time";
			cout << left << setw(15) << "Capacity" << endl;

			cout << left << setw(15) << name;
			cout << left << setw(15) << dest;
			cout << right << setw(2) << setfill('0') << hour << ':';
			cout << right << setw(2) << setfill('0') << min << left << setw(10) << setfill(' ') << ' ';;
			cout << left << cap << setw(15) << endl;
			cout << "========================================================================" << endl;
			flag = 0;
			break;
		}
		i++;
	}
	if (flag == 1) {
		SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_RED);
		cout << "\nVehicle was not found. ";
		SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
	}

	cout << "\n\nRETURN" << endl;
	cout << "-----------------------------------------------------------------------" << endl;
	cout << "1.Back to Previous Menu" << endl;
	cout << "2.Back to Main Menu" << endl;
	cin >> choice;
	if (choice == 1) {
		return 9;
	}
	else if (choice == 2) {
		return 1;
	}
	else {
		SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_RED);
		cout << "\nInvalid option Returning to Main Menu";
		SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
		this_thread::sleep_for(chrono::seconds(1));  // stay for 1 sec to show warning
		return 1;
	}
	return choice;
}


int modifyEditVehicle(vector<Vehicle>& vhList) {
	system("CLS");
	string name, newName, dest, newDest, newvehTime, key;
	string row[3], word;
	int hour = 0, min = 0, cap = 0, newCap = 0;
	size_t i = -1;
	int choice, flag = 1, choice2 = 0, check = 0;


	SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE);
	cout << "Edit Vehicle: " << endl;
	SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);

	cout << "-----------------------------------------------------------------------" << endl;
	string vehiclechange;
	cout << "\n Please enter the Vehicle name to be edited (eg. V01 to V36) : ";
	cin >> vehiclechange;
	int vehiclepos = 0;

	for (size_t x = 0; x < vhList.size() + 1; x++)
	{
		if (x == vhList.size())
		{
			SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_RED);
			cout << "No Vehicle Name found! Do check if the correct vehicle is keyed in,caps do matter" << endl;
			SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
			cout << "Please enter the Vehicle name to be edited (eg. V01 to V36) : ";
			cin >> vehiclechange;
			x = 0;
		}
		if (vehiclechange == vhList[x].getName())
		{
			vehiclepos = x;
			break;
		}


	}

	while (i < vhList.size()) {
		i += 1;
		flag = vhList[i].getName() == name;
		if (flag) {
			SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_RED);
			cout << "\nVehicle Found";
			SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
			cout << "\n========================================================================" << endl;
			cout << left << setw(15) << "Vehicle ID";
			cout << left << setw(15) << "Destination";
			cout << left << setw(15) << "Time";
			cout << left << setw(15) << "Capacity" << endl;

			cout << left << setw(15) << vhList[i].getName();
			cout << left << setw(15) << vhList[i].getDestination();
			cout << left << vhList[i].getHour() << ":" << setw(20) << vhList[i].getMin();
			cout << left << setw(15) << vhList[i].getCapacity() << endl;
			cout << "\n========================================================================" << endl;
			break;
		}

	}
	while (choice2 != 9) {
		SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE);
		cout << "Edit Vehicle Items: " << endl;
		SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
		cout << "-----------------------------------------------------------------------" << endl;
		cout << "1. Vehicle Name" << endl;
		cout << "2. Capacity" << endl;
		cout << "3. Destination" << endl;
		cout << "4. Recharging Time" << endl;
		cout << "5. Back to previous menu" << endl;
		cout << "6. Back to Main menu" << endl;
		cout << "\n Please enter your option: ";
		cin >> choice2;
		switch (choice2) {
		case 1:
		{

			cout << "\nCurrrent vehicle name: " << vhList[vehiclepos].getName();
			cout << "\nEnter New vehicle name: ";
			cin >> newName;
			SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_GREEN);
			cout << "\nVehicle Name: " << vhList[vehiclepos].getName() << " is changed to " << newName << " in vehicle list!" << endl;
			SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
			vhList[vehiclepos].setName(newName);
			break;
		}
		case 2:
		{
			check = 0;


			cap = vhList[vehiclepos].getCapacity();

			while (check != 2)
			{
				cout << "\nCurrrent vehicle Capacity: " << vhList[vehiclepos].getCapacity();
				cout << "\nEnter New vehicle Capacity(2 for small,4 for large,6 for super): ";
				cin >> key;

				if ((key == "2") || (key == "4") || (key == "6"))
				{
					newCap = stoi(key);
				}
				
				if ((key == "small") || (key == "Small") || (key == "SMALL") || (newCap == VEHICLE_SMALL))
				{
					check++;
					newCap = VEHICLE_SMALL;
				}
				else if ((key == "large") || (key == "Large") || (key == "LARGE") || (newCap == VEHICLE_LARGE))
				{
					check++;
					newCap = VEHICLE_LARGE;
				}
				else if ((key == "super") || (key == "Super") || (key == "SUPER") || (newCap == VEHICLE_SUPER))
				{
					check++;
					newCap = VEHICLE_SUPER;
				}
				//cout << "newcap is : " << newCap << " Cap is : " << cap << endl;
				if (newCap != cap)
				{

					check++;
				}
				//cout << "check 2: " << check<<endl;
				if (check != 2)
				{
					check = 0;
					SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_RED);
					cout << "\nInvalid Capacity,Must be 2,4,6 or small,large,super and must not be the same as Current Capacity" << endl;
					SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
				}

			}

			SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_GREEN);
			cout << "\nVehicle Capacity: " << cap << " is changed to " << newCap << " in vehicle list!" << endl;
			SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
			vhList[vehiclepos].setCapacity(newCap);
			break;
		}
		case 3:
			cout << "\nCurrrent vehicle Destination: " << vhList[vehiclepos].getDestination();
			cout << "\nEnter New vehicle Destination: ";
			cin >> newDest;

			SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_GREEN);
			cout << "\nVehicle Destination: " << vhList[vehiclepos].getDestination() << " is changed to " << newDest << " in vehicle list!" << endl;
			SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);

			vhList[vehiclepos].setDestination(newDest);
			break;
		case 4:
			cout << "\nCurrent vehicle Time:" << right << setw(2) << setfill('0') << vhList[vehiclepos].getHour() << ":";
			cout << right << setw(2) << setfill('0') << vhList[vehiclepos].getMin() << left << setw(10) << setfill(' ') << ' ';
			cout << "\nEnter Vehicle time in hh:mm : ";
			cin >> newvehTime;

			setTime(newvehTime, hour, min);

			SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_GREEN);
			cout << "\nVehicle time: " << right << setw(2) << setfill('0') << vhList[vehiclepos].getHour() << ":";
			cout << right << setw(2) << setfill('0') << vhList[vehiclepos].getMin() << left << setw(2) << setfill(' ') << " is changed to " << newvehTime << " in vehicle list!" << endl;
			SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);

			vhList[vehiclepos].setHour(hour);
			vhList[vehiclepos].setMin(min);
			break;
		case 5:
			choice2 = 9;
			choice = 9;
			break;
		case 6:
		default:
			choice2 = 9;
			choice = 1;
			break;
		}
	}
	cout << "\n========================================================================" << endl;
	cout << left << setw(15) << "Vehicle ID";
	cout << left << setw(15) << "Destination";
	cout << left << setw(15) << "Time";
	cout << left << setw(15) << "Capacity" << endl;

	cout << left << setw(15) << name;
	cout << left << setw(15) << dest;
	cout << right << setw(2) << setfill('0') << hour << ':';
	cout << right << setw(2) << setfill('0') << min << left << setw(10) << setfill(' ') << ' ';
	cout << left << setw(15) << cap << endl;
	cout << "\n========================================================================" << endl;
	return choice;
}
// Split entered time hh:mm to hour and nmin //

void setTime(string &enteredTime, int& hour, int& min) {
	
	string row[3], word;
	size_t i = 0;

	for (int j = 0; enteredTime.find(":") != 2; cin >> enteredTime) {
		SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_RED);
		cout << "Invalid format!" << endl;
		SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
		cout << "\nPlease enter in the format HH:MM  : ";
	
		i = 0;
	}

	stringstream time(enteredTime);	//repeat to split time at row[3] with ':' delimiter
	while (getline(time, word, ':')) {
		row[i] = word;
		i++;
	}
	hour = stoi(row[0]);
	min = stoi(row[1]);
}

int modifyAddCommuter(vector<Commuter>& cmList) {
	system("CLS");
	string name, dest, line, comTime, word;
	string row[3];								//array of string
	int hour, min, choice;
	int i = 0;

	SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE);
	cout << "Add Commuter: " << endl;
	cout << "-----------------------------------------------------------------------" << endl;
	SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);

	cout << "\n Commuter name (eg. C01 : C36) : ";
	cin >> name;
	cout << "\nAvailable Destination: " << endl;
	cout << "Aljunied\n" << "Bedok\n" << "Bishan\n"
		<< "Changi\n" << "ChuaChuKang\n" << "Holland\n"
		<< "Jurong\n" << "Marine Parade\n" << "Punggol\n"
		<< "ToaPayoh\n" << "Tampines\n" << "Yishun\n" << "WestCoast" << endl;
	cout << "\nEnter commuter destination in string : ";
	cin >> dest;
	cout << "\nEnter commuter time in hh:mm : ";
	cin >> comTime;
	setTime(comTime, hour, min);

	Commuter tmpCom;							//temporary class buffer
	tmpCom.setName(name);					//store Vehicle ID
	tmpCom.setDestination(dest);			//store Destination
	tmpCom.setHour(hour);					//convert string to integer and store
	tmpCom.setMin(min);						//convert string to integer and store
	cmList.push_back(tmpCom);				//save temp buffer into vector list

	SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_GREEN);
	cout << "\nSuccesfully added Commuter!" << endl;
	SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
	cout << "========================================================================" << endl;
	cout << left << setw(15) << "Commuter ID";
	cout << left << setw(15) << "Destination";
	cout << left << setw(15) << "Time" << endl;

	cout << left << setw(15) << name;
	cout << left << setw(15) << dest;
	cout << left << hour << ":" << setw(15) << min << endl;
	cout << "========================================================================" << endl;

	cout << "\n\nRETURN" << endl;
	cout << "-----------------------------------------------------------------------" << endl;
	cout << "1.Back to Previous Menu" << endl;
	cout << "2.Back to Main Menu" << endl;
	cin >> choice;
	if (choice == 1) {
		return 9;
	}
	else if (choice == 2) {
		return 1;
	}
	else {
		SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_RED);
		cout << "\nInvalid option Returning to Main Menu";
		SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
		this_thread::sleep_for(chrono::seconds(1));  // stay for 1 sec to show warning
		return 1;
	}
	return choice;

}
int modifyDeleteCommuter(vector<Commuter>& cmList) {
	system("CLS");
	string name, dest;
	int hour, min;
	int choice, flag = 1;
	size_t i = 0;

	SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE);
	cout << "Delete Commuter: " << endl;
	cout << "-----------------------------------------------------------------------" << endl;
	SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);

	cout << "\n Please enter the Commuter name to be deleted (eg. C01 : C36) : ";
	cin >> name;

	while (i < cmList.size()) {
		if (cmList[i].getName() == name) {
			SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_GREEN);
			cout << "\nCommuter Found" << endl;
			SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
			dest = cmList[i].getDestination();
			hour = cmList[i].getHour();
			min = cmList[i].getMin();
			cmList.erase(cmList.begin() + i);

			SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_RED);
			cout << "\n Commuter Name: " << name << " deleted from the current Commuter list!" << endl;
			SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);

			cout << "========================================================================" << endl;
			cout << left << setw(15) << "Commuter ID";
			cout << left << setw(15) << "Destination";
			cout << left << setw(15) << "Time" << endl;

			cout << left << setw(15) << name;
			cout << left << setw(15) << dest;
			cout << left << hour << ":" << setw(15) << min << endl;
			cout << "========================================================================" << endl;
			flag = 0;
			break;
		}
		i++;
	}
	if (flag == 1) {
		SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_RED);
		cout << "\nCommuter was not found. ";
		SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
	}

	cout << "\n\nRETURN" << endl;
	cout << "-----------------------------------------------------------------------" << endl;
	cout << "1.Back to Previous Menu" << endl;
	cout << "2.Back to Main Menu" << endl;
	cin >> choice;
	if (choice == 1) {
		return 9;
	}
	else if (choice == 2) {
		return 1;
	}
	else {
		SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_RED);
		cout << "\nInvalid option Returning to Main Menu";
		SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
		this_thread::sleep_for(chrono::seconds(1));  // stay for 1 sec to show warning
		return 1;
	}
	return choice;
}
int modifyEditCommuter(vector<Commuter>& cmList) {
	system("CLS");
	string name, newName, dest, newDest, newcomTime;
	string row[3], word;
	int hour = 0, min = 0;
	int choice, flag = 1, choice2 = 0;
	size_t i = -1;
	SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE);
	cout << "Edit Commuter: " << endl;
	SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
	cout << "-----------------------------------------------------------------------" << endl;
	string commuterchange;
	cout << "\n Please enter the Commuter name to be edited (eg. C01 to C36) : ";
	cin >> commuterchange;
	int commuterpos = 0;

	for (size_t x = 0; x < cmList.size() + 1; x++)
	{
		if (x == cmList.size())
		{
			SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_RED);
			cout << "No Commuter name found! Do check if the correct Commuter is keyed in,caps do matter" << endl;
			SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
			cout << "Please enter the Commuter name to be edited (eg. C01 to C36) : ";
			cin >> commuterchange;
			x = 0;
		}
		if (commuterchange == cmList[x].getName())
		{
			commuterpos = x;
			break;
		}

	}
	while (i < cmList.size()) {
		i += 1;
		if (cmList[i].getName() == name) {
			SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_GREEN);
			cout << "\nCommuter Found";
			SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
			cout << "\n========================================================================" << endl;
			cout << left << setw(15) << "Commuter ID";
			cout << left << setw(15) << "Destination";
			cout << left << setw(15) << "Time";

			cout << left << setw(15) << cmList[i].getName();
			cout << left << setw(15) << cmList[i].getDestination();
			cout << left << cmList[i].getHour() << ":" << setw(20) << cmList[i].getMin();
			cout << "\n========================================================================" << endl;
			break;
		}

	}
	while (choice2 != 10) {
		SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE);
		cout << "Edit Commuter Items: " << endl;
		SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
		cout << "-----------------------------------------------------------------------" << endl;
		cout << "1. Commuter Name" << endl;
		cout << "2. Destination" << endl;
		cout << "3. Edit Commuter Time" << endl;
		cout << "4. Back to previous menu" << endl;
		cout << "5. Back to Main menu" << endl;
		cout << "\n Please enter your option: ";
		cin >> choice2;
		switch (choice2) {
		case 1:
			cout << "\nCurrrent Commuter name: " << cmList[commuterpos].getName();
			cout << "\nEnter New Commuter name: ";
			cin >> newName;
			SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_GREEN);
			cout << "\nCommuter Name: " << cmList[commuterpos].getName() << " is changed to " << newName << " in commuter list!" << endl;
			SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
			cmList[commuterpos].setName(newName);

			break;
		case 2:
			cout << "\nCurrrent commuter Destination: " << cmList[i].getDestination();
			cout << "\nEnter New commuter Destination: ";
			cin >> newDest;
			SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_GREEN);
			cout << "\nCommuter Destination: " << cmList[commuterpos].getDestination() << " is changed to " << newDest << " in commuter list!" << endl;
			SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
			cmList[commuterpos].setDestination(newDest);
			break;
		case 3:
			cout << "\nCurrent commuter Time:" << setfill('0') << setw(2) << cmList[commuterpos].getHour() << ":" << cmList[commuterpos].getMin();
			cout << "\nEnter Commuter time in hh:mm : ";
			cin >> newcomTime;

			setTime(newcomTime, hour, min);

			SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_GREEN);
			cout << "\nCommuter time: " << setfill('0') << setw(2) << cmList[commuterpos].getHour() << ":" << cmList[commuterpos].getMin() << " is changed to " << newcomTime << " in commuter list!" << endl;
			SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);

			cmList[commuterpos].setHour(hour);
			cmList[commuterpos].setMin(min);
			break;
		case 4:
			choice2 = 10;
			choice = 10;
			break;
		default:
			choice2 = 10;
			choice = 1;
			break;
		}
	}
	cout << "\n========================================================================" << endl;
	cout << left << setw(15) << "Vehicle ID";
	cout << left << setw(15) << "Destination";
	cout << left << setw(15) << "Time" << endl;

	cout << left << setw(15) << name;
	cout << left << setw(15) << dest;
	cout << left << hour << ":" << setw(20) << min << endl;
	cout << "\n========================================================================" << endl;
	return choice;
}
// Split entered time hh:mm to hour and nmin //

int modify_menu() {
	int choice2 = 0;
	system("CLS");
	cout << "-----------------------------------------------------------------------" << endl;
	cout << "1. Add" << endl;
	cout << "2. Delete" << endl;
	cout << "3. Edit" << endl;
	cout << "4. Back to Main Menu" << endl;
	cout << "Key -1 to End Program" << endl;
	cout << "Please enter your option: ";
	cin >> choice2;
	return choice2;
}


//**********************************************************//
// Save Scheduling Plan
//**********************************************************//
int m_save_SchedulingPlan(vector<Schedule>& shList) {
	int choice = M_MAIN;
	string filename;
	system("CLS");
	SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE);
	cout << "Save Schedule Plan:" << endl;
	cout << "-----------------------------------------------------------------------" << endl;
	SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
	//cout << "\nEnter File Name: ";
	//cin >> filename;
	ofstream ofile;

	int i = 0;

	if (shList.size() == 0) {
		SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_RED);
		cout << "\nSchedule  has not been planned" << endl;
		SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
	}
	else {
		ofile.open("Schedule.txt");

		ofile << "Schedule Plan:" << endl;
		ofile << "---------------------------------------------------------------------------------------------------------------" << endl;

		ofile << left << setw(15) << "ID";
		ofile << left << setw(15) << "Vehicle";
		ofile << left << setw(15) << "Destination";
		ofile << left << setw(15) << "Time";
		ofile << left << setw(15) << "Capacity";
		ofile << left << setw(15) << "Commuter";
		ofile << left << setw(15) << "Destination";
		ofile << left << setw(15) << "Time" << endl;

		if (ofile) {
			for (auto& v : shList) {
				ofile << v << endl;
			}
			SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_GREEN);
			cout << "\nSuccesfully saved file : " << "Schedule.txt" << endl;
			SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
		}
		else {
			cout << "\nError Opening Schedule.txt File" << endl;
		}
	}
	ofile.close();


	/* Ask User to return to Main Menu */
	cout << "Please enter any number key to return to main menu:\n>";

	if (cin.peek() != '\n') {
		cin >> choice;
	}
	cin.ignore();		//flush cin buffer
	choice = M_MAIN;	//overwrite choice 

	return choice;

}

/**************************************************************************************
  User-Customizable Comparison Function for Vehicle List

  1. If destinationis different, sort in ascending destination (in alphabetical order)
  2. If destination is same, sort in descending capacity (Large to Small)

***************************************************************************************/
bool compVehDestDescendCap(Vehicle v1, Vehicle v2)
{
	if (v1.getDestination() != v2.getDestination())
		return (v1.getDestination() < v2.getDestination());
	return (v1.getCapacity() > v2.getCapacity());
}

/**************************************************************************************
  User-Customizable Comparison Function for Commuter List

  1. If destinationis different, sort in ascending destination (in alphabetical order)
  2. If destination is same, sort in ascending time (Small to Large)

***************************************************************************************/
bool compComDestTime(Commuter c1, Commuter c2) {
	if (c1.getDestination() != c2.getDestination())
		return (c1.getDestination() < c2.getDestination());
	return (c1.getTime() < c2.getTime());
}

/**************************************************************************************
  User-Customizable Comparison Function for Vehicle List

  1. If destinationis different, sort in ascending destination (in alphabetical order)
  2. If destination is same, sort in ascending time (Small to Large)

***************************************************************************************/

bool compVehDestAscTime(Vehicle v1, Vehicle v2) {
	if (v1.getDestination() != v2.getDestination())
		return (v1.getDestination() < v2.getDestination());
	return (v1.getTime() < v2.getTime());
}