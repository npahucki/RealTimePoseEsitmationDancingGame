//
// Created by Nathan  Pahucki on 10/5/18.
//

#ifndef GESTURES_EXTRAXT_FROM_VIDEO_H
#define GESTURES_EXTRAXT_FROM_VIDEO_H

#include <vector>
#include <utility>
#include <sstream>
#include <string>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <cmath>
#include <tuple>
#include <unistd.h>
#include <poll.h>

#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/highgui/highgui_c.h"
#include "opencv2/imgproc/imgproc.hpp"
//#include "opencv2/gpu/gpu.hpp" // TODO: need this?

// OpenPose dependencies
#include <openpose/headers.hpp>
#include <openpose/face/faceDetector.hpp>
#include <openpose/face/faceParameters.hpp>
#include <openpose/hand/handDetector.hpp>
#include <openpose/hand/handRenderer.hpp>
#include <openpose/core/renderer.hpp>
#include <openpose/utilities/fastMath.hpp>
#include <openpose/utilities/keypoint.hpp>
#include <openpose/pose/poseParameters.hpp>

#include "boost/archive/text_oarchive.hpp"
#include "boost/archive/text_iarchive.hpp"
#include "boost/serialization/vector.hpp"

using namespace std;


#endif //GESTURES_EXTRAXT_FROM_VIDEO_H
