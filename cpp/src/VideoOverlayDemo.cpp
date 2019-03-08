//
// Created by Nathan  Pahucki on 10/11/18.
//

#include <iostream>
#include "VideoOverlayDemo.h"
#include "UpperBodyRaisedHandPredictor.h"
#include "extract_utils.h"
#include <openpose/tracking/headers.hpp>
#include <VideoOverlayDemo.h>


VideoOverlayDemo::VideoOverlayDemo(const string &windowName_) : VideoPoseCapture(windowName_) {}

VideoOverlayDemo::~VideoOverlayDemo() {
    delete personIdentifier_;
    delete scoreTracker_;
    delete storage_;
    delete frameKeypoints_;
    delete themeManager_;
}

cv::Mat VideoOverlayDemo::getRenderBaseImage(const cv::Mat &inputImage) {
    return themeManager_->getBackgroundImage(currentBackgroundIdx_);
}

void VideoOverlayDemo::handleKeyPress(int key) {
    if (key == 's') {
        if (smoothingWeight_ != 0) enableSmoothing_ = !enableSmoothing_;
        std::cout << "Smoothing enabled: " << (enableSmoothing_ ? "YES" : "NO") << std::endl;
    }
}

bool VideoOverlayDemo::processKeypoints(const op::Array<float> &originalKeypoints, const cv::Mat &inputImage,
                                        cv::Mat &outputImage) {



    // Make sure to write to the DB periodically to ensure that in the event of a crash we don't lose the data.
    if (getFramesProcessed() % (30/* assumed fps*/ * 60/*secs*/ * 1/*mins*/) == 0) {
        currentBackgroundIdx_ = themeManager_->getNextBackgroundNumber();
        storage_->flush();
    }

    if (getFramesProcessed() % (30/* assumed fps*/ * 10/*secs*/) == 0) {
        updateLeaderBoard();
    }

        // Include the FPS on the image
    std::stringstream fpsText;
    fpsText << std::setprecision(4) << getFps() << " fps";
    cv::putText(outputImage, fpsText.str(), cv::Point(10, 30), cv::FONT_HERSHEY_PLAIN, 1.5F, cv::Scalar(255, 255, 255));

    const int numPersons = frameKeypoints_->getNumPersonsInCurrentFrame();
    const auto personIds = this->personIdentifier_->identifyPersons(*frameKeypoints_, inputImage);

    for (int personIdx = 0; personIdx < numPersons; personIdx++) {
        try {
            const auto personId = personIds[personIdx];
            const auto personInfo = storage_->lookupPerson(personId);


            const auto nose = frameKeypoints_->getBodyPartPoint(personIdx, 0);
            const auto neck = frameKeypoints_->getBodyPartPoint(personIdx, 1);
            const auto leftEye = frameKeypoints_->getBodyPartPoint(personIdx, 16);
            const auto rightEye = frameKeypoints_->getBodyPartPoint(personIdx, 15);
            if (nose.x == 0 || neck.x == 0) continue;


            cv::Mat &headImage = themeManager_->getHeadImage(personInfo.head);
            cv::Mat imageToOverlay;

            // First scale it correctly
            // This is a fairly reliable metric that we can use for scaling
            const auto neckToNoseDist = frameKeypoints_->getDistanceBetweenParts(personIdx, 0, 1);
            // TODO: This depend on how much padding there is around the image
            // TODO: WE should define some standard for the padding on the head images.
            const double scale = (neckToNoseDist / headImage.cols) * 2.0;
            cv::resize(headImage, imageToOverlay, cv::Size(), scale, scale);

            if (rightEye.x > 0 && leftEye.x > 0) {
                const double angle = atan2(leftEye.y - rightEye.y, leftEye.x - rightEye.x);
                const auto degrees = static_cast<float>(angle * -180 / M_PI);
                const auto center = cv::Point{imageToOverlay.cols / 2, imageToOverlay.rows / 2};
                auto m = cv::getRotationMatrix2D(center, degrees, 1.0);
                auto longestEdge = max(imageToOverlay.cols,
                                       imageToOverlay.rows); // TODO: figure out how to not cut off edges when rotated
                // NOTE: Trying to do a rotate and scale at the same time with warpAffine does not seem to work,
                // the image_ is drawn with the 0,0 coordinates off the canvas
                cv::warpAffine(imageToOverlay, imageToOverlay, m, cv::Size{longestEdge, longestEdge});
            }

            const auto imageToOverlayCenter = cv::Point{imageToOverlay.cols / 2, imageToOverlay.rows / 2};
            overlayImage(outputImage, imageToOverlay, outputImage, nose - imageToOverlayCenter);

            //////////////////// Score and name stuff /////////////////////
            std::string displayText = "-----";
            if (!personId.empty()) {
                // Render score
                const int score = scoreTracker_->computeNewScoreForPerson(*frameKeypoints_, personIdx, personId);
                displayText = "00000";
                std::ostringstream ss;
                ss << personInfo.name << " - " << std::setw(5) << std::setfill('0') << score << " pts";
                displayText = ss.str();
                // Update leader board in realtime
                for(auto &line: leaderStats_) {
                    if(line.first ==  personInfo.name) {
                        line.second = std::to_string(score);
                        break;
                    }
                }
            }

            const int fontFace = cv::FONT_HERSHEY_PLAIN;
            const double fontScale = 1.25;
            const int thickness = 2;
            int baseline = 0;
            const auto textSize = cv::getTextSize(displayText, fontFace, fontScale, thickness, &baseline);
            baseline += thickness;
            const cv::Point textOrg(nose.x - textSize.width / 2, std::max(0, nose.y - imageToOverlay.rows / 2));
            cv::putText(outputImage, displayText, textOrg, fontFace, fontScale, cv::Scalar(0, 255, 0), thickness);
        } catch (const std::exception &e) {
            std::cerr << "Exception processing person " << personIdx << ". Error:" << e.what();
            continue;
        }
    }

    //            //////////////// Leader board ///////////////////
    int totalWidth = 0, totalHeight = 0;
    const int fontFace = cv::FONT_HERSHEY_PLAIN;
    const double fontScale = 1.0;
    const int thickness = 1;
    const int padding = 3;
    for(auto line: leaderStats_) {
        int baseline = 0;
        const auto nameSize = cv::getTextSize(line.first, fontFace, fontScale, thickness, &baseline);
        const auto scoreSize = cv::getTextSize(line.second, fontFace, fontScale, thickness, &baseline);
        totalWidth = std::max(totalWidth, nameSize.width + scoreSize.width + 4*padding);
        totalHeight += std::max(nameSize.height, scoreSize.height) + 2*padding;
    }
    // Now that we kow the full width and height, we can actually draw
    if(totalWidth < 200) totalWidth = 300;
    if(totalHeight < 200) totalHeight = 200;

    const auto boardRect = cv::Rect(outputImage.cols - (totalWidth + 10), 10, totalWidth, totalHeight + padding*4);
    cv::GaussianBlur(outputImage(boardRect), outputImage(boardRect), cv::Size(0, 0), 4);
    cv::rectangle(outputImage,boardRect,cv::Scalar(0, 255, 0),1);
    // Write the names
    int textYPosition = boardRect.y + padding;
    for(auto line: leaderStats_) {
        int baseline = 0;
        const auto nameSize = cv::getTextSize(line.first, fontFace, fontScale, thickness, &baseline);
        const auto scoreSize = cv::getTextSize(line.second, fontFace, fontScale, thickness, &baseline);
        textYPosition += std::max(nameSize.height, scoreSize.height) + padding*2;
        const cv::Point nameOrg(boardRect.x + padding   , textYPosition);
        const cv::Point scoreOrg(boardRect.x + boardRect.width - (scoreSize.width + padding), textYPosition);
        cv::putText(outputImage, line.first, nameOrg, fontFace, fontScale, cv::Scalar(0, 255, 0), thickness);
        cv::putText(outputImage, line.second, scoreOrg, fontFace, fontScale, cv::Scalar(0, 255, 0), thickness);
    }

    return true;
}

