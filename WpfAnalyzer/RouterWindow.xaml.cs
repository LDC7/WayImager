namespace WpfAnalyzer
{
    using RouteAnalyzer;
    using System.Collections.Generic;
    using System.Drawing;
    using System.Windows;

    public partial class RouterWindow : Window
    {
        private Router router;
        private Bitmap infBmp;
        private Bitmap inpBmp;

        private List<Route> data { get; set; }

        public RouterWindow(Filter filter)
        {
            InitializeComponent();
            router = new Router(filter);
            if (filter.HasInpMat)
            {
                filter.GetInpImg(ref inpBmp);
            }
            filter.GetInfMap(ref infBmp);
            ImageInfMap.Source = ConvertBitmapToBitmapImage.Convert(infBmp);
            data = new List<Route>();
            RoutesDataGrid.ItemsSource = data;

            
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
                rw = new RouteWindow(inpBmp, data);
            }
            else
            {
                rw = new RouteWindow(infBmp, data);
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
                rw = new RouteWindow(inpBmp, data, r);
            }
            else
            {
                rw = new RouteWindow(infBmp, data, r);
            }
            rw.Show();
        }

        private void ButtonEvaluate_Click(object sender, RoutedEventArgs e)
        {
            if (RoutesDataGrid.SelectedIndex >= 0)
            {
                router.CreateRouteValue(data[RoutesDataGrid.SelectedIndex]);
            }
        }

        private void ButtonAltPaths_Click(object sender, RoutedEventArgs e)
        {
            if (RoutesDataGrid.SelectedIndex >= 0)
            {
                router.AlternativePaths(data[RoutesDataGrid.SelectedIndex]);
            }
        }

        private void RoutesDataGrid_SelectionChanged(object sender, System.Windows.Controls.SelectionChangedEventArgs e)
        {

        }
    }
}
