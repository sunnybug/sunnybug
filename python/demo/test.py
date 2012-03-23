#coding=utf-8
import time

def listDirectory(directory, fileExtList):
    "get list of file info objects for files of particular extensions"
    fileList = [os.path.normcase(f)
                for f in os.listdir(directory)]
    print ( fileList)
    fileList = [os.path.join(directory, f)
               for f in fileList
                if os.path.splitext(f)[1] in fileExtList]
    print ( fileList)


def LOGERR(str):
    print ( str)

def process_line(line_str):
    ''

import os
def process_dir(parent_dir, name):
    ''
def process_file(parent_dir, name):
    full_path = os.path.join(parent_dir, name)
    f = file(full_path)
    if not f:
        LOGERR('文件打开失败:%s' + full_path)

def walk_dir(dir, topdown=True):
    for root, dirs, files in os.walk(dir, topdown):
        for name in files:
            process_file(root, name)
        for name in dirs:
            process_dir(root, name)

#listDirectory(r'd:\demo\python_demo', '.h')

import datetime,time
format="%Y-%m-%d %H:%M:%S"
p = time.strptime("2008-01-31 00:11:23",format)
print(p)
print(datetime.datetime(p))