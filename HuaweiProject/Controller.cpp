#include "Controller.h"
#include "DataManager.h"
#include "Server.h"

unsigned int Controller::PurchaseServer(ServerType type)
{
	Server server(type);
	return 0;
}

void Controller::CreateList()
{
	//��һ��
	for (int i = 0; i < dataManager.dayCounts; i++) {
		//��i��
		for (int j = 0; j < dataManager.requestList[i].size(); j++) {
			//��j������
			RequestType request = dataManager.requestList[i][j];
			if (request.isAdd) {
				for (int k = 0; k < dataManager.serverList.size(); j++) {
					//��k̨���������
					Server& server = dataManager.serverList[i];
					if (server.AddVmware(request.ID,true)) {

					}
					else if (server.AddVmware(request.ID, false)) {

					}
					else {
						dataManager.purchaseList[i]["host0Y6DP"];
					}
				}
			}
			else {

			}
		}
	}
}
