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
	for (auto i = dataManager.vmwareTypeList.cbegin(); i != dataManager.vmwareTypeList.cend(); i++) {
		float ratio = i->second.ratio;
		int mid = 0, inter = 9;
		string target = " ";
		while (mid < dataManager.ratioList.size()) {
			if (dataManager.serverTypeList[dataManager.ratioList[mid]].cores >= i->second.cores &&
				dataManager.serverTypeList[dataManager.ratioList[mid]].memory >= i->second.memory)
				target = dataManager.ratioList[mid];
			if (ratio <= dataManager.serverTypeList[dataManager.ratioList[mid]].ratio &&
				target != " ") {
				break;
			}
			mid++;
		}
		if (mid + inter > dataManager.ratioList.size())
			mid = dataManager.ratioList.size() - inter;
		if (mid - inter < 0)
			mid = inter;
		for (int j = mid - inter; j < mid + inter; j++) {
			if (dataManager.performance[dataManager.ratioList[j]] < dataManager.performance[target] &&
				dataManager.serverTypeList[dataManager.ratioList[j]].cores >= i->second.cores &&
				dataManager.serverTypeList[dataManager.ratioList[j]].memory >= i->second.memory) {
				target = dataManager.ratioList[j];
			}
		}
		vmwareToServer[i->first] = target;
	}
	//float thre = 0.15;
	//for (auto i = dataManager.vmwareTypeList.cbegin(); i != dataManager.vmwareTypeList.cend(); i++) {
	//	string serName;
	//	bool flag = false; //�Ƿ���Ҫ���ҵ���һ��������
	//	//if (i->second.cores > i->second.memory) {
	//	//	float target = float(i->second.cores) / float(i->second.memory);
	//	//	target = target > dataManager.maxRatioS ? dataManager.maxRatioS : target;
	//	//	for (auto iter = dataManager.pfmList.begin(); iter != dataManager.pfmList.end(); iter++) {
	//	//		float temp = float(dataManager.serverTypeList[*iter].cores) / float(dataManager.serverTypeList[*iter].memory);
	//	//		if (temp < 0.8)
	//	//			continue;
	//	//		//����CPU��memory֮��ѡ�������
	//	//		if ((target <= temp + thre) && (target >= temp - thre)
	//	//			&& (dataManager.serverTypeList[*iter].cores >= i->second.cores)
	//	//			&& (dataManager.serverTypeList[*iter].memory >= i->second.memory)) {
	//	//			serName = *iter;
	//	//			flag = true;
	//	//			break;
	//	//		}
	//	//	}
	//	//}
	//	//else {
	//	//	float target = float(i->second.memory) / float(i->second.cores);
	//	//	target = target > (1 / dataManager.maxRatioS) ? (1 / dataManager.maxRatioS) : target;
	//	//	for (auto iter = dataManager.pfmList.begin(); iter != dataManager.pfmList.end(); iter++) {
	//	//		float temp = float(dataManager.serverTypeList[*iter].memory) / float(dataManager.serverTypeList[*iter].cores);
	//	//		if (temp < 0.8)
	//	//			continue;
	//	//		//����memory��CPU֮��ѡ�������
	//	//		if ((target <= temp + thre) && (target >= temp - thre)
	//	//			&& (dataManager.serverTypeList[*iter].cores >= i->second.cores)
	//	//			&& (dataManager.serverTypeList[*iter].memory >= i->second.memory)) {
	//	//			serName = *iter;
	//	//			flag = true;
	//	//			break;
	//	//		}
	//	//	}
	//	//}
	//	float target = logf(float(i->second.cores) / float(i->second.memory));
	//	target = target > dataManager.maxRatioS ? dataManager.maxRatioS : target;
	//	target = target < dataManager.minRatioS ? dataManager.minRatioS : target;
	//	for (auto iter = dataManager.pfmList.begin(); iter != dataManager.pfmList.end(); iter++) {
	//		float temp = logf(float(dataManager.serverTypeList[*iter].cores) / float(dataManager.serverTypeList[*iter].memory));
	//		//����CPU��memory֮��ѡ�������
	//		if ((target <= temp + thre) && (target >= temp - thre)
	//			&& (dataManager.serverTypeList[*iter].cores >= i->second.cores)
	//			&& (dataManager.serverTypeList[*iter].memory >= i->second.memory)) {
	//			serName = *iter;
	//			flag = true;
	//			break;
	//		}
	//	}
	//	if (!flag) {
	//		for (auto iter = dataManager.serverTypeList.begin(); iter != dataManager.serverTypeList.end(); iter++) {
	//			//����memory��CPU֮��ѡ�������
	//			if ((iter->second.cores >= i->second.cores) && (iter->second.memory >= i->second.memory)) {
	//				serName = iter->first;
	//				break;
	//			}
	//		}
	//	}
	//	vmwareToServer[i->first] = serName;
	//}
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
* vmwareID:ҪǨ�Ƶ������ID
* goal��Ǩ�ƺ��Ŀ���������goal.first�Ƿ�����ID��goal.second��0��ʾǨ�Ƶ�A�ڵ㣬1��ʾǨ�Ƶ�B�ڵ�,2��ʾ˫�ڵ�
* days����ǰ�ǵڼ���
*/
void Controller::moveFun(const unsigned vmwareID, pair<int, int>& goal, int days)
{
	unsigned tempServerID = dataManager.vmwareList[vmwareID].serverID;
	if (vmwareID == 56663036)
		int s = 0;
	MoveData move_data = { vmwareID, goal.first, dataManager.vmwareList[vmwareID].myType.isDouble, goal.second == 0 };;
	dataManager.moveList[days].emplace_back(move_data);

	//ɾ������
	Server& server2 = dataManager.serverList[tempServerID];
	server2.DeleteVmware(vmwareID);
	//��ӷ������仯���б�
#if isVisual
	ChangeData temp = { server2.GetID(),server2.GetA().usedCores,server2.GetA().usedMemory,server2.GetB().usedCores,server2.GetB().usedMemory };
	dataManager.changeList[days].emplace_back(temp);
#endif
	if (find(usedServerList.begin(), usedServerList.end(), server2.GetID()) == usedServerList.end())
		usedServerList.emplace_back(server2.GetID());

	//��Ӳ���
	Server& server = dataManager.serverList[goal.first];
	if (goal.second == 0)
		server.AddVmwareA(vmwareID);
	else if (goal.second == 1)
		server.AddVmwareB(vmwareID);
	else
		server.AddVmwareD(vmwareID);
	//��ӷ������仯���б�
#if isVisual
	ChangeData temp1 = { server.GetID(),server.GetA().usedCores,server.GetA().usedMemory,server.GetB().usedCores,server.GetB().usedMemory };
	dataManager.changeList[days].emplace_back(temp1);
#endif
	if ((server.GetA().unusedCores < leastCore || server.GetA().unusedMemory < leastMemory)
		&& (server.GetB().unusedCores < leastCore || server.GetB().unusedMemory < leastMemory)) {
		usedServerList.erase(find(usedServerList.begin(), usedServerList.end(), server.GetID()));
	}
}

