#include <fstream>

#include "FeatureVector.hpp"


FeatureVector::FeatureVector(const std::vector<Chain>& chains) {
	chainLengths = std::vector<double>(chains.size());
	for (uint i = 0; i < chains.size(); i++) {
		chainLengths[i] = chains[i].totalLength();
	}
}

FeatureVector::FeatureVector(const std::vector<std::pair<double, double>>& distances) {
	chainLengths = std::vector<double>(distances.size());
	chainFarthestPoints = std::vector<double>(distances.size());

	for (uint i = 0; i < distances.size(); i++) {
		chainLengths[i] = distances[i].first;
		chainFarthestPoints[i] = distances[i].second;
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
	//for (const double chainLength : chainLengths) {
	for (uint i = 0; i < chainLengths.size(); i++) {
		out << chainLengths[i] << ":" << chainFarthestPoints[i] << " ";
	}

	return true;
}