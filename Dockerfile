# To run the pose capture from this image, use the following command:
#`docker run -t -i --net=host -e DISPLAY --device=/dev/video0 --runtime=nvidia -v --rm 721466574657.dkr.ecr.us-east-1.amazonaws.com/gestures:latest`
#
# To build a new version of this image: `docker build -t 721466574657.dkr.ecr.us-east-1.amazonaws.com/gestures:latest .`
# To publish it" `docker push 721466574657.dkr.ecr.us-east-1.amazonaws.com/gestures:latest`
# For additional command line params: https://github.com/CMU-Perceptual-Computing-Lab/openpose/blob/master/doc/demo_overview.md

FROM 721466574657.dkr.ecr.us-east-1.amazonaws.com/openpose:cuda-9.0
MAINTAINER Edmodo

WORKDIR /gestures

RUN pip install tensorflow_gpu

RUN apt-get update && apt-get install -y libdb5.3++-dev libdb5.3-dev libdb5.3-stl-dev && rm -rf /var/lib/apt/lists/*


COPY models models
RUN cd models && \
curl -O http://dlib.net/files/shape_predictor_5_face_landmarks.dat.bz2 && \
curl -O http://dlib.net/files/dlib_face_recognition_resnet_model_v1.dat.bz2 && \
bunzip2 -f shape_predictor_5_face_landmarks.dat.bz2 && \
bunzip2 -f dlib_face_recognition_resnet_model_v1.dat.bz2


COPY python python
COPY bin bin
COPY samples samples
COPY cpp cpp

RUN cd cpp && mkdir -p build/Release && cd build && cmake .. && cmake --build  . --config Release -- -j `nproc` 
RUN ln -s /openpose/models/pose models/pose

ENTRYPOINT ["/bin/bash"]

