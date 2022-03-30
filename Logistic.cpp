#include <iostream>
#include <string>
#include <iomanip>  //for set() and setfill()
using namespace std;

#include "Logistic.h"

Logistic::Logistic(string name, string destination, int hour, int min)
{
	this->name = name;
	this->destination = destination;
	this->hour = hour;
	this->min = min;
}

void Logistic::setName(string name) {
	this->name = name;
}
string Logistic::getName() {
	return name;
}
void Logistic::setDestination(string destination) {
	this->destination = destination;
}
string Logistic::getDestination() {
	return destination;
}
void Logistic::setHour(int hour) {
	this->hour = hour;
}
int Logistic::getHour() {
	return hour;
}
void Logistic::setMin(int min) {
	this->min = min;
}
int Logistic::getMin() {
	return min;
}
double Logistic::getTime() {
	double time = (double)(hour)+(double)(min / 60);
	return time;
}

bool Logistic::getGrace(int ohour, int omin) {
	double obj = (double)(ohour)+(double)(omin / 60.0);
	double rhs = (double)(hour)+(double)(min / 60.0);
	double diff = obj - rhs;				//Commuter time - Vehicle time, arrival earlier
	if ((diff >= 0) && (diff <= 30.0 / 60.0))
		return true;
	else
		return false;
}
void Logistic::displayLogistic() {
	cout << left << setw(15) << name;
	cout << left << setw(15) << destination;
	cout << right << setw(2) << setfill('0') << hour << ':'; //2 spaces + 1 space for colon
	cout << right << setw(2) << setfill('0') << min;		 //2 spaces
	cout << left << setw(10) << setfill(' ') << ' ';		 //arrange 10 space more
}
