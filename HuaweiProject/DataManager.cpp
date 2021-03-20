#include "DataManager.h"
#include "Server.h"


/*
   ��˳���ȡ�������ݴ���serverTypeList��vmwareTypeList��requestList�У�
   ����ADD��������Ӷ�Ӧ��id��������������ã���ֵ����vmwareList��
*/
void DataManager::ReadAll()
{
	FILE *stream;
	freopen_s(&stream, "./training-1.txt", "r", stdin);
	int num;
	string serverName, vmwareName, requestName;
	unsigned int cores, memory, price, costPerDay, isDouble;
	char buff;
	//�����������Ϣ
	cin >> num;
	for (int i = 0; i < num; i++) {
		cin >> serverName >> cores >> buff >> memory >> buff >> price >> buff >> costPerDay >> buff;
		serverName = serverName.substr(1, serverName.size() - 2);
		serverTypeList[serverName] = ServerType(serverName, cores/2, memory/2, price, costPerDay);
		float temp = float(cores) / float(memory);
		minRatioS = temp > minRatioS ? minRatioS : temp;
		maxRatioS = temp > maxRatioS ? temp : maxRatioS;
		serverTypeList[serverName].ratio = float(cores) / float(memory);
	}
	//�����������Ϣ
	cin >> num;
	for (int i = 0; i < num; i++) {
		cin >> vmwareName >> cores >> buff >> memory >> buff >> isDouble >> buff;
		vmwareName = vmwareName.substr(1, vmwareName.size() - 2);
		if (isDouble) {
			vmwareTypeList[vmwareName] = VmwareType(vmwareName, cores/2, memory/2, isDouble == 1);
		}
		else {
			vmwareTypeList[vmwareName] = VmwareType(vmwareName, cores, memory, isDouble == 1);
		}
		vmwareTypeList[vmwareName].ratio = float(cores) / float(memory);
	}
	//��������
	int days;
	cin >> days;
	dayCounts = days;
	for (int i = 0; i < days; i++) {
		cin >> num;
		for (int j = 0; j < num; j++) {
			cin >> buff >> requestName;
			requestName = requestName.substr(0, requestName.size() - 1);
			unsigned int ID;
			if (requestName == "add") {
				cin >> vmwareName >> ID >> buff;
				vmwareName = vmwareName.substr(0, vmwareName.size() - 1);
				requestList[i].emplace_back(true, vmwareName, ID);
				vmwareList[ID].id = ID;
				vmwareList[ID].myType = vmwareTypeList[vmwareName];
			}
			else {
				cin >> ID >> buff;
				requestList[i].emplace_back(false, " ", ID);
			}
		}
	}
	fclose(stdin);
}

/*
	��purchaseList��moveList��addList��ÿ���˳�����
*/
void DataManager::OutputAll()
{
	FILE* stream;
	freopen_s(&stream, "./result.txt", "w", stdout);
	for (unsigned int i = 0; i < dayCounts; i++) {
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
	}
	fclose(stdout);
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
	freopen_s(&stream, "output.txt", "w", stdout);
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
	fclose(stdout);
#endif 
}

/*
  �Է��������Լ۱ȡ�ratio�������򣬼��Աȼ��㹫ʽ��(price + costPerDay * daycounts * 0.8) * (1 / cores + 1 / memory)
*/
void DataManager::init(unsigned int dayCounts)
{	
	//��ȡ�Լ۱�
	for (auto i = serverTypeList.cbegin(); i != serverTypeList.cend(); i++) {
		performance[i->first] = double(i->second.price + int(i->second.costPerDay) * int(dayCounts) * 0.8)
			/ double(i->second.cores) + double(i->second.price + int(i->second.costPerDay) * int(dayCounts) * 0.8)
			/ double(i->second.memory);
		pfmList.emplace_back(i->first);
		ratioList.emplace_back(i->first);
	}
	//ð�����򣬵õ��Լ۱ȵݼ����б�
	for (int i = 0; i < pfmList.size() - 1; i++) {
		for (int j = 0; j < pfmList.size() - 1 - i; j++) {
			if (performance[pfmList[j]] > performance[pfmList[j + 1]]) {
				string temp = pfmList[j];
				pfmList[j] = pfmList[j + 1];
				pfmList[j + 1] = temp;
			}
		}
	}
	//ð�����򣬵õ�ratio�����ķ������б�
	for (int i = 0; i < pfmList.size() - 1; i++) {
		for (int j = 0; j < pfmList.size() - 1 - i; j++) {
			if (serverTypeList[pfmList[j]].ratio > serverTypeList[pfmList[j + 1]].ratio) {
				string temp = pfmList[j];
				pfmList[j] = pfmList[j + 1];
				pfmList[j + 1] = temp;
			}
		}
	}
}