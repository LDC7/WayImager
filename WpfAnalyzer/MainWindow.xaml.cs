namespace WpfAnalizer
{
    using RouteAnalyzer;
    using System;
    using System.IO;
    using System.Reflection;
    using System.Text;
    using System.Windows;
    using System.Windows.Forms;

    /// <summary>
    /// Логика взаимодействия для MainWindow.xaml
    /// </summary>
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
            }
            catch (Exception ex)
            {
                System.Windows.MessageBox.Show(ex.Message, "ERROR", MessageBoxButton.OK);
                using (FileStream fs = new FileStream(TextBoxOutPath.Text + "log.txt", FileMode.Create))
                {
                    byte[] buf = Encoding.UTF8.GetBytes(GetExceptionsLog(ex));
                    fs.Write(buf, 0, buf.Length);
                }
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
    }
}
