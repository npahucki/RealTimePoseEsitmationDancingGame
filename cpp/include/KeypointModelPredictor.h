//
// Created by Nathan  Pahucki on 10/11/18.
//

#ifndef GESTURES_KEYPOINTMODELPREDICTOR_H
#define GESTURES_KEYPOINTMODELPREDICTOR_H

#include <openpose/utilities/headers.hpp>
#include <boost/serialization/vector.hpp>
#include "keras_model.h"

class KeypointModelPredictor {

private:
    const std::vector<int> keypointFilter_;
    KerasModel model_;

protected:
    /**
     * Performs L2 normailzation on the vector so it can be fed to kerasify and used in a model.
     * @param target The vector in which to normalize the values
     */
    void normalizeVector(std::vector<float> &target);

public:
    KeypointModelPredictor(const std::vector<int> &keypointFilter, const std::string &modelFolder,
                           const std::string &modelName);

    /**
     * Runs the prediction and returns a vector of probabilities for each category in the model.
     * @param keypoints The raw keypoints to feed to the model - these will be L2 normalized
     * before passing to the model.
     * @return a vector with a length equal to the number of possible categories in the model,
     * the value being the probability for each category.
     */
    std::vector<float> predict(const op::Array<float> &keypoints, const int &person);

};


#endif //GESTURES_KEYPOINTMODELPREDICTOR_H
