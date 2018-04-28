#pragma once
#include "Snapshot.h"
#include "FileManager.h"
#include "Blob.h"
#include "BlobTypes.h"
#include "PointsInfo.h"
#include <vector>
#include <set>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
using namespace cv;
using namespace std;

class Filter
{
public:

	double MinCorrValue;

	double CorrWindowSize;

	double MinMaxWindowSize;

	double WavePercent;

	double BlobSquare1Px;

	double BlobSquare2Px;

	double ZonePercent;

	double NeighbourRadius;

	double MaxTimeDead;

	FileManager Manager;

	Filter() { }

	void ProcessSnapshot(Snapshot &snapshot, PointsInfo &pointsInfo)
	{
		string fileName = snapshot.Filename;
		string fileNameOnly = fileName.substr(0, fileName.rfind('.'));

		Mat &inImg = snapshot.Image;
		//resize(inImg, inImg, Size(320, 240));
		ResizeImage(inImg, 320, 240);
		Mat ip, ipp, corrImg;

		cvtColor(inImg, inImg, CV_BGR2GRAY);
		IntegralImage(inImg, ip, ipp);
		Correlation(ip, ipp, corrImg);


		Mat m = Mat::zeros(corrImg.size(), CV_64F);

		for (int y = 0; y < corrImg.rows; y++)
		{
			double *res_row = m.ptr<double>(y);
			double *corrImg_row = m.ptr<double>(y);

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
		Mat out_img;
		vector<Blob> blobs;

		Mat pointsMap;
		ProcessMinMax(corrImg, pointsMap);
		GetBlobs(pointsMap, corrImg, blobs, pointsInfo);
		FilterBlobs(blobs, pointsInfo);

		int zoneTopBorder = inImg.rows * ZonePercent / 100;
		CheckTrackedPoints(snapshot.Index, pointsInfo, zoneTopBorder);
	}

private:

	//void ConvertToGrayscale(const Mat inImage, Mat &outImage)
	//{
	//	outImage = Mat(inImage.rows, inImage.cols, CV_8U);

	//	for (int y = 0; y < inImage.rows; y++)
	//	{
	//		const Vec3b* in_row = inImage.ptr<Vec3b>(y);
	//		uchar* out_row = outImage.ptr<uchar>(y);

	//		for (int x = 0; x < inImage.cols; x++)
	//		{
	//			int B = 7 * (int)in_row[x][0];
	//			int G = 72 * (int)in_row[x][1];
	//			int R = 21 * (int)in_row[x][2];
	//			out_row[x] = (B + G + R) / 100;
	//		}
	//	}
	//}

	void ResizeImage(Mat &inImg, int width, int height)
	{
		int inWidth = inImg.cols;
		int inHeight = inImg.rows;
		double s = (double)inWidth / (double)inHeight;

		int dw = abs(inWidth - width);
		int dh = abs(inHeight - height);

		if (dw <= dh)
		{
			resize(inImg, inImg, Size(width, width / s));
		}
		else
		{
			resize(inImg, inImg, Size(height * s, height));
		}

		int x = (inImg.cols - width) / 2;
		int y = (inImg.rows - height) / 2;
		
		getRectSubPix(inImg, Size(320, 240), Point2f(inImg.cols / 2, inImg.rows / 2), inImg);

		//inImg = inImg.
		//inImg.copyTo(inImg);
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
};