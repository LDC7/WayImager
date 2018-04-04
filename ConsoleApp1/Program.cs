namespace ConsoleApp1
{
    using WayImages;

    public class Program
    {
        public static void Main(string[] args)
        {
            //MyPoint pointA = new MyPoint(56.8736729m, 53.3000993m, 0);
            //MyPoint pointB = new MyPoint(56.8706826m, 53.179019m, 0);
            //MyPoint pointС = new MyPoint(56.8540261m, 53.189255m, 0);

            MyPoint pointA = new MyPoint(56.8746803m, 53.3047833m, 0);
            MyPoint pointB = new MyPoint(56.8745807m, 53.2929387m, 0);
            MyPoint pointС = new MyPoint(56.8699838m, 53.2988932m, 0);


            WayImager.Points.Add(pointA);
            WayImager.Points.Add(pointB);
            WayImager.Points.Add(pointС);
            WayImager.Points.Add(pointA);

            WayImager.Speed = 0.0004m;

            WayImager.MakeWays();
            WayImager.MakeImages();
        }
    }
}
