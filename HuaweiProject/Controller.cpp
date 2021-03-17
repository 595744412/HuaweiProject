#include "Controller.h"
#include "DataManager.h"
#include "Server.h"

Server& Controller::PurchaseServer(string type)
{
	Server server(dataManager.serverTypeList[type]);
	dataManager.serverList.emplace_back(server);
	usedServerList.emplace_back(server.GetID());
	return dataManager.serverList[server.GetID()];
}

void Controller::Init()
{
	//���������Ͱ��˴�����������б�
	for (auto j = dataManager.serverTypeList.cbegin(); j != dataManager.serverTypeList.cend(); j++) {
		if (ratioServerList.size() == 0) {
			//�б�Ϊ��
			ratioServerList.emplace_back(j->second);
		}
		else {
			//���������
			int a = 0, b = ratioServerList.size();
			while (a != b) {
				int k = (a + b) / 2;
				if (ratioServerList[k].ratio == j->second.ratio) {
					a = b = k;
				}
				else if (ratioServerList[k].ratio > j->second.ratio) {
					b = k;
				}
				else {
					a = k + 1;
				}
			}
			ratioServerList.insert(ratioServerList.begin() + a, j->second);
		}
	}
	//���˴�Ƚ������������������ӳ��
	for (auto j = dataManager.vmwareTypeList.cbegin(); j != dataManager.vmwareTypeList.cend(); j++) {
		float ratio = j->second.ratio;
		unsigned int cores = j->second.cores, memory = j->second.memory;
		if (ratio > 1) {
			int i = ratioServerList.size() - 1;
			while (ratioServerList[i].ratio > ratio || ratioServerList[i].cores < cores || ratioServerList[i].memory < memory) {
				i--;
			}
			vmwareToServer[j->first] = ratioServerList[i].name;
		}
		else {
			int i = 0;
			while (ratioServerList[i].ratio < ratio || ratioServerList[i].cores < cores || ratioServerList[i].memory < memory) {
				i++;
			}
			vmwareToServer[j->first] = ratioServerList[i].name;
		}
	}
}

void Controller::CreateList()
{
	//��һ��
	Init();
	for (unsigned int i = 0; i < dataManager.dayCounts; i++) {
		//��i��
		unsigned int purchaseCount = 0;
		for (unsigned int j = 0; j < dataManager.requestList[i].size(); j++) {
			//��j������
			RequestType request = dataManager.requestList[i][j];
			if (request.isAdd) {
				bool success = false;
				for (unsigned int k = 0; k < usedServerList.size(); k++) {
					//��k̨���÷�����
					Server& server = dataManager.serverList[usedServerList[k]];
					if (server.AddVmware(request.ID, true) || server.AddVmware(request.ID, false)) {
						//��ӷ������仯���б�
#if isVisual
						dataManager.changeList[i].emplace_back(server);
#endif
						success = true;
						break;
					}
					if (server.GetA().unusedCores <= leastCore || server.GetA().unusedMemory <= leastMemory) {
						if (server.GetB().unusedCores <= leastCore || server.GetB().unusedMemory <= leastMemory) {
							usedServerList.erase(usedServerList.begin() + k);
							k--;
						}
					}
				}
				if (!success) {
					purchaseCount++;
					string name = vmwareToServer[request.name];
					dataManager.purchaseList[i][name] += 1;
					Server& server = PurchaseServer(name);
					server.AddVmware(request.ID, true);
					//����������������б�
#if isVisual
					dataManager.newList[i].emplace_back(server);
#endif

				}
				AddData it = { dataManager.vmwareList[request.ID].serverID ,dataManager.vmwareList[request.ID].myType.isDouble,dataManager.vmwareList[request.ID].isNodeA };
				Server server = dataManager.serverList[dataManager.vmwareList[request.ID].serverID];
				dataManager.addList[i].emplace_back(it);
			}
			else {
				Vmware& vmware = dataManager.vmwareList[request.ID];
				Server& server = dataManager.serverList[vmware.serverID];
				server.DeleteVmware(request.ID);
				//��ӷ������仯���б�
#if isVisual
				dataManager.changeList[i].emplace_back(server);
#endif
				usedServerList.emplace_back(server.GetID());
			}
		}
		//����˳������������ӳ��
		unsigned int newID = Server::GetCount() - purchaseCount;
		unordered_map<string, unsigned int> purchaseIDList;
		for (auto j = dataManager.purchaseList[i].cbegin(); j != dataManager.purchaseList[i].cend(); j++) {
			purchaseIDList[j->first] = newID;
			newID += j->second;
		}
		for (unsigned int j = Server::GetCount() - purchaseCount; j < Server::GetCount(); j++) {
			string name = dataManager.serverList[j].GetServerType().name;
			dataManager.serverIDList[j] = purchaseIDList[name];
			purchaseIDList[name]++;
		}
	}
}
