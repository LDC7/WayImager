#pragma once
#include "ImgPoint.h"
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
        int min = 256;
        int max = -1;
        RoutePart(ImgPoint ^start, ImgPoint ^end)
        {
            pointStart = start;
            pointEnd = end;
        }
        void SetMinMax(int val)
        {
            if (min > val)
            {
                min = val;
            }
            if (max < val)
            {
                max = val;
            }
        }
    };
}