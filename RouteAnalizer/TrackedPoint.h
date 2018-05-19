#pragma once
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>
using namespace cv;
using namespace std;

struct TrackedPoint
{
	vector<Point2i> Coords;

	int LifeTime;

	int Type;

	int TimeBorn;

	int TimeDead;
};