// stl_demo.cpp : Defines the entry point for the console application.
//

#include <windows.h>
#include <tchar.h>
#include <time.h>
#include <iostream>
#include <set>
#include <xTickCount.h>
using namespace std;

//////////////////////////////////////////////////////////////////////////
//比较map与hash_map的效率
#include <map>
#include <hash_map>
using namespace stdext;
void map_demo(const int ITEM_COUNT, const int FIND_COUNT, const int nBucket=0)
{
	typedef map<int, int> IntMap;
	typedef hash_map<int, int> IntHashMap;

	cout << "个数:" << ITEM_COUNT << ", 查找次数:" << FIND_COUNT << ", Bucket:" << nBucket << endl;

	IntMap mapInt;
	xTickCount tickMapPush;
	for (int i=0; i<ITEM_COUNT; ++i)
	{
		mapInt[i] = i;
	}
	tickMapPush.Finish();

	xTickCount tickMapFind;
	for (int i=0; i< FIND_COUNT; ++i)
	{
		mapInt.find(ITEM_COUNT);
	}
	tickMapFind.Finish();

	xTickCount tickMapRandFind;
	for (int i=0; i< FIND_COUNT; ++i)
	{
		mapInt.find(rand()%FIND_COUNT);
	}
	tickMapRandFind.Finish();

	cout << "map push use " << tickMapPush.GetUsedMS() << " ms" 
		<< ", Find use " << tickMapFind.GetUsedMS() << " ms" 
		<< ", RandFind use " << tickMapRandFind.GetUsedMS() << " ms" 
		<< endl;

	IntHashMap hashmapInt;
 	if (nBucket > 0)
 		hashmapInt.rehash(nBucket);
	
	xTickCount tickHashMapPush;
	for (int i=0; i<ITEM_COUNT; ++i)
	{
		hashmapInt[i] = i;
	}
	tickHashMapPush.Finish();

	xTickCount tickHashMapFind;
	for (int i=0; i< FIND_COUNT; ++i)
	{
		hashmapInt.find(ITEM_COUNT);
	}
	tickHashMapFind.Finish();

	xTickCount tickHashMapRandFind;
	for (int i=0; i< FIND_COUNT; ++i)
	{
		hashmapInt.find(rand()%FIND_COUNT);
	}
	tickHashMapRandFind.Finish();


	cout << "hash_map push use " << tickHashMapPush.GetUsedMS() << " ms" 
		<< ", Find use " << tickHashMapFind.GetUsedMS() << " ms" 
		<< ", RandFind use " << tickHashMapRandFind.GetUsedMS() << " ms" 
		<< endl;
	cout << "size:" << hashmapInt.size() << ", bucket_count:" << hashmapInt.bucket_count() << ", bucket_size:" << hashmapInt.bucket_size(0) << endl;
	
}

void test_map()
{
	map_demo(5, 10000000);
	map_demo(100, 10000000);
	map_demo(500, 10000000);
	map_demo(10000, 10000000);
}

//////////////////////////////////////////////////////////////////////////
//if(!p)
void test_bool_point()
{
	class CBoolPoint
	{
	public:
		operator bool()	{ return false; }
	};
	CBoolPoint b;
	if(b)
		return;
}

