#coding=utf-8
import sys
from PyQt4 import QtGui
sys.path.append('../ui')
sys.path.append('../../../toolkit')
sys.path.append('../')
from PyQt4.QtGui import *
from xLogDlg import LogDlg

from PyQt4 import QtCore, QtGui
from PyQt4.QtCore import *
from PyQt4.QtGui import *

class DownLogTab(QtGui.QWidget):
    def __init__(self, parent=None):
        super(DownLogTab, self).__init__(parent)

        mainLayout = QtGui.QVBoxLayout()
        mainLayout.addWidget(LogDlg())
        mainLayout.addStretch(1)
        self.setLayout(mainLayout)
        
class TabDialog(QtGui.QDialog):
    def __init__(self, fileName, parent=None):
        super(TabDialog, self).__init__(parent)
        fileInfo = QtCore.QFileInfo(fileName)

        tabWidget = QtGui.QTabWidget()
        tabWidget.addTab(LogDlg(),  "General")

        buttonBox = QtGui.QDialogButtonBox(QtGui.QDialogButtonBox.Ok | QtGui.QDialogButtonBox.Cancel)

        buttonBox.accepted.connect(self.accept)
        buttonBox.rejected.connect(self.reject)

        mainLayout = QtGui.QVBoxLayout()
        mainLayout.addWidget(tabWidget)
        mainLayout.addWidget(buttonBox)
        self.setLayout(mainLayout)

        self.setWindowTitle("Tab Dialog")
        
def RunTabDlg():
    import sys
    app = QtGui.QApplication(sys.argv)
    tabdialog = TabDialog(".")
    sys.exit(tabdialog.exec_())

if __name__ == "__main__":
    #RunTabDlg()
    #sys.exit(app.exec_())
    app = QtGui.QApplication(sys.argv)
    myapp = LogDlg()
    myapp.show()
    sys.exit(app.exec_())
 
    '''import sys
    app = QtGui.QApplication(sys.argv)
    Dialog = QtGui.QDialog()
    ui = Ui_Dialog()
    ui.setupUi(Dialog)
    Dialog.show()
    sys.exit(app.exec_())'''
