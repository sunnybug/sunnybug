import mmap
import os

def Testmmap():
    file_path = '2.test'
    f = open(file_path,  'wb')
    f.seek(10)  #跳过文件第一行长度
    size = os.path.getsize(file_path) 
    start_pos = size-f.tell()
    map = mmap.mmap(f.fileno(),  0)  #跳过第一行开始映射
    for line in map:
        print(line)
        
def Testmmap2():
    # write a simple example file
#    with open("hello.txt", "wb") as f:
 #       f.write(b"Hello Python!\n")

    with open("file.test", "rb") as f:
        # memory-map the file, size 0 means whole file
        map = mmap.mmap(f.fileno(), 0)
        # read content via standard file methods
        print(map.readline())  # prints b"Hello Python!\n"
        # read content via slice notation
        print(map[:5])  # prints b"Hello"
        # update content using slice notation;
        # note that new content must have same size
        #map[6:] = b" world!\n"
        # ... and read again using standard file methods
        map.seek(0)
        print(map.readline())  # prints b"Hello  world!\n"
        # close the map
        map.close()

def TestOpen():
    # write a simple example file
#    with open("file.test", "w") as f:
#        f.write("Hello Python!\n")
#        f.write("猪 Python!\n")
    
    #尝试读取文件编码类型
    with open('file.test',  'r') as f:
        print(f.readlines())
   
def utf8Detect(text):
    'Detect if a string is utf-8 encoding'
    lastch=0
    begin=0
    BOM=True
    BOMchs=(0xEF, 0xBB, 0xBF)
    good=0
    bad=0
    for char in text:
        ch=ord(char)
        if begin<3:
            BOM=(BOMchs[begin]==ch) and BOM
            begin += 1
            continue
        if (begin==4) and (BOM==True):
            break;
        if (ch & 0xC0) == 0x80:
            if (lastch & 0xC0) == 0xC0:
                good += 1 
            elif (lastch &0x80) == 0:
                bad += 1
        elif (lastch & 0xC0) == 0xC0:
            bad += 1
       
        lastch = ch
           
    if (((begin == 4) and (BOM == True)) or
        (good >= bad)):
        return True
    else:
        return False
    
def TestDetectFileType():
 #   with open("file.test", "w") as f:
 #       f.write("猪Hello Python!\n")
  #      f.write("猪 Python!\n")

    text=open('file.test',  'r').read()
    print(utf8Detect(text))
    
if __name__ == "__main__":
    Testmmap2()
