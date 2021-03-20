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

//��ʼ��
void Controller::init(void)
{ 
	dataManager.init(dataManager.dayCounts);
	float thre = 0.15;
	for (auto i = dataManager.vmwareTypeList.cbegin(); i != dataManager.vmwareTypeList.cend(); i++) {
		string serName;
		bool flag = false; //�Ƿ���Ҫ���ҵ���һ��������
		//if (i->second.cores > i->second.memory) {
		//	float target = float(i->second.cores) / float(i->second.memory);
		//	target = target > dataManager.maxRatioS ? dataManager.maxRatioS : target;
		//	for (auto iter = dataManager.pfmList.begin(); iter != dataManager.pfmList.end(); iter++) {
		//		float temp = float(dataManager.serverTypeList[*iter].cores) / float(dataManager.serverTypeList[*iter].memory);
		//		if (temp < 0.8)
		//			continue;
		//		//����CPU��memory֮��ѡ�������
		//		if ((target <= temp + thre) && (target >= temp - thre)
		//			&& (dataManager.serverTypeList[*iter].cores >= i->second.cores)
		//			&& (dataManager.serverTypeList[*iter].memory >= i->second.memory)) {
		//			serName = *iter;
		//			flag = true;
		//			break;
		//		}
		//	}
		//}
		//else {
		//	float target = float(i->second.memory) / float(i->second.cores);
		//	target = target > (1 / dataManager.maxRatioS) ? (1 / dataManager.maxRatioS) : target;
		//	for (auto iter = dataManager.pfmList.begin(); iter != dataManager.pfmList.end(); iter++) {
		//		float temp = float(dataManager.serverTypeList[*iter].memory) / float(dataManager.serverTypeList[*iter].cores);
		//		if (temp < 0.8)
		//			continue;
		//		//����memory��CPU֮��ѡ�������
		//		if ((target <= temp + thre) && (target >= temp - thre)
		//			&& (dataManager.serverTypeList[*iter].cores >= i->second.cores)
		//			&& (dataManager.serverTypeList[*iter].memory >= i->second.memory)) {
		//			serName = *iter;
		//			flag = true;
		//			break;
		//		}
		//	}
		//}
		float target = logf(float(i->second.cores) / float(i->second.memory));
		target = target > dataManager.maxRatioS ? dataManager.maxRatioS : target;
		target = target < dataManager.minRatioS ? dataManager.minRatioS : target;
		for (auto iter = dataManager.pfmList.begin(); iter != dataManager.pfmList.end(); iter++) {
			float temp = logf(float(dataManager.serverTypeList[*iter].cores) / float(dataManager.serverTypeList[*iter].memory));
			//����CPU��memory֮��ѡ�������
			if ((target <= temp + thre) && (target >= temp - thre)
				&& (dataManager.serverTypeList[*iter].cores >= i->second.cores)
				&& (dataManager.serverTypeList[*iter].memory >= i->second.memory)) {
				serName = *iter;
				flag = true;
				break;
			}
		}
		if (!flag) {
			for (auto iter = dataManager.serverTypeList.begin(); iter != dataManager.serverTypeList.end(); iter++) {
				//����memory��CPU֮��ѡ�������
				if ((iter->second.cores >= i->second.cores) && (iter->second.memory >= i->second.memory)) {
					serName = iter->first;
					break;
				}
			}
		}
		vmwareToServer[i->first] = serName;
	}
}

//��ȡChangeData
void getAddData(Server& server, ChangeData& data)
{
	data.coresA = server.GetServerType().cores - server.GetA().unusedCores;
	data.memoryA = server.GetServerType().memory - server.GetA().unusedMemory;
	data.coresB = server.GetServerType().cores - server.GetB().unusedCores;
	data.memoryB = server.GetServerType().memory - server.GetB().unusedMemory;
	data.serverID = server.GetID();
}

