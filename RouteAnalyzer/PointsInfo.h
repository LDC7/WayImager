#pragma once
#include "Blob.h"
#include "TrackedPoint.h"
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>
using namespace cv;
using namespace std;
struct PointsInfo
{
	vector <Blob> Blobs;
	Mat MaxBlobsMap;
	Mat MinBlobsMap;
	vector<TrackedPoint> Points;
	vector<Point2f> Track;
	double SumLifeTime;
	double NumberOfDeadPoints;
};