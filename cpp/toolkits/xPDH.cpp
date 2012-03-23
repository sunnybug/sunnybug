
#include "xPDH.h"
#include <windows.h>
#include <malloc.h>
#include <stdio.h>
#include <pdh.h>
#include <pdhmsg.h>
#include <tchar.h>
#include "xString.h"

#pragma comment(lib, "pdh.lib")

const TCHAR* COUNTER_OBJECT = _T("Process");

std::string CPDH::GetPDHInfo(void)
{
	std::string str;

	PDH_STATUS status = ERROR_SUCCESS;
	LPTSTR pwsCounterListBuffer = NULL;
	DWORD dwCounterListSize = 0;
	LPTSTR pwsInstanceListBuffer = NULL;
	DWORD dwInstanceListSize = 0;
	LPTSTR pTemp = NULL;

	// Determine the required buffer size for the data. 
	status = PdhEnumObjectItems(
		NULL,                   // real-time source
		NULL,                   // local machine
		COUNTER_OBJECT,         // object to enumerate
		pwsCounterListBuffer,   // pass NULL and 0
		&dwCounterListSize,     // to get required buffer size
		pwsInstanceListBuffer, 
		&dwInstanceListSize, 
		PERF_DETAIL_WIZARD,     // counter detail level
		0); 

	if (status == PDH_MORE_DATA) 
	{
		// Allocate the buffers and try the call again.
		pwsCounterListBuffer = (LPTSTR)malloc(dwCounterListSize * sizeof(TCHAR));
		pwsInstanceListBuffer = (LPTSTR)malloc(dwInstanceListSize * sizeof(TCHAR));

		if (NULL != pwsCounterListBuffer && NULL != pwsInstanceListBuffer) 
		{
			status = PdhEnumObjectItems(
				NULL,                   // real-time source
				NULL,                   // local machine
				COUNTER_OBJECT,         // object to enumerate
				pwsCounterListBuffer, 
				&dwCounterListSize,
				pwsInstanceListBuffer, 
				&dwInstanceListSize, 
				PERF_DETAIL_WIZARD,     // counter detail level
				0); 

			if (status == ERROR_SUCCESS) 
			{
				_tprintf(_T("Counters that the Process objects defines:\n\n"));

				// Walk the counters list. The list can contain one
				// or more null-terminated strings. The list is terminated
				// using two null-terminator characters.
				for (pTemp = pwsCounterListBuffer; *pTemp != 0; pTemp += _tcslen(pTemp) + 1) 
				{
					str += pTemp;
					str += _T("\n");
				}

				str += (_T("\n===Instances of the Process object:\n"));

				// Walk the instance list. The list can contain one
				// or more null-terminated strings. The list is terminated
				// using two null-terminator characters.
				for (pTemp = pwsInstanceListBuffer; *pTemp != 0; pTemp += _tcslen(pTemp) + 1) 
				{
					str += pTemp;
					str += _T("\n");
				}
			}
			else 
			{
				_tprintf(_T("Second PdhEnumObjectItems failed with %0x%x.\n"), status);
			}
		} 
		else 
		{
			_tprintf(_T("Unable to allocate buffers.\n"));
			status = ERROR_OUTOFMEMORY;
		}
	} 
	else 
	{
		_tprintf(_T("\nPdhEnumObjectItems failed with 0x%x.\n"), status);
	}

	if (pwsCounterListBuffer != NULL) 
		free (pwsCounterListBuffer);

	if (pwsInstanceListBuffer != NULL) 
		free (pwsInstanceListBuffer);

	string_replace(str, "\n", "\r\n");
	return str;
}

CONST ULONG SAMPLE_INTERVAL_MS    = 1000;
CONST PTSTR BROWSE_DIALOG_CAPTION = _T("Select a counter to monitor.");

