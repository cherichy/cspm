#ifndef DRONETEST_H_
#define DRONETEST_H_

    #include <ros/ros.h>
    #include <image_transport/image_transport.h>
    #include <sensor_msgs/image_encodings.h>
    #include <cv_bridge/cv_bridge.h>
    #include <opencv2/highgui/highgui.hpp>
    #include <opencv2/imgproc/imgproc.hpp>
    #include <opencv/cv.h>
    #include <std_msgs/String.h>
    #include <std_msgs/Empty.h>
    #include <iostream>
    #include <geometry_msgs/Twist.h>


    void detect_and_draw( IplImage *image);



    #define SPACE 1048608
    #define ENTER 1048586
    #define UP 1113938
    #define DOWN 1113940
    #define LEFT 1113937
    #define RIGHT 1113939
    #define ESC 1048603
    #define W 1048695
    #define A 1048673
    #define S 1048691
    #define D 1048676
#endif
