#include <vector>
#include <algorithm>
#include <cstdlib>
#include <gtest/gtest.h>
#include "paratool/sort.hpp"

TEST(SortTest, RandomData){
	const int VECTOR_SIZES[] = { 1, 256, 65521 };
	const int VECTOR_SIZE_COUNT = sizeof(VECTOR_SIZES) / sizeof(int);
	for(int i = 0; i < VECTOR_SIZE_COUNT; ++i){
		const int n = VECTOR_SIZES[i];
		std::vector<int> expect(n);
		for(int j = 0; j < n; ++j){ expect[j] = rand(); }
		std::vector<int> actual(expect);
		std::sort(expect.begin(), expect.end(), std::greater<int>());
		paratool::sort(actual.begin(), actual.end(), std::greater<int>());
		std::vector<int>::iterator expect_it = expect.begin();
		std::vector<int>::iterator actual_it = actual.begin();
		while(expect_it != expect.end()){
			EXPECT_EQ(*expect_it, *actual_it);
			++expect_it;
			++actual_it;
		}
	}
}

TEST(SortTest, UniformData){
	const int VECTOR_SIZES[] = { 1, 256, 65521 };
	const int VECTOR_SIZE_COUNT = sizeof(VECTOR_SIZES) / sizeof(int);
	for(int i = 0; i < VECTOR_SIZE_COUNT; ++i){
		const int n = VECTOR_SIZES[i];
		std::vector<int> expect(n, 0);
		std::vector<int> actual(expect);
		std::sort(expect.begin(), expect.end());
		paratool::sort(actual.begin(), actual.end());
		std::vector<int>::iterator expect_it = expect.begin();
		std::vector<int>::iterator actual_it = actual.begin();
		while(expect_it != expect.end()){
			EXPECT_EQ(*expect_it, *actual_it);
			++expect_it;
			++actual_it;
		}
	}
}