//////////////////////////////////////////////////////////////////////////
//比较容器的遍历性能
void demo_enum(const int LOOP_ITEM, const int LOOP_COUNT)
{
	typedef map<int, int> IntMap;
	typedef vector<int> IntVec;

	IntMap m1;
	for (int i=0; i<LOOP_ITEM; ++i)
	{
		m1[i] = i;
	}
	xTickCount tickEnumMap;
	int n1 = 0;
	tickEnumMap.Start();
	for (int i=0; i<LOOP_COUNT; ++i)
	{
		for (IntMap::iterator it=m1.begin(); it!=m1.end(); ++it)
		{
			n1 += it->second;
		}
	}
	tickEnumMap.Finish();

	IntVec v1;
	IntVec* p = &v1;
	for (int i=0; i<LOOP_ITEM; ++i)
	{
		v1.push_back(i);
	}
	xTickCount tickEnumVec;
	int n2 = 0;
	tickEnumVec.Start();
	for (int i=0; i<LOOP_COUNT; ++i)
	{
		for (int n=0; n<v1.size(); ++n)
		{
			n2 += (*p)[n];
		}
	}
	tickEnumVec.Finish();
	xTickCount tickAtVec;
	n2 = 0;
	tickAtVec.Start();
	for (int i=0; i<LOOP_COUNT; ++i)
	{
		for (int n=0; n<v1.size(); ++n)
		{
			n2 += v1.at(n);
		}
	}
	tickAtVec.Finish();

	xTickCount tickEnumMHashap;
	{
		typedef stdext::hash_map<int, int> IntHashMap;
		IntMap m1;
		for (int i=0; i<LOOP_ITEM; ++i)
		{
			m1[i] = i;
		}
		int n1 = 0;
		tickEnumMHashap.Start();
		for (int i=0; i<LOOP_COUNT; ++i)
		{
			for (IntMap::iterator it=m1.begin(); it!=m1.end(); ++it)
			{
				n1 += it->second;
			}
		}
		tickEnumMHashap.Finish();
	}


	cout << "元素个数:" << LOOP_ITEM << ",执行次数:" << LOOP_COUNT 
		<< "] map:" << tickEnumMap.GetUsedMS() << " ms" 
		<< ", hashmap: " << tickEnumMHashap.GetUsedMS() << " ms" 
		<< ", vec: " << tickEnumVec.GetUsedMS() << " ms" 
		<< ", at: " << tickAtVec.GetUsedMS() << " ms" 
		<< ", " << (float)tickEnumMap.GetUsedMS()/(float)tickEnumVec.GetUsedMS() << endl;
}

void test_enum()
{
	demo_enum(10, 10000);
	demo_enum(10000, 10000);
}

//////////////////////////////////////////////////////////////////////////
//比较常量vector list和map的查找性能
void testFind(const int LOOP_ITEM, const int LOOP_COUNT)
{
	srand(time(NULL));
	typedef map<int, int> IntMap;
#if _MSC_VER >=1600
	typedef std::hash_map<int, int> IntHashMap;
#else
	typedef stdext::hash_map<int, int> IntHashMap;
#endif // _DEBUG
	typedef vector<int> IntVec;
	typedef list<int> IntList;
	typedef set<int> IntSet;

	{
		IntMap m1;
		for (int i=0; i<LOOP_ITEM; ++i)
		{
			m1[i] = i;
		}

		xTickCount tFindMap;
		int n1 = 0;
		tFindMap.Start();
		int nRand = 0;
		for (int i=0; i<LOOP_COUNT; ++i)
		{
			IntMap::const_iterator it = m1.find(rand()%LOOP_ITEM);
			if(it != m1.end())
				++nRand;
		}	
		tFindMap.Finish();

		cout << "元素个数:" << LOOP_ITEM << ",执行次数:" << LOOP_COUNT << "] find map:" << tFindMap.GetUsedMS() << " ms" << endl;
	}

	{
		IntHashMap m1;
		for (int i=0; i<LOOP_ITEM; ++i)
		{
			m1[i] = i;
		}

		xTickCount tFindMap;
		int n1 = 0;
		tFindMap.Start();
		int nRand = 0;
		for (int i=0; i<LOOP_COUNT; ++i)
		{
			IntHashMap::const_iterator it = m1.find(rand()%LOOP_ITEM);
			if(it != m1.end())
				++nRand;
		}	
		tFindMap.Finish();

		cout << "元素个数:" << LOOP_ITEM << ",执行次数:" << LOOP_COUNT << "] find hashmap:" << tFindMap.GetUsedMS() << " ms" << endl;
	}
	
	{
		IntSet m1;
		for (int i=0; i<LOOP_ITEM; ++i)
		{
			m1.insert(i);
		}

		xTickCount tFindMap;
		int n1 = 0;
		tFindMap.Start();
		int nRand = 0;
		for (int i=0; i<LOOP_COUNT; ++i)
		{
			IntSet::const_iterator it = m1.find(rand()%LOOP_ITEM);
			if(it != m1.end())
				++nRand;
		}	
		tFindMap.Finish();

		cout << "元素个数:" << LOOP_ITEM << ",执行次数:" << LOOP_COUNT << "] find set:" << tFindMap.GetUsedMS() << " ms" << endl;
	}

	{
		IntVec m1;
		for (int i=0; i<LOOP_ITEM; ++i)
		{
			m1.push_back(i);
		}

		xTickCount tFindVec;
		int n1 = 0;
		tFindVec.Start();
		int nRand = 0;
		for (int i=0; i<LOOP_COUNT; ++i)
		{
			const int nSize = m1.size();
			nRand = rand()%LOOP_ITEM;
			for(int j=0;j<nSize;++j)
			{
				if(m1[j] == nRand)
				{
					++nRand;
					//break;
				}
			}

			//比list快一点点
// 			for(int j=0;j<nSize;++j)
// 			{
// 				if(m1.at(j) == nRand)
// 				{
// 					++nRand;
// 					//break;
// 				}
// 			}

			//比list更慢
// 			for (IntVec::const_iterator it=m1.begin(); it!=m1.end(); ++it)
// 			{
// 				if(*it == nRand)
// 					++nRand;
// 			}

			//n1 += m1.at(rand()%LOOP_ITEM);
		}	
		tFindVec.Finish();
		cout << "元素个数:" << LOOP_ITEM << ",执行次数:" << LOOP_COUNT << "] find vec:" << tFindVec.GetUsedMS() << " ms" << endl;
	}

	{
		IntList m1;
		for (int i=0; i<LOOP_ITEM; ++i)
		{
			m1.push_back(i);
		}

		xTickCount tFindVec;
		int n1 = 0;
		tFindVec.Start();
		int nRand = 0;
		for (int i=0; i<LOOP_COUNT; ++i)
		{
			const int nSize = m1.size();
			nRand = rand()%LOOP_ITEM;
			for (IntList::const_iterator it=m1.begin(); it!=m1.end(); ++it)
			{
				if(*it == nRand)
					++nRand;
			}
		}	
		tFindVec.Finish();
		cout << "元素个数:" << LOOP_ITEM << ",执行次数:" << LOOP_COUNT << "] find list:" << tFindVec.GetUsedMS() << " ms" << endl;
	}
}

