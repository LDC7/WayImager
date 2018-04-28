#pragma once
#include "TrackedPoint.h"
#include "Snapshot.h"
#include "FileManager.h"
#include "Blob.h"
#include "BlobTypes.h"
#include "PointsInfo.h"
#include "MyRectangle.h"
#include "Combinatorics.h"
#include <vector>
#include <set>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
using namespace cv;
using namespace std;

class Drawer
{
private:

	Vec3b MaxPointColor;
	Vec3b MinPointColor;
	Vec3b MaxBlobColor;
	Vec3b MinBlobColor;
	Scalar PathColor;
	Scalar MassCenterColor;
	Scalar LineColor;
	Scalar NewLineColor;
	Scalar DeadLineColor;

public:

	int PointZonePercent;
	int BestTrianglesCount;

	Drawer()
	{
		MaxPointColor = Vec3b(0, 255, 0);
		MinPointColor = Vec3b(0, 128, 255);
		MaxBlobColor = Vec3b(0, 128, 0);
		MinBlobColor = Vec3b(3, 3, 156);
		PathColor = Scalar(255, 0, 0);
		MassCenterColor = Scalar(0, 255, 255);
		LineColor = Scalar(0, 255, 255);
		NewLineColor = Scalar(255, 0, 0);
		DeadLineColor = Scalar(0, 0, 255);
	}

	void DrawBlobs(PointsInfo &info, Mat &outImg)
	{
		for (const auto blob : info.Blobs)
		{
			Blob b = blob;
			// Нарисовать
			int point_count = 0;

			if (blob.Type == PTYPE_MAX)
			{
				DrawBlob(b, b.ExtremumPoint, info.MaxBlobsMap, outImg);
			}
			else
			{
				DrawBlob(b, b.ExtremumPoint, info.MinBlobsMap, outImg);
			}
		}
	}

	void DrawLines(PointsInfo &info, Mat &outImg, int minLifeTime)
	{
		for (auto &o : info.Points)
		{
			if (o.LifeTime > minLifeTime)
			{
				for (int i = 1; i < o.Coords.size(); i++)
				{
					line(outImg, o.Coords[i - 1], o.Coords[i], LineColor);
				}

				if (o.TimeDead > 0)
				{
					line(outImg, o.Coords[0], o.Coords[o.Coords.size() - 1], DeadLineColor, 1);
				}
				else
				{
					line(outImg, o.Coords[0], o.Coords[o.Coords.size() - 1], NewLineColor, 1);
				}
			}
		}
	}

