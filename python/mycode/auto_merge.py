#coding=utf-8

import sys
sys.path.append('../toolkit')
import XswUtility


if __name__ == "__main__":
    strTo = r'E:\Project\复件 ai2008'
    strFrom   = r'file:///E:/svn_project/ai'

    XswUtility.SvnUpdate(strTo)

    #nVer = XswUtility.GetEntryVer(strFrom)
    XswUtility.SvnMerge(strFrom, strTo)

    #print('merge finish. From[%s] To[%s] ver[%d]' % (strFrom, strTo, nVer) )