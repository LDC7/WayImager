namespace WpfAnalyzer
{
    using System.Drawing;
    using System.IO;
    using System.Windows.Media.Imaging;

    abstract class ConvertBitmapToBitmapImage
    {
        public static BitmapImage Convert(Bitmap src)
        {
            BitmapImage image = new BitmapImage();
            MemoryStream ms = new MemoryStream();
            ((Bitmap)src).Save(ms, System.Drawing.Imaging.ImageFormat.Bmp);            
            image.BeginInit();
            ms.Seek(0, SeekOrigin.Begin);
            image.StreamSource = ms;
            image.EndInit();
            return image;
        }
    }
}