void VideoOverlayDemo::updateLeaderBoard() {
    typedef PersonStorage::PersonData pd;

    std::vector<pd> players;
    leaderStats_.clear();

    for (auto tuple : *(storage_->getMap())) {
        players.push_back(tuple.second);
    }

    std::sort(std::begin(players), std::end(players), [](pd &a, pd &b) { return a.score > b.score; });
    const size_t max = 10;
    auto end = std::next(players.begin(), std::min(max, players.size()));
    for(auto it = players.begin(); it != end; it++)    {
        leaderStats_.emplace_back((*it).name,std::to_string((*it).score));
    }
}


void VideoOverlayDemo::renderPose(op::PoseRenderer *renderer, const op::Array<float> &poseKeypoints,
                                  double scaleInputToOutput, op::Array<float> &outputArray) {
    frameKeypoints_->update(poseKeypoints, enableSmoothing_ ? smoothingWeight_ : 0);
    renderer->renderPose(outputArray, filterKeypointsToRender(frameKeypoints_->getCurrentFrameKeypoints()),
                         scaleInputToOutput);
    // This creates a 'thicker' effect
    if (renderLastNFrames_) {
        int idx = (int) frameKeypoints_->getLastNFrames().size();
        for (const auto &pastPoseKeypoints: frameKeypoints_->getLastNFrames()) {
            if (isUseGpuRendering()) {
                ((op::PoseGpuRenderer *) renderer)->setAlphaKeypoint(1.0F / (1.5F * idx--));
            }
            renderer->renderPose(outputArray, filterKeypointsToRender(pastPoseKeypoints), scaleInputToOutput);
        }
    }
}