/*Ǩ�Ʋ���
* vmwareID:ҪǨ�Ƶķ�����ID
* goal��Ǩ�ƺ��Ŀ�������
* days����ǰ�ǵڼ���
*/
void Controller::moveFun(const unsigned vmwareID, pair<int,int>& goal, int days)
{
	unsigned tempServerID = dataManager.vmwareList[vmwareID].serverID;
	MoveData move_data = { vmwareID, goal.first, dataManager.vmwareList[vmwareID].myType.isDouble, goal.second == 0 };;
	dataManager.moveList[days].emplace_back(move_data);
	//��Ӳ���
	Server& server = dataManager.serverList[usedServerList[goal.first]];
	if (goal.second == 0)
		server.AddVmwareA(vmwareID);
	else if (goal.second == 1)
		server.AddVmwareB(vmwareID);
	else
		server.AddVmwareD(vmwareID);
	//��ӷ������仯���б�
#if isVisual
	ChangeData temp1;
	getAddData(server, temp1);
	dataManager.changeList[days].emplace_back(temp1);
#endif
	if ((server.GetA().unusedCores < leastCore || server.GetA().unusedMemory < leastMemory)
		&& (server.GetB().unusedCores < leastCore || server.GetB().unusedMemory < leastMemory)) {
		usedServerList.erase(usedServerList.begin() + goal.first);
	}
	//ɾ������
	server = dataManager.serverList[tempServerID];
	server.DeleteVmware(vmwareID);
	//��ӷ������仯���б�
#if isVisual
	getAddData(server, temp1);
	dataManager.changeList[days].emplace_back(temp1);
#endif
	auto iter = find(usedServerList.begin(), usedServerList.end(), server.GetID());
	if (iter == usedServerList.cend())
		usedServerList.emplace_back(server.GetID());
}

/*
* �������п��÷��������б������ҳ�һ�����ʺϵ�ǰ������ķ�����
* goal:�ҵ���Ŀ�������������
* jumpSeq��Ҫ�����ķ�����ID
*/
void Controller::process(VmwareType& type, pair<int, int>& goal, bool& move, int jumpSeq = -1)
{
	float delta = 1000;
	for (unsigned int k = 0; k < usedServerList.size(); k++) {
		if (jumpSeq == int(k)) {
			break;
		}
		//��k̨���÷�����
		Server& server = dataManager.serverList[usedServerList[k]];
		float target = logf(float(type.cores) / float(type.memory));
		int surCoresA = server.GetA().unusedCores - type.cores;
		int surMemoryA = server.GetA().unusedMemory - type.memory;
		int surCoresB = server.GetB().unusedCores - type.cores;
		int surMemoryB = server.GetB().unusedMemory - type.memory;
		if (type.isDouble) {
			if ((surCoresA >= 0) && (surCoresB >= 0) && (surMemoryA >= 0) && (surMemoryB >= 0)) {
				float ratioA = logf(float(server.GetA().unusedCores) / float(server.GetA().unusedMemory));
				float ratioB = logf(float(server.GetB().unusedCores) / float(server.GetB().unusedMemory));
				float mDelta = fabs(ratioA - target) > fabs(ratioB - target) ? fabs(ratioA - target) : fabs(ratioB - target);
				if (mDelta < delta) {
					delta = mDelta;
					goal = pair<int, int>(k, 2);
					surCoresA = surCoresA ? surCoresA : 1;
					surMemoryA = surMemoryA ? surMemoryA : 1;
					surCoresB = surCoresB ? surCoresB : 1;
					surMemoryB = surMemoryB ? surMemoryB : 1;
					if (fabs(logf(float(surCoresA) / float(surMemoryA))) > logf(20) ||
						fabs(logf(float(surCoresB) / float(surMemoryB))) > logf(20))
						move = true;
				}
			}
		}
		else {
			if ((surCoresA >= 0) && (surMemoryA >= 0)) {
				float ratioA = float(surCoresA) / float(surMemoryA);
				if (fabs(ratioA - target) < delta) {
					delta = fabs(ratioA - target);
					goal = pair<int, int>(k, 0);
					surCoresA = surCoresA ? surCoresA : 1;
					surMemoryA = surMemoryA ? surMemoryA : 1;
					if (fabs(logf(float(surCoresA) / float(surMemoryA))) > logf(20))
						move = true;
				}
			}
			if ((surCoresB >= 0) && (surMemoryB >= 0)) {
				float ratioB = float(surCoresB) / float(surMemoryB);
				if (fabs(ratioB - target) < delta) {
					delta = fabs(ratioB - target);
					goal = pair<int, int>(k, 1);
					surCoresB = surCoresB ? surCoresB : 1;
					surMemoryB = surMemoryB ? surMemoryB : 1;
					if (fabs(logf(float(surCoresB) / float(surMemoryB))) > logf(20))
						move = true;
				}
			}
		}
	}
}

