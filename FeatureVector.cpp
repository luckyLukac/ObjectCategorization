#include <fstream>

#include "FeatureVector.hpp"



FeatureVector::FeatureVector(const std::vector<Chain>& chains) {
	chainLengths = std::vector<uint>(chains.size());
	for (uint i = 0; i < chains.size(); i++) {
		chainLengths[i] = chains[i].midPoints.size();
	}
}

bool FeatureVector::writeToFile(const std::string& fileName, const std::string& path) {
	std::ofstream out(path + fileName);

	// If the file is not open, we return no success.
	if (!out.is_open()) {
		return false;
	}

	// Output of the number of chains.
	out << chainLengths.size() << "\n";

	// Output of each chain length.
	for (const uint chainLength : chainLengths) {
		out << chainLength << " ";
	}

	return true;
}