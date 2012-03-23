#coding=utf-8

import telnetlib
import time
import datetime
from XswUtility import xTimer

TELNET_TIMEOUT = 3

def DebugPrint(str):
    print( datetime.datetime.now(), str)

class xTelnet:
    def __init__(self, coding='gbk'):
        self.host = ''
        self.coding = coding
        self.tn = telnetlib.Telnet()
        self.print_after_cmd = True

    def __del__(self):
        """Destructor -- close the connection."""
        self.tn.close()

    def PrintBuf(self, timeout=TELNET_TIMEOUT):
        while True:
            time.sleep(timeout)
            str = self.tn.read_eager()
            if str == b'':
                break;
            DebugPrint(str.decode(self.coding))

    def ExcuteCmd(self, cmd):
        DebugPrint('Cmd[%s]' % cmd)
        self.tn.write(cmd.encode(self.coding) + b"\r\n")
        DebugPrint('====cmd finish====')

    def Wait(self, str, timeout=TELNET_TIMEOUT):
        t = xTimer()
        start = datetime.datetime
        self.tn.expect([str])
        DebugPrint('wait str[%s] time[%d]' % (str ,t.GetMS()))

    def Connect(self, host, user, password, timeout=3):
        self.tn.open(host, 23, timeout)
#        self.PrintBuf()
        self.Wait(b'login:', TELNET_TIMEOUT)
        self.ExcuteCmd(user)
        self.Wait(b'password:', TELNET_TIMEOUT)
        self.ExcuteCmd(password)
        return True;

def TestTelnet():
    host = "127.0.0.1"
    user = "administrator"
    password = "nd.91.com"
    tn = xTelnet()
    tn.Connect(host, user, password)
    tn.PrintBuf()
    #tn.ExcuteCmd('taskkill /f /im notepad.exe')
    tn.ExcuteCmd('notepad')
    tn.PrintBuf()
    del tn

if __name__ == '__main__':
   #TestTelnet()
   hosts = [{'host':'hosts1', 'user':'user1'}, {'host':'hosts2', 'user':'user2'}]
   lst2 = {'hosts', 'hosts1', 'hhh'}
   for host in hosts:
    lst2[host['host']]



