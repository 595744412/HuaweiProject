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
		serverName = serverName.substr(1, serverName.size() - 2);
		serverTypeList[serverName] = ServerType(serverName, cores/2, memory/2, price, costPerDay);
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
}

void DataManager::OutputVisual()
{
	//����data��Ϣ�����ÿһ�������������б�(������ID,�������ͺ�,���ڵ������,���ڵ��ڴ���,�ܳɱ�,ÿ��ɱ�)��ÿһ������������仯(������ID,A�ڵ㵱ǰ����,A�ڵ㵱ǰ�ڴ�,B�ڵ㵱ǰ����,B�ڵ㵱ǰ�ڴ�)��������ÿһ��moveList��requestListΪ��λ����仯��Ϣ��ÿ����һ������������Ǩ�ơ�ɾ������ӵȲ���ʱ��Ҫ��һ���仯��Ϣ��
}
