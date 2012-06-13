#coding=utf-8
import sys
sys.path.append('ui')
sys.path.append('bin')
import os

from Ui_DlgLog import Ui_DlgLog
from cfg import *
from config import *
import XswUtility
import time

from PyQt4 import QtCore, QtGui
from PyQt4.QtCore import *
from PyQt4.QtGui import *
QtCore.Signal = QtCore.pyqtSignal
from cfg import AddWarnLogMsg
try:
	_fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
	_fromUtf8 = lambda s: s
import copy
from xAnalystLog import *
from xTaskMgr import *
from SvrCheckTree import *

#========================================================================
#help func
def CreateFtpDownCmd(strUrl, strUser='', strPw='', strLocalPath='', bSync=False):
	WGET_CMD = r'wget.exe'
	#X:\tool\net\wget\wget.exe -c ftp://up:upup@121.207.234.87/GameServer_syslog/Stat*
	#-b:background download
	#--limit_rate:下载速度限制
	#-P:目标目录
	strCMD = '%s -c --ignore-case --timeout=60 --limit-rate=%sk "ftp://%s:%s@%s" -P "%s" ' % (WGET_CMD, g_cfg.GetDownSpeed(), strUser, strPw, strUrl, strLocalPath)
	return strCMD


class CLogTimeStruct():
	def __init__(self):
		self.year = 0
		self.month = 0
		self.day = 0
		self.hour = 0
		self.minute = 0
		self.second = 0
		self.microsecond = 0
class CLogTime():
	'''格式: 2011-08-14 05:10:52.109  '''
	fmt = '%Y-%m-%d %H:%M:%S.%f'
	reLogHeader = re.compile(r'^(?P<year>\d{4})-(?P<month>\d\d?)-(?P<day>\d{2}) (?P<hour>\d{2}):(?P<minute>\d{2}):(?P<second>\d{2})\.(?P<microsecond>\d{3})\t')
	def __init__(self,  str=''):
		self.time = CLogTimeStruct()
		self.str = ''
		if str!= '':
			ParseString(str)

	def ParseString(str):
		if  (len(str) < 25):
			return False
		if(str[4]=='-') and (str[7]=='-') and (str[10] ==' ') and (str[13]==':') and (str[16]==':') and (str[19]=='.'):
			return True
			self.time.year          = int(str[0: 4])
			self.time.month        = int(str[5: 7])
			self.time.day           = int(str[8: 10])
			self.time.hour          = int(str[11:13])
			self.time.minute       = int(str[14:16])
			self.time.second      = int(str[17:19])
			self.time.microsecond= int(str[20:23])
			self.str = str[0:23]

			#self.time = datetime.datetime.strptime(str[0:23],  self.fmt)    #+7s
			return True
		return False        
	def ParseStringByRegex(str):
		m = self.reLogHeader.search(str)
		if not m:
			return False
		self.time.year          = int(m.group('year'))
		self.time.month        = int(m.group('month'))
		self.time.day           = int(m.group('day'))
		self.time.hour          = int(m.group('hour'))
		self.time.minute        = int(m.group('minute'))
		self.time.second        = int(m.group('second'))
		self.time.microsecond = int(m.group('microsecond'))
		self.str = str[0:23]
		return True

	def GetTimeStr(str):
		if  (len(str) < 25):
			return None
		if(str[4]=='-') and (str[7]=='-') and (str[10] ==' ') and (str[13]==':') and (str[16]==':') and (str[19]=='.'):
			return str[0:23]
		return None


class CLog():
	def __init__(self):
		self.time = ''
		self.content = ''