	void DrawPath(PointsInfo &info, Mat &outImg, Mat &fullImg, Mat oldFullImg, int correctionTime, Snapshot snapshot)
	{
		Point2d old_mass_center(0, 0);
		Point2d new_mass_center(0, 0);
		int correct_count = 0;
		vector<TrackedPoint> &livingPoints = info.Points;

		for (auto point : livingPoints)
		{
			if (point.Coords.size() > correctionTime)
			{
				correct_count++;
				old_mass_center.x += point.Coords[point.Coords.size() - correctionTime].x;
				old_mass_center.y += point.Coords[point.Coords.size() - correctionTime].y;
				new_mass_center.x += point.Coords[point.Coords.size() - 1].x;
				new_mass_center.y += point.Coords[point.Coords.size() - 1].y;
			}
		}

		if (correct_count > 0)
		{
			new_mass_center /= correct_count;
			old_mass_center /= correct_count;
			Point2f diff = new_mass_center - old_mass_center;
			circle(outImg, old_mass_center, 5, MassCenterColor, -1);
			circle(outImg, old_mass_center, 5, MassCenterColor, 1);
			info.Track.push_back(diff);
			Point2f cur_pos = old_mass_center;

			for (auto d : info.Track)
			{
				auto old_pos = cur_pos;
				cur_pos += d;
				line(outImg, old_pos, cur_pos, PathColor, 2);
				circle(outImg, cur_pos, 3, MassCenterColor, -1);
				circle(outImg, cur_pos, 3, MassCenterColor, 1);
			}

			//START

			vector<TrackedPoint> correct_points;

			int deltaPercent = 10;

			for (int i = PointZonePercent; i < 50; i += deltaPercent)
			{
				correct_points.clear();
				int zoneWidth = (outImg.cols / 100) * i;
				int zoneHeight = (outImg.rows / 100) * i;

				Rect rect = Rect(zoneWidth, zoneHeight, outImg.cols - zoneWidth * 2, outImg.rows - zoneHeight * 2);

				for (auto point : info.Points)
				{
					if (point.Coords.size() > correctionTime)
					{
						if (!rect.contains(point.Coords[point.Coords.size() - 1]))
						{
							correct_points.push_back(point);
						}
					}
				}

				if (correct_points.size() > 4)
				{
					break;
				}
			}

			int all_points_count = correct_points.size();
			vector<MyRectangle> best_rectangles;

			vector<TrackedPoint> *rect_points = GetRectanglePoints(correct_points, outImg.cols, outImg.rows);

			bool haveRectangle = FindBiggestRectangles(correct_points, best_rectangles, correctionTime, BestTrianglesCount, outImg.size());

			if (haveRectangle)
			{
				MyRectangle best_rectangle = best_rectangles[0];
				int max_correct_points_count = -1;

				for (int i = 0; i < best_rectangles.size(); i++)
				{
					int correct_points_count = 0;
					MyRectangle new_rectangle = best_rectangles[i];
					MyRectangle old_rectangle = new_rectangle.GetOldRectangle(correct_points, correctionTime);

					Point2f *old_points = old_rectangle.GetPointsArray();
					Point2f *new_points = new_rectangle.GetPointsArray();

					Mat perspectiveMatrix = getPerspectiveTransform(new_points, old_points);

					for (auto point : correct_points)
					{
						Point2f new_point = point.Coords[point.Coords.size() - 1];
						Point2i old_point = point.Coords[point.Coords.size() - correctionTime];
						TransformPoint(new_point, perspectiveMatrix);
						Point2i new_point_i = Point2i(new_point.x, new_point.y);

						for (int i = -1; i <= 1; i++)
						{
							for (int j = -1; j <= 1; j++)
							{
								if (new_point_i == (old_point + Point2i(i, j)))
								{
									correct_points_count++;
									break;
								}
							}
						}
					}

					if (correct_points_count > max_correct_points_count)
					{
						best_rectangle = best_rectangles[i];
						max_correct_points_count = correct_points_count;
					}

					printf("Points_count: %d/%d\n", correct_points_count, all_points_count);
				}

				printf("Max_count: %d/%d\n", max_correct_points_count, all_points_count);

				Mat test_matrix = Mat::zeros(2, 3, CV_64FC1);
				Mat reverse_test_matrix = Mat::zeros(2, 3, CV_64FC1);

				double altitude = snapshot.Altitude;
				double alphaX = snapshot.AlphaX;
				double alphaY = snapshot.AlphaY;

				double meters_per_pixel_x = GetMetersPerPixel(altitude, alphaX, outImg.cols);
				double meters_per_pixel_y = GetMetersPerPixel(altitude, alphaY, outImg.rows);

				Point2i delta = new_mass_center - old_mass_center;
				Point2f delta_meters = Point2f(delta.x * meters_per_pixel_x, delta.y * meters_per_pixel_y);

				double distance = sqrt(delta_meters.x * delta_meters.x + delta_meters.y * delta_meters.y);
				printf("Distance = %f m\n\n", distance);

				int correct_points_count = 0;

				Mat defaultMatrix = Mat::zeros(2, 3, CV_64FC1);
				defaultMatrix.at<double>(Point2f(0, 0)) = 1;
				defaultMatrix.at<double>(Point2f(1, 0)) = 0;
				defaultMatrix.at<double>(Point2f(2, 0)) = (500 - 160);
				defaultMatrix.at<double>(Point2f(0, 1)) = 0;
				defaultMatrix.at<double>(Point2f(1, 1)) = 1;
				defaultMatrix.at<double>(Point2f(2, 1)) = 0;

				Mat reverseDefaultMatrix = Mat::zeros(2, 3, CV_64FC1);
				reverseDefaultMatrix.at<double>(Point2f(0, 0)) = 1;
				reverseDefaultMatrix.at<double>(Point2f(1, 0)) = 0;
				reverseDefaultMatrix.at<double>(Point2f(2, 0)) = -(500 - 160);
				reverseDefaultMatrix.at<double>(Point2f(0, 1)) = 0;
				reverseDefaultMatrix.at<double>(Point2f(1, 1)) = 1;
				reverseDefaultMatrix.at<double>(Point2f(2, 1)) = 0;


				MyRectangle old_rectangle = best_rectangle.GetOldRectangle(correct_points, correctionTime);
				Point2f *old_points = old_rectangle.GetPointsArray();
				Point2f *new_points = best_rectangle.GetPointsArray();

				Mat perspectiveMatrix;
				Mat reversePerspectiveMatrix;

				if (correct_points_count > max_correct_points_count)
				{
					perspectiveMatrix = test_matrix;
					reversePerspectiveMatrix = reverse_test_matrix;
				}
				else
				{
					perspectiveMatrix = getPerspectiveTransform(new_points, old_points);
					reversePerspectiveMatrix = getPerspectiveTransform(old_points, new_points);
				}

				Mat res1 = Mat::zeros(1000, 1000, CV_16UC3);

				Size dsize = Size(1000, 1000);

				oldFullImg.copyTo(fullImg);

				if (fullImg.rows != 0)
				{
					warpAffine(fullImg, fullImg, reverseDefaultMatrix, dsize, INTER_CUBIC, BORDER_CONSTANT, Scalar::all(0));
				}

				warpPerspective(outImg, res1, perspectiveMatrix, dsize, INTER_CUBIC, BORDER_CONSTANT, Scalar::all(0));

				Mat mask = cv::Mat::zeros(dsize, CV_8U); // all 0
				mask(Rect(0, 0, outImg.cols, outImg.rows)) = 1;
				warpPerspective(mask, mask, perspectiveMatrix, dsize, INTER_CUBIC, BORDER_CONSTANT, Scalar::all(0));
				res1.copyTo(fullImg, mask);
				warpPerspective(fullImg, fullImg, reversePerspectiveMatrix, dsize, INTER_CUBIC, BORDER_CONSTANT, Scalar::all(0));

				warpAffine(fullImg, fullImg, defaultMatrix, dsize, INTER_CUBIC, BORDER_CONSTANT, Scalar::all(0));

				line(outImg, new_points[0], new_points[1], Scalar(255, 0, 255), 2);
				line(outImg, new_points[1], new_points[2], Scalar(255, 0, 255), 2);
				line(outImg, new_points[2], new_points[3], Scalar(255, 0, 255), 2);
				line(outImg, new_points[0], new_points[3], Scalar(255, 0, 255), 2);
				circle(outImg, new_points[0], 2, Scalar(255, 255, 255), 1);
				circle(outImg, new_points[1], 2, Scalar(255, 255, 255), 1);
				circle(outImg, new_points[2], 2, Scalar(255, 255, 255), 1);
				circle(outImg, new_points[3], 2, Scalar(255, 255, 255), 1);
			}

			//imshow("old", old);
			//imshow("Track", draw_track);
			//waitKey();


			//END
		}
	}

private:

