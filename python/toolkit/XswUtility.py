#coding=utf-8
import os

def ErrorExit(msg):
    print(msg)
    
################################################
#计时器类
import datetime
class xTimer:
    def __init__(self):
        self.start = datetime.datetime.now()

    def GetMS(self):
        d = datetime.datetime.now() - self.start
        ms = d.days * 24 * 60 * 60 * 1000 + d.seconds * 1000 + d.microseconds / 1000
        return ms

################################################
#执行并输出命令行
def OutputCmd_Old(strCmd):
    import os
    print(strCmd)

    #用popen来执行命令行
    oStdout = os.popen(strCmd)
    #获取输出
    strStdout = oStdout.read()
    print(strStdout)    #xsw:但却是空的输出?

    #获取返回值
    ret = oStdout.close()

    if ret != None:
        print('程序运行失败,cmd=[%s] ret=[%d]'%(strCmd,ret))
    return ret

################################################
#执行并输出命令行
import subprocess

use_shell = True

def RunShellWithReturnCode(command, print_output=True,
                           universal_newlines=True):
  """Executes a command and returns the output from stdout and the return code.
  Args:
    command: Command to execute.
    print_output: If True, the output is printed to stdout.
                  If False, both stdout and stderr are ignored.
    universal_newlines: Use universal_newlines flag (default: True).
  Returns:
    Tuple (output, return code)
  """
  p = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                       shell=use_shell, universal_newlines=universal_newlines)
  if print_output:
    output_array = []
    while True:
      line = p.stdout.readline()
      if not line:
        break
      #print(line.strip("\n"),)
 #     line = line.strip("\n")
      print(line, end='')
      output_array.append(line)
    output = "".join(output_array)
  else:
    output = p.stdout.read()
  p.wait()
  errout = p.stderr.read()
  #if print_output and errout:
  #  print >>sys.stderr, errout
  p.stdout.close()
  p.stderr.close()
  return output, p.returncode

def OutputCmd(command, silent_ok=False, universal_newlines=True,
             print_output=True):
    print(command)
    data, retcode = RunShellWithReturnCode(command, print_output,
                                         universal_newlines)
#    if retcode:
#        ErrorExit("Got error status from %s:\n%s" % (command, data))
#	if not silent_ok and not data:
#		ErrorExit("No output from %s" % command)
    return data,  retcode

################################################
#创建目录
def MKDir(strPath):
    import os
    if os.path.exists(strPath):
        return True
    if not os.mkdir(strPath):
        return False
    return True

################################################
#遍历文件数组，统计目录名
def GetPaths(files):
    import os
    dirs = {}
    file_name = ''
    for file_name in files:
        nSep = file_name.rfind(os.path.sep)
        if nSep != -1:
            file_dir = file_name[0:nSep+1]
            if dirs.get(file_dir) == None:
                dirs[file_dir] = 1;
    return dirs

#父目录 c:\dd\x.bat --> dd
def GetParentPath(strPath):  
    if not strPath:  
        return None;  
      
    lsPath = os.path.split(strPath)
    strParent = ''
    if lsPath[1]:
        strParent = lsPath[0]
    else: 
        lsPath = os.path.split(lsPath[0]);
        strParent = lsPath[0]

    return strParent[strParent.rfind('\\')+1:]

################################################
#创建目录，dirs为相对路径，支持逐级创建目录
def MKDirEx(dirs, base_dir=''):
    import os
    if(base_dir == ''):
        import sys
        base_dir = os.path.dirname(sys.argv[0]) 
    
    if len(dirs) > 0:
        for dir_name in dirs:
            nLastPathIndex = 0
            nPathIndex = 0
            cur_base_dir = base_dir

            while True:
                nPathIndex = dir_name[nLastPathIndex:].find(os.path.sep)
                if (nPathIndex == -1):
                    break;

                nPathIndex = nPathIndex + nLastPathIndex + 1
                create_dir = dir_name[nLastPathIndex:nPathIndex]

                nLastPathIndex = nPathIndex

                if (create_dir.find(':') != -1):
                    break;
                cur_base_dir = cur_base_dir + '\\' + create_dir
                MKDir(cur_base_dir)
    else:
        MKDir(base_dir)
            

################################################
#如果是目录，则末尾加\\
def AppendPathTag(str):
    if(str == ''):
        return str
    if(str[-1] != '/' or str[-1]!='\\'):
                str += '\\'
    return str
    
################################################
#
def CheckPythonCode(strFile):
    import py_compile
    return py_compile.compile(strFile)

