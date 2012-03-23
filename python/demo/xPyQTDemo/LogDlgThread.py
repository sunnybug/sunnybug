reLogHeader = re.compile(r'(^\d{4}-\d{2}\-\d{2} \d{2}:\d{2}:\d{2}\.\d{3})\t')
class CRegexMergeLogFile():
    def __init__(self):
        self.strLogHeader = ''
        self.strLog = ''
        self.strNextLogHeader = ''
        self.strNextLog = ''
        
    def GetCurTime(self):
        '''当前Log时间'''
        if self.strLogHeader != '':
            return self.strLogHeader
        self.GetALog() 
        return self.strLogHeader
        
    def GetALog(self):
        strLogHeader = ''
        strLog = ''
        strNextLogHeader = ''
        strNextLog = ''
        while True:
            line = self.f.readline()
            if not line:
                break   #一个文件处理结束
                
            line = line.replace('\r\n',  '\r')
            line = line.replace('\n',  '\r')
            m = reLogHeader.match(line)
            if m:
                if strLogHeader=='':    #一个log的开始
                    strLogHeader = m.group(0)
                    strLog = line
                else:   #到达下一个log了
                    strNextLogHeader = m.group(0)
                    strNextLog = line
                    break
            elif strLogHeader!='':
                strLog += line
            else:
                ErrPrint('不支持的log格式')
                return False
                
        self.strLogHeader = strLogHeader
        self.strLog = strLog
        self.strNextLog = strNextLog
        self.strNextLogHeader = strNextLogHeader
        return strLog!=''
        
    def GetLogs(self, lstLogs,  strHeaderBegin,  strHeaderEnd):
        '''从当前文件位置读取指定区间的log'''
        if (self.strLog>=strHeaderBegin) & (self.strLog<=strHeaderEnd):
            lstLogs.append(self.strLog)
        if (self.strNextLog>=strHeaderBegin) & (self.strNextLog<=strHeaderEnd):
            lstLogs.append(self.strNextLog)
        
        strLogHeader = ''
        strLog = ''
        while True:
            line = self.f.readline()
            if not line:
                if strLog != '': 
                    lstLogs.append(strLog)
                break   #一个文件处理结束
                
            line = line.replace('\r\n',  '\r')
            line = line.replace('\n',  '\r')
            if line =='\r': #过滤空行
                continue
                
            m = reLogHeader.match(line)
            if m:
                strMatchHeader = m.group(0)
                if (strMatchHeader<strHeaderBegin): continue
                if (strMatchHeader>strHeaderEnd):  
                    if strLog != '': 
                        lstLogs.append(strLog)
                    break
                if strLogHeader=='':    #一个log的开始
                    strLogHeader = strMatchHeader
                    strLog = line
                else:   #到达下一个log了
                    lstLogs.append(strLog)
                    strLogHeader = strMatchHeader
                    strLog = line
            elif strLogHeader!='':
                strLog += line

        return lstLogs
