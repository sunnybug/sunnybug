#include "xString.h"

void  string_replace( std::string &strBig, const std::string &strsrc, const std::string &strdst )  
{  
	std::string::size_type pos = 0;  
	std::string::size_type srclen = strsrc.size();  
	std::string::size_type dstlen = strdst.size();  

	while( (pos=strBig.find(strsrc, pos)) != std::string::npos )  
	{  
		strBig.replace( pos, srclen, strdst );  
		pos += dstlen;  
	}  
}  

