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

bool Server::AddVmware(unsigned int vmwareid, bool addToA = true)
{
	Vmware& myvmware = dataManager.vmwareList[vmwareid];
	VmwareType vmware = myvmware.myType;
	if (vmware.isDouble) {
		if (nodeA.unusedCores >= vmware.cores&&nodeA.unusedMemory >= vmware.memory&&nodeB.unusedCores >= vmware.cores&&nodeB.unusedMemory >= vmware.memory) {
			AddIntoNode(vmware.cores, vmware.memory, nodeA);
			AddIntoNode(vmware.cores, vmware.memory, nodeB);
			nodeA.vmwares[vmwareid] = myvmware;
			nodeB.vmwares[vmwareid] = myvmware;
			myvmware.serverID = id;
			return true;
		}
	}
	else if(addToA){
		if (nodeA.unusedCores >= vmware.cores&&nodeA.unusedMemory >= vmware.memory) {
			AddIntoNode(vmware.cores, vmware.memory, nodeA);
			nodeA.vmwares[vmwareid] = myvmware;
			myvmware.serverID = id;
			myvmware.isNodeA = true;
			return true;
		}
	}
	else {
		if (nodeB.unusedCores >= vmware.cores&&nodeB.unusedMemory >= vmware.memory) {
			AddIntoNode(vmware.cores, vmware.memory, nodeB);
			nodeB.vmwares[vmwareid] = myvmware;
			myvmware.serverID = id;
			myvmware.isNodeA = false;
			return true;
		}
	}
	return false;
}

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
