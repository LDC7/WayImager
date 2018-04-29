namespace WpfApp
{
    using System.Collections.Generic;
    using System.Windows;
    using WayImages;

    public partial class MapWindow : Window
    {
        public List<MyPoint> Data { get; set; }

        public MapWindow(List<MyPoint> data)
        {
            Data = data;
            InitializeComponent();
            Browser.Navigate("https://www.google.ru/maps/@56.8742088,53.3011972,316m/data=!3m1!1e3?hl=ru");
        }

        private void ButtonExit_Click(object sender, RoutedEventArgs e)
        {
            Close();
        }

        private void ButtonOK_Click(object sender, RoutedEventArgs e)
        {
            Data.Clear();

            //распарсить Browser.Source

            Close();
        }
    }
}
