//
// Created by Nathan  Pahucki on 10/11/18.
//

#ifndef GESTURES_VIDEORASIEDARMDEMO_H
#define GESTURES_VIDEORASIEDARMDEMO_H


#include "VideoProcessor.h"
#include "VideoPoseCapture.h"
#include "UpperBodyRaisedHandPredictor.h"

class VideoRasiedArmDemo : public VideoPoseCapture {

private:
    UpperBodyRaisedHandPredictor *predictor_ = nullptr;

public:
    VideoRasiedArmDemo(const string &windowName);

    void start() override;

protected:
    bool processKeypoints(const op::Array<float> &keypoints, const cv::Mat &inputImage, cv::Mat &outputImage) override;
};


#endif //GESTURES_VIDEORASIEDARMDEMO_H
