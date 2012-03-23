#coding=utf-8
import urllib.request
import time

urllib.request.urlopen(r'http://www.pc.nd')
for i in range(310, 310+40):
    url = (r'http://www.pc.nd/index.php?user-profile')
    print('访问:%s' % url)
    urllib.request.urlopen(url)
    time.sleep(10)

;urllib.request.urlopen(r'http://sunnybug.flk123.com')
;for i in range(310, 310+40):
;    url = (r'http://sunnybug.flk123.com/index.php?c=fl&id=%d&a=indexnew&url=&shopurl=' % i)
;    print('访问:%s' % url)
;    urllib.request.urlopen(url)
;    time.sleep(10)


