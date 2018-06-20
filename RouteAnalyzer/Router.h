#pragma once
#include "Filter.h"
#include "Route.h"
namespace RouteAnalyzer
{
	using namespace System;
	public ref class Router
	{
	public:
		Filter ^filter;
		Mat *sum;
		double diag;
		Router(Filter ^filt)
		{
			filter = filt;
			Mat s;
			integral(*filter->outBufMat, s, CV_64F);
			sum = new Mat(s);
			diag = sqrt(filter->options->CameraHeight*filter->options->CameraHeight + filter->options->CameraWidth*filter->options->CameraWidth);
		}
		void CreateRouteValue(Route^ route)
		{
			for (int i = 0; i < route->Parts->Count; i++)
			{
				GetPartValue(route->Parts[i]);
				if (route->min > route->Parts[i]->min)
				{
					route->min = route->Parts[i]->min;
				}
				if (route->max < route->Parts[i]->max)
				{
					route->max = route->Parts[i]->max;
				}
			}
		}
	private:
		void GetPartValue(RoutePart^ rp)
		{			
			rp->IntermidiatePoints = gcnew List<ImgPoint^>();
			double lenY = rp->pointStart->y - rp->pointEnd->y;
			double lenX = rp->pointStart->x - rp->pointEnd->x;
			rp->pointStart->angle = gcnew Angle((float)(atan2(lenY, lenX) * (180 / PI)) + 90);			
			ImgPoint^ now = gcnew ImgPoint(rp->pointStart);
			double len = sqrt(lenX*lenX + lenY*lenY);
			while (len > 2 * diag)
			{
				lenY = cos(now->angle->ToRad()) * diag * 1.5;
				lenX = cos(now->angle->ToRad() + (PI / 2)) * diag * 1.5;
				now->y += lenY;
				now->x += lenX;
				now->value = GetPointValue(now);
				rp->SetMinMax(now->value);
				rp->IntermidiatePoints->Add(now);
				now = gcnew ImgPoint(now);
				lenY = now->y - rp->pointEnd->y;
				lenX = now->x - rp->pointEnd->x;
				now->angle = gcnew Angle((float)(atan2(lenY, lenX) * (180 / PI)) + 90);
				len = sqrt(lenX*lenX + lenY*lenY);
			}
			rp->pointEnd->angle = now->angle;
			rp->pointStart->value = GetPointValue(rp->pointStart);
			rp->SetMinMax(rp->pointStart->value);
			rp->pointEnd->value = GetPointValue(rp->pointEnd);
			rp->SetMinMax(rp->pointEnd->value);
		}
		int GetPointValue(ImgPoint^ p)
		{
			int x1 = p->x - diag*0.6 + 1;
			int x2 = p->x + diag*0.6 + 1;
			int y1 = p->y - diag*0.6 + 1;
			int y2 = p->y + diag*0.6 + 1;
			if (x1 < 0 || y1 < 0 || x2 > filter->outBufMat->cols || y2 > filter->outBufMat->rows)
			{
				return 0;
			}
			double a = (sum->ptr<double>(y1))[x1];
			double b = (sum->ptr<double>(y1))[x2];
			double c = (sum->ptr<double>(y2))[x1];
			double d = (sum->ptr<double>(y2))[x2];
			a = a + d - b - c;
			a = a / ((x2 - x1)*(y2 - y1));
			return a > 255 ? 255 : a;
		}
	public:
		List<Route^>^ AlternativePaths(Route^ route, int minVal)
		{
			List<List<ImgPoint^>^>^ temp;
			List<List<ImgPoint^>^>^ temp2;
			List<ImgPoint^>^temp3 = gcnew List<ImgPoint^>();
			List<List<ImgPoint^>^>^ buf = gcnew List<List<ImgPoint^>^>();
			temp3->Add(route->Parts[0]->pointStart);
			buf->Add(temp3);
			for (int i = 0; i < route->Parts->Count; i++)
			{
				for (int j = 0; j < buf->Count; j++)
				{
					buf[j]->Add(route->Parts[i]->pointStart);
				}
				if (route->Parts[i]->min < minVal)
				{
					temp = SearchAltRoutePart(route->Parts[i], minVal);
					if (temp->Count == 0)
					{
						return gcnew List<Route^>();
					}
					temp2 = gcnew List<List<ImgPoint^>^>(temp->Count * buf->Count);
					for (int j = 0; j < buf->Count; j++)
					{
						for (int k = 0; k < temp->Count; k++)
						{
							temp3 = gcnew List<ImgPoint^>(buf[j]);
							temp3->AddRange(temp[k]);
							temp2->Add(temp3);
						}
					}
					buf = temp2;
				}
				for (int j = 0; j < buf->Count; j++)
				{
					buf[j]->Add(route->Parts[i]->pointEnd);
				}
			}
			List<Route^>^ list = gcnew List<Route^>();
			Route^ r;
			for (int i = 0; i < buf->Count; i++)
			{
				r = gcnew Route();
				r->Parts->Add(gcnew RoutePart(
					gcnew ImgPoint((*buf[i])[0]->x, (*buf[i])[0]->y),
					gcnew ImgPoint((*buf[i])[1]->x, (*buf[i])[1]->y)
				));
				for (int j = 1; j < buf[i]->Count; j++)
				{
					if ((*buf[i])[j]->x != (*buf[i])[j - 1]->x || (*buf[i])[j]->y != (*buf[i])[j - 1]->y)
					{
						r->Parts->Add(gcnew RoutePart(
							gcnew ImgPoint((*buf[i])[j - 1]->x, (*buf[i])[j - 1]->y),
							gcnew ImgPoint((*buf[i])[j]->x, (*buf[i])[j]->y)
						));
					}
				}
				list->Add(r);
			}
			return list;
		}
	private:
		List<List<ImgPoint^>^>^ SearchAltRoutePart(RoutePart^ part, int mVal)
		{
			List<List<ImgPoint^>^>^ buf = gcnew List<List<ImgPoint^>^>();
			int pFromInd;
			int pToInd;
			for (int i = 0; i < part->IntermidiatePoints->Count; i++)
			{
				if (part->IntermidiatePoints[i]->value < mVal)
				{
					pFromInd = i - 1;
					break;
				}
			}
			for (int i = part->IntermidiatePoints->Count - 1; i >= 0; i--)
			{
				if (part->IntermidiatePoints[i]->value < mVal)
				{
					pToInd = i + 1;
					break;
				}
			}
			if (pFromInd < 0 || pToInd >= part->IntermidiatePoints->Count || pFromInd == pToInd)
			{
				return gcnew List<List<ImgPoint^>^>();
			}
			bool leftFlag = true;
			bool rightFlag = true;
			List<ImgPoint^>^temp;
			for (int i = pFromInd; i >= 0; i--)
			{
				if (leftFlag)
				{
					temp = SearchAltTrajectory(part->IntermidiatePoints[i], part->IntermidiatePoints[pToInd], true, mVal);
					if (temp->Count > 0)
					{
						leftFlag = false;
						buf->Add(FindControlPoints(temp));
					}
					temp = SearchAltTrajectory(part->IntermidiatePoints[i], part->pointEnd, true, mVal);
					if (temp->Count > 0)
					{
						leftFlag = false;
						buf->Add(FindControlPoints(temp));
					}
				}
				if (rightFlag)
				{
					temp = SearchAltTrajectory(part->IntermidiatePoints[i], part->IntermidiatePoints[pToInd], false, mVal);
					if (temp->Count > 0)
					{
						rightFlag = false;
						buf->Add(FindControlPoints(temp));
					}
					temp = SearchAltTrajectory(part->IntermidiatePoints[i], part->pointEnd, false, mVal);
					if (temp->Count > 0)
					{
						rightFlag = false;
						buf->Add(FindControlPoints(temp));
					}
				}
			}
			return buf;
		}
		List<ImgPoint^>^ SearchAltTrajectory(ImgPoint^ pS, ImgPoint^ pE, bool left, int mVal)
		{
			Mat* mat = filter->outBufMat;
			List<ImgPoint^>^ list = gcnew List<ImgPoint^>();
			List<ImgPoint^>^ temp;
			int x = pE->x - pS->x;
			int y = pE->y - pS->y;
			double len = sqrt(x*x + y*y);
			int rotateCoof = left ? -1 : 1;
			if (len <= 1.5 * diag)
			{
				list->Add(pS);
				list->Add(pE);
			}
			else
			{
				for (int an = 0; an <= 90; an += 30)
				{
					list->Add(pS);
					ImgPoint^ p = gcnew ImgPoint(pS);
					p->angle = gcnew Angle(p->angle->GetAngle() + rotateCoof * an);
					p->x += 1.5 * diag * cos(p->angle->ToRad());
					p->y += 1.5 * diag * cos(p->angle->ToRad() + PI / 2);
					if (p->x < 0 || p->y < 0 || p->x > mat->cols || p->y > mat->rows)
					{
						list = gcnew List<ImgPoint^>();
						continue;
					}
					p->value = GetPointValue(p);
					if (p->value < mVal)
					{
						list = gcnew List<ImgPoint^>();
						continue;
					}
					RoutePart^ rp = gcnew RoutePart(p, pE);
					GetPartValue(rp);
					if (rp->min < mVal)
					{
						temp = SearchAltTrajectory(p, pE, left, mVal);
						if (temp->Count == 0)
						{
							list = gcnew List<ImgPoint^>();
							continue;
						}
						list->AddRange(temp);
					}
					else
					{
						list->Add(rp->pointStart);
						list->AddRange(rp->IntermidiatePoints);
						list->Add(pE);
					}
					break;
				}
			}
			return list;
		}
		List<ImgPoint^>^ FindControlPoints(List<ImgPoint^>^ temp)
		{
			List<ImgPoint^>^ buf = gcnew List<ImgPoint^>();
			buf->Add(temp[0]);
			for (int i = 1; i < temp->Count - 1; i++)
			{
				if (abs(buf[buf->Count - 1]->angle->GetAngle() - temp[i]->angle->GetAngle()) > 10)
				{
					buf->Add(temp[i]);
				}
			}
			buf->Add(temp[temp->Count - 1]);
			return buf;
		}
	};
}