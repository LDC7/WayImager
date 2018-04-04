namespace WayImages
{
    public class MyPoint
    {
        public decimal Latitude { get; set; }
        public decimal Longitude { get; set; }
        public int H { get; set; }

        public Angle Kren { get; set; }
        public Angle Ten { get; set; }
        public Angle Yaw { get; set; }

        public MyPoint(decimal lat, decimal lon, int h)
        {
            Latitude = lat;
            Longitude = lon;
            H = h;
        }

        public MyPoint(MyPoint mp)
        {
            Latitude = mp.Latitude;
            Longitude = mp.Longitude;
            H = mp.H;
            Kren = mp.Kren;
            Ten = mp.Ten;
            Yaw = mp.Yaw;
        }
    }
}
