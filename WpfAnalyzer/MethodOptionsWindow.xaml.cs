namespace WpfAnalizer
{
    using RouteAnalyzer;
    using System;
    using System.Collections.Generic;
    using System.Windows;

    public partial class MethodOptionsWindow : Window
    {
        private IPointFinder method;
        private List<Tuple<string, double>> list;

        public MethodOptionsWindow(IPointFinder finder)
        {
            method = finder;
            InitializeComponent();
            list = method.GettingOptions();
            DataGridMethodOptions.ItemsSource = list;
            DataGridMethodOptions.CanUserAddRows = false;
        }

        private void ButtonOK_Click(object sender, RoutedEventArgs e)
        {
            method.SetOptions(list);
            Close();
        }
    }
}
