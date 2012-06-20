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

#=================================================================================================
#cfg
TXT_BTN_DOWNLOG_ING = '停止下载'
TXT_BTN_DOWNLOG_IDLE = '开始'
TXT_BTN_ANALYSTLOG_ING = '停止解析'
TXT_BTN_ANALYSTLOG_IDLE = '解析Log'
TXT_BTN_MERGELOG_ING = '停止合并'
TXT_BTN_MERGELOG_IDLE = '合并Log'
TXT_BTN_IMPORTLOG_ING = '停止导入'
TXT_BTN_IMPORTLOG_IDLE = '导入Log'

#LogDlg内部消息
MSG_LOGDLG_BEGIN = 1000
MSG_ANALYST_LOG_START      = MSG_LOGDLG_BEGIN + 3
MSG_ANALYST_LOG_FINISH      = MSG_LOGDLG_BEGIN + 4
MSG_DOWN_LOG_START          = MSG_LOGDLG_BEGIN + 5
MSG_DOWN_LOG_FINISH          = MSG_LOGDLG_BEGIN + 6
MSG_ANALYST_MERGE_START   = MSG_LOGDLG_BEGIN + 7
MSG_ANALYST_MERGE_FINISH   = MSG_LOGDLG_BEGIN + 8
MSG_ANALYST_IMPORT_START   = MSG_LOGDLG_BEGIN + 9
MSG_ANALYST_IMPORT_FINISH   = MSG_LOGDLG_BEGIN + 10

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


#help func
def CreateFtpDownCmd(strUrl, strUser='', strPw='', strLocalPath='', bSync=False):
    WGET_CMD = r'wget.exe'
    #X:\tool\net\wget\wget.exe -c ftp://up:upup@121.207.234.87/GameServer_syslog/Stat*
    #-b:background download
    #--limit_rate:下载速度限制
    #-P:目标目录
    strCMD = '%s -c --ignore-case --timeout=60 --limit-rate=%sk "ftp://%s:%s@%s" -P "%s" ' % (WGET_CMD, g_cfg.GetDownSpeed(), strUser, strPw, strUrl, strLocalPath)
    return strCMD

def CreateDownCmd(strDownStr,  strPath,  lstlstSvrs,  bIsFuzzy):
    '''创建下载命令序列，返回下载命令列表，按ip进行管理'''
    dicCmd = {} #key:ip value:list of string
    for lstSvrs in lstlstSvrs:
        for svr in lstSvrs[FIELD_FTPSVR_LIST]:
            svr_name = svr[FTPSVR_FIELD_NAME]
            ip = svr[FTPSVR_FIELD_IP]
            #path
            path = os.path.join(strPath,  lstSvrs[FIELD_FTPSVR_NAME]+'_'+svr_name+'_'+ip)
            XswUtility.MKDir(path)
            #一个服务器可能有多个目录要下载
            for log_path in svr[FTPSVR_FIELD_PATH]:
                if (bIsFuzzy):#是否模糊匹配
                    strCmd = CreateFtpDownCmd(('%s/%s/*%s*.log' % (ip, log_path, strDownStr)), g_cfg.GetLogFtpUser(), g_cfg.GetLogFtpPw(), path)
                else:
                    strCmd = CreateFtpDownCmd(('%s/%s/%s' % (ip, log_path, strDownStr)), g_cfg.GetLogFtpUser(), g_cfg.GetLogFtpPw(), path)
                if not dicCmd.get(ip):
                    dicCmd[ip] = []
                dicCmd[ip].append(strCmd)

    return dicCmd              
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
            DbgPrint(base_file_name + "     " + file_name_str)
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
        '''文件是否匹配'''
        base_file_name = os.path.split(file_full_name)[1]
        ret = self.IsMatchFileName(base_file_name)
        if ret:
            retDetail = AddWarnLogMsg()
            retDetail.type = self.GetName()
            retDetail.file_full_name = file_full_name
            retDetail.ret = True
            retDetail.match_reason = '文件名匹配'
            retDetail.rule = ret[1]
            retDetail.line = 0
            return retDetail
        return None

class CAutoDownRule(object):
    """docstring for CAutoDownRule"""
    def __init__(self, file_name, svr_name):
        super(CAutoDownRule, self).__init__()
        self.file_name = file_name
        self.svr_name = svr_name
        
