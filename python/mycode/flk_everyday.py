#coding=utf-8
import urllib.request
import time
import re
import os
import sys
import urllib.request, urllib.parse, urllib.error
import urllib.request, urllib.error, urllib.parse
import datetime
import http.cookiejar
import datetime
from urllib.request import URLError,HTTPError

# 返利客
# urllib.request.urlopen(r'http://www.pc.nd')
# for i in range(310, 310+40):
#     url = (r'http://www.pc.nd/index.php?user-profile')
#     print('访问:%s' % url)
#     urllib.request.urlopen(url)
#     time.sleep(10)



#urllib.request.urlopen(r'http://jf.etao.com/getCredit.htm')
#urllib.request.urlopen(r'http://jf.etao.com/?signIn=https://hi.alipay.com/campaign/normal_campaign.htm?campInfo=f8TFC%2B0iCwshcQr4%2BKQCH7zMoy1VtWKh&from=jfb&sign_from=3000')


def check(response):
    """Check whether checkin is successful

Args:
response: the urlopen result of checkin

Returns:
If succeed, return a string like '已经连续签到**天'
** is the amount of continous checkin days
If not, return False
"""
    pattern = re.compile(r'<div class="idh">(已连续签到\d+天)</div>')
    result = pattern.search(response)
    if result:
    	return result.group(1)
    return False

def main():
    """Main process of auto checkin
"""
    # Get log file
    LOG_DIR = os.path.join(os.path.expanduser("~"), '.log')
    if not os.path.isdir(LOG_DIR):
        os.makedirs(LOG_DIR)
    LOG_PATH = os.path.join(LOG_DIR, 'xiami_auto_checkin.log')
    f = LOG_FILE = file(LOG_PATH, 'a')
    print(file=f) # add a blank space to seperate log

    # Get email and password
    if len(sys.argv) != 3:
        print('[Error] Please input email & password as sys.argv!', file=f)
        print(datetime.datetime.now(), file=f)
        return
    email = sys.argv[1]
    password = sys.argv[2]

    # Init
    opener = urllib.request.build_opener(urllib.request.HTTPCookieProcessor(http.cookiejar.CookieJar()))
    urllib.request.install_opener(opener)

    # Login
    login_url = 'http://www.xiami.com/web/login'
    login_data = urllib.parse.urlencode({'email':email, 'password':password, 'LoginButton':'登陆',})
    login_headers = {'Referer':'http://www.xiami.com/web/login', 'User-Agent':'Opera/9.60',}
    login_request = urllib.request.Request(login_url, login_data, login_headers)
    login_response = urllib.request.urlopen(login_request).read()

    # Checkin
    checkin_pattern = re.compile(r'<a class="check_in" href="(.*?)">')
    checkin_result = checkin_pattern.search(login_response)
    if not checkin_result:
        # Checkin Already | Login Failed
        result = check(login_response)
        if result:
            print('[Succeed] Checkin Already!', email, result, file=f)
        else:
            print('[Error] Login Failed!', email, file=f)
        print(datetime.datetime.now(), file=f)
        return
    checkin_url = 'http://www.xiami.com' + checkin_result.group(1)
    checkin_headers = {'Referer':'http://www.xiami.com/web', 'User-Agent':'Opera/9.60',}
    checkin_request = urllib.request.Request(checkin_url, None, checkin_headers)
    checkin_response = urllib.request.urlopen(checkin_request).read()

    # Result
    result = check(checkin_response)
    if result:
        print('[Succeed] Checkin Succeed!', email, result, file=f)
    else:
        print('[Error] Checkin Failed!', file=f)
    print(datetime.datetime.now(), file=f)

class LoginRenren():          
    def __init__(self):  
        """log in www.renren.com"""  
        self.log_url = "http://www.renren.com/PLogin.do"  
        self.cj = http.cookiejar.CookieJar()  
        self.opener = urllib.request.build_opener(urllib.request.HTTPCookieProcessor(self.cj))  
        urllib.request.install_opener(self.opener) #Installing an opener is only necessary if you want urlopen to use that opener        
  
    def login(self, email, password):  
        self.email = email  
        self.password = password  
        params = {"email":self.email, "password":self.password}#, "origURL":self.origURL, "domain":self.domain}  
        params = urllib.parse.urlencode(params)  
        params = params.encode('utf-8')   
        #response = self.opener.open(self.log_url,params)  
        response = urllib.request.urlopen(self.log_url, params)  
        file = open("d:\\renren.html", "wb")  
        file.write(response.read())  
        
