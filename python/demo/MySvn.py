#coding=utf-8
import pysvn
import datetime

client = pysvn.Client()
entry = client.info(r"D:\\demo\\xswrun\\PythonDemo\\")


print(r'SVN路径:',entry.url)
print( r'本地最新版本:',entry.commit_revision.number)
print( r'提交人员:',entry.commit_author)
print( r'更新日期:', datetime.datetime.fromtimestamp(entry.commit_time))
