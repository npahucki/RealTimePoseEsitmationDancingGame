//
// Created by Nathan  Pahucki on 10/9/18.
//

#ifndef GESTURES_VIDEOPOSEDATAEXTRACTOR_H
#define GESTURES_VIDEOPOSEDATAEXTRACTOR_H


#include "VideoPoseCapture.h"

class VideoPoseDataExtractor : public VideoPoseCapture {

private:
    vector<int> pointsToOutput_;
    int sampleCount_ = 0;
    int numberSamples_ = -1;

protected:
    bool processKeypoints(const op::Array<float> &keypoints, const cv::Mat &inputImage, cv::Mat &outputImage) override;

public:
    VideoPoseDataExtractor(const string &windowName_);

    void setNumberSamples(int numberSamples);
    void setPointsToOutput(const vector<int> &pointsToOutput);

};


#endif //GESTURES_VIDEOPOSEDATAEXTRACTOR_H
