#pragma once
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
using namespace cv;

struct Snapshot
{
	int Index;

	std::string Filename;

	//Изображение
	Mat Image;

	//Широта
	double Lattitude;

	//Долгота
	double Longtitude;

	//Высота
	double Altitude;

	//Тангаж
	double Pitch;

	//Крен
	double Roll;

	//Рысканье
	double Yaw;

	//Угол обзора по горизонтали
	double AlphaX;

	//Угол обзора по вертикали
	double AlphaY;
};
