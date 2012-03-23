#coding=utf-8
import threading
from cfg import *
from config import *

import sqlite3
class CLogDB():
    FIELD_ID         = 0
    FIELD_TIME      = 1
    FIELD_CONTENT = 2
    def __init__(self):
        self.tb_name = None
        self.conn = None
        self.c = None
        
    def Create(self,  db_path,  tb_name,  bNew=False):
        self.tb_name = tb_name
        self.conn = sqlite3.connect(db_path)
        self.c = self.conn.cursor()
#        self.conn.execute("DROP TABLE IF EXISTS %s" % tb_name)
        self.conn.execute("""create table IF NOT EXISTS  %s(
                id INTEGER PRIMARY KEY NOT NULL ,
                time text,
                log text)""" % tb_name)
                
        self.conn.execute("""CREATE UNIQUE INDEX IF NOT EXISTS idx_id ON "%s" ("id" ASC)""" % tb_name)
#        self.c.execute('PRAGMA synchronous = OFF') # FULL, OFF
#        self.c.execute('PRAGMA page_size = 4096')  #default:1024
#        self.c.execute('PRAGMA cache_size = 70000') 

        self.conn.commit()
        
    def __del__(self):
        print("del db")
        self.conn.commit()

    def InsertLog(self,  log):
        self.c.execute("""insert into %s(time,log) values ( "%s", "%s")""" % (self.tb_name,  log.time,  log.content))
            
    def Begin(self):
        self.c.execute("BEGIN")
        
    def Save2File(self,  strFilePath):
        f = open(strFilePath,  'w+')
        DbgPrint("ORDER BY time")
        self.c.execute("SELECT * FROM %s ORDER BY time" % self.tb_name)
        DbgPrint("fetchall")
        lstRet = self.c.fetchall()
        DbgPrint("for ret in lstRet:")
        for ret in lstRet:
            f.write(ret[self.FIELD_TIME])
            f.write('\t')
            f.write(ret[self.FIELD_CONTENT])
        f.close()
  
class CLogDBTmp():
    def InsertLog(self,  str):
        ''''''  
class CFastPharseLog():
    reTimeGroup = re.compile(r"(?P<year>\d{4})-(?P<month>\d\d?)-(?P<day>\d{2})\.log", re.IGNORECASE)
    def __init__(self):
        ''''''  
        self.f = None
        self.file_name = ''
               
    def GetTime(self):
        '''根据文件名获取log文件的时间范围
        example:Statistic 2011-8-14.log 22_25_026.log
        '''
        timeBegin = CLogTime()
        timeEnd = CLogTime()
        #get begin
        m = self.reTimeGroup.search(self.file_name)
        if m:
            timeBegin.year = int(m.group('year'))
            timeBegin.month = int(m.group('month'))
            timeBegin.day = int(m.group('day'))
        
        #get end
        timeEnd = timeBegin
        timeEnd.hour = 24
        return timeBegin,  timeEnd
        
    def Save(self,  logRet):
        '''将当前文件位置之后的所有log保存到logRet'''
        log = CLog()
        bGetLog = False
        while True:
            line = self.f.readline()
            if not line:
                if bGetLog:
                    logRet.InsertLog(log)
                break   #一个文件处理结束
                
            line = line.replace('\r\n',  '\r')
            line = line.replace('\n',  '\r')
            if line =='\r': #过滤空行
                continue
            strTime = CLogTime.GetTimeStr(line)    
            if strTime !=None:
                line = line[24:len(line)]
                if bGetLog:  #前面已经取到log头了
                    logRet.InsertLog(log)
                    log.time = strTime
                    log.content = line
                else: 
                    bGetLog = True
                    log.time = strTime
                    log.content = line

            elif bGetLog:
                log.content += line        

