namespace WpfApp
{
    using System;
    using System.Collections.Generic;
    using System.Reflection;
    using System.Windows;
    using System.Windows.Forms;
    using WayImages;

    /// <summary>
    /// Логика взаимодействия для MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public List<WayItem> Data { get; set; }

        public MainWindow()
        {
            Data = new List<WayItem>();
            InitializeComponent();
            MainDataGrid.ItemsSource = Data;
            TextBoxPath.Text = $"{Assembly.GetExecutingAssembly().Location}/{WayImager.Path}";
            InitTestData();
        }

        private void ButtonExit_Click(object sender, RoutedEventArgs e)
        {
            Close();
        }

        private void ButtonExecute_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                ValidateData();
            }
            catch (Exception ex)
            {
                System.Windows.MessageBox.Show(ex.Message, "ALERT", MessageBoxButton.OK, MessageBoxImage.Error);
                return;
            }

            ImageBox.Source = WayImager.CreatePreview(Data);
            WayImager.MakeWays(Data);
            WayImager.MakeImages();
        }

        private void InitTestData()
        {
            Data.Add(
                new WayItem
                {
                    lat1 = 56.8746803m,
                    lon1 = 53.3047833m,
                    h1 = 0,
                    lat2 = 56.8745807m,
                    lon2 = 53.2929387m,
                    h2 = 0,
                    kr = 0,  tn = 0,
                    dkr = 0, dtn = 0, dyw = 0 });
            Data.Add(
                new WayItem
                {
                    lat1 = 56.8745807m,
                    lon1 = 53.2929387m,
                    h1 = 0,
                    lat2 = 56.8699838m,
                    lon2 = 53.2988932m,
                    h2 = 0,
                    kr = 0, tn = 0,
                    dkr = 0, dtn = 0, dyw = 0 });
            Data.Add(
                new WayItem
                {
                    lat1 = 56.8699838m,
                    lon1 = 53.2988932m,
                    h1 = 0,
                    lat2 = 56.8746803m,
                    lon2 = 53.3047833m,
                    h2 = 0,
                    kr = 0, tn = 0,
                    dkr = 0, dtn = 0, dyw = 0 });
        }

        private void ButtonFolder_Click(object sender, RoutedEventArgs e)
        {
            FolderBrowserDialog fbd = new FolderBrowserDialog();
            fbd.Description = "Укажите папку для сохранения.";
            if (fbd.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                TextBoxPath.Text = fbd.SelectedPath;
                WayImager.Path = fbd.SelectedPath;
            }
        }

        private void ValidateData()
        {
            if (Data.Count == 0)
            {
                throw new ArgumentException("Empty data!");
            }

            for (int i = 1; i < Data.Count; i++)
            {
                if (Data[i].lat1 != Data[i-1].lat2 || Data[i].lon1 != Data[i - 1].lon2 || Data[i].h1 != Data[i - 1].h2)
                {
                    throw new Exception($"Line {i + 1}: Start point shuold coincide with the end point of the previous path.");
                }
            }

            int integ;
            if (int.TryParse(TextBoxSpeed.Text, out integ))
            {
                WayImager.Speed = msToCor(integ);
            }
            else
            {
                throw new ArgumentException("Incorrect speed.");
            }

            float fl;
            if (float.TryParse(TextBoxSpeedW.Text, out fl))
            {
                WayImager.SpeedW = fl;
            }
            else
            {
                throw new ArgumentException("Incorrect turn speed.");
            }

            if (int.TryParse(TextBoxSpeedH.Text, out integ))
            {
                WayImager.SpeedH = integ;
            }
            else
            {
                throw new ArgumentException("Incorrect speed of change in altitude.");
            }
        }

        private decimal msToCor(int ms)
        {
            return ms * 0.0001m / 30;
        }
    }
}
