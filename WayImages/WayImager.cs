﻿namespace WayImages
{
    using System;
    using System.Collections.Generic;
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
        public static List<Way> Ways { get; }
        public static decimal Speed { get; set; }
        public static string Path { get; set; }
        public static float SpeedW { get; set; }
        public static int SpeedH { get; set; }

        private static WebClient webClient;

        const int size = 900;

        static WayImager()
        {
            webClient = new WebClient();
            webClient.Headers.Add("User-Agent: Other");
            Ways = new List<Way>();
            Speed = 0.0003m;
            SpeedW = 20;
            Path = "img";
        }

        public static ImageSource CreatePreview(List<WayItem> WayItems)
        {
            StringBuilder sb = new StringBuilder();

            sb.AppendFormat("|{0:f6},{1:f6}", WayItems[0].lat1, WayItems[0].lon1);

            for (int i = 0; i < WayItems.Count; i++)
            {
                sb.AppendFormat("|{0:f6},{1:f6}", WayItems[i].lat2, WayItems[i].lon2);
            }

            Uri uri = new Uri($"http://maps.googleapis.com/maps/api/staticmap?path=color:0xff0000|weight:5{sb.ToString()}&scale=2&format=png&size=1280x1280&maptype=satellite");
            // Сохранение добавить

            return new BitmapImage(uri);
        }

        public static void MakeWays(List<WayItem> WayItems)
        {
            Ways.Clear();

            for (int i = 0; i < WayItems.Count; i++)
            {
                MakeWay(WayItems[i]);
                RotationMove(i, WayItems);
            }
        }

        private static void MakeWay(WayItem wayItem)
        {
            decimal coef;
            decimal spLat;
            decimal spLong;

            MyPoint p1 = new MyPoint(wayItem.lat1, wayItem.lon1, wayItem.h1);
            MyPoint p2 = new MyPoint(wayItem.lat2, wayItem.lon2, wayItem.h2);
            Way way = new Way();

            MyPoint now = p1;
            decimal lenLat = p2.Latitude - now.Latitude;
            decimal lenLong = p2.Longitude - now.Longitude;
            decimal len = decimal.Round((decimal)Math.Sqrt((double)(lenLat * lenLat + lenLong * lenLong)), 7);

            now.Yaw = (float)(Math.Atan2((double)lenLat, (double)lenLong) * (180 / Math.PI)) - 90;
            way.Points.Add(now);

            while (len > Speed)
            {
                coef = Speed / len;
                spLat = coef * lenLat;
                spLong = coef * lenLong;
                now = new MyPoint(now.Latitude + spLat, now.Longitude + spLong, now.H);

                now.Yaw = (float)(Math.Atan2((double)lenLat, (double)lenLong) * (180 / Math.PI)) - 90;
                way.Points.Add(now);

                lenLat = p2.Latitude - now.Latitude;
                lenLong = p2.Longitude - now.Longitude;
                len = decimal.Round((decimal)Math.Sqrt((double)(lenLat * lenLat + lenLong * lenLong)), 7);
            }

            Ways.Add(way);
        }

        private static void RotationMove(int ind, List<WayItem> WayItems)
        {
            if (ind + 1 != WayItems.Count)
            {
                Way way = Ways.Last();
                MyPoint nextPoint = new MyPoint(WayItems[ind + 1].lat2, WayItems[ind + 1].lon2, WayItems[ind + 1].h2);
                MyPoint curPoint = new MyPoint(way.Points.Last());
                decimal lenLat = nextPoint.Latitude - curPoint.Latitude;
                decimal lenLong = nextPoint.Longitude - curPoint.Longitude;
                Angle neededYaw = (float)(Math.Atan2((double)lenLat, (double)lenLong) * (180 / Math.PI)) - 90;
                int temp = neededYaw - curPoint.Yaw > 180 ? -1 : 1;

                // Поворот лишний (неполный)

                while (Math.Abs(neededYaw - curPoint.Yaw) > SpeedW)
                {
                    curPoint.Yaw += SpeedW * temp;
                    way.Points.Add(curPoint);
                    curPoint = new MyPoint(curPoint);
                }
            }
        }

        public static void MakeImages()
        {
            Directory.CreateDirectory(Path);
            int numOfImgs = 1;
            Bitmap temp;

            //Нули добавляем для алгоритма Макса
            int count = 0;
            foreach (var w in Ways)
            {
                count += w.Points.Count;
            }
            
            foreach (var w in Ways)
            {
                for (int i = 0; i < w.Points.Count; i++)
                {
                    string nameForImage = numOfImgs.ToString($"D{count.ToString().Length}");

                    temp = GetImage(w.Points[i]);
                    SaveJPG(temp, $"{Path}/{nameForImage}.jpg");
                    numOfImgs++;
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

            img = img.Clone(new Rectangle(img.Width / 2 - size / 2, img.Height / 2 - size / 2, size, size), img.PixelFormat);

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
