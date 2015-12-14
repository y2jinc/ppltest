// ConsoleApplication2.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"

// parallel-fibonacci.cpp
// compile with: /EHsc
#include <windows.h>
#include <ppl.h>
#include <concurrent_vector.h>
#include <array>
#include <vector>
#include <tuple>
#include <algorithm>
#include <iostream>

using namespace concurrency;
using namespace std;

void __cdecl operator delete(
	void*       block,
	int const   block_use,
	char const* file_name,
	int const   line_number
	)
{
	return operator delete(block);
}

void __cdecl operator delete[](
	void*       block,
	int const   block_use,
	char const* file_name,
	int const   line_number
	)
{
	return operator delete[](block);
}

void* __cdecl operator new(
size_t const size,
int const    block_use,
char const*  file_name,
int const    line_number
)
{
	return operator new(size);
}

void* __cdecl operator new[](
size_t const size,
int const    block_use,
char const*  file_name,
int const    line_number
)
{
	return operator new[](size);
}

void* __cdecl operator new (std::size_t count)
{
	return malloc(count);
}

void* __cdecl operator new[](std::size_t count)
{
	return malloc(count);
}

void operator delete (void* ptr)
{
	free(ptr);
}

void operator delete[] (void* ptr)
{
	free(ptr);
}

// Calls the provided work function and returns the number of milliseconds 
// that it takes to call that function.
template <class Function>
__int64 time_call(Function&& f)
{
	__int64 begin = GetTickCount();
	f();
	return GetTickCount() - begin;
}

// Computes the nth Fibonacci number.
int fibonacci(int n)
{
	if (n < 2)
		return n;
	return fibonacci(n - 1) + fibonacci(n - 2);
}

int wmain()
{
	int * pInt = new int;

	delete pInt;

	__int64 elapsed;

	// An array of Fibonacci numbers to compute.
	array<int, 4> a = { 24, 26, 41, 42 };

	// The results of the serial computation.
	vector<tuple<int, int>> results1;

	// The results of the parallel computation.
	concurrent_vector<tuple<int, int>> results2;

	// Use the for_each algorithm to compute the results serially.
	elapsed = time_call([&]
	{
		for_each(begin(a), end(a), [&](int n) {
			results1.push_back(make_tuple(n, fibonacci(n)));
		});
	});
	wcout << L"serial time: " << elapsed << L" ms" << endl;

	// Use the parallel_for_each algorithm to perform the same task.
	elapsed = time_call([&]
	{
		parallel_for_each(begin(a), end(a), [&](int n) {
			results2.push_back(make_tuple(n, fibonacci(n)));
		});

		// Because parallel_for_each acts concurrently, the results do not 
		// have a pre-determined order. Sort the concurrent_vector object
		// so that the results match the serial version.
		sort(begin(results2), end(results2));
	});
	wcout << L"parallel time: " << elapsed << L" ms" << endl << endl;

	// Print the results.
	for_each(begin(results2), end(results2), [](tuple<int, int>& pair) {
		wcout << L"fib(" << get<0>(pair) << L"): " << get<1>(pair) << endl;
	});
}
