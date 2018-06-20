namespace WpfAnalyzer
{
    using RouteAnalyzer;
    using System;
    using System.Collections.Generic;
    using System.Collections.ObjectModel;
    using System.Drawing;
    using System.Windows;
    using System.Windows.Media;
    using System.Windows.Media.Imaging;
    public partial class RouterWindow : Window
    {
        private Router router;
        private BitmapImage infBmp;
        private BitmapImage inpBmp;
        public ObservableCollection<Route> data;
        private List<string> rNames { get; set; }
        public int nrni = 0;
        public int arni = 0;
        public RouterWindow(Filter filter)
        {
            InitializeComponent();
            router = new Router(filter);
            Bitmap tempBmp = null;
            if (filter.HasInpMat)
            {
                filter.GetInpImg(ref tempBmp);
                inpBmp = ConvertBitmapToBitmapImage.Convert(tempBmp);
            }
            filter.GetInfMap(ref tempBmp);
            infBmp = ConvertBitmapToBitmapImage.Convert(tempBmp);
            ImageInfMap.Source = infBmp;
            data = new ObservableCollection<Route>();
            data.CollectionChanged += ReloadDataGrid;
            rNames = new List<string>();
            RoutesDataGrid.ItemsSource = rNames;
        }
        private void ButtonExit_Click(object sender, RoutedEventArgs e)
        {
            Close();
        }
        private void ButtonAdd_Click(object sender, RoutedEventArgs e)
        {
            RouteWindow rw;
            if (inpBmp != null)
            {
                rw = new RouteWindow(this, inpBmp, ref data);
            }
            else
            {
                rw = new RouteWindow(this, infBmp, ref data);
            }
            rw.Show();
        }
        private void ButtonOpen_Click(object sender, RoutedEventArgs e)
        {
            Route r = data[RoutesDataGrid.SelectedIndex];
            data.RemoveAt(RoutesDataGrid.SelectedIndex);
            RouteWindow rw;
            if (inpBmp != null)
            {
                rw = new RouteWindow(this, inpBmp, ref data, r);
            }
            else
            {
                rw = new RouteWindow(this, infBmp, ref data, r);
            }
            rw.Show();
        }
        private void ButtonEvaluate_Click(object sender, RoutedEventArgs e)
        {
            if (RoutesDataGrid.SelectedIndex >= 0)
            {
                router.CreateRouteValue(data[RoutesDataGrid.SelectedIndex]);
                RoutesDataGrid_SelectionChanged(null, null);
                data[RoutesDataGrid.SelectedIndex].Evaluated = true;
            }
        }
        private void ButtonAltPaths_Click(object sender, RoutedEventArgs e)
        {
            if (RoutesDataGrid.SelectedIndex >= 0)
            {
                Route r = data[RoutesDataGrid.SelectedIndex];
                if (r.Evaluated)
                {
                    AltPathWindow alw = new AltPathWindow(this, router, r);
                    alw.Show();
                }
                else
                {
                    System.Windows.MessageBox.Show("Маршрут должен быть оценён.", "ERROR", MessageBoxButton.OK);
                }
            }
        }
        private void RoutesDataGrid_SelectionChanged(object sender, System.Windows.Controls.SelectionChangedEventArgs e)
        {
            if (RoutesDataGrid.SelectedIndex >= 0)
            {
                int pSize = (int)((infBmp.Width + infBmp.Height) / 128);
                var route = data[RoutesDataGrid.SelectedIndex];
                DrawingVisual dv = new DrawingVisual();
                using (DrawingContext dc = dv.RenderOpen())
                {
                    dc.DrawImage(infBmp, new Rect(0, 0, infBmp.PixelWidth, infBmp.PixelHeight));
                    int min = route.min > 255 ? 0 : route.min;
                    decimal coofMax = route.max <= 0 ? 1 : (255 / route.max);
                    byte val;
                    foreach (var p in route.Parts)
                    {
                        if (p.IntermidiatePoints != null && p.IntermidiatePoints.Count > 0)
                        {
                            val = (byte)((p.IntermidiatePoints[0].value - min) * coofMax);
                            DrawLine(dc, p.pointStart.x, p.pointStart.y, p.IntermidiatePoints[0].x, p.IntermidiatePoints[0].y, val, pSize / 1.5);
                            for (int i = 1; i < p.IntermidiatePoints.Count; i++)
                            {
                                val = (byte)((p.IntermidiatePoints[i].value - min) * coofMax);
                                DrawLine(dc, p.IntermidiatePoints[i - 1].x, p.IntermidiatePoints[i - 1].y, p.IntermidiatePoints[i].x, p.IntermidiatePoints[i].y, val, pSize / 1.5);
                            }
                            val = (byte)((p.IntermidiatePoints[p.IntermidiatePoints.Count - 1].value - min) * coofMax);
                            DrawLine(dc, p.IntermidiatePoints[p.IntermidiatePoints.Count - 1].x, p.IntermidiatePoints[p.IntermidiatePoints.Count - 1].y, p.pointEnd.x, p.pointEnd.y, val, pSize / 1.5);
                        }
                        else
                        {
                            val = (byte)((((p.pointStart.value + p.pointEnd.value) / 2) - min) * coofMax);
                            DrawLine(dc, p.pointStart.x, p.pointStart.y, p.pointEnd.x, p.pointEnd.y, val, pSize / 1.5);
                        }
                        val = (byte)((p.pointStart.value - min) * coofMax);
                        DrawEllipse(dc, p.pointStart.x, p.pointStart.y, val, pSize);
                        val = (byte)((p.pointEnd.value - min) * coofMax);
                        DrawEllipse(dc, p.pointEnd.x, p.pointEnd.y, val, pSize);
                    }
                }
                RenderTargetBitmap rtb = new RenderTargetBitmap((int)infBmp.Width, (int)infBmp.Height, 96, 96, PixelFormats.Pbgra32);
                rtb.Render(dv);
                ImageInfMap.Source = rtb;
            }
        }
        private void DrawLine(DrawingContext dc, int x1, int y1, int x2, int y2, byte val, double th)
        {
            dc.DrawLine(new System.Windows.Media.Pen(new SolidColorBrush(System.Windows.Media.Color.FromRgb((byte)(255 - val), val, 0)),
                                th / 1.5),
                                new System.Windows.Point(x1, y1),
                                new System.Windows.Point(x2, y2));
        }
        private void DrawEllipse(DrawingContext dc, int x, int y, byte val, int rad)
        {
            dc.DrawEllipse(new SolidColorBrush(System.Windows.Media.Color.FromRgb((byte)(255 - val), val, 0)), null,
                            new System.Windows.Point(x, y),
                            rad, rad);
        }
        private void ReloadDataGrid(object sender, EventArgs e)
        {
            rNames.Clear();
            foreach (var l in data)
            {
                rNames.Add(l.Name);
            }
            RoutesDataGrid.ItemsSource = null;
            RoutesDataGrid.ItemsSource = rNames;
        }
    }
}