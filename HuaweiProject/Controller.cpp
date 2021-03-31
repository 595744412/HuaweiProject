#include "Controller.h"
#include "DataManager.h"
#include "Server.h"

/*
* Ŀǰ�����Ŀɵ�������DataManager::init�б�ʾ�Լ۱ȵĦ��ͦ£�Controller�е�thre��threMove��
* Controller::init�е�inter���Լ�createlist�б���waitMoveV��waitMoveS��˳���Լ�����waitMoveSǰ�Ƿ����mySort(waitMoveS)
* ��=0.8����=40��thre=190��threMove=logf(25)��inter=14 �ܳɱ���1461206922
*/


Server& Controller::PurchaseServer(string type)
{
	Server server(dataManager.serverTypeList[type]);
	dataManager.serverList.emplace_back(server);
	usedServerList.emplace_back(server.GetID());
	return dataManager.serverList[server.GetID()];
}

/************************************
���������ܡ�����usuage������ķ������б������������
������������list�������������ŵ��б�
**************************************/
void Controller::mySort(vector<unsigned>& list)
{
	for (int i = 1; i < list.size(); i++) {
		int temp = list[i];
		int low = 0, high = i - 1, mid = 0;
		while (low <= high) {
			mid = (low + high) / 2;
			if (dataManager.serverList[usedServerList[mid]].usuage == dataManager.serverList[temp].usuage) {
				high = mid;
				break;
			}
			else if ((dataManager.serverList[usedServerList[mid]].usuage < dataManager.serverList[temp].usuage))
				low = mid + 1;
			else
				high = mid - 1;
		}
		for (int j = i; j > high + 1; j--) {
			list[j] = list[j - 1];
		}
		list[high + 1] = temp;
	}
}

/********************************************************
���������ܡ�����usuageά�����÷������б�usedServerList��������
������������serverID����������ӻ�ɾ�������ķ��������
��ע�����������ʱû���õ�
**********************************************************/
void Controller::update(const unsigned serverID)
{
	unsigned _usuage = dataManager.serverList[serverID].usuage;
	auto it = find(usedServerList.begin(), usedServerList.end(), serverID);
	if (it != usedServerList.end())
		usedServerList.erase(it);
	//���ֲ���,��֤����usuage����������
	int low = 0, high = usedServerList.size(), mid = 0;
	while (low < high) {
		mid = (low + high) / 2;
		if (dataManager.serverList[usedServerList[mid]].usuage == _usuage) {
			high = mid;
			break;
		}
		else if ((dataManager.serverList[usedServerList[mid]].usuage < _usuage))
			low = mid + 1;
		else
			high = mid;
	}
	usedServerList.insert(usedServerList.begin() + high, serverID);
}


