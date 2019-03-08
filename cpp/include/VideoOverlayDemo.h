//
// Created by Nathan  Pahucki on 10/11/18.
//

#ifndef GESTURES_VIDEOOVERLAYDEMO_H
#define GESTURES_VIDEOOVERLAYDEMO_H


#include "VideoPoseCapture.h"
#include "FrameKeypoints.h"
#include "PersonIdentifier.h"
#include "ScoreTracker.h"
#include "PersonStorage.h"
#include "ThemeManager.h"


class VideoOverlayDemo : public VideoPoseCapture {

private:
    int faceIdFrameInterval_ = 5;
    bool enableSmoothing_ = false;
    float smoothingWeight_ = 0.0;
    int renderLastNFrames_ = 0;
    int currentBackgroundIdx_ = 0;
    std::string dbFolderPath_ = "";
    std::vector<std::pair<std::string, std::string>> leaderStats_;

    std::string themeName_ = "space";

    ScoreTracker *scoreTracker_ = nullptr;
    PersonIdentifier *personIdentifier_ = nullptr;
    FrameKeypoints *frameKeypoints_ = nullptr;
    PersonStorage *storage_ = nullptr;
    ThemeManager *themeManager_ = nullptr;


    void updateLeaderBoard();


public:
    void setDbFilePath(const string &dbFilePath);
    void setRenderLastNFrames(int renderLastNFrames);
    void setSmoothingWeight(float smoothingWeight);
    void setThemeName(std::string themeName);
    void setFaceIdFrameInterval(int faceIdFrameInterval);
    void start() override;
    VideoOverlayDemo(const string &windowName_);

    void handleKeyPress(int key) override;

    virtual ~VideoOverlayDemo();

protected:
    bool processKeypoints(const op::Array<float> &keypoints, const cv::Mat &inputImage, cv::Mat &outputImage) override;

    op::Array<float> filterKeypointsToRender(const op::Array<float> &keypoints);

    cv::Mat getRenderBaseImage(const cv::Mat &inputImage) override;

    void renderPose(op::PoseRenderer *renderer, const op::Array<float> &poseKeypoints, double scaleInputToOutput,
                    op::Array<float> &outputArray) override;

};


#endif //GESTURES_VIDEOOVERLAYDEMO_H
