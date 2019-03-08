//
// Created by Nathan  Pahucki on 10/15/18.
//

#include <VideoRaisedArmDemo.h>

#include <string>
#include <iostream>
#include <gflags/gflags.h>
#include <VideoOverlayDemo.h>

DEFINE_int32(logging_level,             3,              "The logging level. Integer in the range [0, 255]. 0 will output any log() message, while"
                                                        " 255 will not output any. Current OpenPose library messages are in the range 0-4: 1 for"
                                                        " low priority messages and 4 for important ones.");
DEFINE_string(ip_camera,               "",              "The url for an IP camera to use");
DEFINE_int32(camera,                    0,              "The camera index for cv::VideoCapture. "
                                                        "Integer in the range [0, 9].");

DEFINE_string(model_folder,             "models/",      "Folder path (absolute or relative) where the models (pose, face, ...) are located.");
DEFINE_string(db_folder,                "./",           "Folder path (absolute or relative) where the data files should be written.");

DEFINE_string(net_resolution,           "-1x368",       "Multiples of 16. If it is increased, the accuracy potentially increases. If it is"
                                                        " decreased, the speed increases. For maximum speed-accuracy balance, it should keep the"
                                                        " closest aspect ratio possible to the images or videos to be processed. Using `-1` in"
                                                        " any of the dimensions, OP will choose the optimal aspect ratio depending on the user's"
                                                        " input value. E.g. the default `-1x368` is equivalent to `656x368` in 16:9 resolutions,"
                                                        " e.g. full HD (1980x1080) and HD (1280x720) resolutions.");
DEFINE_string(output_resolution,        "1280x720",     "The image resolution (display and output).");
DEFINE_int32(num_gpu_start,             0,              "GPU device start number.");
DEFINE_bool(gpu_render,                 false,          "Use GPU for rendering, required for alpha_pose to have any effect");

DEFINE_string(theme_name,               "space",        "The name of the theme to use");
DEFINE_bool(disable_blending,           false,          "If enabled, it will render the results (keypoint skeletons or heatmaps) on a black"
                                                        " background, instead of being rendered into the original image. Related: `part_to_show`,"
                                                        " `alpha_pose`, and `alpha_pose`.");
DEFINE_bool(mirror,                     true,          "If enabled, the input image will be flipped 180 horizontal before being displayed");

DEFINE_int32(render_last_frames,        5,              "Number of frames to render to give a tracking like effect.");

DEFINE_int32(face_id_frame_interval,    60 * 30,        "Number of frames to skip between updating the person to face id mappings. "
                                                        "The smaller this number the more accurate the face ids will be, the bigger "
                                                        "the number the faster the FPS rendered");
DEFINE_double(smoothing_weight,         0.5,            "The weight to use when smoothing skeleton rendering. A zero disables it.");


DEFINE_int32(max_people,                 -1,            "The maximum number of people to keep. Less than one keeps everyone.");


DEFINE_double(render_threshold,         0.05,           "Only estimated keypoints whose score confidences are higher than this threshold will be"
                                                        " rendered. Generally, a high threshold (> 0.5) will only render very clear body parts;"
                                                        " while small thresholds (~0.1) will also output guessed and occluded keypoints, but also"
                                                        " more false positives (i.e. wrong detections).");
DEFINE_double(alpha_pose,               1.0,            "Blending factor (range 0-1) for the body part rendering. 1 will show it completely, 0 will"
                                                        " hide it. Only valid for GPU rendering.");

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

        if (FLAGS_face_id_frame_interval < 1)
            op::error("Face id frame interval must be greater then 1.", __LINE__, __FUNCTION__, __FILE__);

        if (FLAGS_render_last_frames < 0 || FLAGS_render_last_frames > 120)
            op::error("Render Last frames must be in the range of [0-120].", __LINE__, __FUNCTION__, __FILE__);


        // Logging
        op::log("", op::Priority::Low, __LINE__, __FUNCTION__, __FILE__);


        VideoOverlayDemo demo{"Mr. Edmodo Head"};
        demo.setCamera(FLAGS_camera);
        demo.setIpCamera(FLAGS_ip_camera);
        demo.setModelFolder(FLAGS_model_folder);
        // Lesser used flags
        demo.setDisableBlending(FLAGS_disable_blending);
        demo.setRenderThreshold((float)FLAGS_render_threshold);
        demo.setAlphaPose((float)FLAGS_alpha_pose);
        demo.setNumGpuStart(FLAGS_num_gpu_start);
        demo.setNetInputSize(op::flagsToPoint(FLAGS_net_resolution, "-1x192"));
        demo.setOutputSize(op::flagsToPoint(FLAGS_output_resolution, "-1x-1"));
        demo.setFaceIdFrameInterval(FLAGS_face_id_frame_interval);
        demo.setThemeName(FLAGS_theme_name);
        demo.setMirror(FLAGS_mirror);
        demo.setSmoothingWeight((float)FLAGS_smoothing_weight);
        demo.setRenderLastNFrames(FLAGS_render_last_frames);
        demo.setUseGpuRendering(FLAGS_gpu_render);
        demo.setDbFilePath(FLAGS_db_folder);
        demo.setKeepMaxPeople(FLAGS_max_people);
        demo.start();
        return 0;
    }
    catch (const std::exception& e)
    {
        op::error(e.what(), __LINE__, __FUNCTION__, __FILE__);
        return -1; // Just to keep the compiler happy
    }
}


int main(int argc, char *argv[])
{
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    return processInputs();
}

