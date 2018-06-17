#pragma once

#include "MethodOfContrasts.h"


namespace RouteAnalyzer
{
	using namespace System;
	using namespace System::Collections::Generic;

	public ref class MethodFactory
	{
	public:

		static IPointFinder^ GetMethod(int i)
		{
			IPointFinder ^method;
			switch (i)
			{
			case 0:
				method = gcnew MethodOfContrasts;
			default:
				method = gcnew MethodOfContrasts;
				break;
			}
			return method;
		}

		static List<System::String ^>^ GetList()
		{
			List<System::String ^> ^list = gcnew List<System::String^>();

			list->Add("Метод контрастов");

			return list;
		}
	};
}