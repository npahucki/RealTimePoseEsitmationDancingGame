//
// Created by Nathan  Pahucki on 10/9/18.
//

#ifndef GESTURES_VIDEOPOSEDATACAPTURE_H
#define GESTURES_VIDEOPOSEDATACAPTURE_H


#include "VideoProcessor.h"

using namespace std;

class VideoPoseCapture : public VideoProcessor  {

private:
    string modelFolder_ = "models/";
    float renderThreshold_ = 0.05;
    bool disableBlending_ = false;
    bool useGpuRendering = false;
    int keepMaxPeople_ = -1;

private:
    float alphaPose_ = 0.6;
    int numGpuStart_ = 0;
    op::Point<int> outputSize_{-1, -1};
    op::Point<int> netInputSize_{-1, 368};

    op::PoseRenderer *poseRenderer_ = nullptr;
    op::PoseExtractorCaffe *poseExtractorCaffe_ = nullptr;
    op::KeepTopNPeople *keepTopNPeopleFilter_ = nullptr;
    op::ScaleAndSizeExtractor *scaleAndSizeExtractor_ = nullptr;
    op::CvMatToOpInput *cvMatToOpInput_ = nullptr;
    op::CvMatToOpOutput *cvMatToOpOutput_ = nullptr;
    op::OpOutputToCvMat *opOutputToCvMat_ = nullptr;


public:
    void setKeepMaxPeople(int keepMaxPeople);
    void setModelFolder(const string &modelFolder);
    void setRenderThreshold(float renderThreshold);
    void setDisableBlending(bool disableBlending);
    void setAlphaPose(float alphaPose);
    void setNumGpuStart(int numGpuStart);
    const string &getModelFolder() const;
    const op::Point<int> &getOutputSize() const;
    bool isUseGpuRendering() const;
    void setUseGpuRendering(bool useGpuRendering);
    bool isDisableBlending() const;
    void setOutputSize(const op::Point<int> &outputSize);
    void setNetInputSize(const op::Point<int> &netInputSize);
    explicit VideoPoseCapture(const std::string &windowName_);
    int getNumGpuStart() const;

    void start() override;
    virtual ~VideoPoseCapture();

protected:
    bool processFrame(const int &key, const cv::Mat &inputImage, cv::Mat &outputImage) override;

    virtual bool processKeypoints(const op::Array<float> &keypoints, const cv::Mat &inputImage, cv::Mat &outputImage);
    virtual cv::Mat getRenderBaseImage(const cv::Mat &inputImage);

    virtual void renderPose(op::PoseRenderer *renderer, const op::Array<float> &poseKeypoints, double scaleInputToOutput, op::Array<float> &outputArray);
};


#endif //GESTURES_VIDEOPOSEDATACAPTURE_H
