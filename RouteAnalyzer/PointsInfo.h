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
	//Найденные пятна
	vector <Blob> Blobs;

	//Карта пятен-максимумов
	Mat MaxBlobsMap;

	//Карта пятен минимумов
	Mat MinBlobsMap;

	//Отслеживаемые точки
	vector<TrackedPoint> Points;

	//Пройденный путь
	vector<Point2f> Track;

	//Суммарное время жизни потерянных точек
	double SumLifeTime;

	//Количество потерянных точек
	double NumberOfDeadPoints;
};