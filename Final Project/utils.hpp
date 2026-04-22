#pragma once
#include <iostream>
#include <math.h>
// here we can define utility function (math, helper functions, etcetera).


// below is how I declared the namespace Utils and inside of it is a NESTED namespace called Math.
// Look in utils.cpp to see how the test function is defined
namespace Utils
{
	namespace Math
	{
		void Test(); // remove at some point
	}
}