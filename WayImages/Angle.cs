namespace WayImages
{
    public class Angle
    {
        private float angle;

        public Angle(float a)
        {
            Ang = a;
        }

        public float Ang
        {
            get
            {
                return angle;
            }
            set
            {
                angle = value;
                while (angle > 360)
                {
                    angle -= 360;
                }

                while (angle < 0)
                {
                    angle += 360;
                }
            }
        }

        public static implicit operator float(Angle a)
        {
            return a.Ang;
        }

        public static implicit operator Angle(float a)
        {
            return new Angle(a);
        }
    }
}
