#pragma once

#include <string>
#include <opencv2/core.hpp>
#include "PointsInfo.h"

using namespace cv;
using namespace System::Collections::Generic;

namespace RouteAnalyzer
{
	public ref class IPointFinder
	{
	public:
		virtual void GettingPoints(Mat &inImg, Mat& points, PointsInfo &pointsInfo, string outPath) = 0;
		virtual List<System::Tuple<System::String ^, double> ^>^ GettingOptions() = 0;
		virtual void SetOptions(List<System::Tuple<System::String ^, double> ^>^ list) = 0;
	};
}