void testFindDemo()
{
 	testFind(2, 1000000);
	testFind(5, 1000000);
	testFind(100, 1000000);
//	testFind(10000, 100000);
}

//////////////////////////////////////////////////////////////////////////
void testAddDemo()
{
	const int LOOP_COUNT = 50000000;
	xTickCount tFindVec;
	int n1 = 0;
	tFindVec.Start();
	for (int i=0; i<LOOP_COUNT; ++i)
	{
		n1 += i;
	}	
	tFindVec.Finish();
	cout << "[LOOP:" << LOOP_COUNT << "] use:" << tFindVec.GetUsedMS() << " ms ret:" << n1 << endl;
}

//////////////////////////////////////////////////////////////////////////
//vector的遍历:迭代器和[]哪个快
void testVectorEnum(const int LOOP_ITEM, const int LOOP_COUNT)
{
	typedef vector<int> IntVec;

	IntVec m1;
	for (int i=0; i<LOOP_ITEM; ++i)
	{
		m1.push_back(i);
	}

	xTickCount tickEnum;
	tickEnum.Start();
	for(int i=0; i<LOOP_COUNT; ++i)
	{
		int n = 0;
		for (int j=0; j<LOOP_ITEM; ++j)
		{
			n += m1[j];
		}
	}
	tickEnum.Finish();

	xTickCount tickEnumIter;
	tickEnumIter.Start();
	int n = 0;
	for(int i=0; i<LOOP_COUNT; ++i)
	{
		int n = 0;
		for(IntVec::const_iterator it=m1.begin(); it!=m1.end(); ++it)
		{
			n += *it;
		}
	}
	tickEnumIter.Finish();

	xTickCount tickEnumAt;
	tickEnumAt.Start();
	for(int i=0; i<LOOP_COUNT; ++i)
	{
		int n = 0;
		for (int j=0; j<LOOP_ITEM; ++j)
		{
			n += m1.at(j);
		}
	}
	tickEnumAt.Finish();

	cout << "元素个数:" << LOOP_ITEM << ",执行次数:" << LOOP_COUNT 
		<< "] []:" << tickEnum.GetUsedMS() 
		<< "ms ite:" << tickEnumIter.GetUsedMS()
		<< "ms at:" << tickEnumAt.GetUsedMS()
		<< "ms:"
		<< endl;
}

