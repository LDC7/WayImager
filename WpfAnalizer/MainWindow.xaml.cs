namespace WpfAnalizer
{
    using System.Windows;
    using RouteAnalizer;

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
            RouteAnalizer.Filter fil = new RouteAnalizer.Filter();
        }
    }
}