void CPDH::BrownsePDH()
{
	PDH_STATUS Status;
	HQUERY Query = NULL;
	HCOUNTER Counter;
	PDH_FMT_COUNTERVALUE DisplayValue;
	DWORD CounterType;
	SYSTEMTIME SampleTime;
	PDH_BROWSE_DLG_CONFIG BrowseDlgData;
	TCHAR CounterPathBuffer[PDH_MAX_COUNTER_PATH];

	// Create a query.
	Status = PdhOpenQuery(NULL, NULL, &Query);
	if (Status != ERROR_SUCCESS) 
	{
		_tprintf(_T("\nPdhOpenQuery failed with status 0x%x."), Status);
		goto Cleanup;
	}

	//
	// Initialize the browser dialog window settings.
	//

	ZeroMemory(&CounterPathBuffer, sizeof(CounterPathBuffer));
	ZeroMemory(&BrowseDlgData, sizeof(PDH_BROWSE_DLG_CONFIG));

	BrowseDlgData.bIncludeInstanceIndex = FALSE;
	BrowseDlgData.bSingleCounterPerAdd = TRUE;
	BrowseDlgData.bSingleCounterPerDialog = TRUE;
	BrowseDlgData.bLocalCountersOnly = FALSE;
	BrowseDlgData.bWildCardInstances = TRUE;
	BrowseDlgData.bHideDetailBox = TRUE;
	BrowseDlgData.bInitializePath = FALSE;
	BrowseDlgData.bDisableMachineSelection = FALSE;
	BrowseDlgData.bIncludeCostlyObjects = FALSE;
	BrowseDlgData.bShowObjectBrowser = FALSE;
	BrowseDlgData.hWndOwner = NULL;
	BrowseDlgData.szReturnPathBuffer = CounterPathBuffer;
	BrowseDlgData.cchReturnPathLength = PDH_MAX_COUNTER_PATH;
	BrowseDlgData.pCallBack = NULL;
	BrowseDlgData.dwCallBackArg = 0;
	BrowseDlgData.CallBackStatus = ERROR_SUCCESS;
	BrowseDlgData.dwDefaultDetailLevel = PERF_DETAIL_WIZARD;
	BrowseDlgData.szDialogBoxCaption = BROWSE_DIALOG_CAPTION;

	//
	// Display the counter browser window. The dialog is configured
	// to return a single selection from the counter list.
	//

	Status = PdhBrowseCounters(&BrowseDlgData);

	if (Status != ERROR_SUCCESS) 
	{
		if (Status == PDH_DIALOG_CANCELLED) 
		{
			_tprintf(_T("\nDialog canceled by user."));
		}
		else 
		{
			_tprintf(_T("\nPdhBrowseCounters failed with status 0x%x."), Status);
		}
		goto Cleanup;
	} 
	else if (_tcslen(CounterPathBuffer) == 0) 
	{
		_tprintf(_T("\nUser did not select any counter."));
		goto Cleanup;
	}
	else
	{
		_tprintf(_T("\nCounter selected: %s\n"), CounterPathBuffer);
	}

	//
	// Add the selected counter to the query.
	//

	Status = PdhAddCounter(Query, CounterPathBuffer, 0, &Counter);
	if (Status != ERROR_SUCCESS) 
	{
		_tprintf(_T("\nPdhAddCounter failed with status 0x%x."), Status);
		goto Cleanup;
	}

	//
	// Most counters require two sample values to display a formatted value.
	// PDH stores the current sample value and the previously collected
	// sample value. This call retrieves the first value that will be used
	// by PdhGetFormattedCounterValue in the first iteration of the loop
	// Note that this value is lost if the counter does not require two
	// values to compute a displayable value.
	//

	Status = PdhCollectQueryData(Query);
	if (Status != ERROR_SUCCESS) 
	{
		_tprintf(_T("\nPdhCollectQueryData failed with 0x%x.\n"), Status);
		goto Cleanup;
	}

	//
	// Print counter values until a key is pressed.
	//

	while (true) 
	{
		Sleep(SAMPLE_INTERVAL_MS);

		GetLocalTime(&SampleTime);

		Status = PdhCollectQueryData(Query);
		if (Status != ERROR_SUCCESS) 
		{
			_tprintf(_T("\nPdhCollectQueryData failed with status 0x%x."), Status);
		}

		_tprintf(_T("\n\"%2.2d/%2.2d/%4.4d %2.2d:%2.2d:%2.2d.%3.3d\""),
			SampleTime.wMonth,
			SampleTime.wDay,
			SampleTime.wYear,
			SampleTime.wHour,
			SampleTime.wMinute,
			SampleTime.wSecond,
			SampleTime.wMilliseconds);

		//
		// Compute a displayable value for the counter.
		//

		Status = PdhGetFormattedCounterValue(Counter,
			PDH_FMT_DOUBLE,
			&CounterType,
			&DisplayValue);
		if (Status != ERROR_SUCCESS) 
		{
			_tprintf(_T("\nPdhGetFormattedCounterValue failed with status 0x%x."), Status);
			goto Cleanup;
		}

		_tprintf(_T(",\"%.20g\""), DisplayValue.doubleValue);
	}

Cleanup:

	//
	// Close the query.
	//

	if (Query) 
	{
		PdhCloseQuery(Query);
	}
}
