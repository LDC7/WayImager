namespace WpfAnalyzer
{
    using RouteAnalyzer;
    using System.Collections.Generic;
    using System.Windows;

    public partial class AltPathWindow : Window
    {
        private Router router;
        private Route route;
        private decimal pointsMin;

        public AltPathWindow(RouterWindow window, Router router, Route route)
        {
            InitializeComponent();
            this.Owner = window;
            this.router = router;
            this.route = route;

            decimal temp;
            LabalName.Content = this.route.Name;
            temp = decimal.Round(this.route.min / 255, 5);
            LabalMin.Content = temp;
            temp = decimal.Round(this.route.max / 255, 5);
            LabalMax.Content = temp;
            temp = 256;
            foreach(var p in this.route.Parts)
            {
                if (temp > p.pointStart.value)
                {
                    temp = p.pointStart.value;
                }
                if (temp > p.pointEnd.value)
                {
                    temp = p.pointEnd.value;
                }
            }
            pointsMin = decimal.Round(temp / 255, 5);
            LabalPointsMin.Content = pointsMin;
        }

        private void ButtonExit_Click(object sender, RoutedEventArgs e)
        {
            Close();
        }

        private void ButtonOK_Click(object sender, RoutedEventArgs e)
        {
            decimal minVal;
            RouterWindow win;
            List<Route> list;
            if (decimal.TryParse(TextBoxLimit.Text, out minVal))
            {
                if (minVal >= pointsMin)
                {
                    win = (RouterWindow)this.Owner;
                    list = new List<Route>();
                    foreach (var r in router.AlternativePaths(route, (double)minVal))
                    {
                        r.Name = $"AltRoute{win.arni}";
                        if (win.arni == int.MaxValue)
                        {
                            win.arni = int.MinValue;
                        }
                        router.CreateRouteValue(r);
                        r.Evaluated = true;
                        win.arni++;
                        list.Add(r);
                    }
                    win.data.AddRange(list);
                }
                Close();
            }
        }
    }
}
