#ifndef GESTURES_EXTRACT_UTILS_H
#define GESTURES_EXTRACT_UTILS_H

#include <openpose/utilities/headers.hpp>
#include "boost/serialization/vector.hpp"

float computeDistance(const op::Array<float> &keypoints, int person, int elementA, int elementB, bool _l2_distance);

std::string keypointsAsNumpyTextString(const op::Array<float> &keypoints, const std::vector<int> &vector);

void flattenKeyPoints(const op::Array<float> &keypoints, const int &person, const std::vector<int> &onlyThesePoints,
                      std::vector<float> &flattened);

void overlayImage(const cv::Mat &background, const cv::Mat &foreground,
                  cv::Mat &output, cv::Point2i location);


#endif //GESTURES_EXTRACT_UTILS_H
