namespace WayImages
{
    using System.ComponentModel;

    public class WayItem
    {
        [DisplayName("AAA")]
        public decimal lat1 { get; set; }
        public decimal lon1 { get; set; }
        public int h1 { get; set; }
        public decimal lat2 { get; set; }
        public decimal lon2 { get; set; }
        public int h2 { get; set; }
        public float kr { get; set; }
        public float tn { get; set; }
        public float dkr { get; set; }
        public float dtn { get; set; }
        public float dyw { get; set; }
    }
}
