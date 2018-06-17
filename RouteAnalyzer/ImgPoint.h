#pragma once


namespace RouteAnalyzer
{
	public ref class ImgPoint
	{
	public :
		int x, y;
		int value;

		ImgPoint(int x, int y)
		{
			this->x = x;
			this->y = y;
		}
	};
}