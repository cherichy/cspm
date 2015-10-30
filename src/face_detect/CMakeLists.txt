cmake_minimum_required(VERSION 2.8.3)
project(face_detect)
aux_source_directory(./src SRC_LIST)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  std_msgs
  sensor_msgs
  cv_bridge
  image_transport
)


catkin_package()

find_package(OpenCV)
include_directories(
  ${OpenCV_INCLUDE_DIRS}
)

include_directories(include ${catkin_INCLUDE_DIRS})
add_executable(facedetect ${SRC_LIST})
target_link_libraries(facedetect ${catkin_LIBRARIES} ${OpenCV_LIBRARIES})
add_dependencies(facedetect facedetect_generate_messages_cpp)
