using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace ImagesWF
{
    public partial class Form1 : Form
    {
        private List<PointF> listOfPoints;

        public Form1()
        {
            listOfPoints = new List<PointF>();
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            Loader.Loader loader = new Loader.Loader();
            loader.Load(listOfPoints);
            
            pictureBox1.Load("image.png");
        }

        private void button2_Click(object sender, EventArgs e)
        {
            float x, y;
            float.TryParse(textBox1.Text, out x);
            float.TryParse(textBox2.Text, out y);

            PointF p = new PointF()
            {
                X = x,
                Y = y
            };

            listOfPoints.Add(p);

            listBox1.Items.Add(p);
            listBox1.Refresh();
            textBox1.Text = string.Empty;
            textBox2.Text = string.Empty;
        }

        private void button3_Click(object sender, EventArgs e)
        {
            if (listBox1.SelectedIndex != -1)
            {
                listBox1.Items.RemoveAt(listBox1.SelectedIndex);
            }
        }
    }
}
