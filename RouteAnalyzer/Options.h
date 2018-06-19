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
		unsigned char MinLvlForDensityMap = 90;
		unsigned char MaxLvlForDensityMap = 200;
		int CoefficientForDensity = 11000;
		int CoefficientForDispersion = 1;
		int MinLimitForInfMap = 60;


		Options()
		{
		}
	};
}