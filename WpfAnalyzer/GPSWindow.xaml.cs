namespace WpfAnalyzer
{
    using System;
    using System.Collections.ObjectModel;
    using System.IO;
    using System.Text;
    using System.Windows;
    using System.Windows.Forms;
    public partial class GPSWindow : Window
    {
        private ObservableCollection<Point> data { get; set; }
        private ObservableCollection<Point> points;
        private int width, height;
        public GPSWindow(ObservableCollection<Point> points, int w, int h)
        {
            InitializeComponent();
            data = new ObservableCollection<Point>();
            width = w;
            height = h;
            this.points = points;
        }
        private void ButtonExit_Click(object sender, RoutedEventArgs e)
        {
            Close();
        }
        private void ButtonOK_Click(object sender, RoutedEventArgs e)
        {
            Point p1, p2;
            double x, y;
            if (checkTextBoxes())
            {
                getPoints(out p1, out p2);
                double xc = p1.X * p2.X >= 0 ? p1.X - p2.X : 360 + p2.X - p1.X;
                double yc = p1.Y * p2.Y >= 0 ? p1.Y - p2.Y : 180 + p2.Y - p1.Y;
                double diag = Math.Sqrt(width * width + height * height);
                double diagc = Math.Sqrt(xc * xc + yc * yc);
                double coof = diagc / diag;
                double ang;
                double diagAngC = Math.Atan2(p1.Y - p2.Y, p1.X - p2.X) + Math.PI / 2;
                foreach (var d in data)
                {
                    x = p1.X * d.X >= 0 ? p1.X - d.X : 360 + d.X - p1.X;
                    y = p1.Y * d.Y >= 0 ? p1.Y - d.Y : 180 + d.Y - p1.Y;
                    ang = Math.Atan2(y, x) + Math.PI / 2;
                    ang = (Math.PI / -4) + diagAngC - ang;
                    x = x * coof * Math.Cos(ang);
                    y = y * coof * Math.Cos(ang + Math.PI / 2);
                    points.Add(new Point((int)x, (int)y));
                }
            }
        }
        private void ButtonSave_Click(object sender, RoutedEventArgs e)
        {
            Point p1, p2;
            double x, y;
            if (checkTextBoxes())
            {
                SaveFileDialog sfd = new SaveFileDialog();
                sfd.Title = "CSV";
                sfd.DefaultExt = ".csv";
                sfd.Filter = "CSV documents (.csv)|*.csv";
                if (sfd.ShowDialog() == System.Windows.Forms.DialogResult.OK)
                {
                    getPoints(out p1, out p2);
                    double xc = p1.X * p2.X >= 0 ? p1.X - p2.X : 360 + p2.X - p1.X;
                    double yc = p1.Y * p2.Y >= 0 ? p1.Y - p2.Y : 180 + p2.Y - p1.Y;
                    double diag = Math.Sqrt(width * width + height * height);
                    double diagc = Math.Sqrt(xc * xc + yc * yc);
                    double coof = diag / diagc;
                    double ang;
                    double diagAngC = Math.Atan2(p1.Y - p2.Y, p1.X - p2.X) + Math.PI / 2;
                    StringBuilder sb = new StringBuilder();
                    foreach (var d in points)
                    {
                        ang = Math.Atan2(d.Y, d.X) + Math.PI / 2;
                        ang = (Math.PI / -4) + ang - diagAngC;
                        x = p1.X + (d.X * coof * Math.Cos(ang));
                        y = p1.Y + (d.Y * coof * Math.Cos(ang + Math.PI / 2));
                        x = ToInterval(x, 180);
                        y = ToInterval(y, 90);
                        sb.Append($"{x};{y};{Environment.NewLine}");
                    }
                    byte[] buf = Encoding.UTF8.GetBytes(sb.ToString());
                    using (FileStream stream = new FileStream(sfd.FileName, FileMode.OpenOrCreate))
                    {
                        stream.Write(buf, 0, buf.Length);
                    }
                }
            }
        }
        private bool checkTextBoxes()
        {
            double buf;
            if (double.TryParse(TextBoxLongL.Text, out buf))
            {
                if (double.TryParse(TextBoxLatL.Text, out buf))
                {
                    if (double.TryParse(TextBoxLongR.Text, out buf))
                    {
                        if (double.TryParse(TextBoxLatR.Text, out buf))
                        {
                            return true;
                        }
                    }
                }
            }
            return false;
        }
        private void getPoints(out Point p1, out Point p2)
        {
            p1 = new Point(double.Parse(TextBoxLongL.Text), double.Parse(TextBoxLatL.Text));
            p2 = new Point(double.Parse(TextBoxLongR.Text), double.Parse(TextBoxLatR.Text));
        }
        private double ToInterval(double i, int inter)
        {
            double x = i;
            while (x > inter)
            {
                x -= inter;
            }
            while (x < -inter)
            {
                x += inter;
            }
            return x;
        }
        Tuple<double, double> GetFactMercatorsProjection(double Long, double Lat)
        {
            double mercX, mercY;
            double MAX_LAT = 89.5;
            double R_MAJOR = 6378137.0;
            double R_MINOR = 6356752.3142;
            double ECCENT = Math.Sqrt(1.0 - Math.Pow(R_MINOR / R_MAJOR, 2));
            double ECCNTH = ECCENT * 0.5;
            mercX = Long * R_MAJOR;
            if (Lat > MAX_LAT) Lat = MAX_LAT;
            if (Lat < -MAX_LAT) Lat = -MAX_LAT;
            double phi = Lat;
            double con = ECCENT * Math.Sin(phi);
            con = Math.Pow((1.0 - con) / (1.0 + con), ECCNTH);
            mercY = -R_MAJOR * Math.Log(Math.Tan(0.5 * (Math.PI / 2.0 - phi)) / con);
            return new Tuple<double, double>(mercX, mercY);
        }
        Tuple<double, double> GetCalcCoordinates(double mercX, double mercY)
        {
            double Long, Lat;
            double MAX_LAT = 89.5;
            double R_MAJOR = 6378137.0;
            double R_MINOR = 6356752.3142;
            double ECCENT = Math.Sqrt(1.0 - Math.Pow(R_MINOR / R_MAJOR, 2));
            double ECCNTH = ECCENT * 0.5;
            Long = mercX / R_MAJOR;
            double ts = Math.Exp(-mercY / R_MAJOR);
            double phi = Math.PI * 0.5 - 2.0 * Math.Atan(ts);
            uint i = 0;
            double dPhi = 1;
            while ((dPhi >= 0 ? dPhi : -dPhi) > 0.000000001 && i++ < 15)
            {
                double con = ECCENT * Math.Sin(phi);
                dPhi = Math.PI * 0.5 - 2.0 * Math.Atan(ts * Math.Pow((1.0 - con) / (1.0 + con), ECCNTH)) - phi;
                phi += dPhi;
            }
            Lat = phi;
            return new Tuple<double, double>(Long, Lat);
        }
    }
}