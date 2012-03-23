#! /usr/bin/env python
#coding=utf-8

import sys
import pysvn
client = pysvn.Client()

################################################
#callback
log_message = "reason for change"
def SetLog(str):
    log_message = str

def get_log_message():
    return True, log_message
client.callback_get_log_message = get_log_message

def ssl_server_trust_prompt( trust_dict ):
    #retcode:boolean,   False if no username and password are available.
    #                   True if subversion is to use the username and password.
    #accepted_failures:int, the accepted failures allowed
    #save:boolean,      True if you want subversion to remember the certificate in the configuration directory.
    #                   False to prevent saving the certificate.
    retcode = True
    accepted_failures = 0
    save = True
    return retcode, accepted_failures, save
client.callback_ssl_server_trust_prompt = ssl_server_trust_prompt

################################################
#get svn entry version
def GetEntryVer(strWorkCopy):
	entry = client.info(strWorkCopy)
	return entry.commit_revision.number

################################################
#create branch
def CreateBranch(strFrom, strTo, nFromRev=-1, strLog='create branch'):
    rev = pysvn.Revision( pysvn.opt_revision_kind.number, nFromRev )
    SetLog(strLog)
    retRev = client.copy(strFrom, strTo, rev)
    nRev = (pysvn.opt_revision_kind.number,  retRev)
    return nRev

################################################
#main
def test():
#    print(CreateBranch(r'https://xswrun.svn.sourceforge.net/svnroot/xswrun/ahk', r'https://xswrun.svn.sourceforge.net/svnroot/xswrun/ahk2', 13))
    print(CreateBranch(r'file:///E:/svn_project/Demo/FormTeam/res', r'file:///E:/svn_project/test1', 13))

def testTransaction():
    t = pysvn.Transaction(r'E:\Project\ai2003', 'a')
    all_props = t.revproplist()
    for name, value in all_props.items():
        print( '%s: %s' % (name, value) )

if __name__ == '__main__':
    client.exception_style = 0
    try:
        testTransaction()
    except pysvn.ClientError as e:
        print('Ocuur Err:', str(e))

