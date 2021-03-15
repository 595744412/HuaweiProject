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
			myvmware.serverID = id;
			nodeA.vmwares[vmwareid] = myvmware;
			nodeB.vmwares[vmwareid] = myvmware;
			return true;
		}
	}
	else if(addToA){
		if (nodeA.unusedCores >= vmware.cores&&nodeA.unusedMemory >= vmware.memory) {
			AddIntoNode(vmware.cores, vmware.memory, nodeA);
			myvmware.serverID = id;
			myvmware.isNodeA = true;
			nodeA.vmwares[vmwareid] = myvmware;
			return true;
		}
	}
	else {
		if (nodeB.unusedCores >= vmware.cores&&nodeB.unusedMemory >= vmware.memory) {
			AddIntoNode(vmware.cores, vmware.memory, nodeB);
			myvmware.serverID = id;
			myvmware.isNodeA = false;
			nodeB.vmwares[vmwareid] = myvmware;
			return true;
		}
	}
	return false;
}

bool Server::DeleteVmware(unsigned int vmwareid)
{
	Vmware& myvmware = dataManager.vmwareList[vmwareid];
	VmwareType vmware = myvmware.myType;
	if (vmware.isDouble) {
		if (nodeA.vmwares.count(vmwareid) && nodeB.vmwares.count(vmwareid)) {
			DeleteIntoNode(vmware.cores, vmware.memory, nodeA);
			DeleteIntoNode(vmware.cores, vmware.memory, nodeB);
			nodeA.vmwares.erase(vmwareid);
			nodeB.vmwares.erase(vmwareid);
		}
	}
	else if (myvmware.isNodeA) {
		if (nodeA.vmwares.count(vmwareid)) {
			DeleteIntoNode(vmware.cores, vmware.memory, nodeA);
			nodeA.vmwares.erase(vmwareid);
		}
	}
	else {
		if (nodeB.vmwares.count(vmwareid)) {
			DeleteIntoNode(vmware.cores, vmware.memory, nodeB);
			nodeB.vmwares.erase(vmwareid);
		}
	}
	return false;
}
