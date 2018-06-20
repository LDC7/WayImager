namespace WpfAnalyzer
{
    using RouteAnalyzer;
    using System.Windows;
    public partial class OptionWindow : Window
    {
        private Filter filter;
        private OptionWrapper options;
        private IPointFinder method;
        public OptionWindow(Filter fil)
        {
            filter = fil;
            options = new OptionWrapper(filter.options);
            InitializeComponent();
            DataContext = options;
            foreach (var s in MethodFactory.GetList())
            {
                ComboBoxMethod.Items.Add(s);
            }
            ComboBoxMethod.SelectedIndex = 0;
        }
        private void ButtonExit_Click(object sender, RoutedEventArgs e)
        {
            Close();
        }
        private void ButtonOK_Click(object sender, RoutedEventArgs e)
        {
            filter.pointMethod = method;
            filter.options = options.GetOptions();
            Close();
        }
        private void ButtonMethodOptions_Click(object sender, RoutedEventArgs e)
        {
            MethodOptionsWindow mow = new MethodOptionsWindow(method);
            mow.Show();
        }
        private void ComboBoxMethod_SelectionChanged(object sender, System.Windows.Controls.SelectionChangedEventArgs e)
        {
            method = MethodFactory.GetMethod(ComboBoxMethod.SelectedIndex);
        }
    }
}