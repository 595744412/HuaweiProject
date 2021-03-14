#include "DataManager.h"
/*
   ��˳���ȡ�������ݴ���serverTypeList��vmwareTypeList��requestList�У�
   ����ADD��������Ӷ�Ӧ��id��������������ã���ֵ����vmwareList��
   ����ύ�Ļ�����ǰ���У������һ��ע�͵���
*/
void DataManager::ReadAll()
{
	string filePath = "./training-1.txt";
	FILE *stream;
	freopen_s(&stream, filePath.c_str(), "r" , stdin);
	int num;
	string serverName, vmwareName, requestName;
	unsigned int cores, memory, price, costPerDay, isDouble;
	char buff;
	//�����������Ϣ
	cin >> num;
	for (int i = 0; i < num; i++) {
		cin >> serverName >> cores >> buff >> memory >> buff >> price >> buff >> costPerDay >> buff;
		serverName = serverName.substr(1, size(serverName) - 2);
		this->serverTypeList[serverName] = ServerType(serverName, cores, memory, price, costPerDay);
	}
	//�����������Ϣ
	cin >> num;
	for (int i = 0; i < num; i++) {
		cin >> vmwareName >> cores >> buff >> memory >> buff >> isDouble >> buff;
		vmwareName = vmwareName.substr(1, size(vmwareName) - 2);
		this->vmwareTypeList[vmwareName] = VmwareType(vmwareName, cores, memory, isDouble == 1);
	}
	//��������
	int days;
	cin >> days;
	for (int i = 0; i < days; i++) {
		cin >> num;
		for (int j = 0; j < num; j++) {
			cin >> requestName;
			requestName = vmwareName.substr(1, size(vmwareName) - 2);
			int ID;
			if (requestName == "add") {
				cin >> vmwareName >> ID >> buff;
				vmwareName = vmwareName.substr(0, size(vmwareName) - 1);
				this->requestList[i].emplace_back(true, vmwareName, ID);
			}
			else {
				cin >> ID >> buff;
				this->requestList[i].emplace_back(false, " ", ID);
			}
		}
	}
	fclose(stdin);
}