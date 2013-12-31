#ifndef PARATOOL_UNIQUE_HPP
#define PARATOOL_UNIQUE_HPP

#include <vector>
#include <algorithm>
#include <iterator>
#include <omp.h>

namespace paratool {

template <typename Iterator>
inline Iterator unique(Iterator first, Iterator last){
	typedef typename std::iterator_traits<Iterator>::value_type ValueType;
	const size_t n = std::distance(first, last);
	const int num_threads = omp_get_max_threads();
	if(num_threads <= 1){ return std::unique(first, last); }
	const size_t block_size = (n + num_threads - 1) / num_threads;
	std::vector<int> flags(num_threads);
	std::vector<Iterator> heads(num_threads), tails(num_threads);
	for(int i = 0; i < num_threads; ++i){
		const size_t j = block_size * i;
		if(j > 0 && j < n && *(first + j) == *(first + j - 1)){
			flags[i] = 1;
		}
	}
	size_t written_count = 0;
	std::vector<ValueType> copy_buffer(block_size);
#pragma omp parallel shared(written_count, copy_buffer)
	{
		const int thread_id = omp_get_thread_num();
		const size_t begin = std::min(block_size * thread_id, n);
		const size_t end = std::min(begin + block_size, n);
		Iterator head = first + begin, tail = first + end;
		tails[thread_id] = std::unique(head, tail);
		heads[thread_id] = head + flags[thread_id];
		if(thread_id == 0){
			written_count = std::distance(heads[0], tails[0]);
		}
#pragma omp barrier
		for(int i = 1; i < num_threads; ++i){
			const size_t m = std::distance(heads[i], tails[i]);
#pragma omp for
			for(size_t j = 0; j < m; ++j){
				copy_buffer[j] = *(heads[i] + j);
			}
#pragma omp barrier
#pragma omp for
			for(size_t j = 0; j < m; ++j){
				*(first + written_count + j) = copy_buffer[j];
			}
#pragma omp barrier
#pragma omp single
			written_count += m;
		}
	}
	return first + written_count;
}

}

#endif // PARATOOL_UNIQUE_HPP

