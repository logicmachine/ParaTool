#include <vector>
#include <algorithm>
#include <cstdlib>
#include <gtest/gtest.h>
#include "paratool/unique.hpp"

TEST(UniqueTest, RandomData){
	const int VECTOR_SIZES[] = { 1, 256, 65521 };
	const int VECTOR_SIZE_COUNT = sizeof(VECTOR_SIZES) / sizeof(int);
	for(int i = 0; i < VECTOR_SIZE_COUNT; ++i){
		const int n = VECTOR_SIZES[i];
		std::vector<int> expect(n);
		for(int j = 0; j < n; ++j){ expect[j] = rand(); }
		std::sort(expect.begin(), expect.end());
		std::vector<int> actual(expect);
		std::vector<int>::iterator expect_end =
			std::unique(expect.begin(), expect.end());
		std::vector<int>::iterator actual_end =
			paratool::unique(actual.begin(), actual.end());
		std::vector<int>::iterator expect_it = expect.begin();
		std::vector<int>::iterator actual_it = actual.begin();
		while(expect_it != expect_end){
			EXPECT_NE(actual_end, actual_it);
			EXPECT_EQ(*expect_it, *actual_it);
			++expect_it;
			++actual_it;
		}
		EXPECT_EQ(actual_end, actual_it);
	}
}

TEST(UniqueTest, UniformData){
	const int VECTOR_SIZES[] = { 1, 256, 65521 };
	const int VECTOR_SIZE_COUNT = sizeof(VECTOR_SIZES) / sizeof(int);
	for(int i = 0; i < VECTOR_SIZE_COUNT; ++i){
		const int n = VECTOR_SIZES[i];
		std::vector<int> expect(n, 0);
		std::vector<int> actual(expect);
		std::vector<int>::iterator expect_end =
			std::unique(expect.begin(), expect.end());
		std::vector<int>::iterator actual_end =
			paratool::unique(actual.begin(), actual.end());
		std::vector<int>::iterator expect_it = expect.begin();
		std::vector<int>::iterator actual_it = actual.begin();
		while(expect_it != expect_end){
			EXPECT_NE(actual_end, actual_it);
			EXPECT_EQ(*expect_it, *actual_it);
			++expect_it;
			++actual_it;
		}
		EXPECT_EQ(actual_end, actual_it);
	}
}

