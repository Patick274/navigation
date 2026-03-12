#include <ros/ros.h>
#include <std_msgs/String.h>         
#include </home/thunderobot/catkin_ws/devel/include/qq_msgs/Carry.h>//自定义包在catkin_ws/devel/include中，复制其地址就行

int main(int argc, char  *argv[])
{
    ros::init(argc,argv,"chao_node"); 
    printf("我的枪去而复返\n");

    ros::NodeHandle nh;    
    ros::Publisher pub = nh.advertise<qq_msgs::Carry>("kuai_shang_che_kai_hei_qun",10);

    ros::Rate loop_rate(10);

    while(ros::ok())
    {
        printf("我要开始刷屏了\n");
        qq_msgs::Carry  msg;     //生成一个消息包 
        msg.grade ="王者";
        msg.star =50;
        msg.data ="国服马超带飞";
        pub.publish(msg);
        loop_rate.sleep();
    }
    return 0;
}




