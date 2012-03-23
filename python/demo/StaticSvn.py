#coding=utf-8

import sys
sys.path.append('../toolkit')
import XswUtility


if __name__ == "__main__":
	strRev = r'https://xswrun.svn.sourceforge.net/svnroot/xswrun/autoit'
    strCmd = (r'svn.exe log %s -v --xml > svnlog.log' % ( strRev))
	XswUtility.OutputCmd(strCmd)
