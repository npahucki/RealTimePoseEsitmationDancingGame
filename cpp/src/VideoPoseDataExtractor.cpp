//
// Created by Nathan  Pahucki on 10/9/18.
//

#include "VideoPoseDataExtractor.h"
#include "extract_utils.h"
#include <boost/tokenizer.hpp>
#include <iostream>

VideoPoseDataExtractor::VideoPoseDataExtractor(const string &windowName) : VideoPoseCapture(windowName) {}

bool
VideoPoseDataExtractor::processKeypoints(const op::Array<float> &keypoints, const cv::Mat &inputImage, cv::Mat &outputImage) {
    std::cout << keypointsAsNumpyTextString(keypoints, pointsToOutput_) << endl;

    // Write the samples still neded or taken so far
    std::ostringstream boxText;
    if(numberSamples_ > 0) {
        boxText << "Samples remaining: " << (numberSamples_ - sampleCount_);
    } else {
        boxText << "Samples taken: " << sampleCount_;
    }
    cv::putText(outputImage, boxText.str(), cv::Point(10,20), cv::FONT_HERSHEY_PLAIN, 1.25, CV_RGB(255,255,0), 2);

    sampleCount_++;
    return numberSamples_ < 0 || sampleCount_ < numberSamples_;
}

void VideoPoseDataExtractor::setPointsToOutput(const vector<int> &pointsToOutput) {
    pointsToOutput_ = pointsToOutput;
}

void VideoPoseDataExtractor::setNumberSamples(int numberSamples) {
    numberSamples_ = numberSamples;
}

