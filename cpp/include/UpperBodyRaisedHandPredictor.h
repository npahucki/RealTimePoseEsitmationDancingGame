//
// Created by Nathan  Pahucki on 10/11/18.
//

#ifndef GESTURES_UPPERBODYRAISEDHANDPREDICTOR_H
#define GESTURES_UPPERBODYRAISEDHANDPREDICTOR_H


#include "KeypointModelPredictor.h"

enum class ArmRaised { NONE = 0, RIGHT, LEFT };


class UpperBodyRaisedHandPredictor : KeypointModelPredictor {
public:
    UpperBodyRaisedHandPredictor(const std::string &modelFolder);

    ArmRaised predictRasiedArm(const op::Array<float> &keypoints, const int &person);
};





#endif //GESTURES_UPPERBODYRAISEDHANDPREDICTOR_H
