#include "StringFunc.h"
#include <iostream>
#include <assert.h>

using namespace std;

//找到两个字符串中间的字符
bool GetFirstSubString(const std::string& str, const std::string& strHead, const std::string& strTail, std::string strSub)
{
	string::size_type szHead = str.find(strHead, 0);
	if(szHead == string::npos)
		return false;

	string::size_type szTail = str.find(strTail, szHead+1);
	if(szTail == string::npos)
		return false;

	if(szTail <= szHead)
	{
		assert(false);
		return false;
	}

	strSub = str.substr(szHead+1, szTail-szHead-1);

	return true;
}

bool ParsePathString()
{
	const string pathname = "D:\\demo\\xswrun\\BugLint\\BugLint/BugLint.vcproj";

	// 识别最后一个'\'或者'/'的位置
	string::size_type backslashIndex = pathname.find_last_of('\\');
	string::size_type backslashIndex2 = pathname.find_last_of('/');
	backslashIndex = max(backslashIndex, backslashIndex2);

	//路径名是最后一个'\'之前的字符
	const string path = pathname.substr(0,backslashIndex);

	// 路径名尾部是文件名
	const std::string filename = pathname.substr(backslashIndex+1,-1);

	// 扩展名。
	// 首先找到最后一个'.'的位置。 如果
	// 没有'.'，则dotIndex为-1
	const int dotIndex = filename.find_last_of('.');
	//测试是否有'.'，其余的字符是否为"cpp"
	if (dotIndex != -1 )
	{
		const std::string ext = filename.substr(dotIndex+1, string::npos);
	}

	return 0;
}
