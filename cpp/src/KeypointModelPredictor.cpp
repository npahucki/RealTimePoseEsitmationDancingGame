//
// Created by Nathan  Pahucki on 10/11/18.
//

#include "KeypointModelPredictor.h"
#include <KeypointModelPredictor.h>
#include <extract_utils.h>

KeypointModelPredictor::KeypointModelPredictor(const std::vector<int> &keypointFilter, const std::string &modelFolder,
                                               const std::string &modelName) : keypointFilter_(keypointFilter) {

// TODO: This would not compiler, do it right for this to work on windows
//    fs::path dir (modelFolder);
//    fs::path file (modelName);
//    fs::path full_path = dir / file;
    std::string path = modelFolder + modelName;

    if(!model_.LoadModel(path)) {
        throw std::runtime_error("Could not open model at " + path);
    }
}

std::vector<float> KeypointModelPredictor::predict(const op::Array<float> &keypoints, const int &person) {
    // Create a 1D Tensor on length 10 for input data.
    Tensor in((int)keypointFilter_.size());
    std::vector<float> flattenedPoints;
    flattenKeyPoints(keypoints, person, keypointFilter_, flattenedPoints);
    normalizeVector(flattenedPoints);
    in.data_ = flattenedPoints;

    // Run prediction.
    Tensor out;
    model_.Apply(&in, &out);
    return out.data_;
}

void KeypointModelPredictor::normalizeVector(std::vector<float> &target) {
    double totalOfSquares = 0;
    for (auto& n : target) totalOfSquares += pow(n,2);
    auto divisor = (float) sqrt(totalOfSquares);
    for (auto &i : target) i = i / divisor;
}
