【Memory profiler 2分钟使用入门】
1）通过某种自动化工具（附件或有提供）在你的项目的所有函数开头添加一行宏M_MEMORYPROFILE_FUNC，就像这样：
	void Foo()
	{
		M_MEMORYPROFILE_FUNC

		... // 这个函数原来的代码
	}

2）在你的项目中，随便选择一个.cpp文件，在全局作用域中添加一行宏M_MEMORYPROFILE_OPERATOR_NEW。

3）如果你的项目具有特定的内存模型，定义了自己的内存分配器，那么在某个.cpp文件中提供以下函数的实现：

	namespace mp
	{
		void*	AllocateMemory(size_t bytes);
		void	FreeMemory(void *pMemory);
	}

如果你的项目没有采取任何特定的内存分配方式，那么只需简单地在某个.cpp文件的全局作用域中插入一行宏 M_MEMORYPROFILE_DEFAULT_ALLOCATOR。

4）在你需要查看内存使用的地方（例如程序运行过程中的某个时间点，或程序结束时刻），插入这样的代码，将内存使用情况输出到磁盘文 件中：
	mp::Profiler().Dump
( _T( "MemroyProfile.txt" ) );

5）使用Office Excel导入这个输出文件，仅以“#”作为分隔符，即可舒适、便捷地查看内存统计报告（例如排序、制作图表等）。

6）内存报告中的字段含义：
	Name		函数原型
	Source		所在文件、行号
	ExAllocated	独占分配，此函数内【分配】的内存字节数（不包括它调用的其它函数）
	ExFreed		独占释放，此函数内【释放】的内存字节数（不包括它调用的其它函数）
	ExHeld		独占持有，“ExAllocated - ExFreed”，即此函数未释放的字节数
	InAllocated	非独占分配，此函数及其调用的函数（以及这些函数又调用的函数等）【分配】的内存字节数
	InFreed		非独占释放，此函数及其调用的函数（以及这些函数又调用的函数等）【释放】的内存字节数
	InHeld		非独占持有，“InAllocated - InFreed”，即此函数及其调用未释放的字节数
