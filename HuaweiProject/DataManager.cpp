#include "DataManager.h"
/*
   ��˳���ȡ�������ݴ���serverTypeList��vmwareTypeList��requestList�У�
   ����ADD��������Ӷ�Ӧ��id��������������ã���ֵ����vmwareList��
*/
void DataManager::ReadAll()
{
	/*FILE *stream;
	freopen_s(&stream, "./training-1.txt", "r", stdin);*/
	int num;
	string serverName, vmwareName, requestName;
	unsigned int cores, memory, price, costPerDay, isDouble;
	char buff;
	//�����������Ϣ
	cin >> num;
	for (int i = 0; i < num; i++) {
		cin >> serverName >> cores >> buff >> memory >> buff >> price >> buff >> costPerDay >> buff;
		serverName = serverName.substr(1, size(serverName) - 2);
		serverTypeList[serverName] = ServerType(serverName, cores/2, memory/2, price, costPerDay);
	}
	//�����������Ϣ
	cin >> num;
	for (int i = 0; i < num; i++) {
		cin >> vmwareName >> cores >> buff >> memory >> buff >> isDouble >> buff;
		vmwareName = vmwareName.substr(1, size(vmwareName) - 2);
		vmwareTypeList[vmwareName] = VmwareType(vmwareName, cores, memory, isDouble == 1);
	}
	//��������
	int days;
	cin >> days;
	dayCounts = days;
	for (int i = 0; i < days; i++) {
		cin >> num;
		for (int j = 0; j < num; j++) {
			cin >> buff >> requestName;
			requestName = requestName.substr(0, size(requestName) - 1);
			unsigned int ID;
			if (requestName == "add") {
				cin >> vmwareName >> ID >> buff;
				vmwareName = vmwareName.substr(0, size(vmwareName) - 1);
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
	/*fclose(stdin);*/
}

/*
	��purchaseList��moveList��addList��ÿ���˳�����
*/
void DataManager::OutputAll()
{
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
				cout << "(" << j->vmwareID << ", " << j->serverID << ")" << endl;
			}
			else {
				if (j->isNodeA)
					cout << "(" << j->vmwareID << ", " << j->serverID << ", A)" << endl;
				else
					cout << "(" << j->vmwareID << ", " << j->serverID << ", B)" << endl;
			}
		}
		//�����������
		for (auto j = addList[i].cbegin(); j != addList[i].cend(); j++) {

			if (j->isDouble) {
				cout << "(" << j->serverID << ")" << endl;
			}
			else {
				if (j->isNodeA)
					cout << "(" << j->serverID << ", A)" << endl;
				else
					cout << "(" << j->serverID << ", B)" << endl;
			}
		}
	}
}
