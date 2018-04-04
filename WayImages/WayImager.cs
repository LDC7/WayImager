namespace WayImages
{
    using System;
    using System.Collections.Generic;
    using System.Drawing;
    using System.Drawing.Drawing2D;
    using System.Drawing.Imaging;
    using System.IO;
    using System.Linq;
    using System.Net;

    public abstract class WayImager
    {
        public static List<Way> Ways { get; }
        public static List<MyPoint> Points { get; }
        public static decimal Speed { get; set; }
        public static string Path { get; set; }
        public static float SpeedW { get; set; }

        const int size = 450;

        static WayImager()
        {
            Points = new List<MyPoint>();
            Ways = new List<Way>();
            Speed = 0.0003m;
            SpeedW = 20;
            Path = "img";
        }

        public static void MakeWays()
        {
            if (Points.Count < 2)
            {
                throw new Exception("Not enought points!");
            }

            Directory.CreateDirectory(Path);
            Ways.Clear();
            for (int i = 1; i < Points.Count; i++)
            {
                MakeWay(Points[i - 1], Points[i], i == 1);
                RotationMove(i);
            }
        }

        private static void MakeWay(MyPoint p1, MyPoint p2, bool firstImg = false)
        {
            decimal coef;
            decimal spLat;
            decimal spLong;

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

        private static void RotationMove(int ind)
        {
            if (ind + 1 != Points.Count)
            {
                Way way = Ways.Last();
                MyPoint nextPoint = Points[ind + 1];
                MyPoint curPoint = new MyPoint(way.Points.Last());
                decimal lenLat = nextPoint.Latitude - curPoint.Latitude;
                decimal lenLong = nextPoint.Longitude - curPoint.Longitude;
                Angle neededYaw = (float)(Math.Atan2((double)lenLat, (double)lenLong) * (180 / Math.PI)) - 90;
                int temp = neededYaw - curPoint.Yaw > 180 ? -1 : 1;

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
            int numOfImgs = 1;
            Bitmap temp;

            foreach (var w in Ways)
            {
                for (int i = 0; i < w.Points.Count; i++)
                {
                    temp = GetImage(w.Points[i]);
                    SaveJPG(temp, $"{Path}/{numOfImgs}.jpg");
                    numOfImgs++;
                }
            }
        }

        private static Bitmap GetImage(MyPoint point)
        {
            const string key = "AIzaSyBfmTeq_d7lCghqlL_kX29Qsr2vQIB0UdA";
            int zoom = 15;

            string uri = $"https://maps.googleapis.com/maps/api/staticmap?center= {point.Latitude} {point.Longitude}&zoom={zoom}&size=640x640&maptype=satellite&key={key}&format=BMP";
            WebClient webClient = new WebClient();
            webClient.Headers.Add("User-Agent: Other");

            Bitmap img = new Bitmap(new MemoryStream(webClient.DownloadData(uri)));

            PropertyItem tempProp = img.PropertyItems.First();

            //пока только поворот(и тот по ощущения не правильный)
            img = RotateImage(img, point.Yaw);
            img.Save($"img2/{Guid.NewGuid()}.jpg");

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
            encoderParameters.Param[0] = new EncoderParameter(Encoder.Quality, 100L);
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
