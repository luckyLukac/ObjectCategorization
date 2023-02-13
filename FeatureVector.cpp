#include <fstream>

#include "FeatureVector.hpp"



FeatureVector::FeatureVector(const std::vector<Chain>& chains) {
	chainLengths = std::vector<double>(chains.size());
	for (uint i = 0; i < chains.size(); i++) {
		chainLengths[i] = chains[i].totalLength();
	}
}

FeatureVector::FeatureVector(const std::vector<double>& distances) {
	chainLengths = distances;
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
	for (const double chainLength : chainLengths) {
		out << chainLength << " ";
	}

	return true;
}