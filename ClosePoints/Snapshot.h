#pragma once
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
using namespace cv;

struct Snapshot
{
	int Index;

	std::string Filename;

	//�����������
	Mat Image;

	//������
	double Lattitude;

	//�������
	double Longtitude;

	//������
	double Altitude;

	//������
	double Pitch;

	//����
	double Roll;

	//��������
	double Yaw;

	//���� ������ �� �����������
	double AlphaX;

	//���� ������ �� ���������
	double AlphaY;
};
