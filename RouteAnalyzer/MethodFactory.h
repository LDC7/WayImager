#pragma once

#include "MethodOfContrasts.h"


namespace RouteAnalyzer
{
	public ref class MethodFactory
	{
	public:

		static IPointFinder^ GetMethod(int i)
		{
			IPointFinder ^method;
			switch (i)
			{
			case 0:
				MethodOfContrasts ^method0;
				method = method0;
			default:
				MethodOfContrasts ^methoddef;
				method = methoddef;
				break;
			}
			return method;
		}

		static System::Collections::Generic::List<System::String ^>^ GetList()
		{
			System::Collections::Generic::List<System::String ^> ^list;

			list->Add("Метод контрастов");

			return list;
		}
	};
}