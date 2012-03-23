// 外部工程直接 include 这个文件
//
#ifndef ANALYST_SHARED_COMMON_HEADER
#define ANALYST_SHARED_COMMON_HEADER

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


//1. 所有CLIENT相关的SEND和RECV的时间、数量和消息包尺寸。
//2、所有NPC服务器的SEND和RECV的时间、数量和消息包尺寸。
//3、针对每个线程，统计SOCKET收发、核心逻辑运算(不含数据库)、数据库调用，这3方面的时间，并计算其各自的比例。
//4、针对每个数据库连接，统计SELECT、UPDATE、INSERT、DELETE的消耗量，并计算其各自的比例。SELECT还可以统计一下接收的记录数。
//5、针对每个线程，统计ITEM、USER、其它等3方面的数据库消耗量，并计算其各自的比例。(牧场增加FARMITEM一项)
//6、其它需要统计的内容。

// 1.统计SELECT、UPDATE、INSERT、DELETE的消耗量
// 2.socket字节数
enum ANALYST_WHAT_COMMON
{
		ANALYST_LOGSAVE,
		//...(请添加在这上面)

		// 在COMMON中字义的最大值，请保留。
		ANALYST_WHAT_COMMON_END
};



#endif //ANALYST_SHARED_COMMON_HEADER