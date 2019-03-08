//
// Created by Nathan  Pahucki on 10/9/18.
//

#ifndef GESTURES_VIDEOPROCESSOR_H
#define GESTURES_VIDEOPROCESSOR_H

//
// Created by Nathan  Pahucki on 10/5/18.
//

#include <string>
#include <openpose/core/headers.hpp>
#include <openpose/filestream/headers.hpp>
#include <openpose/gui/headers.hpp>
#include <openpose/pose/headers.hpp>
#include <openpose/utilities/headers.hpp>

class VideoProcessor {

private:
    unsigned long framesProcessed_ = 0 ;
    unsigned long framesSinceSampleStart_ = 0;
    std::chrono::steady_clock::time_point sampleStartTime_ = std::chrono::steady_clock::now();

    float fps_ = 0;
    int camera_ = 0;
    bool mirrored_ = true;

public:
    void setMirror(bool mirror);

private:
    std::string ipCamera_ = "";
    std::string windowName_ = "Edmodo Video Demo";
public:
    const std::string &getWindowName() const;

private:
    bool fullScreen_ = false;
    void runLoop();

protected:
    /**
     * Process the frame and return the image to display. If nullptr is returned then the run loop will exit.
     * @param the last key the user pressed
     * @param inputImage The image captured from the camera
     * @param outputImage The image that will be shown on the screen (output param)
     * @return true to keep processing or null to stop
     */
    virtual bool processFrame(const int &key, const cv::Mat &inputImage, cv::Mat &outputImage);


public:

    explicit VideoProcessor(const std::string &windowName_);

    /**
     * Start capturing the video, this will continue until processFrame returns false.
     */
    virtual void start();

    void setCamera(int camera_);

    void setIpCamera(const std::string &ipCamera_);

    unsigned long getFramesProcessed() const;

    int getFps() const;


    virtual void handleKeyPress(int key);
};


#endif //GESTURES_VIDEOPROCESSOR_H
