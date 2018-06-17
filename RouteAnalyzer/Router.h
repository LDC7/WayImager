#pragma once

#include "Filter.h"
#include "Route.h"

namespace RouteAnalyzer
{
	using namespace System;

	public ref class Router
	{
	private:

		Filter ^filter;

	public:

		Router(Filter ^filt)
		{
			this->filter = filt;
		}

		void CreateRouteValue(Route^ route)
		{
			for (int i = 0; i < route->Parts->Count; i++)
			{
				GetPartValue(route->Parts[i]);
			}
		}

		void GetPartValue(RoutePart^ rp)
		{
			float ang;
			//найти угол
			rp->angle = gcnew Angle(ang);

			while (1>0)
			{

			}
		}

		int GetPointValue(ImgPoint^ p)
		{
			filter->outBufMat;

			return 0;
		}

		void AlternativePaths(Route^ route)
		{

		}
	};
}