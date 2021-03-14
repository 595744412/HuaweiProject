#pragma once
#include "definition.h"
#include "Server.h"
#include "DataManager.h"

DataManager dataManager;
int main()
{
	dataManager.ReadAll();
	cout << 1;
}