/*
* �������п��÷��������б������ҳ�һ�����ʺϵ�ǰ������ķ�����
* goal:�ҵ���Ŀ�������������
* jumpSeq��Ҫ�����ķ�����ID��Ĭ��-1
*/
void Controller::process(unsigned vmwareID, pair<int, int>& goal, bool& move, int jumpSeq = -1)
{
	float delta = 1000;
	int emptySever = -1;
	VmwareType& type = dataManager.vmwareList[vmwareID].myType;
	for (auto iter = usedServerList.cbegin(); iter != usedServerList.cend(); iter++) {
		if (*iter == jumpSeq)
			continue;
		//��k̨���÷�����
		Server& server = dataManager.serverList[*iter];
		float target = type.ratio;
		int surCoresA = server.GetA().unusedCores - type.cores;
		int surMemoryA = server.GetA().unusedMemory - type.memory;
		int surCoresB = server.GetB().unusedCores - type.cores;
		int surMemoryB = server.GetB().unusedMemory - type.memory;
		if (type.isDouble) {
			if ((surCoresA >= 0) && (surCoresB >= 0) && (surMemoryA >= 0) && (surMemoryB >= 0)) {
				if (server.GetA().usedCores == 0 && server.GetB().usedCores == 0 && server.GetA().usedMemory == 0 && server.GetB().usedMemory == 0) {
					emptySever = *iter;
					continue;
				}
				float ratioA = logf(float(server.GetA().unusedCores) / float(server.GetA().unusedMemory));
				float ratioB = logf(float(server.GetB().unusedCores) / float(server.GetB().unusedMemory));
				float mDelta = fabs(ratioA - target) > fabs(ratioB - target) ? fabs(ratioA - target) : fabs(ratioB - target);
				if (mDelta < delta) {
					delta = mDelta;
					goal = pair<int, int>(*iter, 2);
					// surCoresA = surCoresA ? surCoresA : 1;
					// surMemoryA = surMemoryA ? surMemoryA : 1;
					// surCoresB = surCoresB ? surCoresB : 1;
					// surMemoryB = surMemoryB ? surMemoryB : 1;
					// if (fabs(logf(float(surCoresA) / float(surMemoryA))) > logf(20) ||
					// 	fabs(logf(float(surCoresB) / float(surMemoryB))) > logf(20))
					// 	move = true;
					// else
					// 	move = false;
				}
			}
		}
		else {
			if ((surCoresA >= 0) && (surMemoryA >= 0)) {
				if (server.GetA().usedCores == 0 && server.GetB().usedCores == 0 && server.GetA().usedMemory == 0 && server.GetB().usedMemory == 0) {
					emptySever = *iter;
					continue;
				}
				float ratioA = logf(float(server.GetA().unusedCores) / float(server.GetA().unusedMemory));
				if (fabs(ratioA - target) < delta) {
					delta = fabs(ratioA - target);
					goal = pair<int, int>(*iter, 0);
					// surCoresA = surCoresA ? surCoresA : 1;
					// surMemoryA = surMemoryA ? surMemoryA : 1;
					// if (fabs(logf(float(surCoresA) / float(surMemoryA))) > logf(20))
					// 	move = true;
					// else
					// 	move = false;
				}
			}
			if ((surCoresB >= 0) && (surMemoryB >= 0)) {
				if (server.GetA().usedCores == 0 && server.GetB().usedCores == 0 && server.GetA().usedMemory == 0 && server.GetB().usedMemory == 0) {
					emptySever = *iter;
					continue;
				}
				float ratioB = logf(float(server.GetB().unusedCores) / float(server.GetB().unusedMemory));
				if (fabs(ratioB - target) < delta) {
					delta = fabs(ratioB - target);
					goal = pair<int, int>(*iter, 1);
					// surCoresB = surCoresB ? surCoresB : 1;
					// surMemoryB = surMemoryB ? surMemoryB : 1;
					// if (fabs(logf(float(surCoresB) / float(surMemoryB))) > logf(20))
					// 	move = true;
					// else
					// 	move = false;
				}
			}
		}

		// if (type.isDouble) {
		// 	if ((surCoresA >= 0) && (surCoresB >= 0) && (surMemoryA >= 0) && (surMemoryB >= 0)) {
		// 		surCoresA = surCoresA ? surCoresA : 1;
		// 		surMemoryA = surMemoryA ? surMemoryA : 1;
		// 		surCoresB = surCoresB ? surCoresB : 1;
		// 		surMemoryB = surMemoryB ? surMemoryB : 1;
		// 		float ratioA = logf(float(surCoresA * server.GetServerType().memory) / float(server.GetServerType().cores * surMemoryA));
		// 		float ratioB = logf(float(surCoresB * server.GetServerType().memory) / float(server.GetServerType().cores * surMemoryB));
		// 		float mDelta = fabs(ratioA) > fabs(ratioB) ? fabs(ratioA) : fabs(ratioB);
		// 		if (mDelta < delta) {
		// 			delta = mDelta;
		// 			goal = pair<int, int>(*iter, 2);
		// 			// if (fabs(logf(float(surCoresA) / float(surMemoryA))) > logf(20) ||
		// 			// 	fabs(logf(float(surCoresB) / float(surMemoryB))) > logf(20))
		// 			// 	move = true;
		// 			// else
		// 			// 	move = false;
		// 		}
		// 	}
		// }
		// else {
		// 	if ((surCoresA >= 0) && (surMemoryA >= 0)) {
		// 		surCoresA = surCoresA ? surCoresA : 1;
		// 		surMemoryA = surMemoryA ? surMemoryA : 1;
		// 		float ratioA = logf(float(surCoresA * server.GetServerType().memory) / float(server.GetServerType().cores * surMemoryA));
		// 		if (fabs(ratioA) < delta) {
		// 			delta = fabs(ratioA);
		// 			goal = pair<int, int>(*iter, 0);
		// 			// if (fabs(logf(float(surCoresA) / float(surMemoryA))) > logf(20))
		// 			// 	move = true;
		// 			// else
		// 			// 	move = false;
		// 		}
		// 	}
		// 	if ((surCoresB >= 0) && (surMemoryB >= 0)) {
		// 		surCoresB = surCoresB ? surCoresB : 1;
		// 		surMemoryB = surMemoryB ? surMemoryB : 1;
		// 		float ratioB = logf(float(surCoresB * server.GetServerType().memory) / float(server.GetServerType().cores * surMemoryB));
		// 		if (fabs(ratioB) < delta) {
		// 			delta = fabs(ratioB);
		// 			goal = pair<int, int>(*iter, 1);
		// 			// if (fabs(logf(float(surCoresB) / float(surMemoryB))) > logf(20))
		// 			// 	move = true;
		// 			// else
		// 			// 	move = false;
		// 		}
		// 	}
		// }
	// }
	}

	if (emptySever != -1 && goal == pair<int, int>(-1, -1)) {
		goal = pair<int, int>(emptySever, (type.isDouble == 1) + 1);
	}
}

