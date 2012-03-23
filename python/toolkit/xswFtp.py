# -*- encoding: utf-8 -*-
import sys
import imp
import XswUtility
imp.reload(sys)
sys.setdefaultencoding('utf-8')

#=================================================================================================
#ftplib，不支持中文
import ftplib, os, traceback, glob
class FTP:
    def __init__( self, ip, port, user, pwd, path='' ):
        """ 开启FTP并进入指定目录 """
        self.ftp = ftplib.FTP()
        self.ftp.connect( ip, port )
        self.ftp.login( user, pwd )
        if path != '':
            self.path = path
            self.ftp.cwd( path )

    def close( self ):
        """ 关闭FTP """
        self.ftp.quit()

    def _allFiles( self, search_path, pattern, pathsep=os.pathsep ):
        """ 查找指定目录下符合指定模式的所有文件,以列表形式返回
            @param search_path  查询目录列表
            @param pattern      文件名模式
            @param pathsep      目录分隔符
        """
        for path in search_path.split( pathsep ):
            for match in glob.glob( os.path.join(path, pattern) ):
                yield match

    def searchFileFromFTPOld( self, pattern ):
        """ 在FTP的指定目录下查找文件
            @param pattern  文件名模式
        """
        flist = []
        try:
            self.ftp.dir( pattern, flist.append )
        except:
           flist = []
        # 提取文件名，格式如：-rw-r--r--    1 500      502           881 Aug 29 09:16 AllTest.py
        namelist = []
        for l in flist:
            print(l)
            name = l.split( ' ' )[-1]
            namelist.append( name )
        return namelist

    def searchFileFromFTP( self, pattern ):
        """ 在FTP的指定目录下查找文件
            @param pattern  文件名模式
        """
        flist = []
        try:
           flist = self.ftp.nlst(pattern)
        except ftplib.error_perm as e:
            flist = []
            print( "ftp.nlst fail[%s]"  % e)
        return flist

    def upload( self, fromdir, pattern ):
        """ 上传文件
            @param fromdir  本地文件目录
            @param pattern  文件名模式
        """
        if fromdir[-1] != os.sep:
            fromdir += os.sep
        flist = self._allFiles( fromdir, pattern )
        for f in flist:
            try:
                name = f.split( os.sep )[-1]
                print('正在上传:', name)
                self.ftp.storlines( 'stor ' + name, file( fromdir + name ) )
            except:
                print('上传文件[%s]错误!' % name)
                return False
        return True

    def download( self, todir, pattern, isBin=False ):
        """ 下载文件.注意:文件名中带中文的下载不下来
            @param todir        下载到本地的目录
            @param pattern      文件名,可以使用通配符批量下载文件
            @param isBin        是否是二进制文件.是=True;否=False
        """
        # 若目录最后不是'\',则补充之
        if todir[-1] != os.sep:
            todir += os.sep

        flist = self.searchFileFromFTP( pattern )
        if len( flist ) == 0:
            print('未找到文件[%s]!' % pattern)
            return False
            
        fn = [] # 将下载到本地的文件列表(带目录)
        for f in flist:
            fn.append( todir + f )
            f = self.path + f
        flist2 = []
        for f in flist:
                f = self.path + f
                flist2.append(f)
        print(flist2)
        
        # 循环下载每一个文件
        for i in range( len(fn) ):
            try:
                fp = None
                try:
                    print('正在下载文件:', flist2[i])
                    if isBin:   # 二进制文件
                        fp = open( fn[i] , 'wb' )
                        self.ftp.retrbinary( 'retr ' + flist2[i], fp.write )
                    else:       # 非二进制文件
                        fp = open(fn[i] , 'w' )
                        l = []
                        self.ftp.retrlines( 'retr ' + flist2[i], l.append )
                        for k in l:
                            fp.write( k + b'\r\n'.decode("gbk", "replace"))
                        del l
                finally:
                    if fp:  fp.close()
            except ftplib.error_perm as e:
                print('下载文件[%s]时出错!' % flist2[i],  )
                print(e)
                return False
        return True

    def deleteFileFromFTP( self, pattern ):
        """ 删除文件,注意:文件名不能为中文
            @param pattern  要删除的文件名,可以使用通配符批量删除文件
        """
        flist = self.searchFileFromFTP( pattern )
        if len( flist ) == 0:
            print('未找到文件[%s]!' % pattern)
            return False
        for f in flist:
            try:
                print('正在删除文件:', f)
                self.ftp.delete( f )
            except:
                print('删除文件[%s]时出错!' % f)
                return False
        return True

    def renameFileFromFTP( self, oldname, newname ):
        """ 重命名FTP上的某个文件名
            @param oldname  原文件名
            @param newname  欲改成的文件名
        """
        try:
            self.ftp.sendcmd( 'dele %s' % newname )
        except:
            pass

        try:
            print('正在将文件[%s]改名为[%s]' % ( oldname, newname ))
            self.ftp.sendcmd( 'RNFR ' + oldname )
            self.ftp.sendcmd( 'RNTO %s' % newname )
            return True
        except:
            print('将文件[%s]改名为[%s]时出错!' % ( oldname, newname ))
            return False

