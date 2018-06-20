#pragma once
#include <opencv2/highgui.hpp>
using namespace cv;
struct Blob
{
	int Square;
	Point2i ExtremumPoint;
	double ExtremumValue;
	int Type;
	int Index;
};