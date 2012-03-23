// 通用工具
// 仙剑修，2002.9.14
//////////////////////////////////////////////////////////////////////

#ifndef	COMMON_TYPEDEF_HEADER
#define	COMMON_TYPEDEF_HEADER

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "common.h"
#pragma warning(disable:4786)
#include <string>
using namespace std;

/*/////////////////////////////////////////////////////////////////////
template<TYPENAME T, int SZ, int D=0>
struct Buffer{
	Buffer()					{ memset(m_buf, D, sizeof(T)*SZ); }
	T& operator[](int idx)		{ IF_OK(idx>=0 && idx<SZ); return m_buf[idx]; return m_buf[SZ]; }
protected:
	T		m_buf[SZ+1];
};*/

//////////////////////////////////////////////////////////////////////
struct true_type {};
struct false_type {};

//////////////////////////////////////////////////////////////////////
const int		STRING_FORMAT_SIZE				= 4096;
class String
{
public:
	String()												{}
	String(const char* str)									{ IF_OK(str) m_string = str; }
	String(const String& obj)								{ m_string = obj.m_string; }
	String& operator=(const String& obj)					{ CHECK_(*this, &obj != this); m_string = obj.m_string; return *this; }
	String& operator=(const char* str)						{ IF_OK(str) m_string = str; else m_string = ""; return *this; }
	operator const char*() const							{ return m_string.c_str(); }
protected:
	String(string& s)										{ m_string = s; }

public:
	bool	operator<(const String& rhs) const				{ return m_string < rhs.m_string; }
	bool	operator==(const String& rhs) const				{ return m_string == rhs.m_string; }
	String&	operator+=(const String& rhs)					{ m_string += rhs.m_string; return *this; }
	String&	operator+=(char ch) 							{ m_string += ch; return *this; }
	String	operator+(const String& rhs) const				{ return m_string + rhs.m_string; }
	int		Int()											{ return atol(m_string.c_str()); }
	void	Format(const char *format, ...)					{ CHECK(format); char buf[STRING_FORMAT_SIZE]; 
																va_list argptr; va_start( argptr, format );     
																try{ ASSERT(vsprintf(buf, format, argptr) < STRING_FORMAT_SIZE); 
																}catch(...){ LogSave("String::Format"); } m_string=buf; }
	// 作废
	const String&	FormatLong(long nNum)					{ char buf[]="-2147483648"; m_string = ltoa(nNum, buf, 10); return *this; }
	const String&	FormatDword(DWORD nNum)					{ char buf[]="-2147483648"; m_string = ultoa(nNum, buf, 10); return *this; }
	string&	QueryString()									{ return m_string; }

public:
	static String	ToString(int nNum)						{ char buf[]="-2147483648"; ltoa(nNum, buf, 10); return String(buf); }

protected:
	std::string		m_string;
};


//////////////////////////////////////////////////////////////////////
template<TYPENAME T>
struct dump_traits { static String Dump(const T& obj)		{ return "?"; } };
template<>
struct dump_traits<char> { static String Dump(const char& obj)		{ return String().FormatLong(obj); } };
template<>
struct dump_traits<short> { static String Dump(const short& obj)	{ return String().FormatLong(obj); } };
template<>
struct dump_traits<int> { static String Dump(const int& obj)		{ return String().FormatLong(obj); } };
template<>
struct dump_traits<long> { static String Dump(const long& obj)		{ return String().FormatLong(obj); } };
template<>
struct dump_traits<unsigned char> { static String Dump(const unsigned char& obj)	{ return String().FormatLong(obj); } };
template<>
struct dump_traits<unsigned short> { static String Dump(const unsigned short& obj)	{ return String().FormatLong(obj); } };
template<>
struct dump_traits<unsigned int> { static String Dump(const unsigned int& obj)		{ return String().FormatLong(obj); } };
template<>
struct dump_traits<unsigned long> { static String Dump(const unsigned long& obj)	{ return String().FormatLong(obj); } };
template<>
struct dump_traits<char*> { static String Dump(const char*& obj)		{ return String(obj); } };
template<>
struct dump_traits<const char*> { static String Dump(const const char*& obj)	{ return String(obj); } };
/*
template<>
struct dump_traits<XXX> { static String Dump(const T& obj) { return obj.Dump(); } };
*/
template<TYPENAME T>
inline	String	DumpString(const T& obj)	{ return dump_traits<T>::Dump(obj); }

//-------------------------------------
typedef	const String&	StringRef;
template<>
struct dump_traits<String> { static String Dump(const String& obj) { return obj; } };


//////////////////////////////////////////////////////////////////////
template<TYPENAME T>
class POINT_NO_RELEASE : public T
{
	virtual void	Release()			= 0;		// prevent call this function
//	virtual void	RleaseByOwner()		= 0;		// prevent call this function
};

//////////////////////////////////////////////////////////////////////
template<int I>
struct auto_long_
{
	auto_long_() : hide_data(I) {}
	auto_long_<I>& operator =(long n)	{ hide_data=n; return *this; }
	auto_long_<I>& operator +=(long n)		{ hide_data+=n; return *this; }
	auto_long_<I>& operator -=(long n)		{ hide_data-=n; return *this; }
	long operator ++()		{ return ++hide_data; }
	long operator --()		{ return --hide_data; }
	long operator ++(int)	{ return hide_data++; }
	long operator --(int)	{ return hide_data--; }
	operator long() const	{ return hide_data; }

	long	hide_data;
};
typedef	auto_long_<0>	auto_long;
//////////////////////////////////////////////////////////////////////
template<int I>
struct auto_bool_
{
	auto_bool_() : hide_data(I) {}
	auto_bool_<I>& operator =(bool n)	{ hide_data=n; return *this; }
	operator bool() const	{ return hide_data; }

	bool	hide_data;
};
typedef	auto_bool_<false>	auto_bool;

//////////////////////////////////////////////////////////////////////
template<TYPENAME T>
struct auto_object
{
	auto_object() : hide_ptr(0) {}
	auto_object<T>& operator =(T* p)	{ hide_ptr=p; return *this; }
	operator T*() const	{ return hide_ptr; }

	T*	hide_ptr;
};

//////////////////////////////////////////////////////////////////////
template<TYPENAME T>
struct auto_point
{
	auto_point() : hide_ptr(0) {}
	auto_point<T>& operator =(T* p)	{ hide_ptr=p; return *this; }
	operator T*() const	{ return hide_ptr; }
	T* operator ->() const	{ return hide_ptr; }

	T*	hide_ptr;
};

//////////////////////////////////////////////////////////////////////
template<TYPENAME T>
class hide_type
{
	T	hide_data;
public:
	T&	HideData()		{ return hide_data; }
};

typedef	POINT	CMyPos;
typedef	SIZE	CMySize;

#endif // COMMON_TYPEDEF_HEADER