class CCfg:
    def __init__(self,  ini_file,  history_file,  app_dir):
        self.ini_file = os.path.join(app_dir, ini_file)
        self.history_file = os.path.join(app_dir, history_file)
        self.app_dir = ''
        self.app_dir = XswUtility.AppendPathTag(app_dir)
        self.lstAutoDown = []

        self.xml_cfg = ET.parse(self.ini_file)
        self.xml_cfg_log = self.xml_cfg.getroot().find('log')
        if self.xml_cfg_log == None:
            self.xml_cfg_log = ET.Element("log")
            self.xml_cfg.getroot().append(self.xml_cfg_log)
            
        self.xml_cfg_autodown = self.xml_cfg.getroot().find('auto_down')
        if self.xml_cfg_autodown == None:
            self.xml_cfg_autodown = ET.Element("auto_down")
            self.xml_cfg.getroot().append(self.xml_cfg_autodown)

        self.xml_history = ET.parse(self.history_file)
        self.xml_history_log = self.xml_history.getroot().find('log')
        if self.xml_history_log == None:
            self.xml_history_log = ET.Element("log")
            self.xml_history.getroot().append(self.xml_history_log)

        self.LoadCfg()

    def LoadCfg(self):
        self.ErrRules = []  #CErrRule数组
        xml_err_log = self.xml_cfg.getroot().find('err_log')
        if xml_err_log != None:
            for xml_err in xml_err_log:
                rule = CErrRule(xml_err.get('val')) #开始解析一个CErrRule
                for xml_rule in xml_err:
                    rule.AddRule(xml_rule.tag,  xml_rule.get('val'))
                    self.ErrRules.append(rule)

        for xml in self.xml_cfg_autodown:
            self.lstAutoDown.append( CAutoDownRule(xml.get('file_name'), xml.get('svr_name')))
                    
    def __del__(self):
        self.Write()
        
    # def LoadHistory(self):
    #     xml_tree = ET.parse(self.history_file)
    #     xml_root = xml_tree.getroot()     
    #     xml_log = xml_root.find('log')
    #     # if (xml_log != None):
    #     #     self.down_speed = xml_log.get('down_speed')
    #     #     self.check_log_path = xml_log.get('check_log_path')
    #     #     self.check_log_minute = xml_log.get('check_log_minute')
     
    def Write(self):
        # xml_log.set('down_speed',  self.GetDownSpeed())
        self.xml_history.write(self.history_file)
                    
    # def GetDayLogPath(self,  strDay, svr=''):
    #     if(svr == ''):
    #         return self.LogPath + (strDay)
    #     return self.LogPath + strDay + '\\' + svr
        
    def GetLogFtpUser(self):
        return self.xml_cfg_log.get('ftp_user')
        
    def GetLogFtpPw(self):
        return self.xml_cfg_log.get('ftp_pw')
    
    #是否错误的log
    def IsErrLogFile(self,  file_name):
        for rule in self.ErrRules:
            ret = rule.IsMatch(file_name)
            if ret:
                return ret
        return None
                
    def GetLastLogPath(self):
        return self.xml_history_log.get('last_log_path')
    def SetLastLogPath(self,  val):
        self.xml_history_log.set('last_log_path',  val)
        self.Write()

    def GetCheckPath(self):
        return self.xml_history_log.get('check_path')
    def SetCheckPath(self,  val):
        self.xml_history_log.set('check_path',  val)
        self.Write()

    def GetCheckMiniute(self):
        return self.xml_history_log.get('check_minute')
    def SetCheckMiniute(self,  val):
        self.xml_history_log.set('check_minute',  val)
        self.Write()
        
    def GetDownSpeed(self):
        return self.xml_history_log.get('down_speed')
    def SetDownSpeed(self,  val):
        self.xml_history_log.set('down_speed',  val)
        self.Write()

    def GetAutoDownList(self):
        return self.lstAutoDown
      
#    def GetTodayLogPath(self,  svr=''):
#        '''本日log目录 '''
#        tm = time.localtime()
#        strDay = ('%d-%d' % (tm.tm_mon,  tm.tm_mday))
#        path = self.GetDayLogPath(strDay,  svr)
#        return path
            
g_cfg = CCfg('sunnybug.xml',  'history.xml', os.path.dirname(sys.argv[0]))

if __name__ == "__main__":
    print(g_cfg.IsErrLogFile('AutoIDWarn 2012-5-29.log'))