#========================================================================
#
class CAutoCheckLogTab():
	def __init__(self, parent):
		self.parent = parent
		self.thdCheckLogThread = None

	def OnBtnCheckLog(self):
		if(self.parent.ui.btnCheckLog.text() == TXT_BTN_ANALYSTLOG_IDLE):
			g_cfg.SetCheckPath(self.parent.ui.edtCheckLogPath.text())
			g_cfg.SetCheckMiniute(self.parent.ui.edtCheckLogMinute.text())
			self.parent.ui.btnCheckLog.setText(TXT_BTN_ANALYSTLOG_ING)
			self.thdCheckLogThread = CAutoCheckLogThread(self.parent)
			self.thdCheckLogThread.start()
		elif self.thdCheckLogThread is not None:
			self.parent.ui.btnCheckLog.setText(TXT_BTN_ANALYSTLOG_IDLE)
			self.thdCheckLogThread.stop()
			self.thdCheckLogThread = None


#========================================================================
#treeViewWarnLog
WARNLIST_TYPE       = 0
WARNLIST_SVR = 1
WARNLIST_FILENAME = 2
WARNLIST_INFO        = 3

class CWarnListUI():
	def __init__(self,  parent,  treeview):
		self.model = QtGui.QStandardItemModel(0, 4, parent)
		self.model.setHeaderData(WARNLIST_TYPE, QtCore.Qt.Horizontal, "类型")
		self.model.setHeaderData(WARNLIST_SVR, QtCore.Qt.Horizontal, "服务器")
		self.model.setHeaderData(WARNLIST_FILENAME, QtCore.Qt.Horizontal, "文件名")
		self.model.setHeaderData(WARNLIST_INFO, QtCore.Qt.Horizontal, "对应规则")

		self.treeview = treeview
		self.treeview.setModel(self.model)
		self.treeview.setSortingEnabled(True)        
		self.treeview.setColumnWidth (WARNLIST_TYPE, 80)
		self.treeview.setColumnWidth (WARNLIST_SVR, 200)
		self.treeview.setColumnWidth (WARNLIST_FILENAME, 200)

		#历史信息缓存
		self.dicCache = {}

	def Add(self,  type,  filename,  info, svr):
		#过滤服务器名、文件名、规则一致的
		str = filename + "x" + info + "x" + svr
		if self.dicCache.get(str)  != None:
			return
		self.dicCache[str] = 1

		self.model.insertRow(0)
		self.model.setData(self.model.index(0, WARNLIST_TYPE), type)
		self.model.setData(self.model.index(0, WARNLIST_SVR), svr)
		self.model.setData(self.model.index(0, WARNLIST_FILENAME), filename)
		self.model.setData(self.model.index(0, WARNLIST_INFO), info)

