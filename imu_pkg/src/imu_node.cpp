#include <ros/ros.h>              // ROS核心头文件，提供节点初始化、话题订阅/发布等基础功能
#include <sensor_msgs/Imu.h>      // IMU数据类型头文件，包含IMU的姿态、角速度、线加速度等信息
#include <tf/tf.h>                // TF库头文件，提供四元数与欧拉角的转换工具
#include <geometry_msgs/Twist.h>  // 机器人速度指令数据类型头文件，用于发布运动控制指令

ros::Publisher vel_pub;           // 全局发布者对象，用于发布速度控制指令

void IMUCallback(sensor_msgs::Imu msg)
{
    /* 检查IMU姿态数据是否有效：orientation_covariance[0]<0表示数据无效 */
   if(msg.orientation_covariance[0] < 0)
     return;
   
   tf::Quaternion quaternion    // 将IMU的四元数数据（x,y,z,w）转换为tf::Quaternion对象
   (
       msg.orientation.x,
       msg.orientation.y,
       msg.orientation.z,
       msg.orientation.w
   );  

   double roll , pitch , yaw;   // 定义欧拉角变量：roll（滚转）、pitch（俯仰）、yaw（朝向）
   tf::Matrix3x3(quaternion).getRPY(roll , pitch , yaw); // 将四元数转换为欧拉角（弧度制）
   
   /*弧度转换为角度*/
   roll = roll*180/M_PI;
   pitch = pitch*180/M_PI;
   yaw = yaw*180/M_PI;

   ROS_INFO("滚转 = %.0f 俯仰 = %.0f 朝向 = %.0f ", roll , pitch , yaw); // 打印欧拉角信息（保留0位小数）


   double target_yaw =90;                           // 目标朝向角（90度）
   double diff_angel =target_yaw -yaw;              // 当前朝向与目标的差值
   geometry_msgs::Twist vel_cmd;                    // 速度指令对象
   vel_cmd.angular.z = diff_angel * 0.01;           // 角速度与角度差成正比（比例控制）
   vel_cmd.linear.x  =0.1;                          // X轴方向速度
   vel_pub.publish(vel_cmd);                        // 发布速度指令到/cmd_vel话题
}

int main(int argc, char  *argv[])
{
    setlocale(LC_ALL,"");  // 设置中文输出（避免ROS_INFO打印中文乱码）
    ros::init(argc,argv,"imu_node"); // 初始化ROS节点，命名为"imu_node"

    ros::NodeHandle n;
    ros::Subscriber imu = n.subscribe("/imu/data",10,IMUCallback);// 订阅IMU话题"/imu/data"，队列大小10，回调函数为IMUCallback
    vel_pub = n.advertise<geometry_msgs::Twist>("/cmd_vel",10);

    ros::spin(); // 进入事件循环，阻塞等待回调函数触发（持续监听"/imu/data"话题）

    return 0;
}


