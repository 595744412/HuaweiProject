#include "DataManager.h"
#include "Server.h"

/*
   ��˳���ȡ�������ݴ���serverTypeList��vmwareTypeList��requestList�У�
   ����ADD��������Ӷ�Ӧ��id��������������ã���ֵ����vmwareList��
*/
void DataManager::initRead()
{
#if isVisual
	freopen_s(&inStream, "./training-1.txt", "r", stdin);
#endif
	int num;
	string serverName, vmwareName, requestName;
	unsigned int cores, memory, price, costPerDay, isDouble;
	char buff;
	//�����������Ϣ
	cin >> num;
	for (int i = 0; i < num; i++) {
		cin >> serverName >> cores >> buff >> memory >> buff >> price >> buff >> costPerDay >> buff;
		serverName = serverName.substr(1, serverName.size() - 2);
		serverTypeList[serverName] = ServerType(serverName, cores / 2, memory / 2, price, costPerDay);
		float temp = logf(float(cores) / float(memory));
		minRatioS = temp > minRatioS ? minRatioS : temp;
		maxRatioS = temp > maxRatioS ? temp : maxRatioS;
		serverTypeList[serverName].ratio = temp;
	}
	//�����������Ϣ
	cin >> num;
	for (int i = 0; i < num; i++) {
		cin >> vmwareName >> cores >> buff >> memory >> buff >> isDouble >> buff;
		vmwareName = vmwareName.substr(1, vmwareName.size() - 2);
		if (isDouble) {
			vmwareTypeList[vmwareName] = VmwareType(vmwareName, cores / 2, memory / 2, isDouble == 1);
			minCores = (minCores > cores / 2) ? (cores / 2) : minCores;
			minMemory = (minMemory > memory / 2) ? (memory / 2) : minMemory;
		}
		else {
			vmwareTypeList[vmwareName] = VmwareType(vmwareName, cores, memory, isDouble == 1);
			minCores = (minCores > cores) ? cores : minCores;
			minMemory = (minMemory > memory) ? memory : minMemory;
		}
		vmwareTypeList[vmwareName].ratio = logf(float(cores) / float(memory));
	}
	//�����ʼK�������
	unsigned T, K;
	cin >> T;
	dayCounts = T;
	cin >> K;
	tempDay = 0;
	while (tempDay < K) {
		cin >> num;
		for (int j = 0; j < num; j++) {
			cin >> buff >> requestName;
			requestName = requestName.substr(0, requestName.size() - 1);
			unsigned int ID;
			if (requestName == "add") {
				cin >> vmwareName >> ID >> buff;
				vmwareName = vmwareName.substr(0, vmwareName.size() - 1);
				requestList[tempDay].emplace_back(true, vmwareName, ID);
				vmwareList[ID].id = ID;
				vmwareList[ID].myType = vmwareTypeList[vmwareName];
			}
			else {
				cin >> ID >> buff;
				requestList[tempDay].emplace_back(false, "None", ID);
			}
		}
		tempDay++;
	}
}
//����ÿһ�������
void DataManager::readRequests()
{
	if (tempDay == dayCounts) {
#if isVisual
		if(inStream)
			fclose(inStream);
#endif
		return;
	}
	int num;
	string vmwareName, requestName;
	char buff;
	cin >> num;
	for (int j = 0; j < num; j++) {
		cin >> buff >> requestName;
		requestName = requestName.substr(0, requestName.size() - 1);
		unsigned int ID;
		if (requestName == "add") {
			cin >> vmwareName >> ID >> buff;
			vmwareName = vmwareName.substr(0, vmwareName.size() - 1);
			requestList[tempDay].emplace_back(true, vmwareName, ID);
			vmwareList[ID].id = ID;
			vmwareList[ID].myType = vmwareTypeList[vmwareName];
		}
		else {
			cin >> ID >> buff;
			requestList[tempDay].emplace_back(false, "None", ID);
		}
	}
	tempDay++;
	return;
}