void Controller::CreateList()
{
	init();
	//try1.0�汾
	for (unsigned int i = 0; i < dataManager.dayCounts; i++) {
		//��i��
		unsigned int purchaseCount = 0;
		//����Ǩ��
		int moveNum = 1;
		int sd = 0;
		for (auto iter = waitMoveS.begin(); iter != waitMoveS.end();) {
			bool success = true;
			int asd = dataManager.serverList[*iter].GetA().vmwares.size();
			int as = 0;
			for (int jk = 0; jk < dataManager.serverList[*iter].GetA().vmwares.size(); jk++) {
				as++;
				if (as == 4 && i == 91)
					int dasjk = 0;
				if (moveNum > int(dataManager.vmSize * 0.005))
					break;
				pair<int, int> goal = pair<int, int>(-1, -1);
				bool move = false;
				//�������п��÷��������б������ҳ�һ����õ�
				process(dataManager.serverList[*iter].GetA().vmwares[jk], goal, move, *iter);
				//if (goal == pair<int, int>(-1, -1) || move == true) {
				if (goal == pair<int, int>(-1, -1)) {
					success = false;
					break;
				}
				else {
					moveNum++;
					moveFun(dataManager.serverList[*iter].GetA().vmwares[jk--], goal, i);
				}
			}
			if (!success) {
				iter++;
				continue;
			}
			for (int jk = 0; jk < dataManager.serverList[*iter].GetB().vmwares.size(); jk++) {
				if (moveNum > int(dataManager.vmSize * 0.005))
					break;
				pair<int, int> goal = pair<int, int>(-1, -1);
				bool move = false;
				//�������п��÷��������б������ҳ�һ����õ�
				process(dataManager.serverList[*iter].GetB().vmwares[jk], goal, move, *iter);
				//if (goal == pair<int, int>(-1, -1) || move == true) {
				if (goal == pair<int, int>(-1, -1)) {
					success = false;
					break;
				}
				else {
					moveNum++;
					moveFun(dataManager.serverList[*iter].GetB().vmwares[jk--], goal, i);
				}
			}
			if (success) {
				iter = waitMoveS.erase(iter);
			}
			else {
				iter++;
			}
		}
		//����ÿ������
		for (unsigned int j = 0; j < dataManager.requestList[i].size(); j++) {
			//��j������
			RequestType request = dataManager.requestList[i][j];
			if (request.isAdd) {
				dataManager.vmSize++;
				VmwareType type = dataManager.vmwareList[request.ID].myType;
				pair<int, int> goal = pair<int, int>(-1, -1);
				bool move = false; //�Ƿ�Ǩ��
				//�������п��÷��������б������ҳ�һ����õ�
				process(request.ID, goal, move);
				//����ҵ���һ�����е�
				if (goal != pair<int, int>(-1, -1)) {
					Server& server = dataManager.serverList[goal.first];
					if (goal.second == 0)
						server.AddVmwareA(request.ID);
					else if (goal.second == 1)
						server.AddVmwareB(request.ID);
					else
						server.AddVmwareD(request.ID);
					if ((server.GetA().usedCores > thre || server.GetA().usedMemory > thre ||
						server.GetB().usedCores > thre || server.GetB().usedMemory > thre) && !move) {
						//waitMoveV.erase(request.ID);
						auto it1 = find(waitMoveS.begin(), waitMoveS.end(), goal.first);
						if (it1 != waitMoveS.end())
							waitMoveS.erase(it1);
					}
					else {
						//waitMoveV.insert(request.ID);
						if (find(waitMoveS.begin(), waitMoveS.end(), goal.first) == waitMoveS.end())
							waitMoveS.emplace_back(goal.first);
					}
					//��ӷ������仯���б�
#if isVisual
					ChangeData temp1 = { server.GetID(),server.GetA().usedCores,server.GetA().usedMemory,server.GetB().usedCores,server.GetB().usedMemory };
					dataManager.changeList[i].emplace_back(temp1);
#endif
					if ((server.GetA().unusedCores < leastCore || server.GetA().unusedMemory < leastMemory)
						&& (server.GetB().unusedCores < leastCore || server.GetB().unusedMemory < leastMemory)) {
						usedServerList.erase(find(usedServerList.begin(), usedServerList.end(), goal.first));
					}
				}
				//���û���ҵ����еľ�����µķ�����
				else {
					string serName = vmwareToServer[type.name];
					purchaseCount++;
					dataManager.purchaseList[i][serName] += 1;
					Server& server = PurchaseServer(serName);
					if (type.isDouble)
						server.AddVmwareD(request.ID);
					else
						server.AddVmwareA(request.ID);
					//����������������б�
#if isVisual
					dataManager.newList[i].emplace_back(server);
#endif
				}
				AddData it = { dataManager.vmwareList[request.ID].serverID ,dataManager.vmwareList[request.ID].myType.isDouble,dataManager.vmwareList[request.ID].isNodeA };
				Server server = dataManager.serverList[dataManager.vmwareList[request.ID].serverID];
				dataManager.addList[i].emplace_back(it);
			}
			//ɾ������
			else {
				dataManager.vmSize--;
				int ID = dataManager.vmwareList[request.ID].serverID;
				Server& server = dataManager.serverList[ID];
				if (server.GetID() != ID)
					int dsa = 1;
				server.DeleteVmware(request.ID);
				//��ӷ������仯���б�
#if isVisual
				ChangeData temp1 = { server.GetID(),server.GetA().usedCores,server.GetA().usedMemory,server.GetB().usedCores,server.GetB().usedMemory };
				dataManager.changeList[i].emplace_back(temp1);
#endif
				if (find(usedServerList.begin(), usedServerList.end(), server.GetID()) == usedServerList.end())
					usedServerList.emplace_back(server.GetID());
				//������������������ʱ�����Ǩ���б�
				if ((server.GetA().usedCores <= thre && server.GetA().usedMemory <= thre &&
					server.GetB().usedCores <= thre && server.GetB().usedMemory <= thre)) {
					if (find(waitMoveS.begin(), waitMoveS.end(), server.GetID()) == waitMoveS.end())
						waitMoveS.emplace_back(server.GetID());
					/*for (auto jk = server.GetA().vmwares.cbegin(); jk != server.GetA().vmwares.cend(); jk++) {
						waitMoveV.insert(*jk);
					}
					for (auto jk = server.GetB().vmwares.cbegin(); jk != server.GetB().vmwares.cend(); jk++) {
						waitMoveV.insert(*jk);
					}*/
				}
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