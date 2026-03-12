#include <ros/ros.h>
#include <std_msgs/String.h>         

int main(int argc, char  *argv[])
{
    ros::init(argc,argv,"yao_node"); 
    printf("我是yao\n");

    ros::NodeHandle nh;    
    ros::Publisher pub = nh.advertise<std_msgs::String>("RC",10);

    ros::Rate loop_rate(10);

    while(ros::ok())
    {
        printf("我要打RC!\n");
        std_msgs::String  msg;     //生成一个消息包 
        msg.data ="毕业设计";
        pub.publish(msg);
        loop_rate.sleep();
    }
    return 0;
}