void testVectorEnumDemo()
{
	testVectorEnum(10, 100000);
	testVectorEnum(100, 100000);
	testVectorEnum(1000, 100000);
	testVectorEnum(10000, 100000);
}

//////////////////////////////////////////////////////////////////////////
//OBJPOS自定义hash会不会更快
/*
struct OBJPOS
{
	int x;
	int y;
	int z;
	OBJPOS(int _x, int _y, int _z):x(_x),y(_y),z(_z){}
	OBJPOS():x(0),y(0),z(0){}
	OBJPOS operator+(const OBJPOS& b) const
	{
		return OBJPOS(x+b.x, y+b.y, z+b.z);
	}
	bool operator == (const OBJPOS& a) const { return x==a.x && y==a.y; }
	OBJPOS& operator = (const OBJPOS& a) { x=a.x; y=a.y; return *this; }
};
inline bool operator < (const OBJPOS& a, const OBJPOS& b) { return (a.x<b.x) || ((a.x>b.y) && (a.y<b.y)); }
inline bool operator > (const OBJPOS& a, const OBJPOS& b) { return !(a<b); }
void TestHashmap(const int nItem, const int nCount)
{
	typedef stdext::hash_map<OBJPOS, int> OBJPOS_HASHMAP;
	OBJPOS_HASHMAP hm;
	for (int i=0; i<nItem; ++i)
		hm[OBJPOS(i,i,i)] = i;

	typedef std::map<OBJPOS, int> OBJPOS_MAP;
	OBJPOS_MAP m;
	for (int i=0; i<nItem; ++i)
		m[OBJPOS(i,i,i)] = i;

}
void DemoHashMap()
{
	TestHashmap(100, 10000);
}*/

//////////////////////////////////////////////////////////////////////////
//busy
void testBusy()
{
	while(true)
	{
		int n = 3;
	}
}

//////////////////////////////////////////////////////////////////////////
static int NEWTEST_COUNT = 0;
class CNewTest
{
public:
	CNewTest(){ ++NEWTEST_COUNT;}
	~CNewTest(){ --NEWTEST_COUNT;}
	char buf[2048];
};

void testNew(const int LOOP_COUNT)
{
	xTickCount tickStack;
	tickStack.Start();
	for(int i=0; i<LOOP_COUNT; ++i)
	{
		CNewTest o1;
	}
	tickStack.Finish();

	xTickCount tickNew;
	tickNew.Start();
	for(int i=0; i<LOOP_COUNT; ++i)
	{
		CNewTest* p = new CNewTest();
		delete p;
	}
	tickNew.Finish();

	cout << "LOOP:" << LOOP_COUNT 
		<< "stack:" << tickStack.GetUsedMS()
		<< "ms new:" << tickNew.GetUsedMS()
		<< "ms:"
		<< endl;
}

void testNewDemo()
{
	testNew(1000);
	testNew(100000);
}

//////////////////////////////////////////////////////////////////////////
void testFloat()
{
	float f1 = 3.0f;
	float f2 = 3.0f;
	const int LOOP_COUNT = 1000000;
	xTickCount tickNew;
	tickNew.Start();
	for (int i=0; i<LOOP_COUNT; ++i)
	{
		float f = 3.0f;
		f1 = f2 * f;
	}
	tickNew.Finish();
	cout << "LOOP:" << LOOP_COUNT 
		<< "use:" << tickNew.GetUsedMS()
		<< "ms:"
		<< endl;
}

//////////////////////////////////////////////////////////////////////////
void StringMapDemo(const int ITEM_COUNT, const int FIND_COUNT)
{
	typedef map<string, int> StringMap;
	typedef std::vector<char*> CharVector;
	CharVector vecChar;
	for (int i=0; i<ITEM_COUNT; ++i)
	{
		char* sz = new char[16];
		vecChar.push_back(itoa(i, sz, 10));
	}
	
	xTickCount tickMapFind;
	for (int i=0; i<FIND_COUNT; ++i)
	{
		string str = vecChar[rand()%ITEM_COUNT];
	}
	tickMapFind.Finish();

 
 	cout << "个数:" << ITEM_COUNT << ", 查找次数:" << FIND_COUNT 
 		<< "  use " << tickMapFind.GetUsedMS() 
		<< " ms" 
 		<< endl;
}