#========================================================================
#LogForm
class LogDlg(QtGui.QMainWindow):
	signalShowMsg = QtCore.Signal(str)
	signalProcessMsg = QtCore.Signal(int)
	signalAddWarnLog = QtCore.Signal(AddWarnLogMsg)#(err,  rule,  full_name)
	signalTaskStop = QtCore.Signal(int)
	signalAllTaskFinish = QtCore.Signal()
	def __init__(self, parent=None):
		QtGui.QWidget.__init__(self, parent)
		self.ui = Ui_DlgLog()
		self.ui.setupUi(self)
		self.taskMgr = CCmdTaskMgr(self)
		self.thdAnalystLog = CAnalystLogThread(self)

		#tab
		self.tabCheckLog = CAutoCheckLogTab(self)

		#追加文本自动滚动
		self.ui.edtMsg.ensureCursorVisible()    
		#服务器treeview列表
		self.modelSvrGroup = SvrCheckTree()
		for svr in g_lstFtpGroup:
			self.modelSvrGroup.AddItem([svr[FIELD_FTPSVR_NAME]])
		self.ui.tvSvr.setModel(self.modelSvrGroup)
		self.ui.chkAllGroup.stateChanged[int].connect(self.chkAllGroupStateChanged)

		##########
		#init ui context
		self.ui.btnCheckLog.setText(TXT_BTN_ANALYSTLOG_IDLE)
		self.ui.btnMergeLog.setText(TXT_BTN_MERGELOG_IDLE)
		self.ui.btnImportLog.setText(TXT_BTN_IMPORTLOG_IDLE)
		self.ui.edtLogPath.setText(g_cfg.GetLastLogPath())
		self.ui.edtCheckLogPath.setText(g_cfg.GetCheckPath())
		self.ui.edtCheckLogMinute.setText(g_cfg.GetCheckMiniute())
		self.uiWarnList = CWarnListUI(self,  self.ui.treeViewWarnLog) 
		self.ui.edtDownSpeed.setText(g_cfg.GetDownSpeed())

		##########
		#服务器列表
		self.svrButtonGroup = QtGui.QButtonGroup()
		self.svrButtonGroup.setExclusive(False)

		self.chkAllSvr = QtGui.QCheckBox(self.ui.horizontalLayoutWidget)
		self.ui.horizontalLayout.addWidget(self.chkAllSvr)
		self.chkAllSvr.setText(QtGui.QApplication.translate("DlgLog", 'All', None, QtGui.QApplication.UnicodeUTF8))
		self.chkAllSvr.setChecked(True)
		self.chkAllSvr.stateChanged[int].connect(self.chkAllSvrStateChanged)
		self.DrawSvrGroupList()

		##########
		#操作界面的信号槽
		self.signalShowMsg.connect(self.ShowMsg)
		self.signalProcessMsg.connect(self.ProcessMsg)
		self.signalAddWarnLog.connect(self.AddWarnLog)
		self.signalAllTaskFinish.connect(self.OnAllTaskFinish)

		self.ShowMsg('启动完成')

	def CreateDownCmd(self,  strDownStr,  strPath,  lstlstSvrs,  bIsFuzzy):
		'''创建下载命令序列，返回下载命令列表，按ip进行管理'''
		dicCmd = {} #key:ip value:list of string
		for lstSvrs in lstlstSvrs:
			for svr in lstSvrs[FIELD_FTPSVRrLIST]:
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

	def DownLogByFileName(self):
		str = self.ui.btnDownLogByFileName.text()
		if(str == TXT_BTN_DOWNLOG_IDLE):
			self.ui.btnDownLogByFileName.setText(TXT_BTN_DOWNLOG_ING)
			self.taskMgr.StartTask()
		else:
			self.StopAllTask() 

	def StopAllTask(self):
		os.system('taskkill /f /im wget.exe')
		self.taskMgr.StopAllTask()
		self.ui.btnDownLogByFileName.setText(TXT_BTN_DOWNLOG_IDLE)

	def OnBtnCheckLog(self):
		self.tabCheckLog.OnBtnCheckLog()

	def ImportLog(self):
		self.thdAnalystLog = CAnalystLogThread(self)
		self.thdAnalystLog.SetCmd(CAnalystLogThread.LOG_CMD_IMPORT)
		self.thdAnalystLog.ProcessImport()
		return

		if(self.ui.btnImportLog.text() == TXT_BTN_IMPORTLOG_IDLE):
			self.thdAnalystLog = CAnalystLogThread(self)
			self.thdAnalystLog.SetCmd(CAnalystLogThread.LOG_CMD_IMPORT)
			self.thdAnalystLog.start()
		elif self.thdAnalystLog is not None:
			self.thdAnalystLog.stop()
			self.thdAnalystLog = None

	def MergeLog(self):
		self.thdAnalystLog = CAnalystLogThread(self)
		self.thdAnalystLog.SetCmd(CAnalystLogThread.LOG_CMD_ANALYST)
		self.thdAnalystLog.ProcessMerge()
		return

		if(self.ui.btnMergeLog.text() == TXT_BTN_MERGELOG_IDLE):
			self.thdAnalystLog = CAnalystLogThread(self)
			self.thdAnalystLog.SetCmd(CAnalystLogThread.LOG_CMD_MERGE)
			self.thdAnalystLog.start()
		elif self.thdAnalystLog is not None:
			self.thdAnalystLog.stop()
			self.thdAnalystLog = None

	def ShowMsg(self,  str):
		todayStr = time.strftime('%Y-%m-%d %X ',  time.localtime(time.time()))
		self.ui.edtMsg.append(todayStr + str)

	def ProcessMsg(self,  msg):
		if msg == MSG_DOWN_LOG_START:
			self.ShowMsg('下载log:开始')
			self.ui.btnDownLogByFileName.setText(TXT_BTN_DOWNLOG_ING)
			self.ui.edtLogPath.setText(g_cfg.GetLastLogPath())
		elif msg == MSG_DOWN_LOG_FINISH:
			self.ShowMsg('下载log:结束')
			self.ui.btnDownLogByFileName.setText(TXT_BTN_DOWNLOG_IDLE)   
		elif msg == MSG_ANALYST_LOG_START:
			self.ShowMsg('自动检查log任务开始')
			#self.ui.btnCheckLog.setText(TXT_BTN_ANALYSTLOG_ING)
		elif msg == MSG_ANALYST_LOG_FINISH:
			self.ShowMsg('自动检查log任务结束')
			#self.ui.btnCheckLog.setText(TXT_BTN_ANALYSTLOG_IDLE)
		elif msg == MSG_ANALYST_MERGE_START:
			self.ShowMsg('合并Log:开始')
			self.ui.btnMergeLog.setText(TXT_BTN_MERGELOG_ING)
		elif msg == MSG_ANALYST_MERGE_FINISH:
			self.ShowMsg('合并Log:结束')
			self.ui.btnMergeLog.setText(TXT_BTN_MERGELOG_IDLE)
		elif msg == MSG_ANALYST_IMPORT_START:
			self.ShowMsg('导入Log:开始')
			self.ui.btnImportLog.setText(TXT_BTN_IMPORTLOG_ING)
		elif msg == MSG_ANALYST_IMPORT_FINISH:
			self.ShowMsg('导入Log:结束')
			self.ui.btnImportLog.setText(TXT_BTN_IMPORTLOG_IDLE)

	def AddWarnLog(self,  stAddWarnLogMsg):
		self.uiWarnList.Add(stAddWarnLogMsg.type,  os.path.split(stAddWarnLogMsg.file_full_name)[1],  stAddWarnLogMsg.rule,
			XswUtility.GetParentPath(stAddWarnLogMsg.file_full_name))

	def OnAllTaskFinish(self):
		if(self.ui.chkCompressOnFinish.isChecked()):
			strFile = self.ui.edtLogPath.text()
			if(strFile[-1] == '/' or strFile[-1]=='\\'):
				strFile = strFile[0:-2]
			strFile += ".7z"
			XswUtility.Compress(self.ui.edtLogPath.text(),  strFile)

	def OnAddTask(self):
		strPath = g_cfg.GetLastLogPath()
		XswUtility.MKDir(strPath)
		g_cfg.SetLastLogPath(strPath)
		dicCmd = self.CreateDownCmd(self.ui.edtDownByFileName.text(),  strPath,  self.GetSvrListFromTreeView(),  self.ui.chkFuzzy.isChecked())
		for ip, lstCmd in dicCmd.items():
			if( not self.ui.chkCompressOnFinish.isChecked()):
				for cmd in lstCmd:
					if cmd.find('gmlog')!=-1:
						lstCmd.remove(cmd)
			self.taskMgr.AddTaskCmd(ip,  lstCmd)

	def OnClearTask(self):
		self.taskMgr.StopAllTask()

	def OnDBClickedWarnLogList(self,  stQModelIndex):
		DbgPrint(stQModelIndex)

	def chkAllSvrStateChanged(self,  nState):
		#0:unchecked 2:checked
		for btn in self.svrButtonGroup.buttons():
			btn.setChecked(nState==2)
	
	def chkAllGroupStateChanged(self,  nState):
		#0:unchecked 2:checked
		for btn in range(0, self.modelSvrGroup.GetCount()):
			self.modelSvrGroup.SetCheck(self.modelSvrGroup.index(btn,  0,  self.ui.tvSvr.rootIndex() ),  nState==2)
		
