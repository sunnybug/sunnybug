#coding=utf-8
import sys
import os
import traceback
from cx_Freeze import setup, Executable

executables = [
        Executable("run.pyw")
]

#include path
strBasePath = os.path.dirname(sys.argv[0])
strBasePath = os.path.split(strBasePath)[0]#../
strIncludePath = strBasePath + r';../ui' + r';../../../toolkit' + r';C:\Python31\Lib\site-packages\PyQt4' + r';C:\Python31\Lib'

buildOptions = dict(
        compressed = True,
        includes = ['sip','QtCore','xLogDlg','Ui_DlgLog','cfg','XswUtility','zipimport','traceback'], #modules to include
        path = strIncludePath)  #paths to search

print(buildOptions)
setup(
        name = "advanced_cx_Freeze_sample",
        version = "0.1",
        description = "Advanced sample cx_Freeze script",
        options = dict(build_exe = buildOptions),
        executables = executables)

