/* recon.h */
/* recon.h anything related to information gathering */

// NOTE: identifier for the victim machine
// generateId() 

// getComputerName()

// getUsername()

// getLocalIp()


// NOTE: is os linux, macos, windows? 32 or 64 bit
// getOsVersion()

// getRam()

// getProccessor/number threads

// NOTE: information related to the process, stuff like if it's 32/64bit
// getProcessInfo() ?

// getUserLanguage / getKeyboardLanguage / getCodePageOEM

// NOTE: is the user an admin or something?
// getPermissionLevel() 


// getSystemInfo()


struct ReconMessage
{
	// _OSVERSIONINFOEXW
	ULONG dwMajorVersion;
	ULONG dwMinorVersion;
	ULONG dwBuildNumber;
	UCHAR wProductType;

	// ram
	// processor

	// localIp

	//this->username      = _GetUserName();
	//this->domain_name   = _GetDomainName();
	//this->computer_name = _GetHostName();
	//this->process_name  = _GetProcessName();

	//computername;
	//username;

} typedef ReconMessage;


/*
 * GetVersionExA()
 * GetComputerNameA()
 * GetUserNameA()
 * GetSystemInfo()
 * GlobalMemoryStatusEx()
 * GetCurrentDirectoryA()
 * GetSystemDirectoryA()
 *
 * */