#	def RedrawSvrList(self):
#		#clear
#		for btn in self.svrButtonGroup.buttons():
#			self.svrButtonGroup.removeButton(btn)
#
#		while self.ui.horizontalLayout.count() > 0:
#			item = self.ui.horizontalLayout.itemAt(0)
#			self.ui.horizontalLayout.removeItem(item)
#
#		self.ui.horizontalLayout.addWidget(self.chkAllSvr)
#		self.chkAllSvr.setChecked(True)
#
#		lstSvrLst = self.GetSvrList(False)
#		for  svr in lstSvrLst[FIELD_FTPSVR_LIST]:
#			check = QtGui.QCheckBox(self.ui.horizontalLayoutWidget)
#			self.ui.horizontalLayout.addWidget(check)
#			check.setText(QtGui.QApplication.translate("DlgLog", svr[FTPSVR_FIELD_NAME], None, QtGui.QApplication.UnicodeUTF8))
#			check.setChecked(True)
#			self.svrButtonGroup.addButton(check)
			
	def DrawSvrGroupList(self):
		#self.ui.horizontalLayout.removeWidget(self.chkAllSvr)
		self.ui.horizontalLayout.addWidget(self.chkAllSvr)
		self.chkAllSvr.setChecked(True)

		lstSvrLst = ['HS','LS','DB','GS1','GS2','GS3','GS4','GS5','GS6']
		for  svr in lstSvrLst:
			check = QtGui.QCheckBox(self.ui.horizontalLayoutWidget)
			self.ui.horizontalLayout.addWidget(check)
			check.setText(QtGui.QApplication.translate("DlgLog", svr, None, QtGui.QApplication.UnicodeUTF8))
			check.setChecked(True)
			self.svrButtonGroup.addButton(check)
			
