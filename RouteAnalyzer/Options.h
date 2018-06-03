#pragma once

namespace RouteAnalyzer
{
	public ref class Options
	{
	public:
		int MapWinSize = 61;
		double DispWinSize = 9;
		int CameraWidth = 20;
		int CameraHeight = 15;

		Options()
		{
		}
	};
}