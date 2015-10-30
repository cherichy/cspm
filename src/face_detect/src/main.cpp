#include "stdafx.h"

using namespace std;
using namespace cv;

static const char WINDOW[]="RGB Image";
static const std_msgs::Empty e;


class cspm_robot{

    ros::Publisher takeoff_pub;
    ros::Publisher land_pub;
    image_transport::Subscriber image_sub;
    geometry_msgs::Twist cmd_vel;
    geometry_msgs::Twist fresh_vel;
    ros::Publisher vel_pub;
    Mat last_image;//上一张图片
    Mat current_image;//下一张图片
    bool flag = false;

    void forward(float i){
        cmd_vel = fresh_vel;
        cmd_vel.linear.x = i;
        vel_pub.publish(cmd_vel);
    }
    void back(float i){
        cmd_vel = fresh_vel;

        cmd_vel.linear.x = -i;
        vel_pub.publish(cmd_vel);

    }
    void rotation_clock(float i){
        cmd_vel = fresh_vel;

        cmd_vel.angular.z = i;
        vel_pub.publish(cmd_vel);

    }
    void rotation_inclock(float i){
        cmd_vel = fresh_vel;

        cmd_vel.angular.z = -i;
        vel_pub.publish(cmd_vel);

    }
    void move(float x, float y, float z, float rx, float ry, float rz){
        cmd_vel = fresh_vel;
        cmd_vel.linear.x = x;
        cmd_vel.linear.y = y;
        cmd_vel.linear.z = z;
        cmd_vel.angular.x = rx;
        cmd_vel.angular.y = ry;
        cmd_vel.angular.z = rz;
        vel_pub.publish(cmd_vel);
    }
    void process(const sensor_msgs::ImageConstPtr& cam_image){
        cv_bridge::CvImagePtr cv_ptr;

        try
        {
          cv_ptr = cv_bridge::toCvCopy(cam_image,sensor_msgs::image_encodings::BGR8);
        } catch (cv_bridge::Exception& e){
          ROS_ERROR("cv_bridge exception:%s",e.what());

          return;
        }
        current_image = cv_ptr->image;
        imshow(WINDOW,current_image);
        int k = waitKey(1);
        if(k != -1)
            cout << k << endl;

        if(k == UP)//up for forward
            forward(1);
        if(k == DOWN)//down for back
            back(1);
        if(k == LEFT)//left for turn_left
            rotation_clock(1);
        if(k == RIGHT)//right for turn_right
            rotation_inclock(1);
        return;

        }
public:
    /*
     *init the subscribers and publishers
     */
    cspm_robot(char* image_topic){

        ros::NodeHandle n;
        image_transport::ImageTransport it(n);
        image_sub = it.subscribe(image_topic,1,&cspm_robot::process,this);
        vel_pub = n.advertise<geometry_msgs::Twist>("/cmd_vel",1);
    }



};



int main(int argc, char **argv){
    ros::init(argc,argv,"facedetect");
    cv::namedWindow(WINDOW);

    cspm_robot cr(argv[1]);


    ros::spin();
    return 0;
    }
