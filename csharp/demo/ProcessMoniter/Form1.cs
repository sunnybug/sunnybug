using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using ProcessMoniter;

namespace ProcessMoniterApp
{
    public partial class Form1 : PMoniter
    {
        public Form1()
        {
            InitializeComponent();
            Init(500);
        }

        private void button1_Click(object sender, EventArgs e)
        {
            AddProcessName(textNewProcessName.Text);
        }

    }
}
