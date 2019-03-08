cmake_minimum_required(VERSION 3.5.1)
project(gestures)

set(CMAKE_VERBOSE_MAKEFILE off)
set(CMAKE_CXX_STANDARD 14)
set(USE_AVX_INSTRUCTIONS 1)
set(DLIB_USE_CUDA 1)

# Thirdpaty stuff
add_subdirectory(thirdparty/dlib thirdparty/dlib_build)

add_executable(from_image src/extract_from_image.cpp src/extract_utils.cpp src/keras_model.cpp src/KeypointModelPredictor.cpp src/UpperBodyRaisedHandPredictor.cpp)
add_executable(from_video src/extract_from_video.cpp src/extract_utils.cpp src/VideoProcessor.cpp src/VideoPoseCapture.cpp src/VideoPoseDataExtractor.cpp)
add_executable(detect_hands_up src/extract_utils.cpp src/detect_hands.cpp src/VideoProcessor.cpp src/VideoPoseCapture.cpp src/keras_model.cpp src/KeypointModelPredictor.cpp src/UpperBodyRaisedHandPredictor.cpp src/VideoRaisedArmDemo.cpp)
add_executable(edmodo_head src/edmodo_head.cpp src/extract_utils.cpp src/VideoProcessor.cpp src/VideoPoseCapture.cpp src/VideoOverlayDemo.cpp src/PersonIdentifier.cpp src/keras_model.cpp src/KeypointModelPredictor.cpp src/UpperBodyRaisedHandPredictor.cpp src/ScoreTracker.cpp src/FrameKeypoints.cpp src/PersonStorage.cpp src/ThemeManager.cpp src/PersonNamer.cpp)

add_definitions(-std=c++11)



list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules")

find_package(GFlags)
find_package(Glog)
find_package(OpenCV)
find_package(OpenPose REQUIRED)
find_package(Boost REQUIRED)
find_package(BerkeleyDB REQUIRED)

include_directories(include ${OpenPose_INCLUDE_DIRS} ${GFLAGS_INCLUDE_DIR} ${GLOG_INCLUDE_DIR} ${OpenCV_INCLUDE_DIRS} ${BOOST_INCLUDE_DIRS} ${BerkeleyDB_INCLUDE_DIR})
target_link_libraries(from_image ${BOOST_LIBS} ${OpenPose_LIBS} ${GFLAGS_LIBRARY} ${GLOG_LIBRARY} ${OpenCV_LIBS} )
target_link_libraries(from_video ${BOOST_LIBS} ${OpenPose_LIBS} ${GFLAGS_LIBRARY} ${GLOG_LIBRARY} ${OpenCV_LIBS} )
target_link_libraries(detect_hands_up ${BOOST_LIBS} ${OpenPose_LIBS} ${GFLAGS_LIBRARY} ${GLOG_LIBRARY} ${OpenCV_LIBS} )
target_link_libraries(edmodo_head ${BOOST_LIBS} ${OpenPose_LIBS} ${GFLAGS_LIBRARY} ${GLOG_LIBRARY} ${OpenCV_LIBS} ${BerkeleyDB_LIBRARIES} dlib::dlib )

