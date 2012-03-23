#include <vector>
#include <memory>
#include "..\mp\MemoryProfiler.h"

#include "TestFunctions.cpp"

M_MEMORYPROFILE_OPERATOR_NEW
M_MEMORYPROFILE_DEFAULT_ALLOCATOR

int *g_pInt = new int;

void B()
{
	M_MEMORYPROFILE_FUNC;

	std::vector<int> a;
	a.push_back( 1 );
	a.push_back( 2 );
	a.push_back( 3 );

	delete g_pInt;
}

void A()
{
	M_MEMORYPROFILE_FUNC;

	std::auto_ptr<int> ptr( new int( 10 ) );

	B();
}

int _tmain(int argc, _TCHAR* argv[])
{
	M_MEMORYPROFILE_FUNC;

	A();

	for( int i = 0; i < 2 * 10000; ++i )
		Foo1();

	mp::Profiler().Dump( _T( "MemroyProfile.txt" ) );

	return 0;
}
