#include <vector>
#include <algorithm>
#include <cstdlib>
#include <gtest/gtest.h>
#include "paratool/prefix_scan.hpp"

template <typename T>
static int reference_prefix_scan(std::vector<T> &v){
	const int n = v.size();
	T sum = T();
	for(int i = 0; i < n; ++i){
		const T t = v[i];
		v[i] = sum;
		sum += t;
	}
	return sum;
}

TEST(PrefixScanTest, RandomData){
	const int VECTOR_SIZES[] = { 1, 256, 65521 };
	const int VECTOR_SIZE_COUNT = sizeof(VECTOR_SIZES) / sizeof(int);
	for(int i = 0; i < VECTOR_SIZE_COUNT; ++i){
		const int n = VECTOR_SIZES[i];
		std::vector<int> expect(n);
		for(int j = 0; j < n; ++j){ expect[j] = rand(); }
		std::vector<int> actual(n);
		const int actual_sum = paratool::prefix_scan(
			expect.begin(), expect.end(), actual.begin());
		const int expect_sum = reference_prefix_scan(expect);;
		EXPECT_EQ(expect_sum, actual_sum);
		std::vector<int>::iterator expect_it = expect.begin();
		std::vector<int>::iterator actual_it = actual.begin();
		while(expect_it != expect.end()){
			EXPECT_EQ(*expect_it, *actual_it);
			++expect_it;
			++actual_it;
		}
	}
}

TEST(PrefixScanTest, UniformData){
	const int VECTOR_SIZES[] = { 1, 256, 65521 };
	const int VECTOR_SIZE_COUNT = sizeof(VECTOR_SIZES) / sizeof(int);
	for(int i = 0; i < VECTOR_SIZE_COUNT; ++i){
		const int n = VECTOR_SIZES[i];
		std::vector<int> expect(n, 1);
		std::vector<int> actual(n);
		const int actual_sum = paratool::prefix_scan(
			expect.begin(), expect.end(), actual.begin());
		const int expect_sum = reference_prefix_scan(expect);;
		EXPECT_EQ(expect_sum, actual_sum);
		std::vector<int>::iterator expect_it = expect.begin();
		std::vector<int>::iterator actual_it = actual.begin();
		while(expect_it != expect.end()){
			EXPECT_EQ(*expect_it, *actual_it);
			++expect_it;
			++actual_it;
		}
	}
}

