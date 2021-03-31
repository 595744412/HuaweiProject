#include "Controller.h"
#include "DataManager.h"
#include "Server.h"

/*
* 目前方法的可调参数：DataManager::init中表示性价比的α和β，Controller中的thre和threMove，
* Controller::init中的inter，以及createlist中遍历waitMoveV和waitMoveS的顺序，以及遍历waitMoveS前是否进行mySort(waitMoveS)
* α=0.8，β=40，thre=190，threMove=logf(25)，inter=14 总成本是1461206922
*/


Server& Controller::PurchaseServer(string type)
{
	Server server(dataManager.serverTypeList[type]);
	dataManager.serverList.emplace_back(server);
	usedServerList.emplace_back(server.GetID());
	return dataManager.serverList[server.GetID()];
}

/************************************
【函数功能】按照usuage对输入的服务器列表进行升序排序
【函数参数】list：储存服务器编号的列表
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
【函数功能】按照usuage维护可用服务器列表即usedServerList的有序性
【函数参数】serverID：进行了添加或删除操作的服务器编号
【注】这个函数暂时没有用到
**********************************************************/
void Controller::update(const unsigned serverID)
{
	unsigned _usuage = dataManager.serverList[serverID].usuage;
	auto it = find(usedServerList.begin(), usedServerList.end(), serverID);
	if (it != usedServerList.end())
		usedServerList.erase(it);
	//二分插入,保证按照usuage的升序排列
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
【函数功能】初始化，获取虚拟机到服务器的映射
【函数参数】无
**********************************************************/
void Controller::init(void)
{
	dataManager.init(dataManager.dayCounts);
	for (auto i = dataManager.vmwareTypeList.cbegin(); i != dataManager.vmwareTypeList.cend(); i++) {
		float ratio = i->second.ratio;
		int mid = 0, inter = 14;
		string target = " ";
		//遍历按照核存比升序排列的服务器列表，找到和目标虚拟机核存比最接近且能装下的服务器
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
		//在找到的服务器前后2*inter的范围内搜索性价比最高且能装下的那个服务器，作为目标虚拟机的映射
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

	/*下面注释掉的代码是遍历按照服务器的性价比降序排列的服务器列表，找到
	核存比和目标虚拟机的核存比相近（是否相近设了一个阈值来判断）的第一个服务器*/
	//float thre = 0.15;
	//for (auto i = dataManager.vmwareTypeList.cbegin(); i != dataManager.vmwareTypeList.cend(); i++) {
	//	string serName;
	//	bool flag = false; //是否按照要求找到了一个服务器
	//	//if (i->second.cores > i->second.memory) {
	//	//	float target = float(i->second.cores) / float(i->second.memory);
	//	//	target = target > dataManager.maxRatioS ? dataManager.maxRatioS : target;
	//	//	for (auto iter = dataManager.pfmList.begin(); iter != dataManager.pfmList.end(); iter++) {
	//	//		float temp = float(dataManager.serverTypeList[*iter].cores) / float(dataManager.serverTypeList[*iter].memory);
	//	//		if (temp < 0.8)
	//	//			continue;
	//	//		//根据CPU和memory之比选择服务器
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
	//	//		//根据memory和CPU之比选择服务器
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
	//		//根据CPU和memory之比选择服务器
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
	//			//根据memory和CPU之比选择服务器
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
【函数功能】进行虚拟机的迁移操作
【函数参数】vmwareID：要迁移的虚拟机的ID；
           goal：迁移的目标服务器的信息，其中goal.first表示目标服务器的ID，goal.second=0表示迁移到A节点， 1表示迁移到B节点，2表示迁移到双节点；
		   day：第几天进行的迁移
**********************************************************/
void Controller::moveFun(const unsigned vmwareID, pair<int, int>& goal, int day)
{
	unsigned tempServerID = dataManager.vmwareList[vmwareID].serverID;
	MoveData move_data = { vmwareID, goal.first, dataManager.vmwareList[vmwareID].myType.isDouble, goal.second == 0 };;
	dataManager.moveList[day].emplace_back(move_data);

	//删除操作
	Server& server2 = dataManager.serverList[tempServerID];
	server2.DeleteVmware(vmwareID);
	//添加服务器变化到列表
#if isVisual
	ChangeData temp = { server2.GetID(),server2.GetA().usedCores,server2.GetA().usedMemory,server2.GetB().usedCores,server2.GetB().usedMemory };
	dataManager.changeList[day].emplace_back(temp);
#endif
	//添加到usedServerList里面
	if (find(usedServerList.begin(), usedServerList.end(), server2.GetID()) == usedServerList.end())
		usedServerList.emplace_back(server2.GetID());

	//添加操作
	Server& server = dataManager.serverList[goal.first];
	if (goal.second == 0)
		server.AddVmwareA(vmwareID);
	else if (goal.second == 1)
		server.AddVmwareB(vmwareID);
	else
		server.AddVmwareD(vmwareID);
	//添加服务器变化到列表
#if isVisual
	ChangeData temp1 = { server.GetID(),server.GetA().usedCores,server.GetA().usedMemory,server.GetB().usedCores,server.GetB().usedMemory };
	dataManager.changeList[day].emplace_back(temp1);
#endif
	//不能再加入虚拟机就从usedServerList里面清除
	if ((server.GetA().unusedCores < dataManager.minCores || server.GetA().unusedMemory < dataManager.minMemory)
		&& (server.GetB().unusedCores < dataManager.minCores || server.GetB().unusedMemory < dataManager.minMemory)) {
		usedServerList.erase(find(usedServerList.begin(), usedServerList.end(), goal.first));
	}
}

/**********************************************************************************************
【函数功能】遍历所有可用服务器的列表，尝试找出一个最适合当前虚拟机的服务器
【函数参数】vmwareID：目标虚拟机的ID；
		   goal：找到的目标服务器放在这，其中goal.first表示目标服务器的ID，goal.second=0表示迁移到A节点， 1表示迁移到B节点，2表示迁移到双节点；
		   jumpSeq：遍历usedServerList时要跳过的服务器ID，默认-1，即不跳过
【返回值】bool类型，当找到的最适合放的服务器放入该虚拟机的核存比过大或过小（设一个指标），返回true，表示该虚拟机日后需要迁移，否则返回false
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
		//第k台可用服务器
		Server& server = dataManager.serverList[*iter];
		float target = type.ratio;
		//AB节点的CPU余量、内存余量
		int surCoresA = server.GetA().unusedCores - type.cores;
		int surMemoryA = server.GetA().unusedMemory - type.memory;
		int surCoresB = server.GetB().unusedCores - type.cores;
		int surMemoryB = server.GetB().unusedMemory - type.memory;
		if (type.isDouble) {
			if ((surCoresA >= 0) && (surCoresB >= 0) && (surMemoryA >= 0) && (surMemoryB >= 0)) {
				//如果当前遍历的服务器是闲置的，就记下编号，直接遍历下一个服务器
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
				//获取AB节点没有使用部分的核存比，选取核存比和目标虚拟机核存比差距最小的服务器
				float ratioA = logf(float(server.GetA().unusedCores) / float(server.GetA().unusedMemory));
				float ratioB = logf(float(server.GetB().unusedCores) / float(server.GetB().unusedMemory));
				float mDelta = fabs(ratioA - target) > fabs(ratioB - target) ? fabs(ratioA - target) : fabs(ratioB - target);
				if (mDelta < delta) {
					delta = mDelta;
					goal = pair<int, int>(*iter, 2);
					surCoresA = surCoresA ? surCoresA : 1;    //防止0的干扰，下同
					surMemoryA = surMemoryA ? surMemoryA : 1;
					surCoresB = surCoresB ? surCoresB : 1;
					surMemoryB = surMemoryB ? surMemoryB : 1;
					//判断是否满足迁移该虚拟机的条件
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
	//没有在不闲置的服务器中找到能放下的，就只能尝试选择一个闲置的服务器
	if (emptyServer != -1 && goal == pair<int, int>(-1, -1)) {
		goal = pair<int, int>(emptyServer, (type.isDouble == 1) + 1);
		//判断是否满足迁移该虚拟机的条件
		Server& server = dataManager.serverList[goal.first];
		int surCores = server.GetServerType().cores - type.cores;
		int surMemory = server.GetServerType().memory - type.memory;
		surCores = surCores ? surCores : 1;    //防止0的干扰，下同
		surMemory = surMemory ? surMemory : 1;
		if (fabs(logf(float(surCores) / float(surMemory))) > threMove)
			move = true;
		else
			move = false;
	}
	return move;
}

/***************************************
【函数功能】处理每天的请求信息，获取每天的操作
****************************************/
void Controller::CreateList()
{ 
	init();
	for (unsigned int i = 0; i < dataManager.dayCounts; i++) {
		//第i天
#if isVisual
		if (i % 50 == 0) {
		cout << i << endl;
		}
#endif
		unsigned int purchaseCount = 0;
		//先找到当天需要删除的所有服务器
		unordered_set<unsigned> delList;
		for (unsigned k = i; k < i + 1 && k < dataManager.tempDay; k++) {
			for (unsigned int j = 0; j < dataManager.requestList[k].size(); j++) {
				//第j个请求
				RequestType request = dataManager.requestList[k][j];
				if (!request.isAdd)
					delList.insert(request.ID);
			}
		}
		//进行迁移
		int moveNum = 1;
		//遍历需要迁移的虚拟机列表
		for (auto iter = waitMoveV.begin(); iter != waitMoveV.end();) {
			//超过每天可移动虚拟机数量上限，跳出循环
			if (moveNum > int(dataManager.vmSize * moveRatio))
				break;
			//如果当前虚拟机即将删除，跳过
			if (delList.find(*iter) != delList.end()) {
				iter = waitMoveV.erase(iter);
				continue;
			}
			//再次检查是否需要迁移
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
			//开始迁移
			pair<int, int> goal = pair<int, int>(-1, -1);
			//遍历所有可用服务器的列表，尝试找出一个最好的
			bool move = myFind(*iter, goal, tempSID);
			if (goal == pair<int, int>(-1, -1) || move == true) { 	//没有找到合适的
				iter++;
				continue;
			}
			else {                                                  //找到了一个合适的
				moveNum++;
				moveFun(*iter, goal, i);
				//如果删除完虚拟机后服务器满足清空条件，加入清空服务器列表
				if (dataManager.serverList[tempSID].needEmpty(thre)) {
					if (find(waitMoveS.begin(), waitMoveS.end(), tempSID) == waitMoveS.end())
						waitMoveS.emplace_back(tempSID);
				}
				iter++;
			}
		}
		//遍历需要迁移的服务器列表
		//mySort(waitMoveS);
		for (auto iter = waitMoveS.begin(); iter != waitMoveS.end();) {
			if (dataManager.serverList[*iter].isEmpty()) {
				iter = waitMoveS.erase(iter);
				continue;
			}
			bool success = true;
			//清空A节点上的虚拟机
			for (int jk = 0; jk < dataManager.serverList[*iter].GetA().vmwares.size(); jk++) {
				if (moveNum > int(dataManager.vmSize * moveRatio))
					break;
				//如果当前虚拟机即将删除，跳过
				if (delList.find(dataManager.serverList[*iter].GetA().vmwares[jk]) != delList.end()) {
					continue;
				}
				pair<int, int> goal = pair<int, int>(-1, -1);
				//遍历所有可用服务器的列表，尝试找出一个最好的
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
			//清空B节点上的虚拟机
			for (int jk = 0; jk < dataManager.serverList[*iter].GetB().vmwares.size(); jk++) {
				if (moveNum > int(dataManager.vmSize * moveRatio))
					break;
				//如果当前虚拟机即将删除，跳过
				if (delList.find(dataManager.serverList[*iter].GetB().vmwares[jk]) != delList.end()) {
					continue;
				}
				pair<int, int> goal = pair<int, int>(-1, -1);
				//遍历所有可用服务器的列表，尝试找出一个最好的
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
		//处理每个请求
		for (unsigned int j = 0; j < dataManager.requestList[i].size(); j++) {
			//第j个请求
			RequestType request = dataManager.requestList[i][j];
			if (request.isAdd) {
				dataManager.vmSize++;
				VmwareType type = dataManager.vmwareList[request.ID].myType;
				pair<int, int> goal = pair<int, int>(-1, -1);
				//遍历所有可用服务器的列表，尝试找出一个最好的
				bool move = myFind(request.ID, goal);
				//如果找到了一个可行的
				if (goal != pair<int, int>(-1, -1)) {
					Server& server = dataManager.serverList[goal.first];
					if (goal.second == 0)
						server.AddVmwareA(request.ID);
					else if (goal.second == 1)
						server.AddVmwareB(request.ID);
					else
						server.AddVmwareD(request.ID);
					//虚拟机是否加入待迁移虚拟机列表
					if (move)
						waitMoveV.emplace_back(request.ID);
					//服务器是否加入待迁移服务器列表
					if (!server.needEmpty(thre) && !move) {
						auto it1 = find(waitMoveS.begin(), waitMoveS.end(), goal.first);
						if (it1 != waitMoveS.end())
							waitMoveS.erase(it1);
					}
					//添加服务器变化到列表
#if isVisual
					ChangeData temp1 = { server.GetID(),server.GetA().usedCores,server.GetA().usedMemory,server.GetB().usedCores,server.GetB().usedMemory };
					dataManager.changeList[i].emplace_back(temp1);
#endif
					if ((server.GetA().unusedCores < dataManager.minCores || server.GetA().unusedMemory < dataManager.minMemory)
						&& (server.GetB().unusedCores < dataManager.minCores || server.GetB().unusedMemory < dataManager.minMemory)) {
						usedServerList.erase(find(usedServerList.begin(), usedServerList.end(), goal.first));
					}
				}
				//如果没有找到可行的就添加新的服务器
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
					//添加新增服务器到列表
#if isVisual
					dataManager.newList[i].emplace_back(server);
#endif
				}
				AddData it = { dataManager.vmwareList[request.ID].serverID ,dataManager.vmwareList[request.ID].myType.isDouble,dataManager.vmwareList[request.ID].isNodeA };
				Server server = dataManager.serverList[dataManager.vmwareList[request.ID].serverID];
				dataManager.addList[i].emplace_back(it);
			}
			//删除操作
			else {
				dataManager.vmSize--;
				int ID = dataManager.vmwareList[request.ID].serverID;
				Server& server = dataManager.serverList[ID];
				server.DeleteVmware(request.ID);
				//从waitMoveV中删除该虚拟机（如果存在的话）
				auto itV = find(waitMoveV.begin(), waitMoveV.end(), request.ID);
				if (itV != waitMoveV.end())
					waitMoveV.erase(itV);
				//添加服务器变化到列表
#if isVisual
				ChangeData temp1 = { server.GetID(),server.GetA().usedCores,server.GetA().usedMemory,server.GetB().usedCores,server.GetB().usedMemory };
				dataManager.changeList[i].emplace_back(temp1);
#endif
				if (find(usedServerList.begin(), usedServerList.end(), server.GetID()) == usedServerList.end())
					usedServerList.emplace_back(server.GetID());
				//满足服务器的清空条件时加入待迁移列表
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
		//建立顺序服务器序号重映射
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
		//输出当天的操作
		dataManager.myWrite(i);
		//清空缓存区
#if !isVisual
		fflush(stdout);
#endif
		//读入新的的请求信息
		dataManager.readRequests();
	}
}