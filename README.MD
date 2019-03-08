# A fun demonstration of using OpenPose output data fed into another neural network for making predictions on people poses

[![Watch the video](demo.gif)](https://www.youtube.com/watch?v=dON8OgfUPXU)

## About

### History

This is a project that I had the fortune to work on while consulting for the Machine Learning department at [Edmodo Inc.](https://www.edmodo.com/), 
and education company with a focus on technology. I developed the project on my own, and Edmodo has graciously given me
permission to publish this work as open source so as to benefit anyone else looking to integrate tightly with the []OpenPose](https://github.com/CMU-Perceptual-Computing-Lab/openpose)
project, use [OpenCV](https://opencv.org/) graphics libraries for creating games, or those interested using Computer Vision to detect when humans are
doing certain things based on their body movements. There is also a demonstration of using [dlib](http://dlib.net/) to persistently identify
persons and match them up with existing data about them.

### The Game

The game object is pretty simple, you and some friends dance around a bit and whoever has the best moves get the most points. 
In reality, the scoring is determined by how many up/down arm movements the system detects in a period of time. Each player
gets assigned a random name and his face print is recorded. If the same player returns later on his score and avatar head
are remembered. There is also a leader board showing who has the top score. The game supports multiple themes, such that
you could plug in different themes by placing files in the `cpp/assets/xxx` folder where xxx is the name of your theme. 

### The Architecture

All of the code, except the training code was written in C++. The training for recognizing whether an arm is up or down
was done in [keras](https://keras.io/) and python scripts were used to train the models. I have also included this code
so different models can be trained. I trained the hand up/down models with 1000 samples taken from a video of me with my 
hand raised or not. There is certainly room for much better training here to get better results, though what I have
seems to mostly work fine. The models were trained with a stripped down and normalized output from OpenPose, thus the
model is only dealing with a handful of data points so the model is pretty compact and can make predictions pretty fast.

The basic components of the project are OpenPose for pose estimation, fed into a keras model that is loaded from C++
via some code adapted from [Kerasify by](https://github.com/moof2k/kerasify/blob/master/keras_model.cc) and rendered 
via OpenCV graphic libraries. DLib is used to create a face print of each user and BerkeleyDB is used to map the 
faceprint calculated by dlib to a avatar, name and score. 

### Hardware 
   
This has been tested on hardware that uses a single NVIDIA GeForce GTX 1080 and a Mac OS with a Radeon Pro 560X. 
On the former you expect 30fps at least, but on the Mac the best I ever got was about 3fps. For tips speeding things up
check out [OpenPose's page](https://github.com/CMU-Perceptual-Computing-Lab/openpose/blob/master/doc/speed_up_openpose.md).     
   
### The Images
IMPORTANT NOTE: The images contained in [cpp/assets](./cpp/assets) were found on the internet and are not owned by me, 
if you want to use them you will need to get permission from the copyright owners. 
The [skulls](https://www.shutterstock.com/collections/135675925?verification_code=94e62228&utm_source=collections&utm_medium=copylink&utm_campaign=lightbox)  
in [cpp/assets/halloween/heads](./cpp/assets/halloween/heads) I purchased from shutterstock.com and you will 
need to purchase your own licence if you wish to use them in any derivative works.


## Installation and Running
The easiest way to get this running is by building the docker image using the Dockerfile and running it from there, though
this has only been tested on Ubuntu.

1) Make sure you have [NVIDIA docker extensions working](https://github.com/NVIDIA/nvidia-docker).
1) From the project root:  `docker build -t posegame:latest .`
2) From a terminal within an X session: `./bin/runHeadDemoWithWebCam` 

## Alternate Installation and Development
If you want to install it locally without using Docker 
1) [Install and test open OpenPose](https://github.com/CMU-Perceptual-Computing-Lab/openpose#installation-reinstallation-and-uninstallation)
2) Install Python 3.x
3) Install keras stuff: `pip install tensorflow_gpu`
4) Install BerkeleyDB libs: `sudo apt-get update && apt-get install -y libdb5.3++-dev libdb5.3-dev libdb5.3-stl-dev`
5) Use `cmake` to build the project. 

### Scripts and executables

#### Bash Scripts

`bin/runHeadDemoWithWebCam` - runs the docker image with the pose game

#### C++ Binaries
`from_image` - extracts keypose data from an image and outputs to console. See [extract_from_image.cpp](./cpp/src/extract_from_image.cpp)
`from_video` - extracts keypose data from a video and outputs to console. This useful for collecting training data. See [extract_from_video.cpp](./cpp/src/extract_from_video.cpp)
`detect_hands_up` - demonstrates running predictions on pose data. See [detect_hands.cpp](./cpp/src/detect_hands.cpp).
`edmodo_head` - runs the game. See [edmodo_head.cpp](./cpp/src/edmodo_head.cpp).
#### Python
`kerasify.py` - used to convert the keras model into something that can be used in C++.
`train_model.py` - used to train a model from input data. 

## Inspiration and References
  - https://github.com/davisking/dlib/blob/master/examples/dnn_face_recognition_ex.cpp
  - https://github.com/srianant/computer_vision
  - https://github.com/moof2k/kerasify
  - https://github.com/davisking/dlib
  - https://github.com/CMU-Perceptual-Computing-Lab/openpose
  - https://github.com/opencv/opencv
  
  
 
  
    

