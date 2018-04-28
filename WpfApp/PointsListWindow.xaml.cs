namespace WpfApp
{
    using System.Collections.Generic;
    using System.Windows;
    using WayImages;

    public partial class PointsListWindow : Window
    {
        public List<MyPoint> Data { get; set; }

        public PointsListWindow(List<MyPoint> data)
        {
            Data = data;
            InitializeComponent();
            PointsDataGrid.ItemsSource = Data;
        }

        private void ButtonExit_Click(object sender, RoutedEventArgs e)
        {
            Close();
        }
    }
}
