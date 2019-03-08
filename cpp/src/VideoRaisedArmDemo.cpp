//
// Created by Nathan  Pahucki on 10/11/18.
//

#include <VideoRaisedArmDemo.h>
#include <extract_utils.h>
#include <iostream>

#include "VideoRaisedArmDemo.h"

VideoRasiedArmDemo::VideoRasiedArmDemo(const string &windowName) : VideoPoseCapture(windowName) { }

void VideoRasiedArmDemo::start() {
    predictor_ = new UpperBodyRaisedHandPredictor(getModelFolder());
    VideoPoseCapture::start();
    delete predictor_;
}


bool
VideoRasiedArmDemo::processKeypoints(const op::Array<float> &keypoints, const cv::Mat &inputImage, cv::Mat &outputImage) {

        const ArmRaised prediction = predictor_->predictRasiedArm(keypoints, 0);

        // Write the samples still need or taken so far
        std::ostringstream boxText;
        boxText <<" Person 0: ";
        switch (prediction) {
            case ArmRaised::NONE:
                boxText << "No Arm Raised";
                break;
            case ArmRaised::RIGHT:
                boxText << "Right Arm Raised";
                break;
            case ArmRaised::LEFT:
                boxText << "Left Arm Raised";
                break;
            default:
                boxText << "Unknown!!";
                break;
        }

        cv::putText(outputImage, boxText.str(), cv::Point(5,30), cv::FONT_HERSHEY_PLAIN, 2.50, CV_RGB(255, 100, 0), 2);

    return true;
}

