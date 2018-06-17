#pragma once

#define PI 3.14159265358979323846

namespace RouteAnalyzer
{
	public ref class Angle
	{
	private:
		float angle;

	public:
		Angle(float val)
		{
			SetAngle(val);
		}

		float GetAngle()
		{
			return angle;
		}

		void SetAngle(float val)
		{
			angle = val;
			while (angle >= 360)
			{
				angle -= 360;
			}
			while (angle < 0)
			{
				angle += 360;
			}
		}

		double ToRad()
		{
			return (double)(PI * angle / 180);
		}
	};
}

