namespace WayImages
{
    using System.Collections.Generic;

    public class Way
    {
        public List<MyPoint> Points { get; private set; }

        public Way()
        {
            Points = new List<MyPoint>();
        }
    }
}
