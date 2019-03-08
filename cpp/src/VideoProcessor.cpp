//
// Created by Nathan  Pahucki on 10/9/18.
//

#include "VideoProcessor.h"
#include <opencv2/highgui/highgui.hpp>

VideoProcessor::VideoProcessor(const std::string &windowName) : windowName_(windowName) {}

void VideoProcessor::runLoop() {
    cv::VideoCapture capture;
    cv::Mat cameraImage;

    if (!ipCamera_.empty()) {
        if (!capture.open(ipCamera_)) {
            op::error("Could not open IP Camera: " + ipCamera_, __LINE__, __FUNCTION__, __FILE__);
        }
    } else {
        if (!capture.open(camera_)) {
            op::error("Could not open WebCam", __LINE__, __FUNCTION__, __FILE__);
        }
        capture.set(CV_CAP_PROP_FRAME_WIDTH,1280);
        capture.set(CV_CAP_PROP_FRAME_HEIGHT,720);
    }




    // Need to do this once so an image appears right away even on slower machines
    capture.read(cameraImage);

    cv::Mat outputImage = cv::Mat::zeros(cameraImage.size(), cameraImage.type());
    cv::imshow(windowName_, outputImage);

    while (true) {
        // -------------------------------------------------------
        // Check if user pushed any keys and process the request
        // ------------------------q-------------------------------
        auto key = cv::waitKey(1);

        // Toggle full screen
        if (key == 'f') {
            fullScreen_ = !fullScreen_;
            if (fullScreen_) {
                cv::setWindowProperty(windowName_, CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
            } else {
                cv::setWindowProperty(windowName_, CV_WND_PROP_FULLSCREEN, CV_WINDOW_NORMAL);
            }
        } else if (key == 'q') {
            break;
        } else {
            // Pass to subclasses to handle
            handleKeyPress(key);
        }

        capture.read(cameraImage);
        if (cameraImage.empty())
            op::error("Could not open video", __LINE__, __FUNCTION__, __FILE__);

        auto &inputImage = cameraImage;
        if (mirrored_) {
            cv::Mat flipped;
            cv::flip(cameraImage, flipped, 1);
            inputImage = flipped;
        }

        if (!processFrame(key, inputImage, outputImage)) break;

        cv::imshow(windowName_, outputImage);
        framesProcessed_++;
        framesSinceSampleStart_++;

        const auto now = std::chrono::steady_clock::now();
        std::chrono::duration<double> diff = now - sampleStartTime_;

        if(diff.count() > 1.0) {
            fps_ = (float)(framesSinceSampleStart_ / diff.count());
            sampleStartTime_ = now;
            framesSinceSampleStart_ = 0;
        }
    } // end of while

    capture.release();
    cameraImage.release(); // Not clear if this is actually needed
    outputImage.release();
}

void VideoProcessor::start() {
    cv::namedWindow(windowName_, CV_WINDOW_NORMAL);
    cv::moveWindow(windowName_, 640, 20);
    cv::resizeWindow(windowName_, 1280, 720);
    this->runLoop();
    cv::destroyAllWindows();
}

bool VideoProcessor::processFrame(const int &key, const cv::Mat &inputImage, cv::Mat &outputImage) {
    outputImage = inputImage;
    return true;
}

void VideoProcessor::setCamera(int camera_) {
    VideoProcessor::camera_ = camera_;
}

void VideoProcessor::setIpCamera(const std::string &ipCamera_) {
    VideoProcessor::ipCamera_ = ipCamera_;
}

unsigned long VideoProcessor::getFramesProcessed() const {
    return framesProcessed_;
}

int VideoProcessor::getFps() const {
    return fps_;
}

const std::string &VideoProcessor::getWindowName() const {
    return windowName_;
}

void VideoProcessor::setMirror(bool mirror) {
    mirrored_ = mirror;
}

void VideoProcessor::handleKeyPress(int key) {}

