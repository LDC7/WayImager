#pragma once

#include "Angle.h"

namespace RouteAnalyzer
{
	using namespace System::Drawing;

	public ref class ImgPoint
	{
	public :
		int x, y;
		int value;
		Angle^ angle;

		ImgPoint(int x, int y)
		{
			this->x = x;
			this->y = y;
		}

		ImgPoint(System::Drawing::Point^ p)
		{
			this->x = p->X;
			this->y = p->Y;
		}

		ImgPoint(ImgPoint^ p)
		{
			this->x = p->x;
			this->y = p->y;
			this->value = p->value;
		}
	};
}