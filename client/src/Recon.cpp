#include "recon.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

//int getClientInfo()
//{
//}

int generateReconMessage(ReconMessage* msg) // TODO: should return message
{
	if (!msg) return 1;

	ZeroMemory(msg, sizeof(ReconMessage));

	// OS Version Info
	OSVERSIONINFOEX osInfo;
	osInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	if (GetVersionEx((OSVERSIONINFO*)&osInfo)) {
		msg->dwMajorVersion = osInfo.dwMajorVersion;
		msg->dwMinorVersion = osInfo.dwMinorVersion;
		msg->dwBuildNumber = osInfo.dwBuildNumber;
		msg->wProductType = osInfo.wProductType;
	}

	// RAM Info
	MEMORYSTATUSEX memoryStatus;
	memoryStatus.dwLength = sizeof(memoryStatus);
	if (GlobalMemoryStatusEx(&memoryStatus)) {
		msg->totalPhysicalMemory = memoryStatus.ullTotalPhys;
		msg->availablePhysicalMemory = memoryStatus.ullAvailPhys;
	}

	// Processor Info
	SYSTEM_INFO systemInfo;
	GetSystemInfo(&systemInfo);
	msg->numberOfProcessors = systemInfo.dwNumberOfProcessors;
	msg->processorArchitecture = systemInfo.wProcessorArchitecture;

	// Get processor name from registry (simplified)
	//GetProcessorName(msg->processorName, sizeof(msg->processorName));

	// Local IP
	//GetLocalIpAddress(msg->localIp, sizeof(msg->localIp));

	// System Names
	//DWORD size = sizeof(msg->computerName);
	//GetComputerNameA(msg->computerName, &size);

	//size = sizeof(msg->userName);
	//GetUserNameA(msg->userName, &size);

	// Domain name (simplified)
	//GetDomainName(msg->domainName, sizeof(msg->domainName));

	// Process name
	//GetModuleFileNameA(NULL, msg->processName, sizeof(msg->processName));


	return 0;
}
