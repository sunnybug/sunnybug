# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'D:\doc\gitpub\github_sunnybug\python\demo\xPyQTDemo\ui\DlgLog.ui'
#
# Created: Sun Apr 01 14:31:35 2012
#      by: PyQt4 UI code generator 4.7.7
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    _fromUtf8 = lambda s: s

class Ui_DlgLog(object):
    def setupUi(self, DlgLog):
        DlgLog.setObjectName(_fromUtf8("DlgLog"))
        DlgLog.setEnabled(True)
        DlgLog.resize(793, 726)
        self.edtMsg = QtGui.QTextEdit(DlgLog)
        self.edtMsg.setGeometry(QtCore.QRect(0, 480, 791, 251))
        self.edtMsg.setReadOnly(True)
        self.edtMsg.setObjectName(_fromUtf8("edtMsg"))
        self.btnAnalystLog = QtGui.QPushButton(DlgLog)
        self.btnAnalystLog.setEnabled(True)
        self.btnAnalystLog.setGeometry(QtCore.QRect(320, 90, 75, 23))
        self.btnAnalystLog.setCheckable(False)
        self.btnAnalystLog.setObjectName(_fromUtf8("btnAnalystLog"))
        self.edtLogPath = QtGui.QLineEdit(DlgLog)
        self.edtLogPath.setGeometry(QtCore.QRect(80, 0, 401, 20))
        self.edtLogPath.setObjectName(_fromUtf8("edtLogPath"))
        self.treeViewWarnLog = QtGui.QTreeView(DlgLog)
        self.treeViewWarnLog.setGeometry(QtCore.QRect(290, 310, 491, 161))
        self.treeViewWarnLog.setObjectName(_fromUtf8("treeViewWarnLog"))
        self.btnMergeLog = QtGui.QPushButton(DlgLog)
        self.btnMergeLog.setEnabled(True)
        self.btnMergeLog.setGeometry(QtCore.QRect(240, 90, 75, 23))
        self.btnMergeLog.setCheckable(False)
        self.btnMergeLog.setObjectName(_fromUtf8("btnMergeLog"))
        self.label = QtGui.QLabel(DlgLog)
        self.label.setGeometry(QtCore.QRect(0, 0, 71, 16))
        self.label.setObjectName(_fromUtf8("label"))
        self.groupSvrList = QtGui.QGroupBox(DlgLog)
        self.groupSvrList.setGeometry(QtCore.QRect(100, 20, 501, 51))
        self.groupSvrList.setObjectName(_fromUtf8("groupSvrList"))
        self.horizontalLayoutWidget = QtGui.QWidget(self.groupSvrList)
        self.horizontalLayoutWidget.setGeometry(QtCore.QRect(0, 10, 501, 41))
        self.horizontalLayoutWidget.setObjectName(_fromUtf8("horizontalLayoutWidget"))
        self.horizontalLayout = QtGui.QHBoxLayout(self.horizontalLayoutWidget)
        self.horizontalLayout.setObjectName(_fromUtf8("horizontalLayout"))
        self.btnImportLog = QtGui.QPushButton(DlgLog)
        self.btnImportLog.setEnabled(True)
        self.btnImportLog.setGeometry(QtCore.QRect(160, 90, 75, 23))
        self.btnImportLog.setCheckable(False)
        self.btnImportLog.setObjectName(_fromUtf8("btnImportLog"))
        self.edtDownByFileName = QtGui.QLineEdit(DlgLog)
        self.edtDownByFileName.setGeometry(QtCore.QRect(80, 70, 231, 20))
        self.edtDownByFileName.setObjectName(_fromUtf8("edtDownByFileName"))
        self.btnDownLogByFileName = QtGui.QPushButton(DlgLog)
        self.btnDownLogByFileName.setEnabled(True)
        self.btnDownLogByFileName.setGeometry(QtCore.QRect(0, 120, 75, 23))
        self.btnDownLogByFileName.setCheckable(False)
        self.btnDownLogByFileName.setObjectName(_fromUtf8("btnDownLogByFileName"))
        self.edtDownSpeed = QtGui.QLineEdit(DlgLog)
        self.edtDownSpeed.setGeometry(QtCore.QRect(650, 40, 41, 20))
        self.edtDownSpeed.setInputMask(_fromUtf8(""))
        self.edtDownSpeed.setObjectName(_fromUtf8("edtDownSpeed"))
        self.label_2 = QtGui.QLabel(DlgLog)
        self.label_2.setGeometry(QtCore.QRect(600, 40, 51, 16))
        self.label_2.setObjectName(_fromUtf8("label_2"))
        self.label_3 = QtGui.QLabel(DlgLog)
        self.label_3.setGeometry(QtCore.QRect(700, 40, 31, 16))
        self.label_3.setObjectName(_fromUtf8("label_3"))
        self.label_4 = QtGui.QLabel(DlgLog)
        self.label_4.setGeometry(QtCore.QRect(0, 70, 71, 16))
        self.label_4.setObjectName(_fromUtf8("label_4"))
        self.cbSvrGroup = QtGui.QComboBox(DlgLog)
        self.cbSvrGroup.setGeometry(QtCore.QRect(0, 30, 101, 22))
        self.cbSvrGroup.setObjectName(_fromUtf8("cbSvrGroup"))
        self.chkFuzzy = QtGui.QCheckBox(DlgLog)
        self.chkFuzzy.setGeometry(QtCore.QRect(320, 70, 72, 17))
        self.chkFuzzy.setChecked(True)
        self.chkFuzzy.setObjectName(_fromUtf8("chkFuzzy"))
        self.btnAddTask = QtGui.QPushButton(DlgLog)
        self.btnAddTask.setEnabled(True)
        self.btnAddTask.setGeometry(QtCore.QRect(0, 90, 75, 23))
        self.btnAddTask.setCheckable(False)
        self.btnAddTask.setObjectName(_fromUtf8("btnAddTask"))
        self.btnClearTask = QtGui.QPushButton(DlgLog)
        self.btnClearTask.setEnabled(True)
        self.btnClearTask.setGeometry(QtCore.QRect(80, 90, 75, 23))
        self.btnClearTask.setCheckable(False)
        self.btnClearTask.setObjectName(_fromUtf8("btnClearTask"))
        self.chkCompressOnFinish = QtGui.QCheckBox(DlgLog)
        self.chkCompressOnFinish.setEnabled(True)
        self.chkCompressOnFinish.setGeometry(QtCore.QRect(450, 70, 81, 17))
        self.chkCompressOnFinish.setChecked(False)
        self.chkCompressOnFinish.setObjectName(_fromUtf8("chkCompressOnFinish"))
        self.btnDumpCmd = QtGui.QPushButton(DlgLog)
        self.btnDumpCmd.setEnabled(True)
        self.btnDumpCmd.setGeometry(QtCore.QRect(80, 120, 75, 23))
        self.btnDumpCmd.setCheckable(False)
        self.btnDumpCmd.setObjectName(_fromUtf8("btnDumpCmd"))
        self.chkGmlog = QtGui.QCheckBox(DlgLog)
        self.chkGmlog.setEnabled(True)
        self.chkGmlog.setGeometry(QtCore.QRect(390, 70, 51, 17))
        self.chkGmlog.setChecked(False)
        self.chkGmlog.setObjectName(_fromUtf8("chkGmlog"))
        self.tvSvr = QtGui.QTreeView(DlgLog)
        self.tvSvr.setGeometry(QtCore.QRect(0, 160, 151, 321))
        self.tvSvr.setObjectName(_fromUtf8("tvSvr"))
        self.chkAllGroup = QtGui.QCheckBox(DlgLog)
        self.chkAllGroup.setGeometry(QtCore.QRect(160, 120, 72, 17))
        self.chkAllGroup.setChecked(False)
        self.chkAllGroup.setObjectName(_fromUtf8("chkAllGroup"))

        self.retranslateUi(DlgLog)
        QtCore.QObject.connect(self.btnAnalystLog, QtCore.SIGNAL(_fromUtf8("clicked()")), DlgLog.AnalystLog)
        QtCore.QObject.connect(self.treeViewWarnLog, QtCore.SIGNAL(_fromUtf8("doubleClicked(QModelIndex)")), DlgLog.OnDBClickedWarnLogList)
        QtCore.QObject.connect(self.btnMergeLog, QtCore.SIGNAL(_fromUtf8("clicked()")), DlgLog.MergeLog)
        QtCore.QObject.connect(self.edtLogPath, QtCore.SIGNAL(_fromUtf8("editingFinished()")), DlgLog.OnLogPathEditFinish)
        QtCore.QObject.connect(self.btnImportLog, QtCore.SIGNAL(_fromUtf8("clicked()")), DlgLog.ImportLog)
        QtCore.QObject.connect(self.btnDownLogByFileName, QtCore.SIGNAL(_fromUtf8("clicked()")), DlgLog.DownLogByFileName)
        QtCore.QObject.connect(self.edtDownSpeed, QtCore.SIGNAL(_fromUtf8("editingFinished()")), DlgLog.OnDownSpeedEditFinish)
        QtCore.QObject.connect(self.btnClearTask, QtCore.SIGNAL(_fromUtf8("clicked()")), DlgLog.OnClearTask)
        QtCore.QObject.connect(self.btnAddTask, QtCore.SIGNAL(_fromUtf8("clicked()")), DlgLog.OnAddTask)
        QtCore.QObject.connect(self.btnDumpCmd, QtCore.SIGNAL(_fromUtf8("clicked()")), DlgLog.OnBtnDumpCmd)
        QtCore.QObject.connect(self.tvSvr, QtCore.SIGNAL(_fromUtf8("clicked(QModelIndex)")), DlgLog.OnSvrTreeClicked)
        QtCore.QMetaObject.connectSlotsByName(DlgLog)

    def retranslateUi(self, DlgLog):
        DlgLog.setWindowTitle(QtGui.QApplication.translate("DlgLog", "对话框试程序", None, QtGui.QApplication.UnicodeUTF8))
        self.btnAnalystLog.setText(QtGui.QApplication.translate("DlgLog", "分析", None, QtGui.QApplication.UnicodeUTF8))
        self.btnMergeLog.setText(QtGui.QApplication.translate("DlgLog", "合并", None, QtGui.QApplication.UnicodeUTF8))
        self.label.setText(QtGui.QApplication.translate("DlgLog", "当前LOG目录:", None, QtGui.QApplication.UnicodeUTF8))
        self.groupSvrList.setTitle(QtGui.QApplication.translate("DlgLog", "服务器", None, QtGui.QApplication.UnicodeUTF8))
        self.btnImportLog.setText(QtGui.QApplication.translate("DlgLog", "导入", None, QtGui.QApplication.UnicodeUTF8))
        self.btnDownLogByFileName.setText(QtGui.QApplication.translate("DlgLog", "开始", None, QtGui.QApplication.UnicodeUTF8))
        self.label_2.setText(QtGui.QApplication.translate("DlgLog", "下载速度", None, QtGui.QApplication.UnicodeUTF8))
        self.label_3.setText(QtGui.QApplication.translate("DlgLog", "KB/s", None, QtGui.QApplication.UnicodeUTF8))
        self.label_4.setText(QtGui.QApplication.translate("DlgLog", "文件名", None, QtGui.QApplication.UnicodeUTF8))
        self.chkFuzzy.setText(QtGui.QApplication.translate("DlgLog", "模糊匹配", None, QtGui.QApplication.UnicodeUTF8))
        self.btnAddTask.setText(QtGui.QApplication.translate("DlgLog", "加任务", None, QtGui.QApplication.UnicodeUTF8))
        self.btnClearTask.setText(QtGui.QApplication.translate("DlgLog", "清任务", None, QtGui.QApplication.UnicodeUTF8))
        self.chkCompressOnFinish.setText(QtGui.QApplication.translate("DlgLog", "下载完压缩", None, QtGui.QApplication.UnicodeUTF8))
        self.btnDumpCmd.setText(QtGui.QApplication.translate("DlgLog", "生成命令行", None, QtGui.QApplication.UnicodeUTF8))
        self.chkGmlog.setText(QtGui.QApplication.translate("DlgLog", "gmlog", None, QtGui.QApplication.UnicodeUTF8))
        self.chkAllGroup.setText(QtGui.QApplication.translate("DlgLog", "AllGroup", None, QtGui.QApplication.UnicodeUTF8))


if __name__ == "__main__":
    import sys
    app = QtGui.QApplication(sys.argv)
    DlgLog = QtGui.QDialog()
    ui = Ui_DlgLog()
    ui.setupUi(DlgLog)
    DlgLog.show()
    sys.exit(app.exec_())