#========================================================================
#thread of down log
class CAnalystLogThread(threading.Thread):
    LOG_CMD_NONE         = 0
    LOG_CMD_MERGE        = 1
    LOG_CMD_ANALYST     = 2
    LOG_CMD_IMPORT      = 3
    
    def __init__(self, dlg):
        #在重写__init__方法的时候要记得调用基类的__init__方法
        threading.Thread.__init__(self)
        self.no=2
        self.interval=1
        self.dlg = dlg
        self.stop_flag = False
        self.cmd = CAnalystLogThread.LOG_CMD_NONE
        
    def SetCmd(self,  cmd):
        self.cmd = cmd
        
    def ProcessAnalyst(self):
        try:
            self.dlg.signalProcessMsg.emit(MSG_ANALYST_LOG_START)
             
            #分析指定目录的log文件(支持子目录)
            directory = self.dlg.ui.edtLogPath.text()
            for root, dirs, files in os.walk(directory, True):
                for name in files:
                    if(self.stop_flag):
                        break
                    full_name = os.path.join(root, name)
                    retMatch = g_cfg.IsErrLogFile(full_name)
                    if retMatch.ret:
                        self.dlg.signalAddWarnLog.emit(retMatch)
               
        except Exception as ex:
            ErrPrint(ex)
 
        self.dlg.signalProcessMsg.emit(MSG_ANALYST_LOG_FINISH)
        
    def ProcessMerge(self):
        try:
            self.dlg.signalProcessMsg.emit(MSG_ANALYST_MERGE_START)
            logDB = CLogDB()
            logDB.Create('d.db',  'log',  False)
            logDB.Save2File('m.log')
        except Exception as ex:
            ErrPrint(ex)
        self.dlg.signalProcessMsg.emit(MSG_ANALYST_MERGE_FINISH)

    def ProcessImport(self):
        try:
            self.dlg.signalProcessMsg.emit(MSG_ANALYST_IMPORT_START)
            
            #打开所有文件
            lstMergeLog = []
            directory = self.dlg.ui.edtLogPath.text()
            #XswUtility.Dir3_GB2312ToUTF8(directory)
            for root, dirs, files in os.walk(directory, True):
                for name in files:
                    if(self.stop_flag):
                        break
                    full_name = os.path.join(root, name)
#                    f = open(full_name,  'r')
                    f = codecs.open(full_name,  'r',  'utf-8')
                    mergeLog = CFastPharseLog()
                    mergeLog.f = f
                    mergeLog.file_name = name
                    lstMergeLog.append(mergeLog)
            logDB = CLogDB()
            logDB.Create('d.db',  'log',  True)
            logDB.Begin()
 #           logDB2 = CLogDBTmp()
            for mergeLog in lstMergeLog:
                self.dlg.signalShowMsg.emit('begin merge:%s' % mergeLog.file_name)
                mergeLog.Save(logDB)
                self.dlg.signalShowMsg.emit('end merge:%s' % mergeLog.file_name)
            del logDB   
        except Exception as ex:
            ErrPrint(ex)
 
        self.dlg.signalProcessMsg.emit(MSG_ANALYST_IMPORT_FINISH)

    def GetTime(self,  mergeLogs):
        timeBegin = CLogTime()
        timeBegin.year = 9999;
        timeEnd = CLogTime()
        for mergeLog in mergeLogs:
            timeMin,  timeMax = mergeLog.GetTime()
            timeBegin = min(timeBegin,  timeMin)
            timeEnd = max(timeEnd,  timeMax)
        
        timeBegin.hour = 0
        timeEnd.hour = 24
        return timeBegin,  timeEnd

              
    def run(self):  #重写run()方法，把自己的线程函数的代码放到这里
        if self.cmd == CAnalystLogThread.LOG_CMD_ANALYST:
            self.ProcessAnalyst();
        elif self.cmd == CAnalystLogThread.LOG_CMD_MERGE:
            self.ProcessMerge();
        elif self.cmd == CAnalystLogThread.LOG_CMD_IMPORT:
            self.ProcessImport();
        
    def stop(self):
        self.stop_flag = True
