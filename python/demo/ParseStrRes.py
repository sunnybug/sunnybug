#coding=gbk
import re
import MySQLdb as mysql
import ConfigParser
import os

#常量
FLAG_SPERARATOR = '|=|'
FILE_RESULT = 'result.txt'
PATH_BUILD = r'tmp/'

#####cfg.ini
#[db]
#name=keeper
#ip=192.168.9.139
#user=root
#pw=ch.91.com
#
#[res]
#new_h = StrID.h
#new_regex = ^\s*(?P<strid>STR_.*?)\s*=\s*_STRRES_BASE_.*?\+.*?(?P<id>\d*)
#old_cpp = Cn_DDRes.cpp
#old_regex = ^\s*DEF_RES_STR\s*\((?P<strid>.*?)\s*,\s*\"(?P<strval>.*?)".*?\).*?


def LOGINFO(str):
    f2 = file(PATH_BUILD+'log.log', 'w+')
    f2.writelines(str)

class CConfig(object):
    def __init__(self):
        #section of ini
        CFG_SECTION_DB = 'db'
        CFG_SECTION_RES = 'res'
        #config api
        config = ConfigParser.ConfigParser()
        config.readfp(open('cfg.ini'))
        #load cfg
        if config :
            self.db_name = config.get(CFG_SECTION_DB, "name")
            self.db_ip = config.get(CFG_SECTION_DB, "ip")
            self.db_user = config.get(CFG_SECTION_DB, "user")
            self.db_pw = config.get(CFG_SECTION_DB, "pw")
            self.res_new_h = config.get(CFG_SECTION_RES, "new_h")
            self.res_new_regex = config.get(CFG_SECTION_RES, "new_regex")
            self.res_old_cpp = config.get(CFG_SECTION_RES, "old_cpp")
            self.res_old_regex = config.get(CFG_SECTION_RES, "old_regex")

class CStrRes():
    def __init__(self):
        self.id = ''
        self.strid = ''
        self.strval = ''
        self.used = 0

class CBaseRes :
    def __init__(self):
        self.ary = []

    def size(self):
        return len(self.ary)

    def get_used(self):
        count = 0
        for i in self.ary:
             if(i.used == 1):
                 count = count + 1
        return count

    def dump_unused(self, file_name):
        if self.size() == self.get_used():
            return 1

        f = file(file_name, 'w+')
        if not f:
            print ( 'open file fail. file=', file_name)
            return 0

        count = 0
        for i in self.ary:
            if(i.used == 0):
                count = count + 1
                print >>f, '%s=%s' % (i.strid, i.strval)
        return 1

class CDbRes(CBaseRes):
    def __init__(self):
        self.ary = []

    def add(self, obj):
        for i in self.ary:
            if (i.strid == obj.strid):
                return 0
        self.ary.append(obj)

class CNewRes(CBaseRes):
    def __init__(self):
        self.ary = []

    def add(self, obj):
        for i in self.ary:
            if (i.strid == obj.strid):
                return 0
        self.ary.append(obj)

class COldRes(CBaseRes):
    def __init__(self):
        self.ary = []

    def add(self, obj):
        for i in self.ary:
            if (i.strid == obj.strid):
                return 0
        self.ary.append(obj)

    def update_str(self, db_rec):
        for i in self.ary:
            if (i.strid == db_rec.strid) and (i.strval != db_rec.strval):
                old_str = i.strval
                i.strval = db_rec.strval
                db_rec.used = 1
                LOGINFO('update_str %s [[%s]]->[[%s]]' % (i.strid, old_str, i.strval))
        return 1

    def update_id(self, new_rec):
        for i in self.ary:
            if (i.strid == new_rec.strid):
                i.id = new_rec.id
                new_rec.used = 1
        return 1

#新的字符串id头文件解析
def LoadNewStrRes(new_res):
    p = re.compile(g_cfg.res_new_regex)

    f = file(g_cfg.res_new_h, 'r')
    f2 = file(PATH_BUILD+g_cfg.res_new_h+'.bak', 'w+')
    if not f2 :
        print ( 'fail')
    for f_line in f.readlines():
        m = p.match(f_line)
        if m:
            s = CStrRes()
            s.strid = m.group('strid')
            s.id = int(m.group('id')) + 1000000
            new_res.add(s)
            print >>f2, s.strid, FLAG_SPERARATOR, s.id
    print ( 'new res count=', new_res.size())
    return 1

def LoadOldStrRes(old_res):
    p = re.compile(g_cfg.res_old_regex)

    f = file(g_cfg.res_old_cpp, 'r')
    f2 = file(PATH_BUILD+g_cfg.res_old_cpp+'.bak', 'w+')
    if not f2 :
        print ( 'fail')
    for f_line in f.readlines():
        m = p.match(f_line)
        if m:
            s = CStrRes()
            s.strid = m.group('strid')
            s.strval = m.group('strval')
            old_res.add(s)
            print >> f2, s.strid, FLAG_SPERARATOR, s.strval
    print ( 'old res count=', old_res.size())
    return 1

def LoadDB(db_res):
    f2 = file(PATH_BUILD+'db.bak', 'w+')
    if not f2 :
        print ( 'fail')

    conn = mysql.connect(g_cfg.db_ip,
    g_cfg.db_user,
    g_cfg.db_pw,
    g_cfg.db_name,
    #use_unicode=True,
    #charset='utf8'
    )
    cur = conn.cursor()
    cur.execute('select * from cq_strres')
    #设置游标的位置，不设置默认为0
    #cur.scroll(0)
    numrows = cur.rowcount
    for x in range(0, numrows):
        row=cur.fetchone()
        #查询游标位置的一条记录，返回值为元组
        s = CStrRes()
        s.id = row[0]
        s.strid = row[1]
        s.strval = row[2]
        db_res.add(s)
        print >>f2, s.strid, FLAG_SPERARATOR, s.strval
    print ( 'load db record size=', db_res.size())
    return 1

#更新strval
def UpdateStr(db_res, old_res):
    for db_str in db_res.ary:
        old_res.update_str(db_str)

    return 1

#更新id
def UpdateID(new_res, old_res):
    for rec in new_res.ary:
        old_res.update_id(rec)
    return 1

def SaveFile(res):
    f = file(FILE_RESULT, 'w+')
    if not f:
        return 0

    for rec in res.ary:
        print >>f, '%d=%s' % (rec.id, rec.strval)
    return 1

g_cfg = CConfig()
g_db_res = CDbRes()
g_new_res = CNewRes()
g_old_res = COldRes()

if __name__=='__main__':
    if LoadDB(g_db_res) != 1:
        print ( 'LoadDB() fail')
    if LoadNewStrRes(g_new_res) != 1:
        print ( 'LoadNewStrRes() fail')
    if LoadOldStrRes(g_old_res) != 1:
        print ( 'LoadOldStrRes() fail')

    if UpdateStr(g_db_res, g_old_res) != 1:
        print ( 'UpdateStr() fail')
    if UpdateID(g_new_res, g_old_res) != 1:
        print ( 'UpdateID() fail')

    if SaveFile( g_old_res) != 1:
        print ( 'SaveFile() fail')

    g_db_res.dump_unused(PATH_BUILD+'db_unused.txt')
    g_new_res.dump_unused(PATH_BUILD+'id_unused.txt')

    print ( '生成文件为:', FILE_RESULT)
    os.system("pause")
