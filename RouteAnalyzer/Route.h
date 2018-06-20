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
        int min = 256;
        int max = -1;
        bool Evaluated = false;
        Route()
        {
            Parts = gcnew List<RoutePart^>();
        }
    };
}