/*
	��purchaseList��moveList��addList��ÿ���˳�����
*/
void DataManager::myWrite(unsigned i)
{
#if isVisual
	if (i == 0) {
		freopen_s(&outStream, "./result.txt", "w", stdout);
	}
	else {
		freopen_s(&outStream, "./result.txt", "a+", stdout);
	}
#endif
	//������������
	cout << "(purchase, " << purchaseList[i].size() << ")" << endl;
	for (auto j = purchaseList[i].cbegin(); j != purchaseList[i].cend(); j++) {
		cout << "(" + j->first + ", " << j->second << ")" << endl;
	}
	//���Ǩ�������
	cout << "(migration, " << moveList[i].size() << ")" << endl;
	for (auto j = moveList[i].cbegin(); j != moveList[i].cend(); j++) {
		if (j->isDouble) {
			cout << "(" << j->vmwareID << ", " << serverIDList[j->serverID] << ")" << endl;
		}
		else {
			if (j->isNodeA)
				cout << "(" << j->vmwareID << ", " << serverIDList[j->serverID] << ", A)" << endl;
			else
				cout << "(" << j->vmwareID << ", " << serverIDList[j->serverID] << ", B)" << endl;
		}
	}
	//�����������
	for (auto j = addList[i].cbegin(); j != addList[i].cend(); j++) {

		if (j->isDouble) {
			cout << "(" << serverIDList[j->serverID] << ")" << endl;
		}
		else {
			if (j->isNodeA)
				cout << "(" << serverIDList[j->serverID] << ", A)" << endl;
			else
				cout << "(" << serverIDList[j->serverID] << ", B)" << endl;
		}
	}
#if isVisual
	freopen_s(&outStream, "CON", "w", stdout);
	if (i == dayCounts && outStream != NULL)
		fclose(outStream);
#endif	
}
/*
  ����data��Ϣ�����ÿһ�������������б�(������ID,�������ͺ�,���ڵ������,���ڵ��ڴ���,�ܳɱ�,ÿ��ɱ�)��
  ÿһ������������仯(������ID,A�ڵ㵱ǰ����,A�ڵ㵱ǰ�ڴ�,B�ڵ㵱ǰ����,B�ڵ㵱ǰ�ڴ�)��
  ������ÿһ��moveList��requestListΪ��λ����仯��Ϣ��ÿ����һ������������Ǩ�ơ�ɾ������ӵȲ���ʱ��Ҫ��һ���仯��Ϣ��
*/
void DataManager::OutputVisual()
{
#if isVisual
	FILE* stream;
	freopen_s(&stream, "E:/Programming/visualize/output.txt", "w", stdout);
	cout << dayCounts << endl;
	for (unsigned int i = 0; i < dayCounts; i++) {
		//�������������
		cout << newList[i].size() << endl;
		for (unsigned int j = 0; j < newList[i].size(); j++) {
			ServerType it = newList[i][j].GetServerType();
			unsigned int serverID = newList[i][j].GetID();
			cout << "(" << serverID << "," << it.name << "," << it.cores << "," << it.memory << "," << it.price << "," << it.costPerDay << ")" << endl;
		}
		//����������仯
		cout << changeList[i].size() << endl;
		for (unsigned int j = 0; j < changeList[i].size(); j++) {
			ChangeData it = changeList[i][j];
			cout << "(" << it.serverID << "," << it.coresA << "," << it.memoryA << "," << it.coresB << "," << it.memoryB << ")" << endl;
		}
	}
	fclose(stream);
#endif 
}


/********************************************************
���������ܡ����ݶ�ȡ������Ϣ���г�ʼ������ȡperformance��pfmList��ratioList
������������dayCounts��������
��ע���Լ۱ȼ��㹫ʽ��(price + costPerDay * daycounts * ��) * (1 / cores + 1 / memory) + �� * max(cores /memory��memory / cores��
�����������ʽ�����������ֵԽС�Լ۱�Խ�ߣ����ͦ±�ʾ�ɵ�������
**********************************************************/
void DataManager::init(unsigned int dayCounts)
{
	//�������������
	vmSize = 0;
	//��ȡ�Լ۱�
	for (auto i = serverTypeList.cbegin(); i != serverTypeList.cend(); i++) {
		/*	performance[i->first] = double(i->second.price + int(i->second.costPerDay) * int(dayCounts) * 0.8)
				/ double(i->second.cores) + double(i->second.price + int(i->second.costPerDay) * int(dayCounts) * 0.8)
				/ double(i->second.memory);*/
		performance[i->first] = double(i->second.price + int(i->second.costPerDay) * int(dayCounts) * 0.8)
			/ double(i->second.cores) + double(i->second.price + int(i->second.costPerDay) * int(dayCounts) * 0.8)
			/ double(i->second.memory) + 0 * max(i->second.cores / i->second.memory, i->second.memory / i->second.cores);
		pfmList.emplace_back(i->first);
		ratioList.emplace_back(i->first);
	}
	//ð�����򣬵õ��Լ۱ȵݼ��ķ������б�
	for (int i = 0; i < pfmList.size() - 1; i++) {
		for (int j = 0; j < pfmList.size() - 1 - i; j++) {
			if (performance[pfmList[j]] > performance[pfmList[j + 1]]) {
				string temp = pfmList[j];
				pfmList[j] = pfmList[j + 1];
				pfmList[j + 1] = temp;
			}
		}
	}
	//ð�����򣬵õ��˴�ȵ����ķ������б�
	for (int i = 0; i < ratioList.size() - 1; i++) {
		for (int j = 0; j < ratioList.size() - 1 - i; j++) {
			if (serverTypeList[ratioList[j]].ratio > serverTypeList[ratioList[j + 1]].ratio) {
				string temp = ratioList[j];
				ratioList[j] = ratioList[j + 1];
				ratioList[j + 1] = temp;
			}
		}
	}
}