//输出所有已安装程序的卸载命令
//c:\>uins.exe|find /i "photoshop"
//得到：
//7)|Adobe Photoshop CS2| |msiexec /I {236BB7C4-4419-42FD-0804-1E257A25E34D}

#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <locale.h>

#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383
#define BUF260 260

void QueryKey(HKEY hKey) 
{ 
	TCHAR    achKey[MAX_KEY_LENGTH];   
	DWORD    cbName;                   
	TCHAR    achClass[MAX_PATH] = TEXT("");  
	DWORD    cchClassName = MAX_PATH;  
	DWORD    cSubKeys=0;               
	DWORD    cbMaxSubKey;              
	DWORD    cchMaxClass;              
	DWORD    cValues;              
	DWORD    cchMaxValue;          
	DWORD    cbMaxValueData;       
	DWORD    cbSecurityDescriptor; 
	FILETIME ftLastWriteTime;      

	DWORD i, retCode; 
	DWORD cchValue = MAX_VALUE_NAME; 

	// Get the class name and the value count. 
	retCode = RegQueryInfoKey(
		hKey,                    
		achClass,                
		&cchClassName,           
		NULL,                    
		&cSubKeys,               
		&cbMaxSubKey,            
		&cchMaxClass,            
		&cValues,                
		&cchMaxValue,            
		&cbMaxValueData,         
		&cbSecurityDescriptor,   
		&ftLastWriteTime);       

	if (cSubKeys)
	{        

		for (i=0; i<cSubKeys; i++) 
		{ 
			cbName = MAX_KEY_LENGTH;
			retCode = RegEnumKeyEx(hKey, i,
				achKey, 
				&cbName, 
				NULL, 
				NULL, 
				NULL, 
				&ftLastWriteTime); 
			if (retCode == ERROR_SUCCESS) 
			{
				_tprintf(TEXT("%d)"), i+1);
				HKEY h = NULL;
				BYTE byteData[BUF260];
				DWORD dwDataSize=BUF260;
				DWORD dwType = NULL;

				if(ERROR_SUCCESS == RegOpenKeyEx(hKey,achKey,NULL,KEY_READ,&h)){
					LONG lResult = RegQueryValueEx(h,_T("DisplayName"),NULL,&dwType,byteData,&dwDataSize);
					_tprintf(TEXT("|%s"),byteData);

					ZeroMemory(byteData,sizeof(byteData));
					dwDataSize = BUF260;
					lResult = RegQueryValueEx(h, _T("Comments"),NULL,&dwType,byteData,&dwDataSize);
					_tprintf(TEXT("|%s"),byteData);

					ZeroMemory(byteData,sizeof(byteData));
					dwDataSize = BUF260;
					lResult = RegQueryValueEx(h, _T("UninstallString"),NULL,&dwType,byteData,&dwDataSize);
					_tprintf(TEXT("|%s\n"),byteData);
				}
			}
		}
		printf( "\nNumber of installed Software: %d\n", cSubKeys);
	} 
}

void _tmain(int ac,TCHAR* av[])
{
	//避免输出unicode乱码
	setlocale(LC_ALL ,"chs");

	HKEY hTestKey;

	if( RegOpenKeyEx( HKEY_LOCAL_MACHINE,
		TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall"),
		0,
		KEY_READ,
		&hTestKey) == ERROR_SUCCESS
		)
	{
		QueryKey(hTestKey);
	}
}
