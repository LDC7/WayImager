#pragma once
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <string>
#include "PointsInfo.h"
#include "Options.h"
#include "IPointFinder.h"
using namespace std;
using namespace cv;
#using <System.Drawing.dll>
using namespace System::Drawing;
namespace RouteAnalyzer
{
	using namespace System;
	public ref class Filter
	{
	private:
		System::String ^inStr;
		System::String ^outStr;
		string *inPath, *outPath;
	public:
		Mat *outBufMat;
		Mat *inpBufMat;
		Options ^options = gcnew Options;
		IPointFinder ^pointMethod;
		bool HasOutMat = false;
		bool HasInpMat = false;
		void Process()
		{
			inPath = new string;
			outPath = new string;
			MarshalString(inStr, *inPath);
			MarshalString(outStr, *outPath);
			Mat Image = imread(*inPath, IMREAD_COLOR);
			Mat &inImg = Image;			
			PointsInfo pointsInfo;
			Mat points;
			Mat pointsMap;
			Mat dispMap;
			cvtColor(inImg, inImg, CV_BGR2GRAY);
			inpBufMat = new Mat(inImg);
			HasInpMat = true;
			pointMethod->GettingPoints(inImg, points, pointsInfo, *outPath);
			GettingPointsMap(points, pointsMap);
			GettingDispersion(inImg, dispMap);
			Unite(pointsMap, dispMap);
			delete inPath;
			delete outPath;
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
			outBufMat = new Mat(Image);
			HasOutMat = true;
		}
		void GetInpImg(Bitmap^% bmp)
		{
			bmp = cvMatToSharpBitmap(inpBufMat);
		}
		void GetInfMap(Bitmap^% bmp)
		{
			bmp = cvMatToSharpBitmap(outBufMat);
		}
	private:
		static Bitmap^ cvMatToSharpBitmap(Mat *mat)
		{
			Bitmap ^bmp = gcnew Bitmap(mat->cols, mat->rows);
			uchar *rowuc;
			for (int y = 0; y < mat->rows; y++)
			{
				rowuc = mat->ptr<uchar>(y);
				for (int x = 0; x < mat->cols; x++)
				{
					bmp->SetPixel(x, y, Color::FromArgb(rowuc[x], rowuc[x], rowuc[x]));
				}
			}
			return bmp;
		}
		static void MarshalString(System::String ^ s, string& os)
		{
			using namespace Runtime::InteropServices;
			const char* chars =
				(const char*)(Marshal::StringToHGlobalAnsi(s)).ToPointer();
			os = chars;
			Marshal::FreeHGlobal(IntPtr((void*)chars));
		}
		void GettingPointsMap(Mat &points, Mat &pointsMap)
		{
			pointsMap = Mat::zeros(points.rows, points.cols, CV_32F);
			Mat MapImg = Mat::zeros(points.rows, points.cols, CV_8U);
			uchar *rowuc;
			uchar temp;
			float mV = 0;
			float *rowf;
			float *rowf2;
			float sum;
			float max = options->MapWinSize*options->MapWinSize;
			for (int y = options->MapWinSize / 2; y < points.rows - options->MapWinSize / 2; y++)
			{
				rowf = pointsMap.ptr<float>(y);
				for (int x = options->MapWinSize / 2; x < points.cols - options->MapWinSize / 2; x++)
				{
					sum = 0;
					for (int a = -options->MapWinSize / 2; a <= options->MapWinSize / 2; a++)
					{
						rowf2 = points.ptr<float>(y + a);
						for (int b = -options->MapWinSize / 2; b <= options->MapWinSize / 2; b++)
						{
							sum += rowf2[x + b];
						}
					}
					if (sum > mV)
					{
						mV = sum;
					}
					rowf[x] = sum;
				}
			}
			for (int y = options->MapWinSize / 2; y < MapImg.rows - options->MapWinSize / 2; y++)
			{
				rowuc = MapImg.ptr<uchar>(y);
				rowf = pointsMap.ptr<float>(y);
				for (int x = options->MapWinSize / 2; x < MapImg.cols - options->MapWinSize / 2; x++)
				{
					temp = (uchar)(rowf[x] * 255 / mV);
					if (temp > options->MinLvlForDensityMap)
					{
						temp = temp * options->MaxLvlForDensityMap / 255;
					}
					else
					{
						temp = 0;
					}
					rowuc[x] = temp;
					rowf[x] /= max;
				}
			}
			imwrite(*outPath + "pointsMap.bmp", MapImg);
		}
		void GettingDispersion(Mat &inImg, Mat &disp)
		{
			disp = Mat::zeros(inImg.rows, inImg.cols, CV_32F);
			Mat DispImg = Mat::zeros(inImg.rows, inImg.cols, CV_8U);
			uchar *rowu;
			uchar *rowu2;
			float *rowf;
			int maxV = 255 * options->DispWinSize * options->DispWinSize - 255;
			float dx = ((float)maxV) / 255.0f;
			int mV = 0;
			int raz;
			float ddx;
			for (int y = options->DispWinSize / 2; y < inImg.rows - options->DispWinSize / 2; y++)
			{
				rowu = inImg.ptr<uchar>(y);
				rowf = disp.ptr<float>(y);
				for (int x = options->DispWinSize / 2; x < inImg.cols - options->DispWinSize / 2; x++)
				{
					raz = 0;
					for (int a = -options->DispWinSize / 2; a <= options->DispWinSize / 2; a++)
					{
						rowu2 = inImg.ptr<uchar>(y + a);
						for (int b = -options->DispWinSize / 2; b <= options->DispWinSize / 2; b++)
						{
							raz += abs(rowu2[x + b] - rowu[x]);
						}
					}
					if (raz > mV)
					{
						mV = raz;
					}
					rowf[x] = ((float)raz) / dx;
				}
			}
			ddx = ((float)mV) / dx;
			for (int y = options->DispWinSize / 2; y < inImg.rows - options->DispWinSize / 2; y++)
			{
				rowu = DispImg.ptr<uchar>(y);
				rowf = disp.ptr<float>(y);
				for (int x = options->DispWinSize / 2; x < inImg.cols - options->DispWinSize / 2; x++)
				{
					rowu[x] = (rowf[x] * 255) / ddx;
				}
			}
			imwrite(*outPath + "dispersion.bmp", DispImg);
		}
		void Unite(Mat &pointsMap, Mat &disp)
		{
			Mat infMap = Mat::zeros(disp.rows, disp.cols, CV_32F);
			Mat outImg = Mat::zeros(disp.rows, disp.cols, CV_8U);
			uchar *rowu;
			float *rowf;
			float *rowf2;
			float *rowf3;
			double coof1 = options->CoefficientForDensity;
			double coof2 = options->CoefficientForDispersion;
			double dx = (sqrt(coof1*coof1 + coof2 *coof2) / sqrt(coof1*coof1 + coof2*coof2));
			double ddx;
			double znam = sqrt(coof1*coof1 + coof2*coof2);
			double temp;
			double mV = 0;
			for (int y = 0; y < disp.rows; y++)
			{
				rowf = pointsMap.ptr<float>(y);
				rowf2 = disp.ptr<float>(y);
				rowf3 = infMap.ptr<float>(y);
				for (int x = 0; x < disp.cols; x++)
				{
					temp = sqrt(coof1 * coof1 * rowf[x] * rowf[x] + coof2 * coof2 * rowf2[x] * rowf2[x]) / znam;
					if (temp > mV)
					{
						mV = temp;
					}
					temp /= dx;
					rowf3[x] = temp;
				}
			}
			ddx = mV / dx;
			for (int y = 0; y < outImg.rows; y++)
			{
				rowu = outImg.ptr<uchar>(y);
				rowf = infMap.ptr<float>(y);
				for (int x = 0; x < outImg.cols; x++)
				{
					temp = (rowf[x] * 255) / ddx;
					if (temp >= options->MinLimitForInfMap)
					{
						rowu[x] = temp;
					}
				}
			}
			imwrite(*outPath + "informMap.bmp", outImg);
			outBufMat = new Mat(outImg);
			HasOutMat = true;
		}
	};
}