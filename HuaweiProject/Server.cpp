#include "Server.h"
#include "DataManager.h"

unsigned int Server::count = 0;

void Server::AddIntoNode(unsigned int cores, unsigned int memory, NodeData& node)
{
	node.unusedCores -= cores;
	node.usedCores += cores;
	node.unusedMemory -= memory;
	node.usedMemory += memory;
}

void Server::DeleteIntoNode(unsigned int cores, unsigned int memory, NodeData& node)
{
	node.unusedCores += cores;
	node.usedCores -= cores;
	node.unusedMemory += memory;
	node.usedMemory -= memory;
}


Server::Server(ServerType serverType):myType(serverType)
{
	id = count++;
	nodeA = { 0,myType.cores,0,myType.memory };
	nodeB = { 0,myType.cores,0,myType.memory };
}

bool Server::AddVmware(unsigned int id, bool addToA = true)
{
	Vmware& myvmware = dataManager.vmwareList[id];
	VmwareType vmware = myvmware.myType;
	if (vmware.isDouble) {
		if (nodeA.unusedCores >= vmware.cores&&nodeA.unusedMemory >= vmware.memory&&nodeB.unusedCores >= vmware.cores&&nodeB.unusedMemory >= vmware.memory) {
			AddIntoNode(vmware.cores, vmware.memory, nodeA);
			AddIntoNode(vmware.cores, vmware.memory, nodeB);
			nodeA.vmwares[id] = myvmware;
			nodeB.vmwares[id] = myvmware;
			myvmware.serverID = id;
			return true;
		}
	}
	else if(addToA){
		if (nodeA.unusedCores >= vmware.cores&&nodeA.unusedMemory >= vmware.memory) {
			AddIntoNode(vmware.cores, vmware.memory, nodeA);
			nodeA.vmwares[id] = myvmware;
			myvmware.serverID = id;
			myvmware.isNodeA = true;
			return true;
		}
	}
	else {
		if (nodeB.unusedCores >= vmware.cores&&nodeB.unusedMemory >= vmware.memory) {
			AddIntoNode(vmware.cores, vmware.memory, nodeB);
			nodeB.vmwares[id] = myvmware;
			myvmware.serverID = id;
			myvmware.isNodeA = false;
			return true;
		}
	}
	return false;
}

bool Server::DeleteVmware(unsigned int id)
{
	Vmware& myvmware = dataManager.vmwareList[id];
	VmwareType vmware = myvmware.myType;
	if (vmware.isDouble) {
		if (!nodeA.vmwares.count(id) && !nodeB.vmwares.count(id)) {
			DeleteIntoNode(vmware.cores, vmware.memory, nodeA);
			DeleteIntoNode(vmware.cores, vmware.memory, nodeB);
			nodeA.vmwares.erase(id);
			nodeB.vmwares.erase(id);
		}
	}
	else if (myvmware.isNodeA) {
		if (!nodeA.vmwares.count(id)) {
			DeleteIntoNode(vmware.cores, vmware.memory, nodeA);
			nodeA.vmwares.erase(id);
		}
	}
	else {
		if (!nodeB.vmwares.count(id)) {
			DeleteIntoNode(vmware.cores, vmware.memory, nodeB);
			nodeB.vmwares.erase(id);
		}
	}
	return false;
}
