#coding=utf-8
import xswPath
import os
import re

class xParseLog:
    def __init__(self):
        self.lstFilePath = []
        self.log_content = ''
        #self.regex = r'^(?P<time>2010-5-6 [\d]*:[\d]*:[\d]*) --(?P<log>.*?)2010'
        self.regex = r'^(?P<time>\d{4}-\d*-\d* \d*:\d*:\d*) -- (?P<log>.*)'
        self.p = re.compile(self.regex)
        self.LogList = {}

    def __del__(self):
        ""
        
	def convert(self, filename, in_enc = "GBK", out_enc="UTF-8" ):   
	    # read the file   
	    content = open( filename ).read()   
	    # convert the concent   
	    try:   
	        new_content = content.decode( in_enc ).encode( out_enc )   
	        #write to file   
	        open( filename, 'w' ).write( new_content )   
	    except:   
	        print ("error... ")  
	  
#	def explore(self, dir ):   
#	    for root, dirs, files in os.walk( dir ):   
#	        for file in files:   
#	            path = os.path.join( root, file )   
#	            convert( path )   

    def ParseLog(self, lstFilePath):
        self.lstFilePath= lstFilePath

        for filePath in lstFilePath:
            #修改文件编码
            self.convert(filePath)
            
            f = open(filePath, 'r')
            if not f:
                break

            for strLine in f.readlines():
                m = self.p.match(strLine )
                if m:
                    strTime = m.group('time')
                    strLog = m.group('log')
                    strLog = strLog + '\r'
                    if len(strTime) > 0:    #新的log块
                        self.__NewLog__(strTime)
                    self.__AddLog__(strTime, strLog)
                else :
                    self.__AddLog__(strTime, strLine)

        #保存log列表到文件中
        oGlobalLog = open('result.log', 'w+')
        for aTime in sorted(self.LogList.keys(), key=None, reverse=False):
            oGlobalLog.write(aTime + '\r')
            for aLog in self.LogList[aTime]:
                oGlobalLog.write(aLog)
        oGlobalLog.close()

    def __NewLog__(self, strTime):
        if not strTime in self.LogList:
            self.LogList[strTime] = []

    def __AddLog__(self, strTime, strLog):
        self.LogList[strTime].append(strLog)



if __name__ == "__main__":
    strPath1 = r"E:\91Download\念小义_790203\syslog"
    param1 = xswPath.xPathWalkerParam()
    param1.excludeDirs = [".SVN"]
    param1.includeFileNames = [".LOG"]

    walker = xswPath.xPathWalker()
    dirs,files  =  walker.filter_walk(strPath1,  param1)

    oParseLog = xParseLog()
    oParseLog.ParseLog(files)
