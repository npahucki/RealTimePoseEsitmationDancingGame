//
// Created by Nathan  Pahucki on 10/11/18.
//

#include <UpperBodyRaisedHandPredictor.h>

#define UPPER_BODY_KEY_POINTS std::vector<int>{0,1,2,3,4,5,6,7}
#define UPPER_BODY_RAISED_HAND_MODEL_NAME "raised-hands-upper-body-only.model"

UpperBodyRaisedHandPredictor::UpperBodyRaisedHandPredictor(const std::string &modelFolder)
        : KeypointModelPredictor(UPPER_BODY_KEY_POINTS, modelFolder, UPPER_BODY_RAISED_HAND_MODEL_NAME) {}

ArmRaised UpperBodyRaisedHandPredictor::predictRasiedArm(const op::Array<float> &keypoints, const int &person) {
    // TODO: Maybe some better training would let us avoid this
    // the model seems to vote for left hand when both hands are missing
    if(keypoints[{person, 4, 0}] == 0.0 && keypoints[{person, 7, 0}] == 0.0) {
        return ArmRaised::NONE;
    }

    const std::vector<float> data = predict(keypoints, person);
    const auto itMax = max_element(data.begin(),data.end());
    const int  index = (int) std::distance(data.begin(), itMax);
    // TODO: Determine minimum confidence - however via kerasify these numbers seem whack-o
    //const float maxValue = *itMax;
    return (ArmRaised)index;
}
