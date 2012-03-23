#coding=utf-8
import urllib.request
import time

for i in range(270, 270+40):
    url = (r'http://sunnybug.flk123.com/index.php?c=fl&id=%d&a=indexnew&url=&shopurl=' % i)
    print('访问:%s' % url)
    urllib.request.urlopen(url)
    time.sleep(10)

