//
// Created by Nathan  Pahucki on 10/19/18.
//

#include "ScoreTracker.h"
#include "FrameKeypoints.h"

ScoreTracker::ScoreTracker(UpperBodyRaisedHandPredictor *predictor, PersonStorage *storage)
        : predictor_(predictor), storage_(storage) {}

int ScoreTracker::computeNewScoreForPerson(const FrameKeypoints &frameKeypoints, const int &personIdx, const std::string personId) {


    auto personInfo = storage_->lookupPerson(personId);

    if(!armInfo_.count(personId)) armInfo_[personId] = ArmRaised::NONE;
    auto lastArmState = armInfo_[personId];

    const ArmRaised currentArmState = predictor_->predictRasiedArm(frameKeypoints.getCurrentFrameKeypoints(), personIdx);

    // TODO: predict BOTH arms up
    int additionalScore = 0;
    if(lastArmState == ArmRaised::LEFT && currentArmState == ArmRaised::RIGHT) {
        additionalScore += 100;
    } else if(lastArmState == ArmRaised::RIGHT && currentArmState == ArmRaised::LEFT) {
        additionalScore += 100;
    } else if(lastArmState == ArmRaised::NONE && currentArmState != ArmRaised::NONE) {
        additionalScore += 10;
    }

    armInfo_[personId] = currentArmState;

    if(additionalScore > 0) {
        personInfo.score += additionalScore;
        storage_->storePerson(personId, personInfo);
    }

    return personInfo.score;
}

