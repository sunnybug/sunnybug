import sys
from PyQt4 import QtGui
sys.path.append('ui')
from PyQt4.QtGui import *

from Ui_dlg import Ui_Dialog

class MyForm(QtGui.QMainWindow):
    def __init__(self, parent=None):
        QtGui.QWidget.__init__(self, parent)
        self.ui = Ui_Dialog()
        self.ui.setupUi(self)
        
    def AddNewResult(self):
        QMessageBox.about(self, "My message box", 'haha')

if __name__ == "__main__":
    app = QtGui.QApplication(sys.argv)
    myapp = MyForm()
    myapp.show()
    sys.exit(app.exec_())

