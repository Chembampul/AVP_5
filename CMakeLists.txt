PROJECT(AVP5 LANGUAGES CXX)

CMAKE_MINIMUM_REQUIRED(VERSION 3.17)


SET(CMAKE_BUILD_TYPE RELEASE)
SET(CMAKE_CUDA_FLAGS "-arch=sm_60 -m64")
SET(CMAKE_CUDA_SEPARABLE_COMPILATION ON)
FILE(GLOB_RECURSE SRC src/*)

FIND_PACKAGE(CUDA REQUIRED)
FIND_PACKAGE(OpenCV REQUIRED)
INCLUDE_DIRECTORIES(include ${CUDA_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(include ${OpenCV_INCLUDE_DIRS})
CUDA_ADD_EXECUTABLE(AVP5 ${SRC})
TARGET_LINK_LIBRARIES(AVP5 ${CUDA_LIBS})
TARGET_LINK_LIBRARIES(AVP5 ${OpenCV_LIBS})