/********************************************************
���������ܡ���ʼ������ȡ���������������ӳ��
��������������
**********************************************************/
void Controller::init(void)
{
	dataManager.init(dataManager.dayCounts);
	for (auto i = dataManager.vmwareTypeList.cbegin(); i != dataManager.vmwareTypeList.cend(); i++) {
		float ratio = i->second.ratio;
		int mid = 0, inter = 14;
		string target = " ";
		//�������պ˴���������еķ������б��ҵ���Ŀ��������˴����ӽ�����װ�µķ�����
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
		//���ҵ��ķ�����ǰ��2*inter�ķ�Χ�������Լ۱��������װ�µ��Ǹ�����������ΪĿ���������ӳ��
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

	/*����ע�͵��Ĵ����Ǳ������շ��������Լ۱Ƚ������еķ������б��ҵ�
	�˴�Ⱥ�Ŀ��������ĺ˴��������Ƿ��������һ����ֵ���жϣ��ĵ�һ��������*/
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


/********************************************************
���������ܡ������������Ǩ�Ʋ���
������������vmwareID��ҪǨ�Ƶ��������ID��
           goal��Ǩ�Ƶ�Ŀ�����������Ϣ������goal.first��ʾĿ���������ID��goal.second=0��ʾǨ�Ƶ�A�ڵ㣬 1��ʾǨ�Ƶ�B�ڵ㣬2��ʾǨ�Ƶ�˫�ڵ㣻
		   day���ڼ�����е�Ǩ��
**********************************************************/
void Controller::moveFun(const unsigned vmwareID, pair<int, int>& goal, int day)
{
	unsigned tempServerID = dataManager.vmwareList[vmwareID].serverID;
	MoveData move_data = { vmwareID, goal.first, dataManager.vmwareList[vmwareID].myType.isDouble, goal.second == 0 };;
	dataManager.moveList[day].emplace_back(move_data);

	//ɾ������
	Server& server2 = dataManager.serverList[tempServerID];
	server2.DeleteVmware(vmwareID);
	//��ӷ������仯���б�
#if isVisual
	ChangeData temp = { server2.GetID(),server2.GetA().usedCores,server2.GetA().usedMemory,server2.GetB().usedCores,server2.GetB().usedMemory };
	dataManager.changeList[day].emplace_back(temp);
#endif
	//��ӵ�usedServerList����
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
	dataManager.changeList[day].emplace_back(temp1);
#endif
	//�����ټ���������ʹ�usedServerList�������
	if ((server.GetA().unusedCores < dataManager.minCores || server.GetA().unusedMemory < dataManager.minMemory)
		&& (server.GetB().unusedCores < dataManager.minCores || server.GetB().unusedMemory < dataManager.minMemory)) {
		usedServerList.erase(find(usedServerList.begin(), usedServerList.end(), goal.first));
	}
}

/**********************************************************************************************
���������ܡ��������п��÷��������б������ҳ�һ�����ʺϵ�ǰ������ķ�����
������������vmwareID��Ŀ���������ID��
		   goal���ҵ���Ŀ������������⣬����goal.first��ʾĿ���������ID��goal.second=0��ʾǨ�Ƶ�A�ڵ㣬 1��ʾǨ�Ƶ�B�ڵ㣬2��ʾǨ�Ƶ�˫�ڵ㣻
		   jumpSeq������usedServerListʱҪ�����ķ�����ID��Ĭ��-1����������
������ֵ��bool���ͣ����ҵ������ʺϷŵķ����������������ĺ˴�ȹ�����С����һ��ָ�꣩������true����ʾ��������պ���ҪǨ�ƣ����򷵻�false
************************************************************************************************/
bool Controller::myFind(unsigned vmwareID, pair<int, int>& goal, int jumpSeq = -1)
{
	bool move = false;
	float delta = 1000;
	int emptyServer = -1;
	VmwareType& type = dataManager.vmwareList[vmwareID].myType;
	for (auto iter = usedServerList.cbegin(); iter != usedServerList.cend(); iter++) {
		if (*iter == jumpSeq)
			continue;
		//��k̨���÷�����
		Server& server = dataManager.serverList[*iter];
		float target = type.ratio;
		//AB�ڵ��CPU�������ڴ�����
		int surCoresA = server.GetA().unusedCores - type.cores;
		int surMemoryA = server.GetA().unusedMemory - type.memory;
		int surCoresB = server.GetB().unusedCores - type.cores;
		int surMemoryB = server.GetB().unusedMemory - type.memory;
		if (type.isDouble) {
			if ((surCoresA >= 0) && (surCoresB >= 0) && (surMemoryA >= 0) && (surMemoryB >= 0)) {
				//�����ǰ�����ķ����������õģ��ͼ��±�ţ�ֱ�ӱ�����һ��������
				if (server.isEmpty()) {
					if (emptyServer == -1)
						emptyServer = *iter;
					else {
						if (fabs(server.GetServerType().ratio - type.ratio) <
							fabs(dataManager.serverList[emptyServer].GetServerType().ratio - type.ratio))
							emptyServer = *iter;
					}
					continue;
				}
				//��ȡAB�ڵ�û��ʹ�ò��ֵĺ˴�ȣ�ѡȡ�˴�Ⱥ�Ŀ��������˴�Ȳ����С�ķ�����
				float ratioA = logf(float(server.GetA().unusedCores) / float(server.GetA().unusedMemory));
				float ratioB = logf(float(server.GetB().unusedCores) / float(server.GetB().unusedMemory));
				float mDelta = fabs(ratioA - target) > fabs(ratioB - target) ? fabs(ratioA - target) : fabs(ratioB - target);
				if (mDelta < delta) {
					delta = mDelta;
					goal = pair<int, int>(*iter, 2);
					surCoresA = surCoresA ? surCoresA : 1;    //��ֹ0�ĸ��ţ���ͬ
					surMemoryA = surMemoryA ? surMemoryA : 1;
					surCoresB = surCoresB ? surCoresB : 1;
					surMemoryB = surMemoryB ? surMemoryB : 1;
					//�ж��Ƿ�����Ǩ�Ƹ������������
					if (fabs(logf(float(surCoresA) / float(surMemoryA))) > threMove ||
						fabs(logf(float(surCoresB) / float(surMemoryB))) > threMove)
						move = true;
					else
						move = false;
				}
			}
		}
		else {
			if ((surCoresA >= 0) && (surMemoryA >= 0)) {
				if (server.isEmpty()) {
					if (emptyServer == -1)
						emptyServer = *iter;
					else {
						if (fabs(server.GetServerType().ratio - type.ratio) <
							fabs(dataManager.serverList[emptyServer].GetServerType().ratio - type.ratio))
							emptyServer = *iter;
					}
					continue;
				}
				float ratioA = logf(float(server.GetA().unusedCores) / float(server.GetA().unusedMemory));
				if (fabs(ratioA - target) < delta) {
					delta = fabs(ratioA - target);
					goal = pair<int, int>(*iter, 0);
					surCoresA = surCoresA ? surCoresA : 1;
					surMemoryA = surMemoryA ? surMemoryA : 1;
					if (fabs(logf(float(surCoresA) / float(surMemoryA))) > threMove)
						move = true;
					else
						move = false;
				}
			}
			if ((surCoresB >= 0) && (surMemoryB >= 0)) {
				if (server.isEmpty()) {
					if (emptyServer == -1)
						emptyServer = *iter;
					else {
						if (fabs(server.GetServerType().ratio - type.ratio) <
							fabs(dataManager.serverList[emptyServer].GetServerType().ratio - type.ratio))
							emptyServer = *iter;
					}
					continue;
				}
				float ratioB = logf(float(server.GetB().unusedCores) / float(server.GetB().unusedMemory));
				if (fabs(ratioB - target) < delta) {
					delta = fabs(ratioB - target);
					goal = pair<int, int>(*iter, 1);
					surCoresB = surCoresB ? surCoresB : 1;
					surMemoryB = surMemoryB ? surMemoryB : 1;
					if (fabs(logf(float(surCoresB) / float(surMemoryB))) > threMove)
						move = true;
					else
						move = false;
				}
			}
		}
		/*if (type.isDouble) {
		   if ((surCoresA >= 0) && (surCoresB >= 0) && (surMemoryA >= 0) && (surMemoryB >= 0)) {
			   if (server.isEmpty()) {
				   if (emptyServer == -1)
					   emptyServer = *iter;
				   else {
					   if (fabs(server.GetServerType().ratio - type.ratio) <
						   fabs(dataManager.serverList[emptyServer].GetServerType().ratio - type.ratio))
						   emptyServer = *iter;
				   }
				   continue;
			   }
			   surCoresA = surCoresA ? surCoresA : 1;
			   surMemoryA = surMemoryA ? surMemoryA : 1;
			   surCoresB = surCoresB ? surCoresB : 1;
			   surMemoryB = surMemoryB ? surMemoryB : 1;
			   float ratioA = logf(float(surCoresA * server.GetServerType().memory) / float(server.GetServerType().cores * surMemoryA));
			   float ratioB = logf(float(surCoresB * server.GetServerType().memory) / float(server.GetServerType().cores * surMemoryB));
			   float mDelta = fabs(ratioA) > fabs(ratioB) ? fabs(ratioA) : fabs(ratioB);
			   if (mDelta < delta) {
				   delta = mDelta;
				   goal = pair<int, int>(*iter, 2);
				   if (fabs(logf(float(surCoresA) / float(surMemoryA))) > threMove ||
					   fabs(logf(float(surCoresB) / float(surMemoryB))) > threMove)
					   move = true;
				   else
					   move = false;
			   }
		   }
		}
		else {
		   if ((surCoresA >= 0) && (surMemoryA >= 0)) {
			   if (server.isEmpty()) {
				   if (emptyServer == -1)
					   emptyServer = *iter;
				   else {
					   if (fabs(server.GetServerType().ratio - type.ratio) <
						   fabs(dataManager.serverList[emptyServer].GetServerType().ratio - type.ratio))
						   emptyServer = *iter;
				   }
				   continue;
			   }
			   surCoresA = surCoresA ? surCoresA : 1;
			   surMemoryA = surMemoryA ? surMemoryA : 1;
			   float ratioA = logf(float(surCoresA * server.GetServerType().memory) / float(server.GetServerType().cores * surMemoryA));
			   if (fabs(ratioA) < delta) {
				   delta = fabs(ratioA);
				   goal = pair<int, int>(*iter, 0);
				   if (fabs(logf(float(surCoresA) / float(surMemoryB))) > threMove)
					   move = true;
				   else
					   move = false;
			   }
		   }
		   if ((surCoresB >= 0) && (surMemoryB >= 0)) {
			   if (server.isEmpty()) {
				   if (emptyServer == -1)
					   emptyServer = *iter;
				   else {
					   if (fabs(server.GetServerType().ratio - type.ratio) <
						   fabs(dataManager.serverList[emptyServer].GetServerType().ratio - type.ratio))
						   emptyServer = *iter;
				   }
				   continue;
			   }
			   surCoresB = surCoresB ? surCoresB : 1;
			   surMemoryB = surMemoryB ? surMemoryB : 1;
			   float ratioB = logf(float(surCoresB * server.GetServerType().memory) / float(server.GetServerType().cores * surMemoryB));
			   if (fabs(ratioB) < delta) {
				   delta = fabs(ratioB);
				   goal = pair<int, int>(*iter, 1);
				   if (fabs(logf(float(surCoresB) / float(surMemoryB))) > threMove)
					   move = true;
				   else
					   move = false;
			   }
		   }
		}*/
	}
	//û���ڲ����õķ��������ҵ��ܷ��µģ���ֻ�ܳ���ѡ��һ�����õķ�����
	if (emptyServer != -1 && goal == pair<int, int>(-1, -1)) {
		goal = pair<int, int>(emptyServer, (type.isDouble == 1) + 1);
		//�ж��Ƿ�����Ǩ�Ƹ������������
		Server& server = dataManager.serverList[goal.first];
		int surCores = server.GetServerType().cores - type.cores;
		int surMemory = server.GetServerType().memory - type.memory;
		surCores = surCores ? surCores : 1;    //��ֹ0�ĸ��ţ���ͬ
		surMemory = surMemory ? surMemory : 1;
		if (fabs(logf(float(surCores) / float(surMemory))) > threMove)
			move = true;
		else
			move = false;
	}
	return move;
}

/***************************************
���������ܡ�����ÿ���������Ϣ����ȡÿ��Ĳ���
****************************************/
void Controller::CreateList()
{ 
	init();
	for (unsigned int i = 0; i < dataManager.dayCounts; i++) {
		//��i��
#if isVisual
		if (i % 50 == 0) {
		cout << i << endl;
		}
#endif
		unsigned int purchaseCount = 0;
		//���ҵ�������Ҫɾ�������з�����
		unordered_set<unsigned> delList;
		for (unsigned k = i; k < i + 1 && k < dataManager.tempDay; k++) {
			for (unsigned int j = 0; j < dataManager.requestList[k].size(); j++) {
				//��j������
				RequestType request = dataManager.requestList[k][j];
				if (!request.isAdd)
					delList.insert(request.ID);
			}
		}
		//����Ǩ��
		int moveNum = 1;
		//������ҪǨ�Ƶ�������б�
		for (auto iter = waitMoveV.begin(); iter != waitMoveV.end();) {
			//����ÿ����ƶ�������������ޣ�����ѭ��
			if (moveNum > int(dataManager.vmSize * moveRatio))
				break;
			//�����ǰ���������ɾ��������
			if (delList.find(*iter) != delList.end()) {
				iter = waitMoveV.erase(iter);
				continue;
			}
			//�ٴμ���Ƿ���ҪǨ��
			bool flag = true;
			unsigned tempSID = dataManager.vmwareList[*iter].serverID;
			if (dataManager.vmwareList[*iter].myType.isDouble) {
				if (fabs(logf(float(dataManager.serverList[tempSID].GetA().unusedCores) / float(dataManager.serverList[tempSID].GetA().unusedMemory))) < threMove
					&& fabs(logf(float(dataManager.serverList[tempSID].GetB().unusedCores) / float(dataManager.serverList[tempSID].GetB().unusedMemory))) < threMove)
					flag = false;
			}
			else if (dataManager.vmwareList[*iter].isNodeA) {
				if (fabs(logf(float(dataManager.serverList[tempSID].GetA().unusedCores) / float(dataManager.serverList[tempSID].GetA().unusedMemory))) < threMove)
					flag = false;
			}
			else {
				if (fabs(logf(float(dataManager.serverList[tempSID].GetB().unusedCores) / float(dataManager.serverList[tempSID].GetB().unusedMemory))) < threMove)
					flag = false;
			}
			if (!flag) {
				iter = waitMoveV.erase(iter);
				continue;
			}
			//��ʼǨ��
			pair<int, int> goal = pair<int, int>(-1, -1);
			//�������п��÷��������б������ҳ�һ����õ�
			bool move = myFind(*iter, goal, tempSID);
			if (goal == pair<int, int>(-1, -1) || move == true) { 	//û���ҵ����ʵ�
				iter++;
				continue;
			}
			else {                                                  //�ҵ���һ�����ʵ�
				moveNum++;
				moveFun(*iter, goal, i);
				//���ɾ�����������������������������������շ������б�
				if (dataManager.serverList[tempSID].needEmpty(thre)) {
					if (find(waitMoveS.begin(), waitMoveS.end(), tempSID) == waitMoveS.end())
						waitMoveS.emplace_back(tempSID);
				}
				iter++;
			}
		}
		//������ҪǨ�Ƶķ������б�
		//mySort(waitMoveS);
		for (auto iter = waitMoveS.begin(); iter != waitMoveS.end();) {
			if (dataManager.serverList[*iter].isEmpty()) {
				iter = waitMoveS.erase(iter);
				continue;
			}
			bool success = true;
			//���A�ڵ��ϵ������
			for (int jk = 0; jk < dataManager.serverList[*iter].GetA().vmwares.size(); jk++) {
				if (moveNum > int(dataManager.vmSize * moveRatio))
					break;
				//�����ǰ���������ɾ��������
				if (delList.find(dataManager.serverList[*iter].GetA().vmwares[jk]) != delList.end()) {
					continue;
				}
				pair<int, int> goal = pair<int, int>(-1, -1);
				//�������п��÷��������б������ҳ�һ����õ�
				bool move = myFind(dataManager.serverList[*iter].GetA().vmwares[jk], goal, *iter);
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
			//���B�ڵ��ϵ������
			for (int jk = 0; jk < dataManager.serverList[*iter].GetB().vmwares.size(); jk++) {
				if (moveNum > int(dataManager.vmSize * moveRatio))
					break;
				//�����ǰ���������ɾ��������
				if (delList.find(dataManager.serverList[*iter].GetB().vmwares[jk]) != delList.end()) {
					continue;
				}
				pair<int, int> goal = pair<int, int>(-1, -1);
				//�������п��÷��������б������ҳ�һ����õ�
				bool move = myFind(dataManager.serverList[*iter].GetB().vmwares[jk], goal, *iter);
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
				//�������п��÷��������б������ҳ�һ����õ�
				bool move = myFind(request.ID, goal);
				//����ҵ���һ�����е�
				if (goal != pair<int, int>(-1, -1)) {
					Server& server = dataManager.serverList[goal.first];
					if (goal.second == 0)
						server.AddVmwareA(request.ID);
					else if (goal.second == 1)
						server.AddVmwareB(request.ID);
					else
						server.AddVmwareD(request.ID);
					//������Ƿ�����Ǩ��������б�
					if (move)
						waitMoveV.emplace_back(request.ID);
					//�������Ƿ�����Ǩ�Ʒ������б�
					if (!server.needEmpty(thre) && !move) {
						auto it1 = find(waitMoveS.begin(), waitMoveS.end(), goal.first);
						if (it1 != waitMoveS.end())
							waitMoveS.erase(it1);
					}
					//��ӷ������仯���б�
#if isVisual
					ChangeData temp1 = { server.GetID(),server.GetA().usedCores,server.GetA().usedMemory,server.GetB().usedCores,server.GetB().usedMemory };
					dataManager.changeList[i].emplace_back(temp1);
#endif
					if ((server.GetA().unusedCores < dataManager.minCores || server.GetA().unusedMemory < dataManager.minMemory)
						&& (server.GetB().unusedCores < dataManager.minCores || server.GetB().unusedMemory < dataManager.minMemory)) {
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
					if (fabs(logf(float(server.GetA().unusedCores) / float(server.GetA().unusedMemory))) > threMove ||
						fabs(logf(float(server.GetB().unusedCores) / float(server.GetB().unusedMemory))) > threMove)
						waitMoveV.emplace_back(request.ID);
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
				server.DeleteVmware(request.ID);
				//��waitMoveV��ɾ�����������������ڵĻ���
				auto itV = find(waitMoveV.begin(), waitMoveV.end(), request.ID);
				if (itV != waitMoveV.end())
					waitMoveV.erase(itV);
				//��ӷ������仯���б�
#if isVisual
				ChangeData temp1 = { server.GetID(),server.GetA().usedCores,server.GetA().usedMemory,server.GetB().usedCores,server.GetB().usedMemory };
				dataManager.changeList[i].emplace_back(temp1);
#endif
				if (find(usedServerList.begin(), usedServerList.end(), server.GetID()) == usedServerList.end())
					usedServerList.emplace_back(server.GetID());
				//������������������ʱ�����Ǩ���б�
				if (server.needEmpty(thre)) {
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
		//�������Ĳ���
		dataManager.myWrite(i);
		//��ջ�����
#if !isVisual
		fflush(stdout);
#endif
		//�����µĵ�������Ϣ
		dataManager.readRequests();
	}
}