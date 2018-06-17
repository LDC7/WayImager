namespace WpfAnalyzer
{
    using RouteAnalyzer;
    using System.Collections.Generic;
    using System.Windows;

    public partial class PointsWindow : Window
    {
        private Route route;
        private List<ImgPoint> data;

        public PointsWindow(Route route)
        {
            this.route = route;
            InitializeComponent();
            SetData();
            DataGridPoints.ItemsSource = data;
        }

        private void ButtonClose_Click(object sender, RoutedEventArgs e)
        {
            Close();
        }

        private void SetData()
        {
            bool firstFlage = true;

            foreach (var r in route.Parts)
            {
                if (firstFlage)
                {
                    data.Add(r.pointStart);
                }
                foreach (var p in r.IntermidiatePoints)
                {
                    data.Add(p);
                }
                data.Add(r.pointEnd);
            }

        }
    }
}