#	def GetSvrList(self, bGetSelected):
#		'''返回等待处理的服务器名列表,类型同g_lstFtpGroup'''
#		lstSvrLst =[]
#		#根据区名取该区所有服务器
#		strSvrGroupName = self.ui.cbSvrGroup.currentText()
#		for ftpGroup in g_lstFtpGroup:
#			if ftpGroup[FIELD_FTPSVR_NAME] == strSvrGroupName:
#				lstSvrLst = copy.deepcopy(ftpGroup)
#				break
#
#		#哪些服务器被选择
#		if bGetSelected:        
#			for btn in self.svrButtonGroup.buttons():
#				if not btn.isChecked():
#					idx = 0
#					for i in lstSvrLst[FIELD_FTPSVR_LIST]:
#						if i[FTPSVR_FIELD_NAME] == btn.text():
#							lstSvrLst[FIELD_FTPSVR_LIST].remove(i)
#							break
#						idx+=1
#		return lstSvrLst
		
	def GetSvrListFromTreeView(self):
		'''返回等待处理的服务器名列表,类型同g_lstFtpGroup'''

		lstSvrLst =[]
		#从配置中读取被选中组的所有服务器
		for ftpGroup in g_lstFtpGroup:
			if self.modelSvrGroup.IsChecked(ftpGroup[FIELD_FTPSVR_NAME]):
				lstSvrLst += [copy.deepcopy(ftpGroup)]
		
		#哪些服务器类型被选择
		for btn in self.svrButtonGroup.buttons():
			if not btn.isChecked():
				for j in lstSvrLst:
					for i in j[FIELD_FTPSVR_LIST]:
						if i[FTPSVR_FIELD_NAME] == btn.text():
							j[FIELD_FTPSVR_LIST].remove(i)
		return lstSvrLst

	def OnLogPathEditFinish(self):
		g_cfg.SetLastLogPath(self.ui.edtLogPath.text())
		g_cfg.Write()

	def OnDownSpeedEditFinish(self):
		g_cfg.SetDownSpeed(self.ui.edtDownSpeed.text())
		g_cfg.Write()

	def OnSvrGropuChanged(self, strCur):
		self.RedrawSvrList()

	def OnBtnDumpCmd(self):
		self.taskMgr.DumpCmd()

	def OnSvrTreeClicked(self,  idx):#idx:QModelIndex
		#DbgPrint(str(idx.internalPointer().itemData[0]))
		self.modelSvrGroup.ReverseCheck(idx)
		self.ui.tvSvr.clearSelection()	#通过这个接口刷新界面


        
if __name__ == "__main__":
	LogDlg()
