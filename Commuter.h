#pragma once
#include "Logistic.h"
class Commuter : public Logistic
{
public:
	Commuter();
	Commuter(string name, string destination, int hour, int min);
	void displayCommuter();
};


