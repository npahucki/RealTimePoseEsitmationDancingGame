//
// Created by Nathan  Pahucki on 10/9/18.
//

#include "VideoPoseCapture.h"
#include <iostream>
#include <assert.h>
#include <VideoPoseCapture.h>

VideoPoseCapture::VideoPoseCapture(const std::string &windowName_) : VideoProcessor(windowName_) {}

VideoPoseCapture::~VideoPoseCapture() {
    delete opOutputToCvMat_;
    delete cvMatToOpOutput_;
    delete cvMatToOpInput_;
    delete scaleAndSizeExtractor_;
    delete poseExtractorCaffe_;
    delete keepTopNPeopleFilter_;

    if (useGpuRendering) {
        delete (op::PoseGpuRenderer *) poseRenderer_;
    } else {
        delete (op::PoseCpuRenderer *) poseRenderer_;
    }
}

void VideoPoseCapture::start() {

    assert(poseRenderer_ == nullptr && "Already started!");
    if (poseRenderer_ != nullptr) return;

    const auto poseModel = op::PoseModel::BODY_25;

    if (useGpuRendering) {
        auto poseExtractorPtr = std::make_shared<op::PoseExtractorCaffe>(poseModel, getModelFolder(), getNumGpuStart());
        const auto gpuRenderer = new op::PoseGpuRenderer(poseModel, poseExtractorPtr, renderThreshold_,
                                                         !disableBlending_, 0.2, 0.0);
        gpuRenderer->setElementToRender(0);
        poseRenderer_ = gpuRenderer;
    } else {
        poseRenderer_ = new op::PoseCpuRenderer(poseModel, renderThreshold_, !disableBlending_, 1.0);
    }

    keepTopNPeopleFilter_ = new op::KeepTopNPeople(keepMaxPeople_);
    poseExtractorCaffe_ = new op::PoseExtractorCaffe(poseModel, modelFolder_, numGpuStart_);
    scaleAndSizeExtractor_ = new op::ScaleAndSizeExtractor(netInputSize_, outputSize_, 1, 0.3);
    cvMatToOpInput_ = new op::CvMatToOpInput(poseModel);
    cvMatToOpOutput_ = new op::CvMatToOpOutput;
    opOutputToCvMat_ = new op::OpOutputToCvMat;

    poseExtractorCaffe_->initializationOnThread();
    poseRenderer_->initializationOnThread();

    // Start the processing loop
    VideoProcessor::start();
}

bool VideoPoseCapture::processFrame(const int &key, const cv::Mat &inputImage, cv::Mat &outputImage) {
    // -------------------------------------------------------
    // Format input image to OpenPose input and output formats
    // -------------------------------------------------------
    const op::Point<int> imageSize{inputImage.cols, inputImage.rows};
    std::vector<double> scaleInputToNetInputs;
    std::vector<op::Point<int>> netInputSizes;
    double scaleInputToOutput;
    op::Point<int> outputResolution;
    std::tie(scaleInputToNetInputs, netInputSizes, scaleInputToOutput,
             outputResolution) = scaleAndSizeExtractor_->extract(imageSize);
    const auto netInputArray = cvMatToOpInput_->createArray(inputImage, scaleInputToNetInputs, netInputSizes);
    poseExtractorCaffe_->forwardPass(netInputArray, imageSize, scaleInputToNetInputs);


    const auto kp = poseExtractorCaffe_->getPoseKeypoints();
    const auto numPeople = kp.getSize(0);
    // NOTE: There is a bug in open pose code that fails if this check is not done.
    // See PR: https://github.com/CMU-Perceptual-Computing-Lab/openpose/pull/913
    auto poseKeypoints = keepMaxPeople_ < 1 || numPeople <= keepMaxPeople_ ? kp :
            keepTopNPeopleFilter_->keepTopPeople(kp, poseExtractorCaffe_->getPoseScores());

    auto outputArray = cvMatToOpOutput_->createArray(getRenderBaseImage(inputImage), scaleInputToOutput,
                                                     outputResolution);
    renderPose(poseRenderer_, poseKeypoints, scaleInputToOutput, outputArray);
    outputImage = opOutputToCvMat_->formatToCvMat(outputArray);
    return processKeypoints(poseKeypoints, inputImage, outputImage);
}

void VideoPoseCapture::renderPose(op::PoseRenderer *renderer, const op::Array<float> &poseKeypoints,
                                  double scaleInputToOutput, op::Array<float> &outputArray) {

    poseRenderer_->renderPose(outputArray, poseKeypoints, scaleInputToOutput);
}

cv::Mat VideoPoseCapture::getRenderBaseImage(const cv::Mat &inputImage) {
    return inputImage;
}

bool
VideoPoseCapture::processKeypoints(const op::Array<float> &keypoints, const cv::Mat &inputImage, cv::Mat &outputImage) {
    return true;
}

void VideoPoseCapture::setModelFolder(const string &modelFolder) {
    modelFolder_ = modelFolder;
}

void VideoPoseCapture::setRenderThreshold(float renderThreshold) {
    renderThreshold_ = renderThreshold;
}

void VideoPoseCapture::setDisableBlending(bool disableBlending) {
    disableBlending_ = disableBlending;
}

void VideoPoseCapture::setAlphaPose(float alphaPose) {
    alphaPose_ = alphaPose;
}

void VideoPoseCapture::setNumGpuStart(int numGpuStart) {
    numGpuStart_ = numGpuStart;
}

int VideoPoseCapture::getNumGpuStart() const {
    return numGpuStart_;
}

void VideoPoseCapture::setOutputSize(const op::Point<int> &outputSize) {
    outputSize_ = outputSize;
}

void VideoPoseCapture::setNetInputSize(const op::Point<int> &netInputSize) {
    netInputSize_ = netInputSize;
}

const string &VideoPoseCapture::getModelFolder() const {
    return modelFolder_;
}

const op::Point<int> &VideoPoseCapture::getOutputSize() const {
    return outputSize_;
}

bool VideoPoseCapture::isDisableBlending() const {
    return disableBlending_;
}

void VideoPoseCapture::setUseGpuRendering(bool useGpuRendering) {
    VideoPoseCapture::useGpuRendering = useGpuRendering;
}

bool VideoPoseCapture::isUseGpuRendering() const {
    return useGpuRendering;
}

void VideoPoseCapture::setKeepMaxPeople(int keepMaxPeople) {
    keepMaxPeople_ = keepMaxPeople;
}


