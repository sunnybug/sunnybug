using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

class xLog
{
    static public void Write(String msg)
    {
        Write("sys", msg);
    }

    static public void Write(String LogPath, String msg)
    {
        string filePath = LogPath + "\\" + DateTime.Now.Year.ToString() + DateTime.Now.Month.ToString();
        string fileName = filePath + "\\" + DateTime.Now.Day.ToString() + ".Log";
        if (!System.IO.Directory.Exists(LogPath))
        {
            System.IO.Directory.CreateDirectory(filePath);
        }
        FileStream filestream = new FileStream(fileName, FileMode.OpenOrCreate, FileAccess.Write, FileShare.None);
        StreamWriter writer = new StreamWriter(filestream, System.Text.Encoding.Default);
        writer.BaseStream.Seek(0, SeekOrigin.End);
        writer.WriteLine("{0} {1}", DateTime.Now.TimeOfDay, msg);
        writer.Flush();
        writer.Close();
        filestream.Close();
    }
}


