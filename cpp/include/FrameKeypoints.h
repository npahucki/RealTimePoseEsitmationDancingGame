//
// Created by Nathan  Pahucki on 10/22/18.
//

#ifndef GESTURES_FRAMESMOOTHER_H
#define GESTURES_FRAMESMOOTHER_H


#include <openpose/core/array.hpp>
#include <deque>

#define smoothInt(xa, xb, w) (int)(xa * w + xb * (1 - w))

class FrameKeypoints {

private:
    op::Array<float> currentFrameKeypoints_;
    op::Array<float> unsmoothedCurrentFrameKeypoints_;
    std::deque<op::Array<float>> lastNFrameKeypoints_;
    const int saveNFrames_;

    unsigned long currentFrameNumber_ = 0;

    void smoothKeypoints(const float &smoothingWeight);

    int distSinceLastFrame(const int &personCurrentFrame, const int &personLastFrame, const int &part) const;
    int distSinceLastFrame(const int &personCurrentFrame, const int &personLastFrame) const;

public:

    enum FrameType {
        CURRENT = 0,
        CURRENT_UNSMOOTHED,
        LAST
    };

    FrameKeypoints(const int &saveNFrames);

    const unsigned long getCurrentFrameNumber() const;
    const std::deque<op::Array<float>> &getLastNFrames() const;
    const op::Array<float> &getLastFrameKeypoints() const;
    const op::Array<float> &getCurrentFrameKeypoints() const;
    const op::Array<float> &getUnsmoothedCurrentFrameKeypoints() const;

    int getClosestPersonInLastFrame(int personIdx) const;
    int getNumPersonsInCurrentFrame()const;
    int getNumPersonsInLastFrame() const;
    const cv::Point getBodyPartPoint(const int &person, const int &part, const FrameType &type = FrameType::CURRENT) const;
    const double getDistanceBetweenParts(const int &person, const int &part1, const int &part2) const;
    void update(const op::Array<float> &keypoints, float smoothingWeight);

};


#endif //GESTURES_FRAMESMOOTHER_H
