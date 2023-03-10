#pragma once

#include <string>
#include <vector>

#include "Chain.hpp"
#include "Constants.hpp"


/// <summary>
/// Structure for storing the feature vector of detected chains.
/// </summary>
struct FeatureVector {
	std::vector<double> chainLengths;  // Vector of chain lengths.
	std::vector<double> chainFarthestPoints;  // Vector of chain lengths.


	/// <summary>
	/// Constructor of the feature vector.
	/// </summary>
	/// <param name="chains">: vector of chains that should be converted to a feature vector</param>
	FeatureVector(const std::vector<Chain>& chains);

	/// <summary>
	/// Constructor of the feature vector with given distances.
	/// </summary>
	/// <param name="chains">: vector of distances</param>
	FeatureVector(const std::vector<std::pair<double, double>>& distances);

	/// <summary>
	/// Output of the feature vector into a file.
	/// </summary>
	/// <param name="fileName">: desired name of the created file</param>
	/// <param name="path">: path on the drive where the created file should be stored</param>
	/// <returns>true if success, false otherwise</returns>
	bool writeToFile(const std::string& fileName, const std::string& path);
};