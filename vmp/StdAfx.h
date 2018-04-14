



#ifndef     __STDAFX_H
#define     __STDAFX_H

#include <windows.h>
#include <winbase.h>
#include <wtypes.h>
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/locking.h>
#include <share.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>

#include <algorithm>
#include <string>
#include <set>
using namespace std;


#include "ProcessEsiEncode.h"
#include "REVmpFixed.h"
#include "file.h"
#include "REVmpDisassembly.h"


#ifndef STDIN_FILENO
#define STDIN_FILENO    (fileno(stdin))
#endif

#ifndef STDOUT_FILENO
#define STDOUT_FILENO    (fileno(stdout))
#endif

#ifndef STDERR_FILENO
#define STDERR_FILENO    (fileno(stderr))
#endif

enum
{
	PEDIR_EXPORT    = 0,
	PEDIR_IMPORT    = 1,
	PEDIR_RESOURCE  = 2,
	PEDIR_EXCEPTION = 3,            // Exception table
	PEDIR_SEC       = 4,            // Certificate table (file pointer)
	PEDIR_RELOC     = 5,
	PEDIR_DEBUG     = 6, 
	PEDIR_COPYRIGHT = 7,            // Architecture-specific data
	PEDIR_GLOBALPTR = 8,            // Global pointer
	PEDIR_TLS       = 9,
	PEDIR_LOADCONF  = 10,           // Load Config Table
	PEDIR_BOUNDIM   = 11,
	PEDIR_IAT       = 12,
	PEDIR_DELAYIMP  = 13,           // Delay Import Descriptor
	PEDIR_COMRT     = 14            // Com+ Runtime Header
};  

#endif      __STDAFX_H