	void DrawBlob(Blob &blob, Point &p, Mat &blobsMap, Mat &outImg)
	{
		int points_count = 0;
		vector<Point2i> points_to_visit;
		vector<Point2i> visited_points;

		if (p.x >= 0 && p.y >= 0 && p.x < blobsMap.cols && p.y < blobsMap.rows)
		{
			if (blobsMap.at<int>(p) == blob.Index)
			{
				points_to_visit.push_back(p);
			}
		}

		while (points_to_visit.size() != 0)
		{
			Point2i point = points_to_visit.back();
			visited_points.push_back(point);
			points_to_visit.pop_back();
			points_count++;

			if (points_count > 1000)
			{
				return;
			}

			if (blob.Type == PTYPE_MAX)
			{
				outImg.at<Vec3b>(point) = MaxBlobColor;
			}
			else if (blob.Type == PTYPE_MIN)
			{
				outImg.at<Vec3b>(point) = MinBlobColor;
			}


			for (int i = -1; i <= 1; i++)
			{
				for (int j = -1; j <= 1; j++)
				{
					if (blobsMap.at<int>(point + Point2i(i, j)) == blob.Index
						&& find(points_to_visit.begin(), points_to_visit.end(), point + Point2i(i, j)) == points_to_visit.end()
						&& find(visited_points.begin(), visited_points.end(), point + Point2i(i, j)) == visited_points.end())
					{
						points_to_visit.push_back(point + Point2i(i, j));
					}
				}
			}
		}

		if (blob.Type == PTYPE_MAX)
		{
			outImg.at<Vec3b>(blob.ExtremumPoint) = MaxPointColor;
		}
		else if (blob.Type == PTYPE_MIN)
		{
			outImg.at<Vec3b>(blob.ExtremumPoint) = MinPointColor;
		}
	}

