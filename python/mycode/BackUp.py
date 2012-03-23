#coding=utf-8

#include
import configparser
import shutil
import sys
sys.path.append('../toolkit')
import XswUtility


#----------------------------------------------------------------------
#;backup.ini示例
#[list]
#count=1
#src1=C:\Documents and Settings\Administrator\Application Data\Wing IDE 3\preferences
#bak1=F:\work\xsw\xswrun\tools_cfg\Wing IDE 3\preferences
#[svn]
#workcopy=F:\work\xsw\xswrun\tools_cfg

#----------------------------------------------------------------------
#BackUp
def BackUp(lstBackup):
    for aBackup in lstBackup:
        shutil.copyfile(aBackup.src_file, aBackup.bak_file)
        
    config = configparser.ConfigParser()
    if config.read('backup.ini') :
        strWorkCopy = config.get('svn', 'workcopy')
        XswUtility.SvnCommit(strWorkCopy)
    
#----------------------------------------------------------------------
def Restored():
    config = configparser.ConfigParser()
    if config.read('backup.ini') :
        strWorkCopy = config.get('svn', 'workcopy')
        XswUtility.SvnUpdate(strWorkCopy)

    for aBackup in lstBackup:
        shutil.copyfile(aBackup.bak_file, aBackup.src_file)
        
########################################################################
class BackUpInfo:
    src_file = ''
    bak_file = ''
    def __init__(self, src_file, bak_file):
        self.src_file = src_file
        self.bak_file = bak_file
   
#----------------------------------------------------------------------
def InitList():
    """初始化要备份的文件列表"""
    config = configparser.ConfigParser()
    if not config.read('backup.ini') :
        return None
    
    count = config.getint('list', "count")
    if count <= 0 :
        return None
    
    lstFiles = []
    for i in range(1, count+1):
        src = config.get('list', 'src' + str(i))
        bak = config.get('list', 'bak' + str(i))
        lstFiles.append(BackUpInfo(src, bak))
    return lstFiles
    
#main
if __name__ == "__main__":
    lstFiles = InitList()
    if lstFiles != None:
        cmd = input('b:backup r:restore')
        if cmd == 'b':
            BackUp(lstFiles)
        elif cmd == 'r':
            Restored(lstFiles)

    print('ok')
