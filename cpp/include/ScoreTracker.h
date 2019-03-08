//
// Created by Nathan  Pahucki on 10/19/18.
//

#ifndef GESTURES_SCORETRACKER_H
#define GESTURES_SCORETRACKER_H


#include "UpperBodyRaisedHandPredictor.h"
#include "FrameKeypoints.h"
#include "PersonStorage.h"


class ScoreTracker {
public:
    explicit ScoreTracker(UpperBodyRaisedHandPredictor *predictor, PersonStorage *storage);

private:
    PersonStorage * storage_ = nullptr;
    UpperBodyRaisedHandPredictor *predictor_ = nullptr;
    std::map<std::string, ArmRaised> armInfo_;

public:
    /**
     * Computes a new score based on the movements.
     * @param keypoints The keypoints for the current frame
     * @param personIdx The person of interest in the keypoints.
     * @param personId The unique identifier for the person.
     * @return The updated score or the old score if no new points earned
     */
    int computeNewScoreForPerson(const FrameKeypoints &frameKeypoints, const int& personIdx, const std::string personId);

};


#endif //GESTURES_SCORETRACKER_H