	vector<TrackedPoint> * GetRectanglePoints(vector<TrackedPoint> correct_points, int width, int height)
	{
		vector<TrackedPoint> rect_points[4];

		Rect rectangles[4];

		rectangles[0] = Rect(0, 0, width / 2 - 1, height / 2 - 1);
		rectangles[1] = Rect(width / 2, 0, width / 2, height / 2);
		rectangles[2] = Rect(0, height / 2, width / 2 - 1, height / 2 - 1);
		rectangles[3] = Rect(width / 2, height / 2, width / 2, height / 2);

		for (auto point : correct_points)
		{
			Point2i p = point.Coords[point.Coords.size() - 1];

			for (int i = 0; i < 4; i++)
			{
				if (rectangles[i].contains(p))
				{
					rect_points[i].push_back(point);
					break;
				}
			}
		}

		return rect_points;
	}

	bool FindBiggestRectangles(vector<TrackedPoint> correct_points, vector<MyRectangle> &best_rectangles, int correction_time, int best_count, Size size)
	{
		int correct_size = correct_points.size();

		if (correct_size < 4)
		{
			return false;
		}

		Combinatorics comb = Combinatorics(correct_size, 4);
		double maxArea = 0;

		do
		{
			int indexes[4];
			Point2i points[4];

			for (int i = 0; i < 4; i++)
			{
				indexes[i] = comb.Value(i);
			}

			for (int i = 0; i < 4; i++)
			{
				for (int j = 1; j < 4; j++)
				{
					Point2i p1 = correct_points[indexes[j]].Coords[correct_points[indexes[j]].Coords.size() - 1];
					Point2i p2 = correct_points[indexes[j - 1]].Coords[correct_points[indexes[j - 1]].Coords.size() - 1];

					if (p1.y <p2.y)
					{
						int temp = indexes[j];
						indexes[j] = indexes[j - 1];
						indexes[j - 1] = temp;
					}
				}
			}

			Point2i p1 = correct_points[indexes[1]].Coords[correct_points[indexes[1]].Coords.size() - 1];
			Point2i p2 = correct_points[indexes[0]].Coords[correct_points[indexes[0]].Coords.size() - 1];

			if (p1.x < p2.x)
			{
				int temp = indexes[1];
				indexes[1] = indexes[0];
				indexes[0] = temp;
			}

			p1 = correct_points[indexes[2]].Coords[correct_points[indexes[2]].Coords.size() - 1];
			p2 = correct_points[indexes[3]].Coords[correct_points[indexes[3]].Coords.size() - 1];

			if (p1.x < p2.x)
			{
				int temp = indexes[2];
				indexes[2] = indexes[3];
				indexes[3] = temp;
			}

			for (int i = 0; i < 4; i++)
			{
				points[i] = correct_points[indexes[i]].Coords[correct_points[indexes[i]].Coords.size() - 1];
			}

			Rect rectangles[4];

			rectangles[0] = Rect(0, 0, size.width / 2, size.height / 2);
			rectangles[1] = Rect(size.width / 2, 0, size.width / 2, size.height / 2);
			rectangles[2] = Rect(size.width / 2, size.height / 2, size.width / 2, size.height / 2);
			rectangles[3] = Rect(0, size.height / 2, size.width / 2, size.height / 2);

			bool is_correct = true;

			for (int i = 0; i < 4; i++)
			{
				if (!rectangles[i].contains(points[i]))
				{
					is_correct = false;
					break;
				}
			}

			if (!is_correct)
			{
				continue;
			}

			MyRectangle rectangle = MyRectangle(indexes[0], indexes[1], indexes[2], indexes[3], correct_points, 1);

			best_rectangles.push_back(rectangle);

		} while (comb.NextSet());

		if (best_rectangles.size() == 0)
		{
			return false;
		}

		sort(best_rectangles.begin(), best_rectangles.end(), greater<MyRectangle>());

		while (best_rectangles.size() > best_count)
		{
			best_rectangles.pop_back();
		}

		return true;
	}

	void TransformPoint(Point2f &point, Mat perspectiveMatrix)
	{
		double *x_row = perspectiveMatrix.ptr<double>(0);
		double *y_row = perspectiveMatrix.ptr<double>(1);
		double *t_row = perspectiveMatrix.ptr<double>(2);

		point.x = (x_row[0] * point.x + x_row[1] * point.y + x_row[2]) / (t_row[0] * point.x + t_row[1] * point.y + t_row[2]);
		point.y = (y_row[0] * point.x + y_row[1] * point.y + y_row[2]) / (t_row[0] * point.x + t_row[1] * point.y + t_row[2]);
	}

	double GetMetersPerPixel(double h, double alpha, int length)
	{
		double pi = 3.14159265;
		double beta = alpha / 2;
		double radian_beta = beta * pi / 180;
		double tangent = tan(radian_beta);

		double d = 2 * h * tangent;
		double D = length;
		return D / d;
	}
};