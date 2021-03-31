#include "definition.h"
#include "Server.h"
#include "DataManager.h"
#include "Controller.h"


DataManager dataManager;
int main()
{
	dataManager.initRead();
#if isVisual
	cout << "read done" << endl;
#endif
	Controller controller = Controller();
	controller.CreateList();
#if isVisual
	dataManager.OutputVisual();
#endif
	return 0;
}
