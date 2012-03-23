// ≥£¡ø∫Õ≈‰÷√
// œ…Ω£–ﬁ£¨2002.9.14
//////////////////////////////////////////////////////////////////////

#include "common.h"
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include "BaseFunc.h"

/////////////////////////////////////////////////////////////////////////////
// function
/////////////////////////////////////////////////////////////////////////////
bool	SafeCopy(char * pTarget, const char * pSource, int nBufLen)
{
	try{
		IF_OK(pTarget)
		{
			pTarget[0] = 0;
			IF_OK(pSource)
			{
				if(nBufLen && (int)strlen(pSource) >= nBufLen)
				{
					strncpy(pTarget, pSource, nBufLen-1);
					pTarget[nBufLen-1] = 0;
					return false;
				}
				strcpy(pTarget, pSource);
				return true;
			}
		}
	}catch(...){ ::LogSave("CATCH: *** SafeCopy() crash! ***\n"); }
	
	assert(!"SafeCopy()");
	return false;
}

bool AddChar(char* pTarget, char cData, int nBufLen)
{
	try{
		if(pTarget && cData)
		{
			int nSize = strlen(pTarget);
			if(nBufLen && nSize >= nBufLen - 1)
				return false;
			pTarget[nSize] = cData;
			pTarget[nSize+1] = 0;
			return true;
		}
	}catch(...){ ::LogSave("CATCH: *** AddChar() crash! ***\n"); }
	
	assert(!"AddChar()");
	return false;
}





