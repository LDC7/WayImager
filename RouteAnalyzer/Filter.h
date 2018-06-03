// ClassLibrary1.h

#pragma once

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <string>
#include "Blob.h"
#include "PointsInfo.h"
#include "TrackedPoint.h"
#include "Options.h"
#include "IPointFinder.h"

using namespace std;
using namespace cv;

namespace RouteAnalyzer
{
	using namespace System;

	public ref class Filter
	{
	private:
		System::String ^inStr;
		System::String ^outStr;

	public:
		Options ^options;
		IPointFinder ^pointMethod;

		Mat *bufferMat;
		string *inPath, *outPath;

		void Process()
		{
			MarshalString(inStr, *inPath);
			MarshalString(outStr, *outPath);

			Mat Image = imread(*inPath, IMREAD_COLOR);
			Mat &inImg = Image;
			PointsInfo pointsInfo;
			Mat points;
			Mat pointsMap;
			Mat dispMap;

			cvtColor(inImg, inImg, CV_BGR2GRAY);

			pointMethod->GettingPoints(inImg, points, pointsInfo, *outPath);

			GettingPointsMap(points, pointsMap);

			GettingDispersion(inImg, dispMap);

			Unite(Image, pointsMap, dispMap, inImg);
		}

		void SetPaths(System::String ^inStr, System::String ^outStr)
		{
			this->inStr = inStr;
			this->outStr = outStr;
		}

		void LoadBufferMat(System::String ^path)
		{
			string Path;
			MarshalString(path, Path);
			Mat Image = imread(Path, IMREAD_GRAYSCALE);
			bufferMat = &Image;
		}

	private:
		static void MarshalString(System::String ^ s, string& os)
		{
			using namespace Runtime::InteropServices;
			const char* chars =
				(const char*)(Marshal::StringToHGlobalAnsi(s)).ToPointer();
			os = chars;
			Marshal::FreeHGlobal(IntPtr((void*)chars));
		}

		void GettingPointsMap(Mat &points, Mat &pointsMap3)
		{
			Mat pointsMap = Mat::zeros(points.rows, points.cols, CV_32F);
			pointsMap3 = Mat::zeros(points.rows, points.cols, CV_8U);
			uchar *row_disp;
			uchar temp;
			float mV = 0;
			float *row;
			float sum;
			float *row_check;

			for (int y = options->MapWinSize / 2; y < points.rows - options->MapWinSize / 2; y++)
			{
				row = pointsMap.ptr<float>(y);
				for (int x = options->MapWinSize / 2; x < points.cols - options->MapWinSize / 2; x++)
				{
					sum = 0;
					for (int a = -options->MapWinSize / 2; a <= options->MapWinSize / 2; a++)
					{
						row_check = points.ptr<float>(y + a);
						for (int b = -options->MapWinSize / 2; b <= options->MapWinSize / 2; b++)
						{
							sum += row_check[x + b];
						}
					}
					if (sum > mV)
					{
						mV = sum;
					}
					row[x] = sum;
				}
			}
			for (int y = options->MapWinSize / 2; y < pointsMap3.rows - options->MapWinSize / 2; y++)
			{
				row_disp = pointsMap3.ptr<uchar>(y);
				row = pointsMap.ptr<float>(y);
				for (int x = options->MapWinSize / 2; x < pointsMap3.cols - options->MapWinSize / 2; x++)
				{
					temp = (uchar)(row[x] * 255 / mV);
					if (temp > 130)
					{
						temp = temp * 200 / 255;
					}
					else
					{
						temp = 0;
					}
					row_disp[x] = temp;
				}
			}
			imwrite(*outPath + "pointsMap.bmp", pointsMap3);
		}

		void GettingDispersion(Mat &inImg, Mat &disp)
		{
			disp = Mat::zeros(inImg.rows, inImg.cols, CV_8U);
			uchar *row;
			uchar *row_check;
			uchar *row_disp;
			int dx;
			int ddx;
			int maxV = 255 * options->DispWinSize * options->DispWinSize - 255;
			int mV = 0;
			int raz;

			for (int y = options->DispWinSize / 2; y < inImg.rows - options->DispWinSize / 2; y++)
			{
				row = inImg.ptr<uchar>(y);
				row_disp = disp.ptr<uchar>(y);
				for (int x = options->DispWinSize / 2; x < inImg.cols - options->DispWinSize / 2; x++)
				{
					raz = 0;
					for (int a = -options->DispWinSize / 2; a <= options->DispWinSize / 2; a++)
					{
						row_check = inImg.ptr<uchar>(y + a);

						for (int b = -options->DispWinSize / 2; b <= options->DispWinSize / 2; b++)
						{
							raz += abs(row_check[x + b] - row[x]);
						}
					}
					if (raz > mV)
					{
						mV = raz;
					}
					dx = maxV / 255;
					raz /= dx;
					row_disp[x] = raz;
				}
			}
			dx = maxV / 255;
			ddx = mV / 255;
			for (int y = options->DispWinSize / 2; y < inImg.rows - options->DispWinSize / 2; y++)
			{
				row_disp = disp.ptr<uchar>(y);
				for (int x = options->DispWinSize / 2; x < inImg.cols - options->DispWinSize / 2; x++)
				{
					row_disp[x] = row_disp[x] * dx / ddx;
				}
			}
			imwrite(*outPath + "dispersion.bmp", disp);
		}

		void Unite(Mat &Image, Mat &pointsMap3, Mat &disp, Mat &inImg)
		{
			Mat points = Mat::zeros(Image.rows, Image.cols, CV_8U);
			Mat outIm = Mat::zeros(disp.rows, disp.cols, CV_8U);
			uchar *row;
			uchar *row_disp;
			uchar *out_row;
			uchar *out_row2;
			double coof1 = 5, coof2 = 15;
			double dx = (sqrt(coof1*coof1 * 255 * 255 + coof2 *coof2 * 255 * 255) / sqrt(coof1*coof1 + coof2*coof2)) / 255.0;
			double ddx;
			double znam = sqrt(coof1*coof1 + coof2*coof2);
			double temp;
			double mV = 0;

			resize(pointsMap3, points, Size(Image.cols, Image.rows));
			for (int y = 0; y < disp.rows; y++)
			{
				row = points.ptr<uchar>(y);
				row_disp = disp.ptr<uchar>(y);
				out_row = outIm.ptr<uchar>(y);
				for (int x = 0; x < disp.cols; x++)
				{
					temp = sqrt(coof1 * coof1 * row[x] * row[x] + coof2 * coof2 * row_disp[x] * row_disp[x]) / znam;
					if (temp > mV)
					{
						mV = temp;
					}
					temp /= dx;
					out_row[x] = temp;
					out_row2[x] = ((row[x] * row_disp[x] + 100) / 2) - 50;
				}
			}

			ddx = mV / 255;
			for (int y = 0; y < outIm.rows; y++)
			{
				out_row = outIm.ptr<uchar>(y);
				for (int x = 0; x < inImg.cols; x++)
				{
					out_row[x] = out_row[x] * dx / ddx;
					if (out_row[x] < 60)
					{
						out_row[x] = 0;
					}
				}
			}
			imwrite(*outPath + "informMap.bmp", outIm);
			bufferMat = &outIm;
		}


	};
}
