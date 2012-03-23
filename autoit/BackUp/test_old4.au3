Run("notepad.exe")
WinWaitActive("无标题 - ")
$Text = "中文 ：请多关照" & @CR
$Text &= "繁體 ：請多關照 " & @LF
$Text &= "日文 ：よろしくお願いします " & @CR
$Text &= "特殊符号： ♂☆⊙①ⅠΘ┆" & @LF
$Text &= "@CRLF 会换行2次" & @CRLF
$Text &= "End"

SendC($Text)

Func SendC($Str)
For $i = 1 To StringLen($Str)
    Send('{ASC ' & StringToBinary(StringMid($Str, $i, 1) & ' ') & '}')
   Next
EndFunc   ;==>SendC