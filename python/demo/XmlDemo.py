#coding=utf-8
import os
import sys

########################################################
#my wrapper
def myint(str, dec=10):
    if (str == None or str == ''):
        return 0
    return int(str, 10)

########################################################
#stack info
#<STACK numallocs="01" size="0157" totalsize="0157">
class Stack:
    def __init__(self):
        self.numallocs = 0
        self.size = 0
        self.totalsize = 0

########################################################
#a frame
#<FRAME num="0" dll="ole32.dll" function ="ComPs_NdrDllCanUnloadNow" offset="0xFA" filename="" line="" addr="0x769AD03B" />
class Frame:
    def __init__(self):
        self.num = 0
        self.dll = ''
        self.function = ''
        self.offset = ''
        self.filename = ''
        self.line = 0
        self.addr = 0x0

#some frames
class Frames:
    def __init__(self):
        self.frames = []

########################################################
#a leak
class Leak:
    def __init__(self):
        self.stack = Stack();
        self.frames = Frames();

#some leaks
class Leaks:
    def __init__(self):
        self.leaks = []

    #将包含指定地址的堆栈都移到另一个leaks对象
    def move(self, target_leaks, addr):
        for leak in self.leaks:
            for frame in leak.frames.frames:
                if (frame.addr == addr):
                    target_leaks.leaks.append(leak)
                    self.leaks.remove(leak)

        return True

    def parse(self, filepaht):
        import xml.etree.ElementTree as ET

        xml_tree = ET.parse(filepaht)
        xml_logdata = xml_tree.getroot()    #root is 'logdata'

        xml_leaks = xml_logdata.find('LEAKS')
        if (xml_leaks == None):
            print('err syntax:not find LEAKS elemnt')
            return False

        for xml_leaks_child in xml_leaks:
            leak = Leak()
            if (xml_leaks_child.tag != 'STACK'):
                continue
            leak.stack.numallocs = int(xml_leaks_child.get('numallocs'))
            leak.stack.size = int(xml_leaks_child.get('size'))
            leak.stack.totalsize = int(xml_leaks_child.get('totalsize'))

            for xml_stack_child in xml_leaks_child:
                if (xml_stack_child.tag != 'FRAME'):
                    continue

                frame = Frame()
                frame.num= int(xml_stack_child.get('num'))
                frame.dll = xml_stack_child.get('dll')
                frame.function = xml_stack_child.get('function')
                frame.offset = xml_stack_child.get('offset')
                frame.filename = xml_stack_child.get('filename')
                frame.line = myint(xml_stack_child.get('line'))
                frame.addr = int(xml_stack_child.get('addr'), 16)
                leak.frames.frames.append(frame)

            self.leaks.append(leak)

        return True

########################################################
if __name__ == "__main__":
    leaks = Leaks()
    leaks.parse('XmlDemo.xml')

    leaks2 = Leaks()
    leaks.move(leaks2, 0x7D5BB5FE)

'''
    #获得根元素的标签名字 :   print(doc.tag)  #@result: configuration
    #Element text:              print(doc.text)   #@result: "\n"  如果所有元素是一行的话，结果为空

    # 子元素
    #* 迭代子元素: for child in doc
    for child in doc:
        print child.tag        #@result: policy-list 只有policy-list一个子元素

    #* 获得第1、2个子元素: doc[0:2]
    #  获得节点的子元素：elem.getchildren() 返回这个节点的子元素(list)
    #* 获得名为policy-list的元素: doc.find('policy-list')
    #  这里需要注意，由于doc是root的元素，这里不能用doc.find("policy")，来找到policy这个节点，会得到NoneType
     #* append(), remove(), insert()方法
    g2 = ET.Element("group")
    g2.text = "3"
    p2 = ET.Element("policy")
    p2.set("id","122334")
    p2.append(g2)  #policy下面增加一个group节点
    policylist = doc.find("policy-list")
    policylist.append(p2) #policy-list下面增加一个policy节点
    tree.write("path/to/Sample.xml") #写入文件

    insert(index,elem) #在制定的index插入一个元素

    del elem[n] #删除第n个节点
    elem.remove(elem2) #从节点elem中删除elem2子节点
    #* getiterator(tag) 返回一个列表，或者另外一个迭代对象

    # 节点的属性操作

    #* 获得节点的属性key列表: policy.keys() #@result: ["id"]
    #* 获得节点的属性数组: policy.items()   #@result: ["id","123456"]
    #* 测试节点是否包含某个属性(NAME):
    if policy.get('NAME') is not None, or if 'NAME' in book.attrib
    #* 获得属性 id的值:
     policy.attrib.get('id'), or
     policy.get('id')
    #* 给属性赋值: policy.set('Name', 'httppolicy')

    # 保存修改后的文件 tree.write("path/to/Sample.xml")
    另外几种保存方法，上面这种保存的文件是ascii格式的，如果要保存为utf-8的，可以用这种方式，
    f = open("path/to/Sample.xml","w")
    tree.write(f,"utf-8")
    # 创建节点
    elem.makeelement(tag,attr_dict)
    example: feed = root.makeelement('feed',{'version':'0.22'})

    ET.Element(tag,attr_dict,**extra)
    policy = ET.Element("policy",{"id":"12121"})

    ET.SubElement(parent,tag,attr_dict,**extra)
    group = ET.SubElement(policy,"group")
'''