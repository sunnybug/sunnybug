#-------------------------------------------------------------------------------
# Name:        模块1
# Purpose:
#
# Author:      Administrator
#
# Created:     13-05-2011
# Copyright:   (c) Administrator 2011
# Licence:     <your licence>
#-------------------------------------------------------------------------------
#!/usr/bin/env python

from tkinter import *


# -*- coding: cp936 -*-
# <Button-1>：鼠标左击事件
# <Button-2>：鼠标中击事件
# <Button-3>：鼠标右击事件
# <Double-Button-1>：双击事件
# <Triple-Button-1>：三击事件
from tkinter import *
root = Tk()

def printCoords(event):
    print(event.x,event.y)

def CreateBtn():
    # 创建第一个Button,并将它与左键事件绑定
    bt1 = Button(root,text = 'leftmost button')
    bt1.bind('<Button-1>',printCoords)

    # 创建二个Button，并将它与中键事件绑定
    bt2 = Button(root,text = 'middle button')
    bt2.bind('<Button-2>',printCoords)

    # 创建第三个Button，并将它与右击事件绑定
    bt3 = Button(root,text = 'rightmost button')
    bt3.bind('<Button-3>',printCoords)

    # 创建第四个Button,并将它与双击事件绑定
    bt4 = Button(root,text = 'double click')
    bt4.bind('<Double-Button-1>',printCoords)

    # 创建第五个Button，并将它与三击事件绑定
    bt5 = Button(root, text = 'triple click')
    bt5.bind('<Triple-Button-1>',printCoords)

    bt1.grid()
    bt2.grid()
    bt3.grid()
    bt4.grid()
    bt5.grid()

def main():
    CreateBtn()
    root.mainloop()

if __name__ == '__main__':
    main()