#pragma once

#include "RoutePart.h"

namespace RouteAnalyzer
{
	using namespace System;

	public ref class Route
	{
	public:
		System::String^ Name;
		List<RoutePart^> ^Parts;

		Route()
		{
			Parts = gcnew List<RoutePart^>();
		}
	};
}