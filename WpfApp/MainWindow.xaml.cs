namespace WpfApp
{
    using System;
    using System.Collections.Generic;
    using System.ComponentModel;
    using System.Reflection;
    using System.Windows;
    using System.Windows.Forms;
    using WayImages;

    public partial class MainWindow : Window
    {
        public List<MyPoint> Data { get; set; }
        private BackgroundWorker worker;

        public MainWindow()
        {
            worker = new BackgroundWorker();
            InitializeComponent();
            Data = new List<MyPoint>();
            MainDataGrid.ItemsSource = Data;
            TextBoxPath.Text = $"{Assembly.GetExecutingAssembly().Location}/{WayImager.Path}";
            InitTestData();
            MainProgressBar.IsIndeterminate = false;
        }

        private void ButtonExit_Click(object sender, RoutedEventArgs e)
        {
            if (worker.IsBusy)
            {
                worker.CancelAsync();
            }
            Close();
        }

        private void ButtonCreateImg_Click(object sender, RoutedEventArgs e)
        {
            worker = new BackgroundWorker();
            worker.WorkerSupportsCancellation = true;
            worker.WorkerReportsProgress = true;
            MainProgressBar.Value = 1;
            MainProgressBar.Maximum = 101;
            MainDataGrid.IsEnabled = false;
            ButtonOpenMap.IsEnabled = false;
            ButtonCreatePoints.IsEnabled = false;
            ButtonOpenPoints.IsEnabled = false;
            ButtonCreateImg.IsEnabled = false;
            ButtonFolder.IsEnabled = false;
            worker.DoWork += delegate
            {
                WayImager.MakeImages(worker);
            };
            worker.RunWorkerCompleted += delegate
            {
                MainDataGrid.IsEnabled = true;
                ButtonOpenMap.IsEnabled = true;
                ButtonCreatePoints.IsEnabled = true;
                ButtonOpenPoints.IsEnabled = true;
                ButtonCreateImg.IsEnabled = true;
                ButtonFolder.IsEnabled = true;
                MainProgressBar.Value = 0;
            };
            worker.ProgressChanged += delegate (object sender2, ProgressChangedEventArgs e2)
            {
                MainProgressBar.Value = e2.ProgressPercentage;
            };
            worker.RunWorkerAsync();
        }

        private void InitTestData()
        {
            Data.Add(
                new MyPoint
                {
                    Latitude = 56.8746803m,
                    Longitude = 53.3047833m,
                    H = 0
                });
            Data.Add(
                new MyPoint
                {
                    Latitude = 56.8806803m,
                    Longitude = 53.2988932m,
                    H = 0
                });
            Data.Add(
                new MyPoint
                {
                    Latitude = 56.8745807m,
                    Longitude = 53.2929387m,
                    H = 0
                });
            Data.Add(
                new MyPoint
                {
                    Latitude = 56.8699838m,
                    Longitude = 53.2988932m,
                    H = 0
                });
            Data.Add(
                new MyPoint
                {
                    Latitude = 56.8746803m,
                    Longitude = 53.3047833m,
                    H = 0
                });
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
            if (Data.Count < 2)
            {
                throw new ArgumentException("Not enought data!");
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

        private void ButtonOpenPoints_Click(object sender, RoutedEventArgs e)
        {
            PointsListWindow window = new PointsListWindow(WayImager.Ways);
            window.Show();
        }

        private void ButtonCreatePoints_Click(object sender, RoutedEventArgs e)
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

            worker = new BackgroundWorker();
            worker.WorkerSupportsCancellation = true;
            MainProgressBar.IsIndeterminate = true;
            ButtonOpenMap.IsEnabled = false;
            ButtonCreatePoints.IsEnabled = false;
            ButtonOpenPoints.IsEnabled = false;
            ButtonCreateImg.IsEnabled = false;
            WayImager.Data = new List<MyPoint>(Data);
            worker.DoWork += delegate
            {
                WayImager.MakeWays(worker);
            };
            worker.RunWorkerCompleted += delegate
            {
                ImageBox.Source = WayImager.CreatePreview(Data);
                ButtonOpenMap.IsEnabled = true;
                ButtonCreatePoints.IsEnabled = true;
                ButtonOpenPoints.IsEnabled = true;
                ButtonCreateImg.IsEnabled = true;
                MainProgressBar.IsIndeterminate = false;
            };
            worker.RunWorkerAsync();
        }

        private void ButtonOpenMap_Click(object sender, RoutedEventArgs e)
        {
            MapWindow window = new MapWindow(WayImager.Ways);
            window.Show();
        }
    }
}
