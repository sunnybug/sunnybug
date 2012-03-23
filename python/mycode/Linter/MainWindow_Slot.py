# -*- coding: utf-8 -*-

"""
Module implementing MainWindow_Slot.
"""

from PyQt4.QtCore import pyqtSlot
from PyQt4.QtGui import QMainWindow

from MainWindow import MainWindow

class MainWindow_Slot(QMainWindow, MainWindow):
    """
    Class documentation goes here.
    """
    def __init__(self, parent = None):
        """
        Constructor
        """
        QMainWindow.__init__(self, parent)
        self.setupUi(self)

    @pyqtSlot()
    def OnBtnStart(self):
        print("OnBtnStart")

