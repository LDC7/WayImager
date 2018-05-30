// ClassLibrary1.h

#pragma once


#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <set>
#include <string>
#include "Blob.h"
#include "PointsInfo.h"
#include "TrackedPoint.h"

#define PTYPE_MAX 1
#define PTYPE_MIN 2

namespace RouteAnalyzer
{
	using namespace System;
	using namespace std;
	using namespace cv;

	public ref class Filter
	{
	private:
		System::String ^inStr;
		System::String ^outStr;

		double ZonePercent = 100;
		double CorrWindowSize = 15;
		double MinCorrValue = 0.3;
		double MinMaxWindowSize = 15;
		double WavePercent = 20;
		double BlobSquare1Px = 0;
		double BlobSquare2Px = 20;
		double NeighbourRadius = 4;
		double MaxTimeDead = 10;
		int MapWinSize = 61;
		double DispWinSize = 9;

	public:
		void Process()
		{
			string inPath, outPath;

			MarshalString(inStr, inPath);
			MarshalString(outStr, outPath);

			Mat Image = imread(inPath, IMREAD_COLOR);
			Mat &inImg = Image;

			cvtColor(inImg, inImg, CV_BGR2GRAY);

			PointsInfo pointsInfo;
			Mat points;

			GettingPoints(inImg, outPath, points, pointsInfo);

			Mat pointsMap;

			GettingPointsMap(points, pointsMap, outPath);

			Mat dispMap;

			GettingDispersion(inImg, dispMap, outPath);

			Unite(Image, pointsMap, dispMap, inImg, outPath);
		}

		void SetPaths(System::String ^inStr, System::String ^outStr)
		{
			this->inStr = inStr;
			this->outStr = outStr;
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

		void GettingPoints(Mat &inImg, string outPath, Mat& points, PointsInfo &pointsInfo)
		{
			Mat ip, ipp, corrImg;
			Mat outImg = Mat::zeros(points.rows, points.cols, CV_8UC3);
			Mat outImg2 = Mat::zeros(points.rows, points.cols, CV_8U);
			Scalar scalar = Scalar(255, 255, 255);
			vector<Blob> blobs;
			uchar *rowuc;
			int zoneTopBorder;
			float *rowf;
			double *res_row;
			double *corrImg_row;

			IntegralImage(inImg, ip, ipp);
			Correlation(ip, ipp, corrImg);

			Mat m = Mat::zeros(corrImg.size(), CV_64F);
			for (int y = 0; y < corrImg.rows; y++)
			{
				res_row = m.ptr<double>(y);
				corrImg_row = m.ptr<double>(y);
				for (int x = 0; x < corrImg.cols; x++)
				{
					if (corrImg_row[x] > 0)
					{
						res_row[x] = 255;
						printf("!");
					}
				}
			}

			// Получение точек
			ProcessMinMax(corrImg, points);
			GetBlobs(points, corrImg, blobs, pointsInfo);
			FilterBlobs(blobs, pointsInfo);
			zoneTopBorder = inImg.rows * ZonePercent / 100;
			CheckTrackedPoints(0, pointsInfo, zoneTopBorder);

			points = Mat::zeros(points.rows, points.cols, CV_8UC3);
			for (const auto blob : pointsInfo.Blobs)
			{
				circle(points, blob.ExtremumPoint, 2, scalar, -1);
			}
			imwrite(outPath + "points.bmp", points);

			for (const auto blob : pointsInfo.Blobs)
			{
				circle(outImg, blob.ExtremumPoint, 3, scalar, -1);
			}
			imwrite(outPath + "pointsC.bmp", outImg);

			outImg = Mat::zeros(points.rows, points.cols, CV_32F);
			for (const auto blob : pointsInfo.Blobs)
			{
				rowf = outImg.ptr<float>(blob.ExtremumPoint.y);
				rowf[blob.ExtremumPoint.x] = abs(blob.ExtremumValue);
			}
			points = outImg;
			imwrite(outPath + "points2.bmp", points);

			for (const auto blob : pointsInfo.Blobs)
			{
				rowuc = outImg2.ptr<uchar>(blob.ExtremumPoint.y);
				rowuc[blob.ExtremumPoint.x] = abs(blob.ExtremumValue) * 512;
			}
			imwrite(outPath + "outImg2.bmp", outImg2);
		}

		void IntegralImage(const Mat &input, Mat &ip, Mat &ipp)
		{
			integral(input, ip, ipp, CV_64F);
		}

		void Correlation(const Mat &ip, const Mat &ipp, Mat &res)
		{
			res = Mat::zeros(ip.size(), CV_64F);
			int window_size = CorrWindowSize;
			int w = window_size / 2;
			int v = static_cast<int>(round(window_size * 0.45)) / 2;
			int vv = v * 2 + 1;

			int nt = window_size * window_size;
			int e = vv * vv;
			int ee = e;
			double ze = sqrt(ee - e * e / nt);

			for (int y = w + 1; y <= ip.rows - 1 - w - 1; y++)
			{
				double *res_row = res.ptr<double>(y);

				const double *ip_row_y_plus_w = ip.ptr<double>(y + w);
				const double *ip_row_y_minus_w = ip.ptr<double>(y - w);
				const double *ip_row_y_minus_w_1 = ip.ptr<double>(y - w - 1);

				const double *ip_row_y_plus_v = ip.ptr<double>(y + v);
				const double *ip_row_y_minus_v = ip.ptr<double>(y - v);
				const double *ip_row_y_minus_v_1 = ip.ptr<double>(y - v - 1);

				const double *ipp_row_y_plus_w = ipp.ptr<double>(y + w);
				const double *ipp_row_y_minus_w = ipp.ptr<double>(y - w);
				const double *ipp_row_y_minus_w_1 = ipp.ptr<double>(y - w - 1);

				for (int x = w + 1; x <= ip.cols - 1 - w - 1; x++)
				{
					double r = ip_row_y_plus_w[x + w]
						+ ip_row_y_minus_w_1[x - w - 1]
						- ip_row_y_minus_w_1[x + w]
						- ip_row_y_plus_w[x - w - 1];
					double rr = ipp_row_y_plus_w[x + w]
						+ ipp_row_y_minus_w_1[x - w - 1]
						- ipp_row_y_minus_w_1[x + w]
						- ipp_row_y_plus_w[x - w - 1];
					double er = ip_row_y_plus_v[x + v]
						+ ip_row_y_minus_v_1[x - v - 1]
						- ip_row_y_minus_v_1[x + v]
						- ip_row_y_plus_v[x - v - 1];
					double chi = er - e * r / nt;
					double bb = abs(chi) < 1e-5 ? 0 : chi / (ze * sqrt(rr - r * r / nt));
					if (abs(bb) > MinCorrValue) res_row[x] = bb;
				}
			}
		}

		void ProcessMinMax(Mat &corrImg, Mat &pointsMap)
		{
			pointsMap = Mat::zeros(corrImg.size(), CV_8U);

			Point2i bias = Point2i(0, 0);

			int windowSize = MinMaxWindowSize;
			assert(windowSize % 2 != 0);
			int nr = (windowSize - 1) / 2;

			for (int y = nr; y < corrImg.rows - nr; y++)
			{
				const double* corr_img_row = corrImg.ptr<double>(y);
				uchar* points_map_row = pointsMap.ptr<uchar>(y + bias.y);

				for (int x = nr; x < corrImg.cols - nr; x++)
				{
					double diff_val = corr_img_row[x];
					double max_val = diff_val;
					double min_val = diff_val;
					int max_count = 0;
					int min_count = 0;

					for (int wy = -nr; wy <= nr; wy++)
					{
						const double* window_img_row = corrImg.ptr<double>(y + wy);

						for (int wx = -nr; wx <= nr; wx++)
						{
							double window_value = window_img_row[x + wx];

							if (window_value > max_val)
							{
								max_val = window_value;
							}

							if (window_value < min_val)
							{
								min_val = window_value;
							}
						}
					}

					for (int wy = -nr; wy <= nr; wy++)
					{
						const double* window_img_row = corrImg.ptr<double>(y + wy);

						for (int wx = -nr; wx <= nr; wx++)
						{
							double window_value = window_img_row[x + wx];

							if (window_value == max_val)
							{
								max_count++;
							}

							if (window_value == min_val)
							{
								min_count++;
							}
						}
					}

					int type = 0;

					if (diff_val == min_val && min_count == 1)
					{
						type = PTYPE_MIN;
					}

					if (diff_val == max_val && max_count == 1)
					{
						type = PTYPE_MAX;
					}

					if (type)
					{
						points_map_row[x + bias.x] = type;
					}
				}
			}
		}

		void GetBlobs(Mat &pointsMap, Mat &corrImg, vector<Blob> &blobs, PointsInfo &info)
		{
			Mat maxBlobsMap = Mat::zeros(pointsMap.rows, pointsMap.cols, CV_32SC1);
			Mat minBlobsMap = Mat::zeros(pointsMap.rows, pointsMap.cols, CV_32SC1);
			int blobIndex = 1;

			for (int y = 0; y < pointsMap.rows; y++)
			{
				const uchar* pointsMapRow = pointsMap.ptr<uchar>(y);
				int* maxBlobsRow = maxBlobsMap.ptr<int>(y);
				int* minBlobsRow = minBlobsMap.ptr<int>(y);
				const double* corrImgRow = corrImg.ptr<double>(y);

				for (int x = 0; x < pointsMap.cols; x++)
				{
					uchar diffVal = pointsMapRow[x];

					if (diffVal == PTYPE_MAX && maxBlobsRow[x] == 0 || diffVal == PTYPE_MIN && minBlobsRow[x] == 0)
					{
						Blob blob;
						blob.ExtremumPoint = Point2i(x, y);
						blob.ExtremumValue = corrImgRow[x];
						blob.Square = 0;
						blob.Type = diffVal;
						blob.Index = blobIndex;

						if (diffVal == PTYPE_MAX)
						{
							DetermineBlob(pointsMap, corrImg, maxBlobsMap, blob);
						}
						else
						{
							DetermineBlob(pointsMap, corrImg, minBlobsMap, blob);
						}

						blobIndex++;
						blobs.push_back(blob);
					}
				}
			}
			info.MaxBlobsMap = maxBlobsMap;
			info.MinBlobsMap = minBlobsMap;
		}

		void DetermineBlob(const Mat pointsMap, const Mat corrImg, Mat &blobsMap, Blob &blob)
		{
			int pointsCount = 0;
			vector<Point2i> pointsToVisit;
			int type = blob.Type;
			Point2i ext = blob.ExtremumPoint;

			if (ext.x >= 0 && ext.y >= 0 && ext.x < pointsMap.cols && ext.y < pointsMap.rows)
			{
				if (blobsMap.at<int>(ext) == 0)
				{
					pointsToVisit.push_back(blob.ExtremumPoint);
				}
			}

			while (pointsToVisit.size() != 0)
			{
				Point2i point = pointsToVisit.back();
				pointsToVisit.pop_back();

				if (blob.Square > 1000)
				{
					return;
				}

				blobsMap.at<int>(point) = blob.Index;
				blob.Square += 1;

				for (int i = -1; i <= 1; i++)
				{
					for (int j = -1; j <= 1; j++)
					{
						Point2i newPoint = point + Point2i(i, j);

						if (CheckBlobPoint(newPoint, pointsMap, blobsMap, pointsToVisit, blob))
						{
							if (CheckPointValue(newPoint, corrImg, blob))
							{
								pointsToVisit.push_back(newPoint);
							}
						}
					}
				}
			}
		}

		bool CheckBlobPoint(Point2i p, Mat pointsMap, Mat blobsMap, vector<Point2i> &pointsToVisit, Blob blob)
		{
			if (p.x >= 0 && p.y >= 0 && p.x < blobsMap.cols && p.y < blobsMap.rows)
			{
				if (blobsMap.at<int>(p) == 0 && find(pointsToVisit.begin(), pointsToVisit.end(), p) == pointsToVisit.end())
				{
					return true;
				}
			}

			return false;
		}

		bool CheckPointValue(Point2i p, Mat corr_img, Blob blob)
		{
			double value = corr_img.at<double>(p);
			double delta = (abs(blob.ExtremumValue) / 100) * WavePercent;
			double minCheckValue = blob.ExtremumValue + delta;
			double maxCheckValue = blob.ExtremumValue - delta;

			return blob.Type == PTYPE_MAX && value >= maxCheckValue
				|| blob.Type == PTYPE_MIN && value <= minCheckValue;
		}

		void FilterBlobs(vector<Blob> &blobs, PointsInfo &info)
		{
			// Фильтрация и вывод пятен
			for (const auto blob : blobs)
			{
				if (blob.Square >= BlobSquare1Px && blob.Square <= BlobSquare2Px)
				{
					Blob b = blob;
					info.Blobs.push_back(b);
				}
			}
		}

		void CheckTrackedPoints(int imgIndex, PointsInfo &info, int zoneTopBorder)
		{
			set<int> used_points;

			// Проход по старым пятнам

			vector<TrackedPoint> &livingPoints = info.Points;
			vector<Blob> &blobs = info.Blobs;

			auto o = livingPoints.begin();
			while (o != livingPoints.end())
			{
				int matched_i = -1;
				Point2i o_last_coord = o->Coords[o->Coords.size() - 1];

				for (int i = 0; i < blobs.size(); i++)
				{
					Blob &blob = blobs[i];
					if (blob.Type == o->Type &&
						abs(blob.ExtremumPoint.x - o_last_coord.x) <= NeighbourRadius &&
						abs(blob.ExtremumPoint.y - o_last_coord.y) <= NeighbourRadius &&
						/*blob.ExtremumPoint.y >= o_last_coord.y &&*/	// запрет сопоставления с точками, которые стали выше
						used_points.find(i) == used_points.end())
					{
						matched_i = i;
						break;
					}
				}
				if (matched_i < 0)
				{
					if (o->TimeDead > MaxTimeDead)
					{
						info.SumLifeTime += o->LifeTime;
						info.NumberOfDeadPoints++;
						o = livingPoints.erase(o);
					}
					else
					{
						o->TimeDead++;
					}

					//VAR1
					//o = livingPoints.erase(o);
					//END VAR1
				}
				else
				{
					//VAR2
					o->TimeDead = 0;
					//END VAR2

					o->LifeTime++;
					o->Coords.push_back(blobs[matched_i].ExtremumPoint);
					used_points.insert(matched_i);
					o++;
				}
			}

			// Проход по новым пятнам
			for (int i = 0; i < blobs.size(); i++)
			{
				Blob &blob = blobs[i];
				if (blob.ExtremumPoint.y < zoneTopBorder && used_points.find(i) == used_points.end())
				{
					TrackedPoint tp;
					tp.LifeTime = 1;
					tp.Type = blob.Type;
					tp.Coords.push_back(blob.ExtremumPoint);
					tp.TimeBorn = imgIndex;
					tp.TimeDead = 0;
					livingPoints.push_back(tp);
				}
			}
		}

		void GettingPointsMap(Mat &points, Mat &pointsMap3, string outPath)
		{
			Mat pointsMap = Mat::zeros(points.rows, points.cols, CV_32F);
			pointsMap3 = Mat::zeros(points.rows, points.cols, CV_8U);
			uchar *row_disp;
			uchar temp;
			float mV = 0;
			float *row;
			float sum;
			float *row_check;

			for (int y = MapWinSize / 2; y < points.rows - MapWinSize / 2; y++)
			{
				row = pointsMap.ptr<float>(y);
				for (int x = MapWinSize / 2; x < points.cols - MapWinSize / 2; x++)
				{
					sum = 0;
					for (int a = -MapWinSize / 2; a <= MapWinSize / 2; a++)
					{
						row_check = points.ptr<float>(y + a);
						for (int b = -MapWinSize / 2; b <= MapWinSize / 2; b++)
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
			for (int y = MapWinSize / 2; y < pointsMap3.rows - MapWinSize / 2; y++)
			{
				row_disp = pointsMap3.ptr<uchar>(y);
				row = pointsMap.ptr<float>(y);
				for (int x = MapWinSize / 2; x < pointsMap3.cols - MapWinSize / 2; x++)
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
			imwrite(outPath + "pointsMap.bmp", pointsMap3);
		}

		void GettingDispersion(Mat &inImg, Mat &disp, string outPath)
		{
			disp = Mat::zeros(inImg.rows, inImg.cols, CV_8U);
			uchar *row;
			uchar *row_check;
			uchar *row_disp;
			int dx;
			int ddx;
			int maxV = 255 * DispWinSize * DispWinSize - 255;
			int mV = 0;
			int raz;

			for (int y = DispWinSize / 2; y < inImg.rows - DispWinSize / 2; y++)
			{
				row = inImg.ptr<uchar>(y);
				row_disp = disp.ptr<uchar>(y);
				for (int x = DispWinSize / 2; x < inImg.cols - DispWinSize / 2; x++)
				{
					raz = 0;
					for (int a = -DispWinSize / 2; a <= DispWinSize / 2; a++)
					{
						row_check = inImg.ptr<uchar>(y + a);

						for (int b = -DispWinSize / 2; b <= DispWinSize / 2; b++)
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
			for (int y = DispWinSize / 2; y < inImg.rows - DispWinSize / 2; y++)
			{
				row_disp = disp.ptr<uchar>(y);
				for (int x = DispWinSize / 2; x < inImg.cols - DispWinSize / 2; x++)
				{
					dx = maxV / 255;
					ddx = mV / 255;
					row_disp[x] = row_disp[x] * dx / ddx;
				}
			}
			imwrite(outPath + "disp.bmp", disp);
		}

		void Unite(Mat &Image, Mat &pointsMap3, Mat &disp, Mat &inImg, string outPath)
		{
			Mat points = Mat::zeros(Image.rows, Image.cols, CV_8U);
			Mat outIm = Mat::zeros(disp.rows, disp.cols, CV_8U);
			Mat outIm2 = Mat::zeros(disp.rows, disp.cols, CV_8U);
			uchar *row;
			uchar *row_disp;
			uchar *out_row;
			uchar *out_row2;
			double coof1 = 5, coof2 = 15;
			double dx = (sqrt(coof1*coof1 * 255 * 255 + coof2 *coof2 * 255 * 255) / sqrt(coof1*coof1 + coof2*coof2)) / 255.0;
			double znam = sqrt(coof1*coof1 + coof2*coof2);
			double temp;
			double mV = 0;
			double maxV = 0;

			resize(pointsMap3, points, Size(Image.cols, Image.rows));
			for (int y = 0; y < disp.rows; y++)
			{
				row = points.ptr<uchar>(y);
				row_disp = disp.ptr<uchar>(y);
				out_row = outIm.ptr<uchar>(y);
				out_row2 = outIm2.ptr<uchar>(y);
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

			for (int y = 0; y < outIm.rows; y++)
			{
				out_row = outIm.ptr<uchar>(y);
				for (int x = 0; x < inImg.cols; x++)
				{
					int dx = maxV / 255;
					int ddx = mV / 255;
					out_row[x] = out_row[x] * dx / ddx;
					if (out_row[x] < 60)
					{
						out_row[x] = 0;
					}
				}
			}
			imwrite(outPath + "out.bmp", outIm);
			imwrite(outPath + "out2.bmp", outIm2);
		}


	};
}
