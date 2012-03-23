Function SendLine(str)
	sh.SendKeys str&"{ENTER}"
End Function 

Function Telnet(host, user, pw)
	SendLine "open "&host
	WScript.Sleep 300
	SendLine ""
	SendLine user
	WScript.Sleep 300
	SendLine pw
	WScript.Sleep 300
End Function 

'保存传递参数
set args = WScript.arguments
host = args(0)
user = args(1)
pw = args(2)
set sh = WScript.CreateObject("WScript.Shell")
Telnet host, user, pw

