namespace WpfAnalyzer
{
    using RouteAnalyzer;
    using System;
    using System.Collections.Generic;
    using System.Drawing;
    using System.Windows;

    public partial class RouteWindow : Window
    {
        private List<Route> rs;
        private Route route;
        private Bitmap background;
        private List<ImgPoint> data { get; set; }

        public RouteWindow(Bitmap back, List<Route> routes, Route r = null)
        {
            InitializeComponent();
            route = r;
            background = back;
            MainImg.Source = ConvertBitmapToBitmapImage.Convert(background);
            data = new List<ImgPoint>();
            RoutesDataGrid.ItemsSource = data;
            Closed += SaveBeforeClose;
            rs = routes;
            if (route == null)
            {
                route = new Route();
                route.Name = $"N{Guid.NewGuid().ToString()}";
            }
            else
            {                
                data.Add(route.Parts[0].pointStart);
                foreach (var p in route.Parts)
                {
                    data.Add(p.pointEnd);
                }
            }
            TextBoxName.Text = route.Name;
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
                route.Parts.Add(new RoutePart(data[i], data[i + 1]));
            }

            rs.Add(route);
        }

        private void ButtonGPS_Click(object sender, RoutedEventArgs e)
        {

        }
    }
}
