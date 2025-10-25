#include <iostream>
#include <chrono>
#include <vector>
#include <immintrin.h>

#define LOOP_TIMES 100000000

using namespace std;

long long GetFuncExecTime(void (*func)(const uint32_t* a, const uint32_t* b, uint32_t* out, int n), const uint32_t* a, const uint32_t* b, uint32_t* out, int n)
{
	chrono::steady_clock::time_point beg = chrono::high_resolution_clock::now();
	func(a, b, out, n);
	return chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now() - beg).count();
}

void Add_X86(const uint32_t* a, const uint32_t* b, uint32_t* out, int n)
{
	for (int i = 0; i < n; ++i)
	{
		out[i] = a[i] + b[i];
	}
}

void Add_AVX2(const uint32_t* a, const uint32_t* b, uint32_t* out, int n)
{
	int i = 0;

	for (i = 0; i + 8 < n; i += 8)
	{
		// 동시에 8개의 값을 SIMD 방식으로 처리 (256 / 32 == 8)
		__m256i a_vector = _mm256_loadu_si256((__m256i const*)(&a[i]));
		__m256i b_vector = _mm256_loadu_si256((__m256i const*)(&b[i]));
		__m256i c_vector = _mm256_add_epi32(a_vector, b_vector);
		_mm256_storeu_si256((__m256i*)(&out[i]), c_vector);
	}

	// n이 8의 배수가 아닌 경우, SIMD로 처리되지 못하고 남은 자료에 대해 수행
	for (; i < n; ++i)
	{
		out[i] = a[i] + b[i];
	}
}

int main()
{
	vector<uint32_t> A(LOOP_TIMES, 1);
	vector<uint32_t> B(LOOP_TIMES, 1);
	vector<uint32_t> OUT(LOOP_TIMES);

	long long time_X86 = GetFuncExecTime(Add_X86, A.data(), B.data(), OUT.data(), LOOP_TIMES);
	long long time_AVX2 = GetFuncExecTime(Add_AVX2, A.data(), B.data(), OUT.data(), LOOP_TIMES);

	std::cout << "time_X86 == " << time_X86 << "us" << " time_AVX2 == " << time_AVX2 << "us" << endl << endl;
	std::cout << "AVX2 instructions are " << (double)time_X86 / time_AVX2 << " times faster than X86 instruction set." << endl;
}