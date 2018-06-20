namespace WpfAnalyzer
{
    using RouteAnalyzer;
    using System;
    using System.Collections.ObjectModel;
    using System.Collections.Specialized;
    using System.Windows;
    using System.Windows.Input;
    using System.Windows.Media;
    using System.Windows.Media.Imaging;
    public partial class RouteWindow : Window
    {
        private ObservableCollection<Route> rs;
        private Route route;
        private BitmapImage background;
        private ObservableCollection<Point> data { get; set; }
        public RouteWindow(RouterWindow owner, BitmapImage back, ref ObservableCollection<Route> routes, Route r = null)
        {
            this.Owner = owner;
            InitializeComponent();
            route = r;
            background = back;
            MainImg.Source = background;
            data = new ObservableCollection<Point>();
            PointsDataGrid.ItemsSource = data;
            Closed += SaveBeforeClose;
            rs = routes;
            if (route == null)
            {
                route = new Route();
                route.Name = $"NewRoute{((RouterWindow)this.Owner).nrni}";
                if (((RouterWindow)this.Owner).nrni == int.MaxValue)
                {
                    ((RouterWindow)this.Owner).nrni = int.MinValue;
                }
                ((RouterWindow)this.Owner).nrni++;
            }
            else
            {
                if (route.Parts.Count > 0)
                {
                    data.Add(new Point(route.Parts[0].pointStart.x, route.Parts[0].pointStart.y));
                    foreach (var p in route.Parts)
                    {
                        data.Add(new Point(p.pointEnd.x, p.pointEnd.y));
                    }
                }
            }
            TextBoxName.Text = route.Name;
            BorderImg.MouseLeftButtonDown += Image_MouseDown;
            data.CollectionChanged += reloadPoints;
            reloadPoints(null, null);
        }
        private void ButtonExit_Click(object sender, RoutedEventArgs e)
        {
            Close();
        }
        private void SaveBeforeClose(object sender, EventArgs e)
        {
            route.Name = TextBoxName.Text;
            route.Parts.Clear();
            for (int i = 0; i < data.Count - 1; i++)
            {
                route.Parts.Add(new RoutePart(new ImgPoint((int)data[i].X, (int)data[i].Y), new ImgPoint((int)data[i + 1].X, (int)data[i + 1].Y)));
            }
            rs.Add(route);
        }
        private void ButtonGPS_Click(object sender, RoutedEventArgs e)
        {
            GPSWindow gpsw = new GPSWindow(data, background.PixelWidth, background.PixelHeight);
            gpsw.Show();
        }
        private void Image_MouseDown(object sender, MouseButtonEventArgs e)
        {
            Point temp = e.GetPosition(this.BorderImg);
            data.Add(new Point(
                (int)(background.Width * temp.X / BorderImg.ActualWidth),
                (int)(background.Height * temp.Y / BorderImg.ActualHeight)));
        }
        private void reloadPoints(object sender, NotifyCollectionChangedEventArgs e)
        {
            int pSize = (int)((background.PixelWidth + background.PixelHeight) / 128);
            DrawingVisual dv = new DrawingVisual();
            using (DrawingContext dc = dv.RenderOpen())
            {
                dc.DrawImage(background, new Rect(0, 0, background.PixelWidth, background.PixelHeight));
                foreach (var p in data)
                {
                    dc.DrawEllipse(Brushes.Green, null, p, pSize, pSize);
                }
            }
            RenderTargetBitmap rtb = new RenderTargetBitmap((int)background.Width, (int)background.Height, 96, 96, PixelFormats.Pbgra32);
            rtb.Render(dv);
            MainImg.Source = rtb;
        }
    }
}