#ifndef PARATOOL_PREFIX_SCAN_HPP
#define PARATOOL_PREFIX_SCAN_HPP

#include <vector>
#include <iterator>
#include <algorithm>
#include <stdint.h>
#include <omp.h>

namespace paratool {

namespace prefix_scan_internal {

template <
	typename InputIterator, typename OutputIterator,
	typename InputIteratorCategory, typename OutputIteratorCategory>
struct ParallelPrefixScanImpl {
	typedef typename std::iterator_traits<OutputIterator>::value_type ValueType;

	static inline ValueType parallel_prefix_scan(
		InputIterator first, InputIterator last, OutputIterator result)
	{
		const size_t n = std::distance(first, last);
		const int num_threads = omp_get_max_threads();
		const size_t block_size = (n + num_threads - 1) / num_threads;
		// Make input segments table
		std::vector<InputIterator> segments(num_threads + 1);
		segments[0] = first;
		for(int i = 1; i < num_threads; ++i){
			if(block_size * i >= n){
				segments[i] = last;
			}else{
				InputIterator it = segments[i - 1];
				for(size_t j = 0; j < block_size; ++j){ ++it; }
				segments[i] = it;
			}
		}
		segments.back() = last;
		// Allocate parallel work buffers
		std::vector<ValueType> global_sums(num_threads);
		std::vector<ValueType> work(n);
		ValueType total;
		// Calculate prefix scan
#pragma omp parallel shared(total, global_sums)
		{
			const int thread_id = omp_get_thread_num();
			const InputIterator begin = segments[thread_id];
			const InputIterator end = segments[thread_id + 1];
			ValueType local_sum = ValueType();
			size_t i = block_size * thread_id;
			for(InputIterator it = begin; it != end; ++it){
				work[i++] = local_sum;
				local_sum += *it;
			}
			global_sums[thread_id] = local_sum;
#pragma omp barrier
#pragma omp single
			{
				ValueType global_sum = ValueType();
				for(int i = 0; i < num_threads; ++i){
					ValueType temp = global_sums[i];
					global_sums[i] = global_sum;
					global_sum += temp;
				}
				total = global_sum;
			}
#pragma omp barrier
			const size_t begin_idx = block_size * thread_id;
			const size_t end_idx = std::min(begin_idx + block_size, n);
			for(size_t i = begin_idx; i < end_idx; ++i){
				work[i] = global_sums[thread_id] + work[i];
			}
		}
		// Write prefix scan to result
		for(size_t i = 0; i < n; ++i){ *(result++) = work[i]; }
		return total;
	}
};

template <typename InputIterator, typename OutputIterator>
struct ParallelPrefixScanImpl<
	InputIterator, OutputIterator,
	std::random_access_iterator_tag, std::random_access_iterator_tag>
{
	typedef typename std::iterator_traits<OutputIterator>::value_type ValueType;

	static inline ValueType parallel_prefix_scan(
		InputIterator first, InputIterator last, OutputIterator result)
	{
		const size_t n = last - first;
		const int num_threads = omp_get_max_threads();
		const size_t block_size = (n + num_threads - 1) / num_threads;
		// Allocate parallel work buffers
		std::vector<ValueType> global_sums(num_threads);
		ValueType total;
		// Calculate prefix scan
#pragma omp parallel shared(total, global_sums)
		{
			const int thread_id = omp_get_thread_num();
			const size_t begin = block_size * thread_id;
			const size_t end = std::min(begin + block_size, n);
			ValueType local_sum = ValueType();
			for(size_t i = begin; i < end; ++i){
				ValueType temp = *(first + i);
				*(result + i) = local_sum;
				local_sum += temp;
			}
			global_sums[thread_id] = local_sum;
#pragma omp barrier
#pragma omp single
			{
				ValueType global_sum = ValueType();
				for(int i = 0; i < num_threads; ++i){
					ValueType temp = global_sums[i];
					global_sums[i] = global_sum;
					global_sum += temp;
				}
				total = global_sum;
			}
#pragma omp barrier
			for(size_t i = begin; i < end; ++i){
				*(result + i) = *(result + i) + global_sums[thread_id];
			}
		}
		return total;
	}
};

}

template <typename InputIterator, typename OutputIterator>
inline typename std::iterator_traits<OutputIterator>::value_type
prefix_scan(InputIterator first, InputIterator last, OutputIterator result){
	return prefix_scan_internal::ParallelPrefixScanImpl<
		InputIterator, OutputIterator,
		typename std::iterator_traits<InputIterator>::iterator_category,
		typename std::iterator_traits<OutputIterator>::iterator_category
	>::parallel_prefix_scan(first, last, result);
}

}

#endif // PARATOOL_PREFIX_SCAN_HPP

