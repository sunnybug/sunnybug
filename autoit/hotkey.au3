;#»Õ±ê¼ü
;!ALT
;^CTRL
;+SHIFT

HotKeySet("#n", "StartNotepad")
HotKeySet("#f", "StartLocate")

While 1
	Sleep(1100)
WEnd


Func StartNotepad()
	Run("notepad")
EndFunc   ;==>StartNotepad

Func StartLocate()
	Run("locate")
EndFunc   ;==>StartNotepad