################################################
#用Fastcopy工具进行快速copy
def FastCopy(strFromPath, strTargetPath, strInclude='', strExclude='', strOther=''):
    #命令行参数
    strParamOpt = r' /auto_close /error_stop /cmd=diff /log=FALSE'
    strParam = strParamOpt + ' /exclude=' + strExclude + ' /include=' + strInclude\
    + ' ' + strFromPath + ' /to=' + strTargetPath + ' '+strOther

    #exe路径
    strExe = 'FastCopy.exe'

    #excute
    return OutputCmd(strExe+strParam)

################################################
#BeyondCompare
def BCompare(strFromPath, strTargetPath, strInclude='', strExclude='', strOther=''):
    #命令行参数
    strParamOpt = r' /auto_close /error_stop /cmd=diff /log=FALSE'
    strParam = strParamOpt + ' /exclude=' + strExclude + ' /include=' + strInclude\
    + ' ' + strFromPath + ' /to=' + strTargetPath + ' '+strOther

    #exe路径
    strExe = 'BCompare.exe'

    #excute
    return OutputCmd(strExe+strParam)

################################################
#命令行访问共享
def AuthNetShare(strNetPath, strUser, strPW):
    strCmd = 'net use ' + strNetPath + ' /user:' + strUser + ' ' + strPW
    return OutputCmd(strCmd)

################################################
#联合编译
def IncrediBuild(strPrj, strAct='build', strCfg='Win32 Debug', strOther=''):
    #命令行参数
    strParamOpt =''
    strParam = strParamOpt + ' /'+strAct+' /cfg=\"'+strCfg+'\" '+strOther

    #exe路径
    strExe = 'BuildConsole.exe'

    #excute
    return OutputCmd(strExe+' '+strPrj+' '+strParam)

################################################
#svn update
def SvnUpdate(strWorkCopy, nVer=0):
    #命令行参数
    #/closeonend:2 auto close if no errors and conflicts
    strParam = ' /command:update /path:"%s" /closeonend:2 '%strWorkCopy
    if nVer!=0:
        strParam = strParam + '/rev:%d'%nVer

    #exe路径
    strExe = 'TortoiseProc.exe'

    #excute
    return OutputCmd(strExe+strParam)

################################################
#svn commit
def SvnCommit(strWorkCopy):
    #命令行参数
    #/closeonend:2 auto close if no errors and conflicts
    strParam = ' /command:commit /path:"%s" /closeonend:2 '%strWorkCopy

    #exe路径
    strExe = 'TortoiseProc.exe'

    #excute
    return OutputCmd(strExe+strParam)

################################################
#svn commit
def SvnMerge(strFrom, strTo, strRange=''):
    #命令行参数
    #/fromurl:URL, /revrange:string, /tourl:URL,
    strParam = ' /command:merge /fromurl:"%s" /path:"%s" /revrange:%s /closeonend:2' % (strFrom, strTo, strRange)

    #exe路径
    strExe = 'TortoiseProc.exe'

    #excute
    return OutputCmd(strExe+strParam)

################################################
#svn commit
def SvnCopy(strPath, strUrl, strLog=''):
    #命令行参数
    #/path:the working copy to branch/tag from, /url:the target URL /logmsg:log
    strParam = ' /command:copy /path:"%s" /url:"%s" /logmsg:"%s"' % (strPath, strUrl, strLog)

    #exe路径
    strExe = 'TortoiseProc.exe'

    #excute
    return OutputCmd(strExe+strParam)
    
################################################
#7z compress
def Compress(strSrcPath, strDstFile):
    #命令行参数
    #7z.exe a -t7z %s.7z "I:\t\t1\*" -mx=9 -ms=200m -mf -mhc -mhcf -m0=LZMA -r
    strParam = ' a -t7z %s "%s" -mx=9 -ms=200m -mf -mhc -mhcf -m0=LZMA -r' % (strDstFile,  strSrcPath)

    #exe路径
    strExe = '7z.exe'

    #excute
    return OutputCmd(strExe+strParam)

#=================================================================================================
#wget封装,支持断点续传
def FtpDown(strUrl, strUser='', strPw='', strLocalPath='', bSync=False):
    WGET_CMD = r'wget.exe'
    #X:\tool\net\wget\wget.exe -c ftp://up:upup@121.207.234.87/GameServer_syslog/Stat*
    #-b:background download
    #--limit_rate:下载速度限制
    #-P:目标目录
    strCMD = '%s -c --limit-rate=500k "ftp://%s:%s@%s" -P "%s"' % (WGET_CMD, strUser, strPw, strUrl, strLocalPath)