def TestFtpClass():
    ip = '121.207.234.87'
    port = 21
    user = 'up'
    pwd = 'upup'
    path = '/GameServer_syslog/'
    myftp = FTP( ip, port, user, pwd, path )
#    if myftp.upload( 'd:\uniplat\log', 'flow_0.log.20080619' ):
#        print('flow_0.log.20080619上传完毕')
#    else:
#        print('flow_0.log.20080619上传失败')
#        print('flow_0.log.20080619上传失败')

    if myftp.download( r'e:\\', r'Statistic 2011-6-22.log 01_09_033.log' ):
        print('下载完毕')
    else:
        print('下载失败')

#    if myftp.deleteFileFromFTP( '*' ):
#        print('删除完毕')
#    else:
#        print('删除失败')

#    if myftp.renameFileFromFTP( 'ftp.py', 'myftp.py' ):
#        print('改名完毕')
#    else:
#        print('改名失败')

    myftp.close()

 
#=================================================================================================
FTPSVR_FIELD_SVR = 0
FTPSVR_FIELD_IP = 1
FTPSVR_FIELD_PATH = 2
lstFtpSvr = [
    ['ls', '121.207.234.86', 'Login/syslog'],
    ['hs', '121.207.234.86', 'HServer/syslog'],
    ['db', '121.207.234.87', 'DBServer/syslog'],
    ['gs1', '121.207.234.85', 'GameServer/syslog'],
    ['gs2', '121.207.234.87', 'GameServer/syslog'],
    ['gs3', '121.207.234.89', 'GameServer/syslog'],
    ['gs4', '121.207.234.89', 'GameServer1/syslog'],
    ['gs5', '121.207.234.90', 'GameServer/syslog'],
    ['gs6', '121.207.234.90', 'GameServer1/syslog']
]

def GetDayLogPath(strDay, svr=''):
    if(svr == ''):
        return r'd:\log\%s\\'% (strDay)
    return r'd:\log\%s\%s\\'% (strDay, svr)
        
def GetDayLog(strDay, strMatch=''):
    for svr in lstFtpSvr:
        path = GetDayLogPath(strDay, svr[FTPSVR_FIELD_SVR])
        XswUtility.MKDirEx('', path)
        if(strMatch==''):
            XswUtility.FtpDown(('%s/%s/*%s*.log' % (svr[FTPSVR_FIELD_IP], svr[FTPSVR_FIELD_PATH], strDay)), 'syslog_log', r'XfjaHxZ3AMN>~rm', path)
        else:
            XswUtility.FtpDown(('%s/%s/*%s*%s*.log' % (svr[FTPSVR_FIELD_IP], svr[FTPSVR_FIELD_PATH], strMatch, strDay)), 'syslog_log', r'XfjaHxZ3AMN>~rm', path)
            
#=================================================================================================
#读取配置文件，看上次

#如果存在关键字，则告警
def ParseString(str):
    ''

#=================================================================================================
if __name__ == '__main__':
    for svr in lstFtpSvr:
        print(svr[FTPSVR_FIELD_SVR])
    GetDayLog('7-25')
