# -*- coding: gbk -*-

import sys

from PyQt4 import QtGui
from MainWindow import MainWindow


#main
if __name__ == "__main__":
    app = QtGui.QApplication(sys.argv)
    #app.setStyle('cleanlooks')
    mgr = MainWindow(app)
    mgr.Process()
    sys.exit(app.exec_())
