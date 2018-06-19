namespace WpfAnalyzer
{
    using RouteAnalyzer;
    using System;
    using System.ComponentModel;
    using System.IO;
    using System.Reflection;
    using System.Text;
    using System.Windows;
    using System.Windows.Forms;

    public partial class MainWindow : Window
    {
        private Filter filter;

        public MainWindow()
        {
            InitializeComponent();
            filter = new Filter();
            string progFolder = Assembly.GetExecutingAssembly().Location;
            progFolder = progFolder.Substring(0, progFolder.LastIndexOf('\\'));
            TextBoxOutPath.Text = $"{progFolder}\\temp\\";
            TextBoxInPath.Text = $"{progFolder}\\input.png";
            filter.SetPaths(TextBoxInPath.Text, TextBoxOutPath.Text);

            filter.pointMethod = MethodFactory.GetMethod(-1);
        }

        private void ButtonExit_Click(object sender, RoutedEventArgs e)
        {
            Close();
        }

        private void ButtonExecute_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                Directory.CreateDirectory(TextBoxOutPath.Text);
                filter.Process();
                System.Windows.MessageBox.Show("Done", "Done", MessageBoxButton.OK);
            }
            catch (Exception ex)
            {
                System.Windows.MessageBox.Show(ex.Message, "ERROR", MessageBoxButton.OK);
            }
        }

        private void ButtonInputFile_Click(object sender, RoutedEventArgs e)
        {
            OpenFileDialog ofd = new OpenFileDialog();
            ofd.Multiselect = false;
            ofd.Title = "Изображение";
            if (ofd.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                TextBoxInPath.Text = ofd.FileName;
                filter.SetPaths(TextBoxInPath.Text, TextBoxOutPath.Text);
            }
        }

        private void ButtonOutFolder_Click(object sender, RoutedEventArgs e)
        {
            FolderBrowserDialog fbd = new FolderBrowserDialog();
            fbd.Description = "Укажите папку для сохранения временных файлов.";
            if (fbd.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                TextBoxOutPath.Text = fbd.SelectedPath + "\\";
                filter.SetPaths(TextBoxInPath.Text, TextBoxOutPath.Text);
            }
        }

        private string GetExceptionsLog(Exception ex)
        {
            string str = ex.Message + '\n';
            if (ex.InnerException != null)
            {
                str = str + GetExceptionsLog(ex.InnerException);
            }

            return str;
        }

        private void ButtonEvaluateRoute_Click(object sender, RoutedEventArgs e)
        {
            if (filter.HasOutMat)
            {
                RouterWindow rw = new RouterWindow(filter);
                rw.Show();
                //this.WindowState = WindowState.Minimized;
            }
            else
            {
                System.Windows.MessageBox.Show("Отсутствует загруженная карта информативности.", "ERROR", MessageBoxButton.OK);
            }
        }

        private void ButtonLoadMap_Click(object sender, RoutedEventArgs e)
        {
            OpenFileDialog ofd = new OpenFileDialog();
            ofd.Multiselect = false;
            ofd.Title = "Карта информативности";
            if (ofd.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                filter.LoadBufferMat(ofd.FileName);
            }
        }

        private void ButtonOptions_Click(object sender, RoutedEventArgs e)
        {
            OptionWindow ow = new OptionWindow(filter);
            ow.Show();
        }
    }
}
