using System;
using System.Diagnostics;
using System.Text;
using System.Collections;
using System.Collections.Generic;
using System.Timers;
using System.Windows.Forms;

namespace ProcessMoniter
{
    using PNameList = List<string>;

    public class PMoniter : Form
    {
        public bool Init(int nMS)
        {
            aTimer.Elapsed += new ElapsedEventHandler(this.TimedEvent);
            aTimer.Interval = nMS;
            aTimer.Enabled = true;
            return true;
        }

        PNameList lstProcesses = new PNameList();
        public bool AddProcessName(string strProcess)
        {
            foreach (string strPName in lstProcesses)
            {
                if (strPName == strProcess)
                {
                    return false;
                }
            }
            lstProcesses.Add(strProcess);
            return true;
        }

        //////////////////////////////////////////////////////////////////////////
        //timer
        System.Timers.Timer aTimer = new System.Timers.Timer();
        private void TimedEvent(object source, ElapsedEventArgs e)
        {
            foreach (string strPName in lstProcesses)
            {
                Process[] process = Process.GetProcessesByName(strPName);
                for (int i = 0; i < process.Length; ++i)
                {
                    this.WriteLog(process[i]);
                }
            }
        }

        //////////////////////////////////////////////////////////////////////////
        //log
        private void WriteLog(Process process)
        {
            string s = "";
// BasePriority 获取关联进程的基本优先级。  
//   Container 获取 IContainer，它包含 Component。（从 Component 继承。）  
//   EnableRaisingEvents 获取或设置在进程终止时是否应激发 Exited 事件。  
//   ExitCode 获取关联进程终止时指定的值。  
//   ExitTime 获取关联进程退出的时间。  
//   Handle 返回关联进程的本机句柄。  
//   HandleCount 获取由进程打开的句柄数。  
//   HasExited 获取指示关联进程是否已终止的值。  
//   Id 获取关联进程的唯一标识符。  
//   MachineName 获取关联进程正在其上运行的计算机的名称。  
//   MainModule 获取关联进程的主模块。  
//   MainWindowHandle 获取关联进程主窗口的窗口句柄。  
//   MainWindowTitle 获取进程的主窗口标题。  
//   MaxWorkingSet 获取或设置关联进程的允许的最大工作集大小。  
//   MinWorkingSet 获取或设置关联进程的允许的最小工作集大小。  
//   Modules 获取已由关联进程加载的模块。  
//   NonpagedSystemMemorySize 获取分配给此进程的未分页的系统内存大小。  
//   NonpagedSystemMemorySize64 获取为关联的进程分配的非分页系统内存量。  
//   PagedMemorySize 获取分页的内存大小。  
//   PagedMemorySize64 获取为关联的进程分配的分页内存量。  
//   PagedSystemMemorySize 获取分页的系统内存大小。  
//   PagedSystemMemorySize64 获取为关联的进程分配的可分页系统内存量。  
//   PeakPagedMemorySize 获取峰值分页内存大小。  
//   PeakPagedMemorySize64 获取关联的进程使用的虚拟内存分页文件中的最大内存量。  
//   PeakVirtualMemorySize 获取峰值虚拟内存大小。  
//   PeakVirtualMemorySize64 获取关联的进程使用的最大虚拟内存量。  
//   PeakWorkingSet 获取关联进程的峰值工作集大小。  
//   PeakWorkingSet64 获取关联的进程使用的最大物理内存量。  
//   PriorityBoostEnabled 获取或设置一个值，该值指示主窗口拥有焦点时是否由操作系统暂时提升关联进程的优先级。  
//   PriorityClass 获取或设置关联进程的总体优先级类别。  
//   PrivateMemorySize 获取专用内存大小。  
//   PrivateMemorySize64 获取为关联的进程分配的专用内存量。  
//   PrivilegedProcessorTime 获取此进程的特权处理器时间。  
//   ProcessName 获取该进程的名称。  
//   ProcessorAffinity 获取或设置可安排此进程中的线程在其上运行的处理器。  
//   Responding 获取指示进程的用户界面当前是否响应的值。  
//   SessionId 获取关联的进程的终端服务会话标识符。  
//   Site 获取或设置 Component 的 ISite。（从 Component 继承。）  
//   StandardError 获取用于读取应用程序错误输出的流。  
//   StandardInput 获取用于写入应用程序输入的流。  
//   StandardOutput 获取用于读取应用程序输出的流。  
//   StartInfo 获取或设置要传递给 Process 的 Start 方法的属性。  
//   StartTime 获取关联进程启动的时间。  
//   SynchronizingObject 获取或设置用于封送由于进程退出事件而发出的事件处理程序调用的对象。  
//   Threads 获取在关联进程中运行的一组线程。  
//   TotalProcessorTime 获取此进程的总的处理器时间。  
//   UserProcessorTime 获取此进程的用户处理器时间。  
//   VirtualMemorySize 获取进程的虚拟内存大小。  
//   VirtualMemorySize64 获取为关联的进程分配的虚拟内存量。  
//   WorkingSet 获取关联进程的物理内存使用情况。  
//   WorkingSet64 获取为关联的进程分配的物理内存量。 

            s += "进程镜像名:" + process.ProcessName;
            s += "PID;" + process.Id;
            s += "启动线程数:" + process.Threads.Count.ToString();
            s += "CPU占用时间:" + process.TotalProcessorTime.ToString();
            s += "线程优先级:" + process.PriorityClass.ToString();
            s += "启动时间:" + process.StartTime.ToLongTimeString();
            s += "专用内存:" + (process.PrivateMemorySize64 / 1024).ToString() + "K";
            s += "峰顶虚拟内存:" + (process.PeakVirtualMemorySize64 / 1024).ToString() + "K";
            s += "峰顶分页内存:" + (process.PeakPagedMemorySize64 / 1024).ToString() + "K";
            s += "分页系统内存:" + (process.PagedSystemMemorySize64 / 1024).ToString() + "K";
            s += "分页内存:" + (process.PagedMemorySize64 / 1024).ToString() + "K";
            s += "未分页系统内存:" + (process.NonpagedSystemMemorySize64 / 1024).ToString() + "K";
            s += "物理内存:" + (process.WorkingSet64 / 1024).ToString() + "K";
            s += "虚拟内存:" + (process.VirtualMemorySize64 / 1024).ToString() + "K"; 
            Trace.TraceInformation(s);
        }
    }
}
