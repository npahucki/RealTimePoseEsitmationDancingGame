#!/bin/bash

# $1 should be training/{label} or testing/{label}

find images/$1 -name *.jpg | xargs -n 1 cmake-build-debug/gestures.bin --model_folder /Users/npahucki/Projects/Edmodo/openpose/models/ --disable_display --image_path