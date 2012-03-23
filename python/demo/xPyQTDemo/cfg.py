import sys
sys.path.append('../../toolkit')
sys.path.append('bin')
import os
import XswUtility
import re
import time
import xml.etree.ElementTree as ET
from config import *
import logging
logging.basicConfig(level=logging.DEBUG)

re.I = True

#========================================================================
#global function
from PyQt4.QtGui import *

def ShowMsgBox(str):
    QMessageBox.about(self, "My message box", 'haha')
  
def DbgPrint(str):
    if g_bDbgLog:
        todayStr = time.strftime('%Y-%m-%d %X ',  time.localtime(time.time()))
        logging.debug('%s :%s' % (todayStr, str))

def InfoPrint(str):
    if g_bInfoLog:
        print('INF:%s' % str)
        
def ErrPrint(str):
    import   traceback 
    if g_bErrLog:
        print('ERR:%s' % str,  )
        print(traceback.format_exc() )
              
#========================================================================
#class CCfg

#err_log
#    <err_log>
#        <err name="严重异常">
#            <!--文件名，部分匹配，支持正则表达式-->
#            <file_name val="*err*" />
#            <!--包含这些log内容则说明有问题，优先于exclude规则-->
#            <include_line val="包含我则有问题" />
#            <!--如果文件内容只有这些行，则不告警，支持部分匹配-->
#            <exclude_line val="排除我吧1" />
#            <exclude_line val="排除我吧2" />
#        </err>
#    </err_log>

class AddWarnLogMsg:
    ret = False
    type = ''
    match_reason = ''
    rule = ''           #匹配的规则
    line = 0           #错误行号
    file_full_name =''   #文件名
    
class CErrRule:
    def __init__(self,  name):
        self.name = name
        self.dicRule = {}
        
    def AddRule(self,  tag,  val):
        rule = self.dicRule.get(tag)
        if rule == None:
            self.dicRule[tag] = []
            rule = self.dicRule.get(tag)
        rule.append(val)
        
    def GetName(self):
        return self.name
    
    def IsMatchFileName(self,  base_file_name):
        '''文件名是否匹配'''
        file_name_rules = self.dicRule['file_name']
        if file_name_rules == None:
            return False
        for file_name_str in file_name_rules:
            reFileName = re.compile(file_name_str,  re.IGNORECASE)
            m = reFileName.match(base_file_name)
            if m:
                return True,  file_name_str
                
        return False
        
    def IsMatchFileContent(self,  full_file_name,  start_file_line=0,  start_file_pos=0):
        '''文件内容是否匹配'''
        f = open(full_file_name, "r")
        if f == None:
            return False
        lines = f.readlines()
        #用内存映射方式读取文件
        f.close()
        for line in lines:
            if self.IsMatchIncludeLine(line):
                return line
            
        return True
            
    def IsMatch(self,  file_full_name):
        retDetail = AddWarnLogMsg()
        retDetail.type = self.GetName()
        retDetail.file_full_name = file_full_name
       
        '''文件是否匹配'''
        base_file_name = os.path.split(file_full_name)[1]
        ret = self.IsMatchFileName(base_file_name)
        if ret:
            retDetail.ret = True
            retDetail.match_reason = '文件名匹配'
            retDetail.rule = ret[1]
            retDetail.line = 0
        
        return retDetail

class CCfg:
    def __init__(self,  ini_file,  history_file,  app_dir):
        self.LogPath = ''
        self.ftp_user = ''
        self.ftp_pw = ''
        self.last_log_path = ''
        self.down_speed = ''
        self.ini_file = ''
        self.history_file = ''
        self.app_dir = ''
        self.ini_file = ini_file
        self.history_file = history_file
        self.app_dir = XswUtility.AppendPathTag(app_dir)
        self.LoadCfg()
        self.LoadHistory()

    def LoadCfg(self):
        xml_tree = ET.parse(self.ini_file)
        xml_root = xml_tree.getroot()
