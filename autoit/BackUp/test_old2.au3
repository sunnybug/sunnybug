;======================================================
;
; Function Name:    _SendUnicode("string")
; Description:      Send a unicode or an ASCII string.
; Parameter(s):     $string is the string you want to send.
; Requirement(s):   String Input.
; Return Value(s):  None
; Author(s):        Robie Zhou (robiezhou@gmail.com)
;
;======================================================
Func _SendUnicode($string)
	Local $char
	Local $code

	For $i = 1 To StringLen($string)

		$char = StringMid($string, $i, 1)
		$code = Asc($char)
		If $code > 127 Then
			$code = $code * 256
			$i = $i + 1
			$char = StringMid($string, $i, 1)
			$code = $code + Asc($char)
		EndIf
		Send("{ASC " & $code & "}")
	Next
EndFunc   ;==>_SendUnicode


Run("notepad.exe")
WinWaitActive("无标题 - 记事本")
_SendUnicode("测试一下AutoIt对于Unicode的支持")