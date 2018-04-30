namespace WayImages
{
    using System;
    using System.Collections.Generic;
    using System.ComponentModel;
    using System.Drawing;
    using System.Drawing.Drawing2D;
    using System.Drawing.Imaging;
    using System.IO;
    using System.Linq;
    using System.Net;
    using System.Text;
    using System.Windows.Media;
    using System.Windows.Media.Imaging;

    public abstract class WayImager
    {
        public static List<MyPoint> Data { get; set; }
        public static List<MyPoint> Ways { get; }
        public static decimal Speed { get; set; }
        public static string Path { get; set; }
        public static float SpeedW { get; set; }
        public static int SpeedH { get; set; }

        private static WebClient webClient;

        const int sizeW = 900;
        const int sizeH = 900;

        static WayImager()
        {
            webClient = new WebClient();
            webClient.Headers.Add("User-Agent: Other");
            Ways = new List<MyPoint>();
            Speed = 0.0003m;
            SpeedW = 20;
            Path = "img";
        }

        public static ImageSource CreatePreview(List<MyPoint> WayItems)
        {
            StringBuilder sb = new StringBuilder();

            foreach (var p in WayItems)
            {
                sb.AppendFormat("|{0:f6},{1:f6}", p.Latitude, p.Longitude);
            }

            Uri uri = new Uri($"http://maps.googleapis.com/maps/api/staticmap?path=color:0xff0000|weight:5{sb.ToString()}&scale=2&format=png&size=1280x1280&maptype=satellite");
            // Сохранение добавить

            return new BitmapImage(uri);
        }

        public static void MakeWays(BackgroundWorker bw)
        {
            Ways.Clear();
            bool firstFlag = true;

            for (int i = 1; i < Data.Count; i++)
            {
                MakeWay(Data[i - 1], Data[i], firstFlag);
                firstFlag = false;

                if (bw.CancellationPending)
                {
                    return;
                }
            }
        }

        private static void MakeWay(MyPoint startPoint, MyPoint endPoint, bool firstFlag = false)
        {
            decimal spLat;
            decimal spLong;
            int rotation;

            MyPoint now = startPoint;
            MyPoint p2 = endPoint;

            decimal lenLat = p2.Latitude - now.Latitude;
            decimal lenLong = p2.Longitude - now.Longitude;
            decimal len = decimal.Round((decimal)Math.Sqrt((double)(lenLat * lenLat + lenLong * lenLong)), 7);
            Angle neededYaw = (float)(Math.Atan2((double)lenLat, (double)lenLong) * (180 / Math.PI)) - 90;

            if (firstFlag)
            {
                now.Yaw = (float)(Math.Atan2((double)lenLat, (double)lenLong) * (180 / Math.PI)) - 90;
                Ways.Add(now);
            }
            else
            {
                now.Yaw = Ways.Last().Yaw;
            }            

            while (len > Speed)
            {
                spLat = (decimal)Math.Cos((new Angle(now.Yaw)).ToRad()) * Speed;// * (Math.Sin((new Angle(now.Yaw)).ToRad()) < 0 ? 1 : -1);
                spLong = (decimal)Math.Cos((new Angle(90 + now.Yaw)).ToRad()) * Speed;// * (Math.Cos((new Angle(now.Yaw)).ToRad()) < 0 ? -1 : 1);
                now = new MyPoint(now);
                now.Latitude += spLat;
                now.Longitude += spLong;

                if (Math.Abs(now.Yaw - neededYaw) > SpeedW)
                {
                    rotation = neededYaw - now.Yaw < 180 ? 1 : -1;
                    now.Yaw += SpeedW * rotation;
                }
                else
                {
                    now.Yaw = neededYaw;
                }

                Ways.Add(now);

                lenLat = p2.Latitude - now.Latitude;
                lenLong = p2.Longitude - now.Longitude;
                len = decimal.Round((decimal)Math.Sqrt((double)(lenLat * lenLat + lenLong * lenLong)), 7);
                neededYaw = (float)(Math.Atan2((double)lenLat, (double)lenLong) * (180 / Math.PI)) - 90;

                if (Ways.Count > 500)
                {
                    break;
                }
            }

            now = new MyPoint(now);
            now.Latitude = endPoint.Latitude;
            now.Longitude = endPoint.Longitude;
            Ways.Add(now);
        }

        public static void MakeImages(BackgroundWorker bw)
        {
            Directory.CreateDirectory(Path);
            int numOfImgs = 1;
            Bitmap temp;

            //Нули добавляем для алгоритма Макса
            int count = Ways.Count;

            foreach (var p in Ways)
            {
                string nameForImage = numOfImgs.ToString($"D{count.ToString().Length}");

                temp = GetImage(p);
                SaveJPG(temp, $"{Path}/{nameForImage}.jpg");
                bw.ReportProgress(100 * numOfImgs / count);
                numOfImgs++;
                
                if (bw.CancellationPending)
                {
                    return;
                }
            }
        }

        private static Bitmap GetImage(MyPoint point)
        {
            const string key = "AIzaSyBfmTeq_d7lCghqlL_kX29Qsr2vQIB0UdA";
            int zoom = 15;

            string uri = $"https://maps.googleapis.com/maps/api/staticmap?center= {point.Latitude} {point.Longitude}&zoom={zoom}&size=1280x1280&maptype=satellite&key={key}&format=png&scale=2";

            Bitmap img = new Bitmap(new MemoryStream(webClient.DownloadData(uri)));

            PropertyItem tempProp = img.PropertyItems.First();

            //пока только поворот(и тот по ощущения не правильный)
            img = RotateImage(img, point.Yaw);

            img = img.Clone(new Rectangle(img.Width / 2 - sizeW / 2, img.Height / 2 - sizeH / 2, sizeW, sizeH), img.PixelFormat);

            //установка широты и долготы
            tempProp.Len = 8;
            tempProp.Type = 5;
            tempProp.Id = 2;
            tempProp.Value = BitConverter.GetBytes(rational((double)point.Latitude));
            img.SetPropertyItem(tempProp);
            tempProp.Id = 4;
            tempProp.Value = BitConverter.GetBytes(rational((double)point.Longitude));
            img.SetPropertyItem(tempProp);

            return img;
        }

        private static Bitmap RotateImage(Bitmap img, float rotationAngle)
        {
            Bitmap bmp = new Bitmap(img.Width, img.Height);

            using (Graphics gfx = Graphics.FromImage(bmp))
            {
                gfx.TranslateTransform((float)bmp.Width / 2, (float)bmp.Height / 2);
                gfx.RotateTransform(rotationAngle);
                gfx.TranslateTransform(-(float)bmp.Width / 2, -(float)bmp.Height / 2);
                gfx.InterpolationMode = InterpolationMode.HighQualityBicubic;
                gfx.DrawImage(img, new Point(0, 0));
            }

            return bmp;
        }

        private static void SaveJPG(Bitmap bmp, string path)
        {
            EncoderParameters encoderParameters = new EncoderParameters(1);
            encoderParameters.Param[0] = new EncoderParameter(System.Drawing.Imaging.Encoder.Quality, 100L);

            // Добавить параметры изображения (высота и т.п.)

            bmp.Save(path, ImageCodecInfo.GetImageEncoders().FirstOrDefault(x => x.FormatID == ImageFormat.Jpeg.Guid), encoderParameters);
        }

        private static ulong rational(double a)
        {
            uint denom = 1000;
            uint num = (uint)(a * denom);
            ulong tmp;
            tmp = (ulong)denom << 32;
            tmp |= (ulong)num;
            return tmp;
        }
    }
}
