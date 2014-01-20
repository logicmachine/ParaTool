#ifndef PARATOOL_SORT_HPP
#define PARATOOL_SORT_HPP

#include <algorithm>

namespace paratool {

namespace sort_internal  {

template <typename Iterator, typename Comparator>
inline void heap_sort(Iterator begin, Iterator end, Comparator &cmp){
	const size_t n = end - begin;
	if(n <= 1){ return; }
	size_t i = 0;
	for(i = 0; i < n; ++i){
		size_t j = i;
		while(j > 0){
			const size_t k = ((j + 1) >> 1) - 1;
			if(cmp(*(begin + k), *(begin + j))){
				std::swap(*(begin + k), *(begin + j));
			}else{
				break;
			}
			j = k;
		}
	}
	while(--i > 0){
		std::swap(*(begin + i), *begin);
		size_t j = 0;
		while(true){
			const size_t left = ((j + 1) << 1) - 1;
			const size_t right = ((j + 1) << 1);
			if(left >= i){ break; }
			size_t t = j;
			if(cmp(*(begin + t), *(begin + left))){
				t = left;
			}
			if(right < i && cmp(*(begin + t), *(begin + right))){
				t = right;
			}
			if(t == j){ break; }
			std::swap(*(begin + j), *(begin + t));
			j = t;
		}
	}
}
template <typename Iterator>
inline void heap_sort(Iterator begin, Iterator end){
	const size_t n = end - begin;
	if(n <= 1){ return; }
	size_t i = 0;
	for(i = 0; i < n; ++i){
		size_t j = i;
		while(j > 0){
			const size_t k = ((j + 1) >> 1) - 1;
			if(*(begin + k) < *(begin + j)){
				std::swap(*(begin + k), *(begin + j));
			}else{
				break;
			}
			j = k;
		}
	}
	while(--i > 0){
		std::swap(*(begin + i), *begin);
		size_t j = 0;
		while(true){
			const size_t left = ((j + 1) << 1) - 1;
			const size_t right = ((j + 1) << 1);
			if(left >= i){ break; }
			size_t t = j;
			if(*(begin + t) < *(begin + left)){
				t = left;
			}
			if(right < i && *(begin + t) < *(begin + right)){
				t = right;
			}
			if(t == j){ break; }
			std::swap(*(begin + j), *(begin + t));
			j = t;
		}
	}
}

template <typename Iterator, typename Comparator>
inline void insertion_sort(Iterator begin, Iterator end, Comparator &cmp){
	for(Iterator it = begin; it != end; ++it){
		Iterator minimum = it;
		for(Iterator jt = it + 1; jt != end; ++jt){
			if(cmp(*jt, *minimum)){ minimum = jt; }
		}
		std::swap(*it, *minimum);
	}
}
template <typename Iterator>
inline void insertion_sort(Iterator begin, Iterator end){
	for(Iterator it = begin; it != end; ++it){
		Iterator minimum = it;
		for(Iterator jt = it + 1; jt != end; ++jt){
			if(*jt < *minimum){ minimum = jt; }
		}
		std::swap(*it, *minimum);
	}
}

template <typename Iterator, typename Comparator>
inline Iterator get_median(Iterator begin, Iterator end, Comparator &cmp){
	size_t n = end - begin;
	Iterator l = begin, r = end - 1;
	Iterator c = begin + (n >> 1);
	if(cmp(*l, *c) && cmp(*l, *r)){
		return cmp(*c, *r) ? c : r;
	}else if(cmp(*c, *l) && cmp(*c, *r)){
		return cmp(*l, *r) ? l : r;
	}else{
		return cmp(*l, *c) ? l : c;
	}
}
template <typename Iterator>
inline Iterator get_median(Iterator begin, Iterator end){
	size_t n = end - begin;
	Iterator l = begin, r = end - 1;
	Iterator c = begin + (n >> 1);
	if(*l < *c && *l < *r){
		return (*c < *r) ? c : r;
	}else if(*c < *l && *c < *r){
		return (*l < *r) ? l : r;
	}else{
		return (*l < *c) ? l : c;
	}
}

template <typename Iterator, typename Comparator>
void intro_sort(
	Iterator begin, Iterator end, Comparator &cmp, int depth)
{
	const int INSERTION_THRESHOLD = 16;
	if(depth == 0){
		heap_sort(begin, end, cmp);
	}else{
		size_t n = end - begin;
		if(n < INSERTION_THRESHOLD){
			insertion_sort(begin, end, cmp);
		}else{
			Iterator median = get_median(begin, end, cmp);
			std::swap(*begin, *median);
			Iterator pivot = begin;
			Iterator left = begin + 1, right = end - 1;
			while(true){
				while(!cmp(*pivot, *left) && left != right){ ++left; }
				while(!cmp(*right, *pivot) && left != right){ --right; }
				if(left == right){ break; }
				std::swap(*left, *right);
			}
			if(cmp(*pivot, *left)){ --left; }
			std::swap(*pivot, *left);
#pragma omp task shared(cmp)
			intro_sort(begin, left, cmp, depth - 1);
#pragma omp task shared(cmp)
			intro_sort(left + 1, end, cmp, depth - 1);
		}
	}
}
template <typename Iterator>
void intro_sort(
	Iterator begin, Iterator end, int depth)
{
	const size_t INSERTION_THRESHOLD = 16;
	if(depth == 0){
		heap_sort(begin, end);
	}else{
		size_t n = end - begin;
		if(n < INSERTION_THRESHOLD){
			insertion_sort(begin, end);
		}else{
			Iterator median = get_median(begin, end);
			std::swap(*begin, *median);
			Iterator pivot = begin;
			Iterator left = begin + 1, right = end - 1;
			while(true){
				while(!(*pivot < *left) && left != right){ ++left; }
				while(!(*right < *pivot) && left != right){ --right; }
				if(left == right){ break; }
				std::swap(*left, *right);
			}
			if(*pivot < *left){ --left; }
			std::swap(*pivot, *left);
#pragma omp task
			intro_sort(begin, left, depth - 1);
#pragma omp task
			intro_sort(left + 1, end, depth - 1);
		}
	}
}

}

template <typename Iterator, typename Comparator>
inline void sort(Iterator begin, Iterator end, Comparator cmp){
	size_t n = end - begin;
	int depth = 0;
	while(n >> depth){ ++depth; }
#pragma omp parallel
	{
#pragma omp single
		sort_internal::intro_sort(begin, end, cmp, depth << 1);
	}
}

template <typename Iterator>
inline void sort(Iterator begin, Iterator end){
	size_t n = end - begin;
	if(n < 2){ return; }
	int depth = 0;
	while(n >> depth){ ++depth; }
#pragma omp parallel
	{
#pragma omp single 
		sort_internal::intro_sort(begin, end, depth << 1);
	}
}

}

#endif // PARATOOL_SORT_HPP

