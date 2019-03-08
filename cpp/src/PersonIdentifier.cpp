//
// Created by Nathan  Pahucki on 10/17/18.
//



#include "PersonIdentifier.h"
#include "FrameKeypoints.h"

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/random_generator.hpp>
#include <dlib/opencv.h>
#include <algorithm>
#include <opencv2/core/mat.hpp>
#include <PersonStorage.h>
#include <ThemeManager.h>

#define THRESHOLD 0.6

using namespace dlib;


PersonIdentifier::PersonIdentifier(const std::string &modelFolder, PersonStorage *storage,
                                   ThemeManager *themeManager, const int &updateInterval) {

    deserialize(modelFolder + "shape_predictor_5_face_landmarks.dat") >> sp_;
    deserialize(modelFolder + "dlib_face_recognition_resnet_model_v1.dat") >> net_;

    updateInterval_ = updateInterval;
    detector_ = get_frontal_face_detector();
    storage_ = storage;
    themeManager_ = themeManager;
}

std::vector<std::string>
PersonIdentifier::identifyPersons(const FrameKeypoints &frameKeypoints, const cv::Mat &inputImage) {

    const int numPersons = frameKeypoints.getNumPersonsInCurrentFrame();
    std::vector<std::string> personIds;
    personIds.assign((unsigned long) numPersons, "");

    if (numPersons > 0) {
        bool stillNeedsIds = frameKeypoints.getCurrentFrameNumber() % updateInterval_ == 0;
        const auto numPersonsInLastFrame = frameKeypoints.getNumPersonsInLastFrame();

        // If the same number of skeletons are on the screen, then we can use our cache
        // However, since the skeletons get returned in random order, we need to compare to
        // the last frame and use the IDs we used for the skeleton at more or less the same position
        if (!stillNeedsIds && numPersons == numPersonsInLastFrame) {
            // If just one person, we can short-circuit
            for (int personIdx = 0; personIdx < numPersons; personIdx++) {
                int closestPerson = frameKeypoints.getClosestPersonInLastFrame(personIdx);
                const auto personId = closestPerson == -1 ? "" : lastFrameIds_[closestPerson];
                if (personId.empty()) {
                    stillNeedsIds = true;
                    break;
                }
                personIds[personIdx] = personId;
            }
        } else {
            stillNeedsIds = true;
        }

        if (stillNeedsIds) {
            for (int personIdx = 0; personIdx < numPersons; personIdx++) {
                const auto leftEar = frameKeypoints.getBodyPartPoint(personIdx, 18, FrameKeypoints::FrameType::CURRENT_UNSMOOTHED);
                const auto rightEar = frameKeypoints.getBodyPartPoint(personIdx, 17, FrameKeypoints::FrameType::CURRENT_UNSMOOTHED);
                if (leftEar.x > 0 && rightEar.x > 0) {
                    // We try to take only part of the image to make face recognition faster
                    const int widthAndHeight = (int) frameKeypoints.getDistanceBetweenParts(personIdx, 17, 18);
                    const auto faceROI = cv::Rect(cv::Point(std::max(rightEar.x - widthAndHeight / 4, 0),
                                                            std::max(rightEar.y - widthAndHeight, 0)),
                                                  cv::Point(std::min(leftEar.x + widthAndHeight / 4, inputImage.cols),
                                                            std::min(leftEar.y + widthAndHeight, inputImage.rows)));
                    cv::Mat croppedImage = inputImage(faceROI);

                    dlib::cv_image<bgr_pixel> image(croppedImage);
                    std::vector<matrix<rgb_pixel>> faces;
                    const auto faceBounds = detector_(image);
                    if (faceBounds.size() == 1) {
                        const auto faceBoundingBox = faceBounds[0];
                        auto shape = sp_(image, faceBoundingBox);
                        matrix<rgb_pixel> face_chip;
                        extract_image_chip(image, get_face_chip_details(shape, 150, 0.25), face_chip);
                        // get the fingerprint
                        std::vector<matrix<float, 0, 1>> faceDescriptors = net_(
                                std::vector<matrix<rgb_pixel>>{std::move(face_chip)});
                        const auto facePrint = faceDescriptors[0];
                        std::string id = findExistingPersonId(facePrint);
                        if (id.empty()) id = createNewPersonId(facePrint);
                        personIds[personIdx] = id;
                    } else {
//                      static int seq = 0;
//                      const auto file = "/tmp/dump-" + std::to_string(seq++) + ".jpg";
//                      cv::imwrite(file, croppedImage);
                        if(numPersons == 1 && numPersonsInLastFrame == 1) personIds[0] = lastFrameIds_[0];
                        // std::cout << "No/Too many faces found for person " << personIdx << std::endl;
                    }
                }
            }
        }
    }
    lastFrameIds_ = personIds;
    return personIds;
}

std::string PersonIdentifier::createNewPersonId(const matrix<float> &facePrint) {
    static boost::uuids::random_generator gen;
    const auto uuid = boost::uuids::to_string(gen());

    PersonStorage::PersonData data;

    std::stringstream ss;
    ss << facePrint;
    data.facePrint = ss.str();
    data.score = 0;
    data.name = personNamer_.getNextName();
    data.head = themeManager_->getNextHeadNumber();
    storage_->storePerson(uuid, data);
    storage_->flush();

    return uuid;
}

std::string PersonIdentifier::findExistingPersonId(const matrix<float> &facePrint) {
    // Find something above threshold then return the best match
    std::string id;
    float bestMatchDistance = THRESHOLD; // closer to zero == better match

    // TODO: Find a more efficient way to do this
    for (auto tuple : *(storage_->getMap())) {
        const auto existingId = tuple.first;
        const auto data = tuple.second;
        matrix<float> existingPrint;

        std::istringstream ss(data.facePrint);
        ss >> existingPrint;
        const float distance = length(existingPrint - facePrint);
        if (distance < bestMatchDistance) {
            bestMatchDistance = distance;
            id = existingId;
        }
    }

    return id;
}


