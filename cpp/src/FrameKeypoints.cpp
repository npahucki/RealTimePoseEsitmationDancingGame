//
// Created by Nathan  Pahucki on 10/22/18.
//

#include <FrameKeypoints.h>
#include <openpose/headers.hpp>

#include "FrameKeypoints.h"

FrameKeypoints::FrameKeypoints(const int &saveNFrames) : saveNFrames_(saveNFrames) {
    // Need at least one for the getLastFrame related stuff to work.
    assert(saveNFrames_ > 0);
}

const op::Array<float> &FrameKeypoints::getLastFrameKeypoints() const {
    return lastNFrameKeypoints_.back();
}

const op::Array<float> &FrameKeypoints::getCurrentFrameKeypoints() const {
    return currentFrameKeypoints_;
}

int FrameKeypoints::getClosestPersonInLastFrame(int personIdx) const {
    int closestPerson = -1;
    const auto numPersonsInLastFrame = getNumPersonsInLastFrame();
    if(numPersonsInLastFrame == getNumPersonsInCurrentFrame()) {
        float lastDistance = FLT_MAX;
        for (int i = 0; i < numPersonsInLastFrame; i++) {
            const auto distance = distSinceLastFrame(personIdx, i);
            if (distance >= 0 && distance < lastDistance) {
                closestPerson = i;
                lastDistance = distance;
            }
        }

        if (closestPerson == -1) {
            std::cout << "No closest person found for person " << personIdx << std::endl;
        }
    }

    return closestPerson;
}

void FrameKeypoints::update(const op::Array<float> &keypoints, float smoothingWeight) {
    currentFrameNumber_++;
    lastNFrameKeypoints_.push_back(currentFrameKeypoints_);
    if (lastNFrameKeypoints_.size() > saveNFrames_) {
        lastNFrameKeypoints_.pop_front();
    }
    currentFrameKeypoints_ = keypoints;
    unsmoothedCurrentFrameKeypoints_ = keypoints;
    smoothKeypoints(smoothingWeight);
}

const std::deque<op::Array<float>> &FrameKeypoints::getLastNFrames() const {
    return lastNFrameKeypoints_;
}

int FrameKeypoints::getNumPersonsInCurrentFrame() const {
    return getCurrentFrameKeypoints().getSize(0);
}

int FrameKeypoints::getNumPersonsInLastFrame() const {
    return getLastFrameKeypoints().getSize(0);
}

const unsigned long FrameKeypoints::getCurrentFrameNumber() const {
    return currentFrameNumber_;
}

const cv::Point
FrameKeypoints::getBodyPartPoint(const int &person, const int &part, const FrameKeypoints::FrameType &type) const {
    if (type == FrameType::CURRENT_UNSMOOTHED) {
        auto keypoints = getUnsmoothedCurrentFrameKeypoints();
        const auto confidence = keypoints[{person, part, 2}];
        if(confidence < 0.6) return cv::Point{0,0};
        return cv::Point{(int) keypoints[{person, part, 0}], (int) keypoints[{person, part, 1}]};
    } else if (type == FrameType::LAST) {
        auto keypoints = getLastFrameKeypoints();
        const auto confidence = keypoints[{person, part, 2}];
        if(confidence < 0.6) return cv::Point{0,0};
        return cv::Point{(int) keypoints[{person, part, 0}], (int) keypoints[{person, part, 1}]};
    } else {
        auto keypoints = getCurrentFrameKeypoints();
        const auto confidence = keypoints[{person, part, 2}];
        if(confidence < 0.6) return cv::Point{0,0};
        return cv::Point{(int) keypoints[{person, part, 0}], (int) keypoints[{person, part, 1}]};
    }
}

const double FrameKeypoints::getDistanceBetweenParts(const int &person, const int &part1, const int &part2) const {
    return op::getDistance(getCurrentFrameKeypoints(), person, part1, part2);
}

void FrameKeypoints::smoothKeypoints(const float &smoothingWeight) {
    if (smoothingWeight != 0 && getNumPersonsInCurrentFrame() == getNumPersonsInLastFrame()) {
        // TODO: might need to do take into account the condifence and maybe not smooth new points that are not high enough confidence?
        for (int personIdx = 0; personIdx < getNumPersonsInCurrentFrame(); personIdx++) {
            const auto matchingPersonInLastFrame = getClosestPersonInLastFrame(personIdx);
            if (matchingPersonInLastFrame != -1) {
                for (int part = 0; part < getCurrentFrameKeypoints().getSize(1); part++) {
                    const auto curPartPoint = getBodyPartPoint(personIdx, part, FrameType::CURRENT);
                    const auto lastPartPoint = getBodyPartPoint(matchingPersonInLastFrame, part, FrameType::LAST);

                    // If any of the points are 0 (not detected), then don't try to smooth this point at all
                    // TODO: maybe look at confidence instead since a part could genuinely be located at coordinate 0,0?
                    if ((curPartPoint.x > 0 || curPartPoint.y > 0) && (lastPartPoint.x > 0 || lastPartPoint.y > 0)) {
                        currentFrameKeypoints_[{personIdx, part, 0}] = smoothInt(curPartPoint.x, lastPartPoint.x,
                                                                                 smoothingWeight);
                        currentFrameKeypoints_[{personIdx, part, 1}] = smoothInt(curPartPoint.y, lastPartPoint.y,
                                                                                 smoothingWeight);
                    }
                }
            }
        }

    }
}

const op::Array<float> &FrameKeypoints::getUnsmoothedCurrentFrameKeypoints() const {
    return unsmoothedCurrentFrameKeypoints_;
}

int FrameKeypoints::distSinceLastFrame(const int &personCurrentFrame, const int &personLastFrame, const int &part) const {
    assert(personCurrentFrame < getNumPersonsInCurrentFrame() && personLastFrame < getNumPersonsInLastFrame());

    const auto p1 = getBodyPartPoint(personCurrentFrame, part, FrameType::CURRENT);
    const auto p2 = getBodyPartPoint(personLastFrame, part, FrameType::LAST);
    if ((p1.x > 0 && p1.y > 0) || (p2.x > 0 && p2.y > 0)) {
        return (int) std::abs(std::sqrt(std::pow(p2.x - p1.x, 2) + std::pow(p2.y - p1.y, 2)));
    }

    return -1;

}

int FrameKeypoints::distSinceLastFrame(const int &personCurrentFrame, const int &personLastFrame) const {
    long dist = -1;
    if(personCurrentFrame < getNumPersonsInCurrentFrame() && personLastFrame < getNumPersonsInLastFrame()) {
        dist = 0;
        const int numParts = getCurrentFrameKeypoints().getSize(1);
        for(int part = 0; part < numParts; part++) {
            const auto partDist = distSinceLastFrame(personCurrentFrame, personLastFrame, part);
            dist+= partDist > 0 ? partDist : 0;
        }
    }
    return (int)dist;
}