#    OutputCmd_Old(strCMD)
    if bSync:
        os.system("start " & strCMD)
    else:
        os.system(strCMD)

#=================================================================================================
#文件编码转换
def File_GB2312ToUTF8(strFilePath):
    strCMD = 'GB2Ue.vbs "%s"' % strFilePath
    print(strCMD)
    os.system(strCMD)
    
def Dir_GB2312ToUTF8(strDir):
    for root, dirs, files in os.walk(strDir, True):
        for name in files:
            full_name = os.path.join(root, name)
            if os.path.isfile(full_name):
                File_GB2312ToUTF8(full_name)

#=================================================================================================
#文件编码转换
import codecs
"这个模块用于读取widons下的文本文件,windows下的文本文件会存为\
(ANSI, utf-8, unicode, unicode big endian)4种编码格式\
这个模块就用于读取这4种格式的文本"

def readUTF8(f_url):
    try:
        f = codecs.open(f_url)
    except IOError as err:
        print("In function readUTF8, ",err)
        return ""
    txt = f.read()
    f.close()
    if txt[:3]==codecs.BOM_UTF8:
        txt = txt[3:].decode("utf-8")
        return txt
    return ""

def readANSI(f_url):
    try:
        f = open(f_url,"r")
    except IOError as err:
        print("In function readANSI, ",err)
        return ""
    txt = f.read()
    return txt
def readUNICODE(f_url):
    try:
        f = codecs.open(f_url)
    except IOError as err:
        print("In function readUNICODE, ",err)
        return ""
    txt = f.read()
    f.close()
    try:
        txt = txt.decode("utf-16")
        return txt
    except UnicodeDecodeError as err:
        print("In function readUNICODE, ",err)
        return ""
def readUBE(f_url):    
    return readUNICODE(f_url)

# 自动选择读取这四种编码格式的
def readTxt(f_url):
    try:
        f = open(f_url,"rb")
    except IOError as err:
        print("In function readTxt, ",err)
        return ""
    bytes = f.read()
    f.close()
    try:
        txt = bytes.decode("utf-8")
        return txt
    except UnicodeDecodeError as err:
        # 先当作 ansi 编码解码
        try:
            txt = bytes.decode("GB2312")
            #print("当作 ansi 编码解码")
            return txt
        except UnicodeDecodeError as err:
            # 再当作unicode 编码解码
            try:
                txt = bytes.decode("utf-16")
               # print("当作 unicode 编码解码")
                return txt
            except UnicodeDecodeError as err:
                print("In function readTxt, ",err)
                return ""
                
def Dir2_GB2312ToUTF8(strDir):
    for root, dirs, files in os.walk(strDir, True):
        for name in files:
            full_name = os.path.join(root, name)
            if os.path.isfile(full_name):
                readTxt(full_name)
                
def File3_GB2312ToUTF8(strFilePath):
    strOutput = '%s.bak' % strFilePath
    strCMD = 'iconv.exe -f GB2312 -t UTF-8 "%s" > "%s"' % (strFilePath,  strOutput)
    data,  ret = OutputCmd(strCMD)
    return
    if ret == 0:
        os.remove(strFilePath)
        os.rename(strOutput,  strFilePath)
    else:
        os.remove(strOutput)
    
def Dir3_GB2312ToUTF8(strDir):
    for root, dirs, files in os.walk(strDir, True):
        for name in files:
            full_name = os.path.join(root, name)
            if os.path.isfile(full_name):
                File3_GB2312ToUTF8(full_name)
   
################################################
#main
if __name__ == '__main__':
    #print('ret:' ,OutputCmd('ipconfig'))
    #OutputCmd('ipconfig')
    #IncrediBuild(r'D:\fdemo\ChildWindow\ChildWindow.dsw')
    #SvnCopy(r'https://xswrun.svn.sourceforge.net/svnroot/xswrun/autoit', r'https://xswrun.svn.sourceforge.net/svnroot/xswrun/autoit3')
    #if not MKDir('syslog'): print('fail')
    #CheckPythonCode(r'D:\\demo\\xswrun\\PythonDemo\\OSDemo.py')
    #Dir3_GB2312ToUTF8(r'D:\demo\xswrun\python\demo\xPyQTDemo\bin\9-14\t')
    GetParentPath(r'D:\doc\gitpub\github_sunnybug\python\toolkit\XswUtility.py')
    GetParentPath(r'D:\doc\gitpub\github_sunnybug\python\toolkit\\')
    ''