op::Array<float> VideoOverlayDemo::filterKeypointsToRender(const op::Array<float> &keypoints) {
    const auto numPersons = keypoints.getSize(0);
    auto filtered = keypoints.clone();
    for (int i = 0; i < numPersons; i++) {
        filtered[{i, 15, 2}] = 0;
        filtered[{i, 16, 2}] = 0;
        filtered[{i, 17, 2}] = 0;
        filtered[{i, 18, 2}] = 0;
        //filtered[{i,0,2}] = 0;
    }

    return filtered;
}

void VideoOverlayDemo::start() {
    // Should not have been initialized already
    assert(this->themeManager_ == nullptr);

    UpperBodyRaisedHandPredictor predictor{getModelFolder()};

    // This is all cleaned up in destructor
    themeManager_ = new ThemeManager(themeName_, cv::Size{getOutputSize().x, getOutputSize().y});
    frameKeypoints_ = new FrameKeypoints(max(1, renderLastNFrames_));
    storage_ = new PersonStorage(dbFolderPath_ + themeName_ + "_persons.bdb");
    scoreTracker_ = new ScoreTracker(&predictor, storage_);
    personIdentifier_ = new PersonIdentifier(getModelFolder(), storage_, themeManager_, faceIdFrameInterval_);

    enableSmoothing_ = smoothingWeight_ > 0;



    VideoPoseCapture::start();
}



void VideoOverlayDemo::setFaceIdFrameInterval(int faceIdFrameInterval) {
    faceIdFrameInterval_ = faceIdFrameInterval;
}

void VideoOverlayDemo::setThemeName(std::string themeName) {
    themeName_ = themeName;
}

void VideoOverlayDemo::setSmoothingWeight(float smoothingWeight) {
    smoothingWeight_ = smoothingWeight;
}

void VideoOverlayDemo::setRenderLastNFrames(int renderLastNFrames) {
    VideoOverlayDemo::renderLastNFrames_ = renderLastNFrames;
}

void VideoOverlayDemo::setDbFilePath(const string &dbFilePath) {
    dbFolderPath_ = dbFilePath;
}