def JifenBao():
    # Init
    opener = urllib.request.build_opener(urllib.request.HTTPCookieProcessor(http.cookiejar.CookieJar()))
    urllib.request.install_opener(opener)

#--------------------------------------------------------------------
def taobao(logfile, username, password):
    logfile.write(str(datetime.datetime.now()) +' taobao\r\n')
    cj = http.cookiejar.CookieJar()
   
    post_data = urllib.parse.urlencode({'TPL_username':'liuyanhong81',
        'TPL_password':'duoduo20110314',
        'TPL_checkcode':'',
        'need_check_code':'',
        'action':'Authenticator',
        'event_submit_do_login':'anything',
        'TPL_redirect_url':'',
        'from':'tb',
        'fc':'default',
        'style':'default',
        'css_style':'',
        'tid':'',
        'support':'000001',
        'CtrlVersion':'1,0,0,7',
        'loginType':'3',
        'minititle':'',
        'minipara':'',
        'umto':'',
        'pstrong':'2',
        'llnick':'',
        'sign':'',
        'need_sign':'',
        'isIgnore':'',
        'full_redirect':'',
        'popid':'',
        'callback':'',
        'guf':'',
        'not_duplite_str':'',
        'need_user_id':'',
        'poy':'',
        'gvfdcname':'10',
        'gvfdcre':'',
        'from_encoding':''
         } )
    binary_data = post_data.encode('utf8')
    path = 'https://login.taobao.com/member/login.jhtml'
    opener = urllib.request.build_opener(urllib.request.HTTPCookieProcessor(cj))
 
    opener.addheaders = [('User-agent', 'Mozilla/5.0 (Windows; U; Windows NT 6.1; zh-CN; rv:1.9.2.13) Gecko/20101203 Firefox/3.6.13')]
   
    urllib.request.install_opener(opener)
    req = urllib.request.Request(path, binary_data)
    req.addheader = ('Host', 'login.taobao.com')
    req.addheader = ('Referer', 'https://login.taobao.com/member/login.jhtml')
    req.addheader = ('Content-Type', 'application/x-www-form-urlencoded')
    #try login
    try:
        conn = urllib.request.urlopen(req)
    except URLError as e:
        logfile.write('URLError:' +e.code + '\r\n')
        return False
    except HTTPError as e:   
        logfile.write('HTTP Error:'+e.reason + '\r\n')
        return False
    logfile.buffer.write(conn.read())

#测试并发
import concurrent.futures
import urllib.request

def load_url(url, file_path):
    urllib.request.urlretrieve(url, file_path)

def hxrc():
    URLS = []
    for i in range(1, 100000):
        URLS.append(["http://app.hxrc.com/hxrcservices/qy/rsdl/ry_DispDetails.aspx?rsid=%d"%i, 'hxrc/%d.html'%i])
    with concurrent.futures.ThreadPoolExecutor(max_workers=5) as executor:
        future_to_url = dict((executor.submit(load_url, url[0], url[1]), url)
                             for url in URLS)

        for future in concurrent.futures.as_completed(future_to_url):
            url = future_to_url[future]
            if future.exception() is not None:
                print('%r generated an exception: %s' % (url, future.exception()))
            else:
                print('%r succ' % (url))

if __name__=='__main__':
    # logfile = open('xtaobao.html','a')
    # taobao(logfile, 'liuyanhong81', '')
    # #金币
    # logfile = open('xjinbi.html','a')
    # ret = urllib.request.urlopen('http://taojinbi.taobao.com/home/award_exchange_home.htm?auto_take=true&tracelog=newmytb_kelingjinbi')
    # logfile.buffer.write(ret.read())

    #下载网页并保存
#    import urllib.request
#    urllib.request.urlretrieve("http://app.hxrc.com/hxrcservices/qy/rsdl/ry_DispDetails.aspx?rsid=1", '1.html')   

    hxrc()
    # print(urllib.request.urlopen(r'http://jf.etao.com/getCredit.htm').read())
    # print("=================================================")
    # print(urllib.request.urlopen(r'http://jf.etao.com/?signIn=https://hi.alipay.com/campaign/normal_campaign.htm?campInfo=f8TFC%2B0iCwshcQr4%2BKQCH7zMoy1VtWKh&from=jfb&sign_from=3000').read())
	# JifenBao()
