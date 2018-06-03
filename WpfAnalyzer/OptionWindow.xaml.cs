namespace WpfAnalizer
{
    using RouteAnalyzer;
    using System.Windows;

    public partial class OptionWindow : Window
    {
        private Filter filter;
        private Options options;

        private IPointFinder method;
        private int mapWinSize;
        private int dispWinSize;
        private int widthCamera;
        private int heightCamera;

        public OptionWindow(Filter fil)
        {
            filter = fil;
            options = filter.options;

            mapWinSize = options.MapWinSize;
            dispWinSize = (int)options.DispWinSize;
            widthCamera = options.CameraWidth;
            heightCamera = options.CameraHeight;

            InitializeComponent();

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
            options.MapWinSize = mapWinSize;
            options.DispWinSize = dispWinSize;
            options.CameraWidth = widthCamera;
            options.CameraHeight = heightCamera;

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
