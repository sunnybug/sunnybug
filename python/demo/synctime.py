#coding=gbk
import re
import os
import string


if __name__=='__main__':
    
    fid = open('result.txt', 'w+')
    
    suc_ip_list = []
    for i in range(132,254):                #遍历ip段
        ip = '192.168.223.' + str(i)
        #ping
        strCmd = r'ping '+ip+' -n 1 -w 10'  #-n:count -w:wait
        if os.system(strCmd) != 0:  
            continue;                       #ping fail
        
        strCmd = r'net time /set \\' + ip +' /y'
        if os.system(strCmd) == 0:          #time success
            print(ip, file=fid)             #save result
            suc_ip_list.append(ip)               #保存到list容器
            if len(suc_ip_list) >= 3:            #需要至少3个ip
                break;
    
    #打印所有ip
    for suc_ip in suc_ip_list:
        strCmd = r'net time /set \\' + suc_ip +' /y'   
        print(os.system(strCmd), file=fid)