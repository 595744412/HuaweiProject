#pragma once
#include "definition.h"
#include "Server.h"
#include "DataManager.h"

DataManager dataManager;
int main()
{
	dataManager.ReadAll();
	Server a(dataManager.serverTypeList["host0Y6DP"]);
	cout << 1;
}
