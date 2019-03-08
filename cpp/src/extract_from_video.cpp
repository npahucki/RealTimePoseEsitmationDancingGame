//
// Created by Nathan  Pahucki on 10/5/18.
//

#include "extract_from_video.h"
#include "extract_utils.h"
#include "VideoPoseCapture.h"

#include <string>
#include <iostream>
#include <gflags/gflags.h>
#include <openpose/core/headers.hpp>
#include <openpose/filestream/headers.hpp>
#include <openpose/gui/headers.hpp>
#include <openpose/pose/headers.hpp>
#include <openpose/hand/headers.hpp>
#include <openpose/utilities/headers.hpp>
#include <boost/tokenizer.hpp>
#include <VideoPoseDataExtractor.h>

DEFINE_int32(logging_level,             3,              "The logging level. Integer in the range [0, 255]. 0 will output any log() message, while"
                                                        " 255 will not output any. Current OpenPose library messages are in the range 0-4: 1 for"
                                                        " low priority messages and 4 for important ones.");

// OpenPose
DEFINE_string(ip_camera,               "",              "The url for an IP camera to use");

DEFINE_string(model_folder,             "models/",      "Folder path (absolute or relative) where the models (pose, face, ...) are located.");
DEFINE_string(net_resolution,           "-1x368",       "Multiples of 16. If it is increased, the accuracy potentially increases. If it is"
                                                        " decreased, the speed increases. For maximum speed-accuracy balance, it should keep the"
                                                        " closest aspect ratio possible to the images or videos to be processed. Using `-1` in"
                                                        " any of the dimensions, OP will choose the optimal aspect ratio depending on the user's"
                                                        " input value. E.g. the default `-1x368` is equivalent to `656x368` in 16:9 resolutions,"
                                                        " e.g. full HD (1980x1080) and HD (1280x720) resolutions.");
DEFINE_string(output_resolution,        "-1x-1",        "The image resolution (display and output). Use \"-1x-1\" to force the program to use the"
                                                        " input image resolution.");
DEFINE_int32(num_gpu_start,             0,              "GPU device start number.");

// Sampling
DEFINE_int32(number_samples,            -1,           "Number of samples to take before quitting automatically");
DEFINE_string(only_points,              "",           "A comma delimited list of the points to output");

// OpenPose Rendering
DEFINE_bool(disable_blending,           false,          "If enabled, it will render the results (keypoint skeletons or heatmaps) on a black"
                                                        " background, instead of being rendered into the original image. Related: `part_to_show`,"
                                                        " `alpha_pose`, and `alpha_pose`.");
DEFINE_double(render_threshold,         0.05,           "Only estimated keypoints whose score confidences are higher than this threshold will be"
                                                        " rendered. Generally, a high threshold (> 0.5) will only render very clear body parts;"
                                                        " while small thresholds (~0.1) will also output guessed and occluded keypoints, but also"
                                                        " more false positives (i.e. wrong detections).");
DEFINE_double(alpha_pose,               0.6,            "Blending factor (range 0-1) for the body part rendering. 1 will show it completely, 0 will"
                                                        " hide it. Only valid for GPU rendering.");
DEFINE_int32(camera,                    0,              "The camera index for cv::VideoCapture. "
                                                        "Integer in the range [0, 9].");

int processInputs()
{
    try
    {
        op::check(0 <= FLAGS_logging_level && FLAGS_logging_level <= 255, "Wrong logging_level value.", __LINE__, __FUNCTION__, __FILE__);
        op::ConfigureLog::setPriorityThreshold((op::Priority)FLAGS_logging_level);
        op::log("", op::Priority::Low, __LINE__, __FUNCTION__, __FILE__);

        // Check no contradictory flags enabled
        if (FLAGS_alpha_pose < 0. || FLAGS_alpha_pose > 1.)
            op::error("Alpha value for blending must be in the range [0,1].", __LINE__, __FUNCTION__, __FILE__);

        // Logging
        op::log("", op::Priority::Low, __LINE__, __FUNCTION__, __FILE__);


        // Turn string into vector
        vector<int> pointsToOutput;
        struct ToInt { int operator()(string const &str) { return atoi(str.c_str()); } };
        boost::tokenizer<> tok(FLAGS_only_points);
        transform(tok.begin(), tok.end(), back_inserter(pointsToOutput), ToInt());



        VideoPoseDataExtractor pde{"Edmodo Pose Data Extraction"};
        // Important flags
        pde.setPointsToOutput(pointsToOutput);
        pde.setCamera(FLAGS_camera);
        pde.setIpCamera(FLAGS_ip_camera);
        pde.setModelFolder(FLAGS_model_folder);
        pde.setNumberSamples(FLAGS_number_samples);
        // Lesser used flags
        pde.setDisableBlending(FLAGS_disable_blending);
        pde.setRenderThreshold(FLAGS_render_threshold);
        pde.setAlphaPose(FLAGS_alpha_pose);
        pde.setNumGpuStart(FLAGS_num_gpu_start);
        pde.setNetInputSize(op::flagsToPoint(FLAGS_net_resolution, "-1x368"));
        pde.setOutputSize(op::flagsToPoint(FLAGS_net_resolution, "-1x-1"));

        pde.start();
        return 0;
    }
    catch (const std::exception& e)
    {
        op::error(e.what(), __LINE__, __FUNCTION__, __FILE__);
        return -1;
    }
}


int main(int argc, char *argv[])
{
    // Parsing command line flags
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    for(int i = 1; i <= 5; i++) {
        std::cerr << "\aStarting in " << i << " seconds..." << endl;
        cv::waitKey(1000);
    }
     return processInputs();
}
