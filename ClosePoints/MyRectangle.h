#pragma once

#include <math.h>
#include <opencv2/highgui.hpp>
#include "TrackedPoint.h"
using namespace cv;

class MyRectangle
{
public:

	int indexes[4];
	Point2f points[4];

	MyRectangle()
	{

	}

	MyRectangle(int index0, int index1, int index2, int index3, vector<TrackedPoint> tracked_points, int n)
	{
		indexes[0] = index0;
		indexes[1] = index1;
		indexes[2] = index2;
		indexes[3] = index3;

		for (int i = 0; i < 4; i++)
		{
			for (int j = 1; j < 4; j++)
			{
				Point2i p1 = tracked_points[indexes[j]].Coords[tracked_points[indexes[j]].Coords.size() - 1];
				Point2i p2 = tracked_points[indexes[j - 1]].Coords[tracked_points[indexes[j - 1]].Coords.size() - 1];

				if (p1.y <p2.y)
				{
					int temp = indexes[j];
					indexes[j] = indexes[j - 1];
					indexes[j - 1] = temp;
				}
			}
		}

		Point2i p1 = tracked_points[indexes[1]].Coords[tracked_points[indexes[1]].Coords.size() - 1];
		Point2i p2 = tracked_points[indexes[0]].Coords[tracked_points[indexes[0]].Coords.size() - 1];

		if (p1.x < p2.x)
		{
			int temp = indexes[1];
			indexes[1] = indexes[0];
			indexes[0] = temp;
		}

		p1 = tracked_points[indexes[2]].Coords[tracked_points[indexes[2]].Coords.size() - 1];
		p2 = tracked_points[indexes[3]].Coords[tracked_points[indexes[3]].Coords.size() - 1];

		if (p1.x < p2.x)
		{
			int temp = indexes[2];
			indexes[2] = indexes[3];
			indexes[3] = temp;
		}

		for (int i = 0; i < 4; i++)
		{
			points[i] = tracked_points[indexes[i]].Coords[tracked_points[indexes[i]].Coords.size() - n];
		}

		this->n = n;
		area = GetArea();
	}

	double Area()
	{
		return area;
	}

	Point2i GetPoint(int index)
	{
		return points[index];
	}

	MyRectangle GetOldRectangle(vector<TrackedPoint> points, int correction_time)
	{
		return MyRectangle(indexes[0], indexes[1], indexes[2], indexes[3], points, correction_time);
	}

	bool operator > (const MyRectangle& rect) const
	{
		return (area > rect.area);
	}

	Point2f * GetPointsArray()
	{
		return points;
	}

private:

	int n;

	double area;

	double GetArea()
	{
		double a = GetSideLength(points[0], points[1]);
		double b = GetSideLength(points[1], points[2]);
		double c = GetSideLength(points[2], points[3]);
		double d = GetSideLength(points[0], points[3]);
		double e = GetSideLength(points[0], points[2]);

		double p_1 = (a + b + e) / 2;
		double a_1 = sqrt(p_1 * (p_1 - a) * (p_1 - b) * (p_1 - e));
		double p_2 = (c + d + e) / 2;
		double a_2 = sqrt(p_2 * (p_2 - c) * (p_2 - d) * (p_2 - e));
		return a_1 + a_2;
	}

	double GetSideLength(Point2i p1, Point2i p2)
	{
		return sqrt((p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y));
	}
};