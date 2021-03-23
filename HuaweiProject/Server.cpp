#include "Server.h"
#include "DataManager.h"

unsigned int Server::count = 0;

void Server::AddIntoNode(unsigned int cores, unsigned int memory, NodeData& node)
{
	node.unusedCores -= cores;
	node.unusedMemory -= memory;
	node.usedCores += cores;
	node.usedMemory += memory;
}

bool Server::DeleteIntoNode(Vmware& vmware, NodeData& node)
{
	auto loca = find(node.vmwares.begin(), node.vmwares.end(), vmware.id);
	if (loca == node.vmwares.cend())
		return false;
	node.unusedCores += vmware.myType.cores;
	node.unusedMemory += vmware.myType.memory;
	node.usedCores -= vmware.myType.cores;
	node.usedMemory -= vmware.myType.memory;
	node.vmwares.erase(loca);
	return true;
}


Server::Server(ServerType serverType):myType(serverType)
{
	id = count++;
	nodeA = { 0,myType.cores,0,myType.memory };
	nodeB = { 0,myType.cores,0,myType.memory };
	usuage = nodeA.usedCores + nodeA.usedMemory + nodeB.usedCores + nodeB.usedMemory;
}

bool Server::AddVmwareA(unsigned int vmwareid)
{
	Vmware& myVmware = dataManager.vmwareList[vmwareid];
	VmwareType& type = myVmware.myType;
	AddIntoNode(type.cores, type.memory, nodeA);
	nodeA.vmwares.emplace_back(vmwareid);
	myVmware.serverID = id;
	myVmware.isNodeA = true;
	//更新使用程度
	usuage = nodeA.usedCores + nodeA.usedMemory + nodeB.usedCores + nodeB.usedMemory;
	return true;
}
bool Server::AddVmwareB(unsigned int vmwareid)
{
	Vmware& myVmware = dataManager.vmwareList[vmwareid];
	VmwareType& type = myVmware.myType;
	AddIntoNode(type.cores, type.memory, nodeB);
	nodeB.vmwares.emplace_back(vmwareid);
	myVmware.serverID = id;
	myVmware.isNodeA = false;
	//更新使用程度
	usuage = nodeA.usedCores + nodeA.usedMemory + nodeB.usedCores + nodeB.usedMemory;
	return true;
}
bool Server::AddVmwareD(unsigned int vmwareid)
{
	Vmware& myVmware = dataManager.vmwareList[vmwareid];
	VmwareType& type = myVmware.myType;
	AddIntoNode(type.cores, type.memory, nodeA);
	AddIntoNode(type.cores, type.memory, nodeB);
	nodeA.vmwares.emplace_back(vmwareid);
	nodeB.vmwares.emplace_back(vmwareid);
	myVmware.serverID = id;
	//更新使用程度
	usuage = nodeA.usedCores + nodeA.usedMemory + nodeB.usedCores + nodeB.usedMemory;
	return true;
}
//bool Server::AddVmware(unsigned int vmwareid, int mode)
//{
//	Vmware& myVmware = dataManager.vmwareList[vmwareid];
//	VmwareType type = myVmware.myType;
//	if (mode == 0) {
//		AddIntoNode(type.cores, type.memory, nodeA);
//		nodeA.vmwares[vmwareid] = myVmware;
//		myVmware.serverID = id;
//		myVmware.isNodeA = true;
//		return true;
//	}
//	else if (mode == 1) {
//		AddIntoNode(type.cores, type.memory, nodeB);
//		nodeA.vmwares[vmwareid] = myVmware;
//		myVmware.serverID = id;
//		myVmware.isNodeA = false;
//		return true;
//	}
//	else if (mode == 2) {
//		AddIntoNode(type.cores, type.memory, nodeA);
//		AddIntoNode(type.cores, type.memory, nodeB);
//		nodeA.vmwares[vmwareid] = myVmware;
//		nodeB.vmwares[vmwareid] = myVmware;
//		myVmware.serverID = id;
//		return true;
//	}
//	else
//		return false;
//}

bool Server::isEmpty()
{
	return nodeA.usedCores == 0 && nodeA.usedMemory == 0 && nodeB.usedCores == 0 && nodeB.usedMemory == 0;
}

bool Server::DeleteVmware(unsigned int vmwareid)
{
	Vmware& myvmware = dataManager.vmwareList[vmwareid];
	VmwareType vmware = myvmware.myType;
	bool success = false;
	if (vmware.isDouble) {
		success = (DeleteIntoNode(myvmware, nodeA) && DeleteIntoNode(myvmware, nodeB));
	}
	else if (myvmware.isNodeA) {
		success = DeleteIntoNode(myvmware, nodeA);
	}
	else {
		success = DeleteIntoNode(myvmware, nodeB);
	}
	//更新使用程度
	usuage = nodeA.usedCores + nodeA.usedMemory + nodeB.usedCores + nodeB.usedMemory;
	return success;
}