#include "cppmemusage.hpp"

#include <vector>
#include <iostream>

int main() {
	const auto sz {1024 * 1024};
	std::vector<int> v;
	for (size_t i {0}; i < sz; ++i) {
		v.push_back(i);
		if (i %10 == 0) {
			size_t currentSize = getCurrentRSS( );
			std::cout << "currentSize=" << currentSize << "\n";
		}
	} 

	size_t peakSize    = getPeakRSS( );
	std::cout << "peakSize=" << peakSize << "\n";
}
