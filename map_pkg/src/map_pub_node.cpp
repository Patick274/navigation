#include <ros/ros.h>                                                // ROS核心头文件（节点初始化、句柄、发布/订阅等基础功能）
#include <nav_msgs/OccupancyGrid.h>                                 // 栅格地图消息类型头文件

int main(int argc, char  *argv[])
{
    ros::init(argc,argv,"map_pub_node");                            // 初始化ROS节点，命名为"map_pub_node"
    ros::NodeHandle n;                                              // 创建节点句柄（与ROS核心通信的接口）
    ros::Publisher pub = n.advertise<nav_msgs::OccupancyGrid>("/map",10);
                                                                    // 创建发布者：发布话题"/map"，消息类型为nav_msgs::OccupancyGrid，队列大小10（缓存最多10条未发送消息）
    ros::Rate r(1);                                                 // 设置循环频率为1Hz（每秒执行1次循环）
    while(ros::ok())                                                // 循环条件：ROS节点正常运行（未收到关闭指令）
    {
        nav_msgs::OccupancyGrid msg;                                // 定义栅格地图消息对象
        // 1. 设置消息头（header）
        msg.header.frame_id = "map";                                // 地图的坐标系名称（通常为"map"，表示全局地图坐标系）
        msg.header.stamp  = ros::Time::now();                       // 消息时间戳（当前系统时间）
 
        // 2.设置地图元信息（info字段）
        msg.info.origin.position.x = 1.0;                           // 地图原点在map坐标系下的X坐标
        msg.info.origin.position.y = 2.0;                           // 地图原点在map坐标系下的Y坐标
        msg.info.resolution = 1.0;                                  // 地图分辨率（每个栅格代表1米）
        msg.info.width = 4;                                         // 地图宽度（4个栅格）
        msg.info.height =2;                                         // 地图高度（2个栅格）

        // 3. 设置地图数据（data字段）
        // 每个元素代表对应栅格的占用状态：
        // -1：未知；0：空闲；100：占用 ；默认为0
        msg.data.resize(4*2);
        msg.data[0] = 100;
        msg.data[1] = 100;
        msg.data[2] = 0;
        msg.data[3] = -1;
        msg.data[4] = 0;
        msg.data[5] = 0;
        msg.data[6] = 0;
        msg.data[7] = 0;

        pub.publish(msg);                                           // 按照1Hz频率休眠（保证循环频率稳定）
        r.sleep();
   
    }

    return 0;
}
