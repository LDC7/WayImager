namespace WayImages
{
    public class MyPoint
    {
        public decimal Latitude { get; set; }
        public decimal Longitude { get; set; }
        public int H { get; set; }

        private Angle kren;
        private Angle ten;
        private Angle yaw;        

        public MyPoint()
        {
            Kren = new Angle(0);
            Ten = new Angle(0);
            Yaw = new Angle(0);
        }

        public MyPoint(decimal lat, decimal lon, int h)
        {
            Latitude = lat;
            Longitude = lon;
            H = h;
            Kren = new Angle(0);
            Ten = new Angle(0);
            Yaw = new Angle(0);
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

        //нужен для работы датагрида
        public float Kren
        {
            get { return kren; }
            set { kren = value; }
        }

        //нужен для работы датагрида
        public float Ten
        {
            get { return ten; }
            set { ten = value; }
        }

        //нужен для работы датагрида
        public float Yaw
        {
            get { return yaw; }
            set { yaw = value; }
        }
    }
}
