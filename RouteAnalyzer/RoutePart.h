#pragma once

#include "ImgPoint.h"
#include "Angle.h"

using namespace std;

namespace RouteAnalyzer
{
	using namespace System;
	using namespace System::Collections::Generic;

	public ref class RoutePart
	{
	public:
		ImgPoint ^pointStart;
		ImgPoint ^pointEnd;
		List<ImgPoint^> ^IntermidiatePoints;
		Angle^ angle;

		RoutePart(ImgPoint ^start, ImgPoint ^end)
		{
			pointStart = start;
			pointEnd = end;
		}
	};
}