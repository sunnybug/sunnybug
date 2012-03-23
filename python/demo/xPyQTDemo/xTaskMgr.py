#coding=utf-8
import sys
sys.path.append('../../../toolkit')

import threading
from threading import Thread,Lock
from cfg import *
from PyQt4 import QtGui,  QtCore
from PyQt4.QtGui import *
QtCore.Signal = QtCore.pyqtSignal
import time
from XswUtility import *
from queue import Queue

FILED_TASKMGR_NAME = 0        
FILED_TASKMGR_CMDS = 1

#========================================================================
#thread of down log
class CCmdTask(threading.Thread):
    def __init__(self, mgr):
        #在重写__init__方法的时候要记得调用基类的__init__方法
        threading.Thread.__init__(self)
        self.interval=1
        self.mgr = mgr
        self.run_flag = False
        self.is_run_already = False
        self.lstCmd = []
        self.task_name = ''
          
    def DoTask(self):
        while(len(self.lstCmd) > 0):
            if not self.run_flag:
               return true
            cmd = self.lstCmd.pop(0)
            DbgPrint("DoTask:" + cmd)
            os.system(cmd)
        #任务都处理完了，再去取
        if len(self.lstCmd) <= 0:
            self.mgr.PopTaskCmd(self.lstCmd)
            
        return (len(self.lstCmd) > 0)
    
    def run(self):  #重写run()方法，把自己的线程函数的代码放到这里
        try:
            while(self.run_flag):
                if not self.DoTask():
                    time.sleep(1) #没任务了，暂时休息
        except Exception as ex:
            ErrPrint(ex) 
             
    def start(self):
        self.run_flag = True
        if not self.is_run_already:
            self.is_run_already = True
            threading.Thread.start(self)
    
    def stop(self):
        self.run_flag = False
        self.lstCmd = []
        
class CCmdTaskMgr():
    MaxTaskCount = 5
    def __init__(self,  dlg):
        self.lstThd = []
        self.CurBusyCount = 0
        
        #待处理任务队列
        self.lockTaskList = Lock()
        self.lstTaskList = []    
        
        self.dlg = dlg
        for i in range(0,  self.MaxTaskCount):
            self.lstThd.append(CCmdTask(self))
        
    def DumpCmd(self):
        for t in self.lstTaskList:
            for cmd in t[FILED_TASKMGR_CMDS]:
                os.system("echo %s >> cmd.txt" % cmd)

       
    def PopTaskCmd(self,  lstCmd):
        with self.lockTaskList:
            
            if len(self.lstTaskList)<=0:
                return
            t = self.lstTaskList.pop(0)
            lstCmd += t[FILED_TASKMGR_CMDS] 
        
        #taskName: 相同的任务名会交给同一个任务线程执行
    def AddTaskCmd(self,  taskName,  lstCmd):
        #############
        #debug
        strDbg = ("+task[%s]\r\n" %  taskName)
        for cmd in lstCmd:
            strDbg += cmd
            strDbg += "\r\n"
        DbgPrint(strDbg)
        
        #############
        #添加到相同taskName
        with self.lockTaskList:
            bFind = False
            for t in self.lstTaskList:
                if t[FILED_TASKMGR_NAME] == taskName:
                    bFind = True
                    t[FILED_TASKMGR_CMDS] += lstCmd
                    DbgPrint("%s match,new size=%d" % (taskName,  len(t[FILED_TASKMGR_CMDS] )))
                    break
            if not bFind:
                self.lstTaskList.append([taskName,  lstCmd])
                DbgPrint("new task:" + taskName)
      
    def StartTask(self):
        for thd in self.lstThd:
            thd.start()
       
    def StopAllTask(self):
        self.lstTaskList = []
        for thd in self.lstThd:
            thd.stop()
           

