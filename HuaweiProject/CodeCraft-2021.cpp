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
#if isVisual
	dataManager.OutputAll();
	dataManager.OutputVisual();
#else
	dataManager.OutputAll();
#endif
	return 0;
}
