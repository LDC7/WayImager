namespace WpfAnalizer
{
    using System.Windows;
    using RouteAnalyzer;

    /// <summary>
    /// Логика взаимодействия для MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
        }

        private void ButtonExit_Click(object sender, RoutedEventArgs e)
        {
            Close();
        }

        private void ButtonExecute_Click(object sender, RoutedEventArgs e)
        {
            Filter filter = new Filter();
            filter.Process();
        }
    }
}
