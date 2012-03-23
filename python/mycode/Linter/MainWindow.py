# -*- coding: utf-8 -*-

import sys

from MainWindow_Slot import MainWindow_Slot
from PyQt4 import QtCore, QtGui

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    _fromUtf8 = lambda s: s
    
class MainWindow(MainWindow_Slot):
    def __init__(self,  app):
        oMainWindow             = QtGui.QMainWindow()
        self.main_window        = MainWindow_Slot()
        self.main_window.setupUi(oMainWindow)
        self.app = app
        
        self.main_window.btnStart.clicked.connect(self.main_window.OnBtnStart)
    
    def Process(self):
        self.main_window.show()
        
if __name__ == "__main__":
    app = QtGui.QApplication(sys.argv)
    mgr = MainWindow(app)
    mgr.Process()
    sys.exit(app.exec_())
