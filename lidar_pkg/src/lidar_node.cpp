#include <ros/ros.h>
#include <sensor_msgs/LaserScan.h>
#include <geometry_msgs/Twist.h>

ros::Publisher vel_pub; //全局变量
int nCount = 0;    // 计数变量，用于控制转向持续时间


void LidarCallback(const sensor_msgs::LaserScan msg)   //回调函数
{
    float  fMidDist = msg.ranges[180];
    ROS_INFO("前方测距 ranges[180] = %f 米",fMidDist);
 
    if(nCount > 0)
    {
        nCount --;
        return;
    }

    geometry_msgs::Twist vel_cmd;
    if(fMidDist < 1.5)
    {
        vel_cmd.angular.z=0.3;
        nCount = 50;
    }
    else
    {
        vel_cmd.linear.x=0.05;
    }
    vel_pub.publish(vel_cmd);
}


int main(int argc, char  *argv[])
{
    setlocale(LC_ALL,"");
    ros::init(argc,argv,"lidar_node");

    ros::NodeHandle n;
    ros::Subscriber lidar_sub = n.subscribe("/scan",10,&LidarCallback);
    vel_pub = n.advertise<geometry_msgs::Twist>("/cmd_vel",10);

    ros::spin(); //// 阻塞等待回调

    return 0;
}
