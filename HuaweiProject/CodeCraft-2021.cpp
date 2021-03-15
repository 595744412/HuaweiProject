#pragma once
#include "definition.h"
#include "Server.h"
#include "DataManager.h"
#include "Controller.h"

DataManager dataManager;
int main()
{
	dataManager.ReadAll();
	Controller controller = Controller();
	controller.CreateList();
	dataManager.OutputAll();
}
