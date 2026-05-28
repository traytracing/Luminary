#include "InputDataInterpolator.h"
#include <glad/glad.h>

struct InterpolatedGoodData {
	std::vector<Spline3D> interpolatedPositions;
	std::vector<std::pair<float, float>> activeTime;

	void setDataPoints(const std::vector<double>& TimeStamps, const std::vector<glm::vec3>& positions) {
		std::vector<glm::vec3> curPositions;
		std::vector<double> curTimes;
		std::pair<double, double> curActive(std::nan(""), std::nan(""));
		for (int i{}; i < TimeStamps.size(); ++i) {
			const auto& position = positions[i];
			if (std::isnan(position.x) || std::isnan(position.y) || std::isnan(position.z)) continue;

			//start of spline
			if (std::isnan(curActive.first)) {
				curActive.first = TimeStamps[i];
			}

			curPositions.push_back(position);
			curTimes.push_back(TimeStamps[i]);



			//end of spline 
			if (i + 1 < TimeStamps.size()) {
				const auto& nextPosition = positions[i + 1];
				if (std::isnan(nextPosition.x) || std::isnan(nextPosition.y) || std::isnan(nextPosition.z)) {
					curActive.second = TimeStamps[i];
					Spline3D cur(curTimes, curPositions);
					interpolatedPositions.push_back(cur);
					activeTime.push_back(curActive);
					curPositions.clear();
					curTimes.clear();
					curActive = std::pair(std::nan(""), std::nan(""));
				}
			}
			else {
				curActive.second = TimeStamps[i];
				Spline3D cur(curTimes, curPositions);
				interpolatedPositions.push_back(cur);
				activeTime.push_back(curActive);
				curPositions.clear();
				curTimes.clear();
				curActive = std::pair(std::nan(""), std::nan(""));
			}




		}

	}

	glm::vec3 eval(float curTime) {
		int splineIndex = -1;
		for (int i{}; i < activeTime.size(); ++i) {
			if (activeTime[i].first <= curTime && activeTime[i].second >= curTime) {
				splineIndex = i;
				break;
			}
		}

		return (splineIndex == -1) ? glm::vec3(std::nan("")) : interpolatedPositions[splineIndex].evaluate(curTime);
	}
};


extern int fps;
extern int totalSimTime;
std::vector<std::pair<std::vector<glm::vec3>, GLfloat>> i_objectsPositionsAcrossTime;
std::vector<InterpolatedGoodData> data;


void InputDataInterpolator::InterpolateData(int timestampsCount, std::vector<double> timestamps, int objectCount, std::vector<std::vector<glm::vec3>> positions) {


	std::vector<std::vector<glm::vec3>> position;
	for (int i = 0; i < objectCount; ++i) {
		std::vector<glm::vec3> cur;
		for (int j = 0; j < timestampsCount; ++j) {
			cur.push_back(positions[j][i]);
		}
		position.push_back(cur);
	}



	for (int i = 0; i < objectCount; ++i) {
		InterpolatedGoodData cur;
		cur.setDataPoints(timestamps, position[i]);
		data.emplace_back(cur);
	}

	double sTime = timestamps[0];
	for (int i = 0; i < fps * totalSimTime; ++i) {
		sTime += 1.0 / fps;

		std::vector<glm::vec3> pos; // update with pairs for unique id etc
		for (int j = 0; j < objectCount; ++j) {
			pos.push_back(data[j].eval(sTime));
		}
		i_objectsPositionsAcrossTime.emplace_back(
			std::pair(pos, sTime)
		);

	}
	std::cout << "Total Unique Frames: " << i_objectsPositionsAcrossTime.size() << std::endl;
}