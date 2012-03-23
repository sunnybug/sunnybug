#coding=utf-8
import time
import   os
import  os.path

PATH_SEP = "\\"
os.path.sep = PATH_SEP

def TRACE(str):
    #print(str)
    ""

class xPathWalkerParam:
    def __init__(self):
        self.excludeDirs  =  []
        self.excludeFileNames  =  []
        self.includeFileNames  =  []

    def is_match_parent_dir(self, strDir):
        ss = strDir.split( PATH_SEP )
        for  s  in  ss:
            s = s.upper()
            #如果被过滤，则返回
            if (s in self.excludeDirs):
                TRACE(s + " not match dir")
                return  False

        return True

    def get_file_ext(self, s):
        nPos = s.find(".")
        if nPos == -1:
            return ""
        return s[nPos:]

    def is_match_file(self, s):
        import re
        #todo:用正则表达式

        #后缀名是否被过滤
        strExt = self.get_file_ext(s)
        strExt = strExt.upper()
        if ( strExt  in  self.excludeFileNames):
            TRACE(s + " not match ext[%s]" % strExt)
            return  False

        #文件名是否需要匹配
        if (len(self.includeFileNames) >0 and strExt not in self.includeFileNames):
            TRACE(strExt + " not match include")
            return False;

        return  True


class xPathWalker:
    def __init__(self):
        ""

    def __del__(self):
        ""

    ######################################
    #public function
    def  filter_walk(self, strDir, param):
        dirList = []
        fileList = []

        for  (parent, dirs, files)  in  os.walk(strDir):
            #检查目录名
            for  d  in  dirs:
                if (d.upper()  in  param.excludeDirs):
                    continue
                 # To check if one of the parent dir should be excluded.
                if not (param.is_match_parent_dir(parent)):
                    continue
                dirList.append(parent + PATH_SEP + d)

            #检查文件
            for  f  in  files:
                if not param.is_match_file(f):
                     continue
                # To check if one of the parent dir should be excluded.
                if not (param.is_match_parent_dir(parent)):
                    continue
                fileList.append(parent + PATH_SEP + f)

        return(dirList,fileList)


# test
if __name__ == '__main__':
    strPath1 = r"D:\demo\xswrun\PythonDemo"
    param1 = xPathWalkerParam()
    param1.excludeDirs = [".SVN"]
    param1.excludeFileNames = []
    param1.includeFileNames = [".LOG"]

    walker = xPathWalker()
    dirs,files  =  walker.filter_walk(strPath1,  param1)

    for  d  in  dirs:
        print('xx' + d)

    for  f  in  files:
        print('==' + f)
