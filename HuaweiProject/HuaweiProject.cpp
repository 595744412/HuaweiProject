#pragma once
#include "definition.h"

DataManager dataManager;

int main()
{
	dataManager.ReadAll();
	Server a(dataManager.serverTypeList["host0Y6DP"]);
}