void Controller::CreateList()
{
	init();
	//try1.0�汾
	vector<unsigned> waitMove;  //������ҪǨ�Ƶ���������б�
	for (unsigned int i = 0; i < dataManager.dayCounts; i++) {
		//��i��
		if (i % 50 == 0)
			cout << i << endl;
		unsigned int purchaseCount = 0;
		//����Ǩ��
		//for (int j = 0, moveNum = 1; j < waitMove.size(); j++) {
		//	if (moveNum > int(dataManager.vmSize * 0.005))
		//		break;
		//	int tempServerID = dataManager.vmwareList[waitMove[j]].serverID;
		//	VmwareType& type = dataManager.vmwareList[waitMove[j]].myType;
		//	pair<int, int> goal = pair<int, int>(-1, -1);
		//	bool move = false;
		//	//�������п��÷��������б������ҳ�һ����õ�
		//	process(type, goal, move, tempServerID);
		//	//Ǩ��
		//	if (!move && goal != pair<int, int>(-1, -1)) {
		//		moveNum++;
		//		moveFun(waitMove[j], goal, i);
		//	}
		//}
		//����ÿ������
		for (unsigned int j = 0; j < dataManager.requestList[i].size(); j++) {
			//��j������
			RequestType request = dataManager.requestList[i][j];
			if (request.isAdd) {
				dataManager.vmSize++;
				bool success = false;
				VmwareType type = dataManager.vmwareList[request.ID].myType;
				pair<int, int> goal = pair<int, int>(-1, -1);
				bool move = false; //�Ƿ�Ǩ��
				//�������п��÷��������б������ҳ�һ����õ�
				process(type, goal, move);
				//����ҵ���һ�����е�
				if (goal != pair<int, int>(-1, -1)) {
					if (move)
						waitMove.emplace_back(request.ID);
					Server& server = dataManager.serverList[usedServerList[goal.first]];
					if (goal.second == 0)
						server.AddVmwareA(request.ID);
					else if (goal.second == 1)
						server.AddVmwareB(request.ID);
					else
						server.AddVmwareD(request.ID);
					//��ӷ������仯���б�
#if isVisual
					ChangeData temp1;
					getAddData(server, temp1);
					dataManager.changeList[i].emplace_back(temp1);
#endif
					if ((server.GetA().unusedCores < leastCore || server.GetA().unusedMemory < leastMemory)
						&& (server.GetB().unusedCores < leastCore || server.GetB().unusedMemory < leastMemory)) {
						usedServerList.erase(usedServerList.begin() + goal.first);
					}
					success = true;
				}
				if (!success) {
					string serName = vmwareToServer[type.name];
					purchaseCount++;
					dataManager.purchaseList[i][serName] += 1;
					Server& server = PurchaseServer(serName);
					if (type.isDouble)
						server.AddVmwareD(request.ID);
					else
						server.AddVmwareA(request.ID);
					usedServerList.emplace_back(server.GetID());
					//����������������б�
#if isVisual
					dataManager.newList[i].emplace_back(server);
#endif
				}
				AddData it = { dataManager.vmwareList[request.ID].serverID ,dataManager.vmwareList[request.ID].myType.isDouble,dataManager.vmwareList[request.ID].isNodeA };
				//cout << dataManager.vmwareList[request.ID].serverID << endl;
				Server server = dataManager.serverList[dataManager.vmwareList[request.ID].serverID];
				dataManager.addList[i].emplace_back(it);
			}
			//ɾ������
			else {
				dataManager.vmSize--;
				Server& server = dataManager.serverList[dataManager.vmwareList[request.ID].serverID];
				server.DeleteVmware(request.ID);
				//��ӷ������仯���б�
#if isVisual
				ChangeData temp1;
				getAddData(server, temp1);
				dataManager.changeList[i].emplace_back(temp1);
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