#<log
#    path = "log"
#    ftp_user = "syslog_log"
#    ftp_pw = "XfjaHxZ3AMN>~rm"
#/>
        xml_log = xml_root.find('log')
        if (xml_log != None):
            self.LogPath = xml_log.get('path')
            self.ftp_user = xml_log.get('ftp_user')
            self.ftp_pw = xml_log.get('ftp_pw')
            self.last_log_path = xml_log.get('last_log_path')
            self.down_speed = xml_log.get('down_speed')

           #create log path
            self.AppPath =  self.app_dir
            if(self.LogPath[1] != ':'):
                self.LogPath = self.app_dir
            if(self.LogPath  != '' and not os.path.exists(self.LogPath)):
                if(self.LogPath[-1] != '/' or self.LogPath[-1]!='\\'):
                    self.LogPath += '\\'
                os.makedirs(self.LogPath)
                
#err_log
#    <err_log>
#        <err name="严重异常">
#            <!--文件名，部分匹配，支持正则表达式-->
#            <file_name val="*err*" />
#            <!--包含这些log内容则说明有问题，优先于exclude规则-->
#            <include_line val="包含我则有问题" />
#            <!--如果文件内容只有这些行，则不告警，支持部分匹配-->
#            <exclude_line val="排除我吧1" />
#            <exclude_line val="排除我吧2" />
#        </err>
#    </err_log>
        self.ErrRules = []  #CErrRule数组
        xml_err_log = xml_root.find('err_log')
        if xml_err_log != None:
            for xml_err in xml_err_log:
                rule = CErrRule(xml_err.get('val')) #开始解析一个CErrRule
                for xml_rule in xml_err:
                    rule.AddRule(xml_rule.tag,  xml_rule.get('val'))
                    self.ErrRules.append(rule)
                    
    def __del__(self):
        self.Write()
        
    def LoadHistory(self):
        xml_tree = ET.parse(self.history_file)
        xml_root = xml_tree.getroot()     
        xml_log = xml_root.find('log')
        if (xml_log != None):
            self.last_log_path = xml_log.get('last_log_path')
            self.down_speed = xml_log.get('down_speed')
       
    def Write(self):
        xml_tree = ET.parse(self.history_file)
        xml_root = xml_tree.getroot()  
        xml_log = xml_root.find('log')
        if xml_log == None:
            xml_log = ET.Element("log")
            xml_root.append(xml_log)
            
        xml_log.set('last_log_path',  self.GetLastLogPath())
        xml_log.set('down_speed',  self.GetDownSpeed())
        xml_tree.write(self.history_file)
                    
    def GetDayLogPath(self,  strDay, svr=''):
        if(svr == ''):
            return self.LogPath + (strDay)
        return self.LogPath + strDay + '\\' + svr
        
    def GetLogFtpUser(self):
        return self.ftp_user
        
    def GetLogFtpPw(self):
        return self.ftp_pw
    
    #是否错误的log
    def IsErrLogFile(self,  file_name):
        for rule in self.ErrRules:
            ret = rule.IsMatch(file_name)
            if ret:
                return ret
        return None
                
    def GetLastLogPath(self):
        '''最近的log目录 '''
        return self.last_log_path
    def SetLastLogPath(self,  val):
        '''最近的log目录 '''
        self.last_log_path = val
        self.Write()
    def GetDownSpeed(self):
        if self.down_speed==None or len(self.down_speed) == 0:
            self.down_speed = '500'
        return self.down_speed
    def SetDownSpeed(self,  val):
        self.down_speed = val
        self.Write()
        
#    def GetTodayLogPath(self,  svr=''):
#        '''本日log目录 '''
#        tm = time.localtime()
#        strDay = ('%d-%d' % (tm.tm_mon,  tm.tm_mday))
#        path = self.GetDayLogPath(strDay,  svr)
#        return path
            
g_cfg = CCfg('sunnybug.xml',  'history.xml', os.path.dirname(sys.argv[0]))

if __name__ == "__main__":
    print(g_cfg.IsErrLogFile('hierr.log'))
