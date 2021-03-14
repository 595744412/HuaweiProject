#include "Server.h"


void Server::AddIntoCore(unsigned int cores, unsigned int memory, CoreData & core)
{
	core.unusedCores -= cores;
	core.usedCores += cores;
	core.unusedMemory -= memory;
	core.usedMemory += memory;
}

void Server::DeleteIntoCore(unsigned int cores, unsigned int memory, CoreData & core)
{
	core.unusedCores += cores;
	core.usedCores -= cores;
	core.unusedMemory += memory;
	core.usedMemory -= memory;
}

bool Server::AddVmware(unsigned int id, bool addToA = true)
{
	Vmware& myvmware = dataManager.vmwareList[id];
	VmwareType& vmware = myvmware.myType;
	if (vmware.isDouble) {
		if (coreA.unusedCores >= vmware.cores&&coreA.unusedMemory >= vmware.memory&&coreB.unusedCores >= vmware.cores&&coreB.unusedMemory >= vmware.memory) {
			AddIntoCore(vmware.cores, vmware.memory, coreA);
			AddIntoCore(vmware.cores, vmware.memory, coreB);
			coreA.vmwares.push_back(id);
			coreB.vmwares.push_back(id);
			myvmware.serverID = id;
			return true;
		}
	}
	else if(addToA){
		if (coreA.unusedCores >= vmware.cores&&coreA.unusedMemory >= vmware.memory) {
			AddIntoCore(vmware.cores, vmware.memory, coreA);
			coreA.vmwares.push_back(id);
			myvmware.serverID = id;
			return true;
		}
	}
	else {
		if (coreB.unusedCores >= vmware.cores&&coreB.unusedMemory >= vmware.memory) {
			AddIntoCore(vmware.cores, vmware.memory, coreB);
			coreB.vmwares.push_back(id);
			myvmware.serverID = id;
			return true;
		}
	}
	return false;
}

bool Server::DeleteVmware(unsigned int id)
{

	return false;
}
