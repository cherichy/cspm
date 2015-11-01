#!/usr/bin/env python
import roslib

roslib.load_manifest('imutf')
import rospy
import tf.transformations as tfs
import tf
import geometry_msgs.msg
from nav_msgs.msg import Odometry
# Odometry:
#   pose:x1,x2,x3,th1,th2,th3
#   twist:v1,v2,v3,w1,w2,w3    the diff of pose
from sensor_msgs.msg import Imu
import numpy as np


# Imu:
# Imu.orientation:quaternion orientation
# Imu.angular_velocity:w1,w2,w3
# Imu.linear_acceleration:a1,a2,a3

# init

class Fuck():
    def __init__(self, x=[0, 0, 0], v=[0, 0, 0], a=[0, 0, 0], e=[0, 0, 0], w=[0, 0, 0]):
        # local coordinate
        self.e = e
        self.w = w
        self.a = a
        # global coordinate
        self.x = x
        self.v = v

    def cal(self, dt, imu):
        Rmartix = tfs.quaternion_matrix(imu.o)[:3, :3]
        th = (lambda x: x + 1.5 * np.pi if x < -np.pi / 2.0 else x - 0.5 * np.pi)(
            np.arctan2(*np.dot(Rmartix, [0, 0, -1])[1::-1]))
        a1, a2, a3 = np.dot(Rmartix, imu.a)
        vx, vy, vz = self.v
        x, y, z = self.x
        wz = imu.w[1]
        vx += (self.a[0] + a1) / 2.0 * dt
        vy += (self.a[1] + a2) / 2.0 * dt
        x += self.v[0] * dt + (a1 + 2 * self.a[0]) / 6.0 * dt ** 2
        y += self.v[1] * dt + (a2 + 2 * self.a[1]) / 6.0 * dt ** 2

        return Fuck([x, y, z], [vx, vy, vz], [a1, a2, a3], [0, 0, th], [0, 0, wz])


class IMU():
    def __init__(self, o=0, w=0, a=0):
        self.o = o
        self.w = w
        self.a = a


def callback(dat):
    global imu, fuck, past
    # dat.orientation
    o1, o2, o3, o4 = dat.orientation.x, dat.orientation.y, dat.orientation.z, dat.orientation.w
    # dat.angular_velocity
    w1, w2, w3 = dat.angular_velocity.x, dat.angular_velocity.y, dat.angular_velocity.z
    # dat.linear_acceleration
    a1, a2, a3 = dat.linear_acceleration.x, dat.linear_acceleration.y, dat.linear_acceleration.z
    imu.o, imu.w, imu.a = [o1, o2, o3, o4], [w1, w2, w3], [a1, a2, a3]
    now = rospy.Time.now()
    dt = (now - past).to_sec()
    fuck = fuck.cal(dt, imu)
    print(fuck.a)


def listener():
    rospy.Subscriber('/android/imu', Imu, callback)
    rospy.spin()


if __name__ == '__main__':
    imu = IMU()
    fuck = Fuck()
    rospy.init_node('imulistener', anonymous=True)
    odom_pub = rospy.Publisher('odom_pub', Odometry, queue_size=50)
    past = rospy.get_rostime(
    listener()
    # odom_pub = rospy.Publisher('odom_pub', Odometry, queue_size=50)
    # rate = rospy.Rate(10)
    # past = rospy.Time.now()
    # fuck = Fuck()
    # print fuck
    # while not rospy.is_shutdown():
    #     now = rospy.Time.now()
    #     dt = now - past
    #     fuck.cal(dt, imu)
    #     odom_quat=geometry_msgs.msg.Quaternion()
    #     odom = Odometry()
    #     odom.header.stamp=now
    #     odom.header.frame_id = "odom"
    #     odom.pose.pose.position.x,odom.pose.pose.position.y,odom.pose.pose.position.z = fuck.x
    #     odom.pose.pose.orientation=fuck.e[2]
    #     odom.child_frame_id='base_link'
    #     odom.twist.twist.linear.x,odom.twist.twist.linear.y,odom.twist.twist.linear.z=fuck.v
    #     odom.twist = fuck.twist()
    #     odom_pub.publish(odom)
    #     past=now
    #     rate.sleep()
