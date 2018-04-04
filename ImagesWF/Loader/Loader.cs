using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading.Tasks;

namespace ImagesWF.Loader
{
    public class Loader
    {
        public void Load(List<PointF> listOfPoints, int fps = 30)
        {
            StringBuilder sb = new StringBuilder();
            WebClient myWebClient = new WebClient();

            foreach (var item in listOfPoints)
            {
                sb.AppendFormat("|{0:f6},{1:f6}", item.X, item.Y);
            }
            /* | 56.510000, 53.130000
             * | 56.520000, 53.180000
             */
             
            var uri = new Uri($"http://maps.googleapis.com/maps/api/staticmap?path=color:0x0000ff|weight:5{sb.ToString()}&scale=2&format=png&size=1280x1280&maptype=satellite");
            string fileName = "image.png";

            myWebClient.DownloadFile(uri, fileName);


            //С заданным шагом между точками качаем картинки
            float shiftX;
            float shiftY;

            for (int i = 0; i < listOfPoints.Count - 1; i++)
            {
                shiftX = (listOfPoints[i + 1].X - listOfPoints[i].X) / fps;
                shiftY = (listOfPoints[i + 1].Y - listOfPoints[i].Y) / fps;

                for (int j = 0; j < fps; j++)
                {
                    int zoom = 15;

                    StringBuilder tmp = new StringBuilder();

                    tmp.Append("http://maps.googleapis.com/maps/api/staticmap?center=")
                       .AppendFormat("{0:f6} {1:f6}", listOfPoints[i].X + shiftX * j, listOfPoints[i].Y + shiftY * j)
                       .Append($"&zoom={zoom}&scale=2&format=png&size=1280x1280&maptype=satellite");
                    try
                    {
                        myWebClient.DownloadFile(tmp.ToString(), $"{i}{j}.jpg");
                    }
                    catch (WebException e)
                    {
                        //403?
                        //tmp.ToString().Replace("zoom=15", "zoom=11");
                        //myWebClient.DownloadFile(tmp.ToString(), $"{i}{j}.jpg");
                    }
                }
            }      
        }
    }
}