void testStringMap()
{
	StringMapDemo(10, 1000000);
	StringMapDemo(10, 1000000);
}

//////////////////////////////////////////////////////////////////////////
//deque/list/vector消息队列效率
#include <deque>
#include <list>
#include <vector>
using namespace std;
void msgqueue_demo(const int ITEM_COUNT, const int FIND_COUNT)
{
	cout << "个数:" << ITEM_COUNT << ", 查找次数:" << FIND_COUNT<< endl;
	

	{
		DWORD dwTickPush = 0;
		DWORD dwTickPop = 0;
		xTickCount tickDequePush;
		xTickCount tickDequePop;

		for (int j=0; j<FIND_COUNT;++j)
		{
			typedef list<int> IntQueue;
			IntQueue mapInt;
			tickDequePush.Start();
			for (int i=0; i<ITEM_COUNT; ++i)
			{
				mapInt.push_back(i);
			}
			tickDequePush.Finish();
			dwTickPush = tickDequePush.GetUsedMS();

			tickDequePop.Start();
			for (int i=0; i< ITEM_COUNT; ++i)
			{
				mapInt.front();
				mapInt.pop_front();
			}
			tickDequePop.Finish();
			dwTickPop = tickDequePop.GetUsedMS();
		}

		cout << "list push use " << dwTickPop << " ms" 
			<< ", pop use " << dwTickPop << " ms" 
			<< endl;
	}

	{
		DWORD dwTickPush = 0;
		DWORD dwTickPop = 0;
		xTickCount tickDequePush;
		xTickCount tickDequePop;

		for (int j=0; j<FIND_COUNT;++j)
		{
			typedef vector<int> IntQueue;
			IntQueue mapInt;
			tickDequePush.Start();
			for (int i=0; i<ITEM_COUNT; ++i)
			{
				mapInt.push_back(i);
			}
			tickDequePush.Finish();
			dwTickPush = tickDequePush.GetUsedMS();

			tickDequePop.Start();
			for (int i=0; i< ITEM_COUNT; ++i)
			{
				mapInt.erase(mapInt.begin());
			}
			tickDequePop.Finish();
			dwTickPop = tickDequePop.GetUsedMS();
		}

		cout << "vector push use " << dwTickPop << " ms" 
			<< ", pop use " << dwTickPop << " ms" 
			<< endl;
	}
	{
		DWORD dwTickPush = 0;
		DWORD dwTickPop = 0;
		xTickCount tickDequePush;
		xTickCount tickDequePop;

		for (int j=0; j<FIND_COUNT;++j)
		{
			typedef deque<int> IntQueue;
			IntQueue mapInt;
			tickDequePush.Start();
			for (int i=0; i<ITEM_COUNT; ++i)
			{
				mapInt.push_back(i);
			}
			tickDequePush.Finish();
			dwTickPush = tickDequePush.GetUsedMS();

			tickDequePop.Start();
			for (int i=0; i< ITEM_COUNT; ++i)
			{
				mapInt.front();
				mapInt.pop_front();
			}
			tickDequePop.Finish();
			dwTickPop = tickDequePop.GetUsedMS();
		}

		cout << "deque push use " << dwTickPop << " ms" 
			<< ", pop use " << dwTickPop << " ms" 
			<< endl;
	}

}

void test_msgqueue()
{
	//msgqueue_demo(100, 100000);
	msgqueue_demo(100000, 1);
}

const char *stristr(const char *String, const char *Pattern)
{
	const char *pptr, *sptr, *start;

	for (start = (const char *)String; *start; start++)
	{
		/* find start of pattern in string */
		for ( ; (*start && (toupper(*start) != toupper(*Pattern))); start++)
			;
		if (!*start)
			return 0;

		pptr = (const char *)Pattern;
		sptr = (const char *)start;

		while (toupper(*sptr) == toupper(*pptr))
		{
			sptr++;
			pptr++;

			/* if end of pattern then pattern was found */

			if (!*pptr)
				return (start);
		}
	}
	return 0;
}

template<typename T>
void operator delete(void* p) 
{

	free(p);
}

template<typename T>
void tttt(void* p)
{
ddd
	free(p);
}

//////////////////////////////////////////////////////////////////////////
int _tmain(int argc, _TCHAR* argv[])
{
	test_enum();
	return 0;
}

