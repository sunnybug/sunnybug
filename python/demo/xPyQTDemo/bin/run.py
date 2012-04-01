#coding=utf-8
import sys
from PyQt4 import QtGui
sys.path.append('../ui')
sys.path.append('../../../toolkit')
sys.path.append('../')
from PyQt4.QtGui import *
from xLogDlg import LogDlg


if __name__ == "__main__":
    app = QtGui.QApplication(sys.argv)
    myapp = LogDlg()
    myapp.show()
    sys.exit(app.exec_())
 
