#include "Server.h"
#include "DataManager.h"

unsigned int Server::count = 0;

void Server::AddIntoNode(unsigned int cores, unsigned int memory, NodeData& node)
{
	node.unusedCores -= cores;
	node.unusedMemory -= memory;
}

void Server::DeleteIntoNode(unsigned int cores, unsigned int memory, NodeData& node)
{
	node.unusedCores += cores;
	node.unusedMemory += memory;
}


Server::Server(ServerType serverType):myType(serverType)
{
	id = count++;
	nodeA = { 0,myType.cores,0,myType.memory };
	nodeB = { 0,myType.cores,0,myType.memory };
}

bool Server::AddVmwareA(unsigned int vmwareid)
{
	Vmware& myVmware = dataManager.vmwareList[vmwareid];
	VmwareType& type = myVmware.myType;
	AddIntoNode(type.cores, type.memory, nodeA);
	nodeA.vmwares[vmwareid] = myVmware;
	myVmware.serverID = id;
	myVmware.isNodeA = true;
	return true;
}
bool Server::AddVmwareB(unsigned int vmwareid)
{
	Vmware& myVmware = dataManager.vmwareList[vmwareid];
	VmwareType& type = myVmware.myType;
	AddIntoNode(type.cores, type.memory, nodeB);
	nodeA.vmwares[vmwareid] = myVmware;
	myVmware.serverID = id;
	myVmware.isNodeA = false;
	return true;
}
bool Server::AddVmwareD(unsigned int vmwareid)
{
	Vmware& myVmware = dataManager.vmwareList[vmwareid];
	VmwareType& type = myVmware.myType;
	AddIntoNode(type.cores, type.memory, nodeA);
	AddIntoNode(type.cores, type.memory, nodeB);
	nodeA.vmwares[vmwareid] = myVmware;
	nodeB.vmwares[vmwareid] = myVmware;
	myVmware.serverID = id;
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


void Server::DeleteVmware(unsigned int vmwareid)
{
	Vmware& myvmware = dataManager.vmwareList[vmwareid];
	VmwareType vmware = myvmware.myType;
	if (vmware.isDouble) {
		DeleteIntoNode(vmware.cores, vmware.memory, nodeA);
		DeleteIntoNode(vmware.cores, vmware.memory, nodeB);
		nodeA.vmwares.erase(vmwareid);
		nodeB.vmwares.erase(vmwareid);
	}
	else if (myvmware.isNodeA) {
		DeleteIntoNode(vmware.cores, vmware.memory, nodeA);
		nodeA.vmwares.erase(vmwareid);
	}
	else {
		DeleteIntoNode(vmware.cores, vmware.memory, nodeB);
		nodeB.vmwares.erase(vmwareid);
	}
}