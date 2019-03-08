/**
 * Given an image (which assumes there is just a single face in the image)
 * return a unique identifier for the person.
 */
#ifndef GESTURES_PERSONIDENTIFIER_H
#define GESTURES_PERSONIDENTIFIER_H

#include <string>
#include <openpose/core/headers.hpp>
#include <openpose/filestream/headers.hpp>
#include <openpose/gui/headers.hpp>
#include <openpose/pose/headers.hpp>
#include <openpose/utilities/headers.hpp>

#include <dlib/dnn.h>
#include <dlib/clustering.h>
#include <dlib/string.h>
#include <dlib/image_io.h>
#include <dlib/image_processing/frontal_face_detector.h>

#include "FrameKeypoints.h"
#include "PersonStorage.h"
#include "ThemeManager.h"
#include "PersonNamer.h"

using namespace dlib;

// Defines the Neural Net
template<template<int, template<typename> class, int, typename> class block, int N,
        template<typename> class BN, typename SUBNET>
using residual = add_prev1<block<N, BN, 1, tag1<SUBNET>>>;
template<template<int, template<typename> class, int, typename> class block, int N,
        template<typename> class BN, typename SUBNET>
using residual_down = add_prev2<avg_pool<2, 2, 2, 2, skip1<tag2<block<N, BN, 2, tag1<SUBNET>>>>>>;
template<int N, template<typename> class BN, int stride, typename SUBNET>
using block  = BN<con<N, 3, 3, 1, 1, relu<BN<con<N, 3, 3, stride, stride, SUBNET>>>>>;
template<int N, typename SUBNET> using ares      = relu<residual<block, N, affine, SUBNET>>;
template<int N, typename SUBNET> using ares_down = relu<residual_down<block, N, affine, SUBNET>>;
template<typename SUBNET> using alevel0 = ares_down<256, SUBNET>;
template<typename SUBNET> using alevel1 = ares<256, ares<256, ares_down<256, SUBNET>>>;
template<typename SUBNET> using alevel2 = ares<128, ares<128, ares_down<128, SUBNET>>>;
template<typename SUBNET> using alevel3 = ares<64, ares<64, ares<64, ares_down<64, SUBNET>>>>;
template<typename SUBNET> using alevel4 = ares<32, ares<32, ares<32, SUBNET>>>;
using anet_type = loss_metric<fc_no_bias<128, avg_pool_everything<
        alevel0<
                alevel1<
                        alevel2<
                                alevel3<
                                        alevel4<
                                                max_pool<3, 3, 2, 2, relu<affine<con<32, 7, 7, 2, 2,
                                                        input_rgb_image_sized<150>
                                                >>>>>>>>>>>>;


class PersonIdentifier {

private:


    shape_predictor sp_;
    anet_type net_;
    int updateInterval_ = 5 * 30;
    frontal_face_detector detector_;
    PersonStorage * storage_ = nullptr;
    PersonNamer personNamer_;
    ThemeManager * themeManager_ = nullptr;
    std::vector<std::string> lastFrameIds_;

    std::string createNewPersonId(const matrix<float> &facePrint);
    std::string findExistingPersonId(const matrix<float> &facePrint);




public:
    PersonIdentifier(const std::string &modelFolder, PersonStorage *storage,
                     ThemeManager *themeManager, const int &updateInterval);

    std::vector<std::string> identifyPersons(const FrameKeypoints &keypoints, const cv::Mat &inputImage);

};


#endif //GESTURES_PERSONIDENTIFIER_H
