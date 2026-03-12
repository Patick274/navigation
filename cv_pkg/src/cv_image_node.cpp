#include <ros/ros.h>
#include <cv_bridge/cv_bridge.h>//转换图像格式
#include <sensor_msgs/image_encodings.h>//图像编码格式
#include <opencv2/imgproc/imgproc.hpp> //opencv的图像处理函数
#include <opencv2/highgui/highgui.hpp>//opencv的图形化显示函数

using namespace cv;
//简化opencv函数的书写

void Cam_RGB_Callback(const sensor_msgs::Image& msg) // 收到图像时自动调用
//                 const常量修饰符：参数不可修改   Image图像消息结构体
{
    cv_bridge::CvImagePtr cv_ptr;        // 智能指针，指向转换后的 OpenCV 图像
    try
    {
        // 将 ROS 图像消息转换为 OpenCV 格式
        // sensor_msgs::image_encodings::BGR8 = 蓝绿红 8位色深 (OpenCV 默认格式)
        cv_ptr = cv_bridge::toCvCopy(msg,sensor_msgs::image_encodings::BGR8);
        // cv_bridge命名空间（库）  toCCopy()OpenCV函数
        // msg消息包  sensor_msg:命名空间：ROS 标准消息库 
        // image_encodings:子命名空间：图像编码定义  BGRB:蓝-绿-红 8位色深编码格式 
        // cv_bridge::toCvCopy 是 ROS 图像消息 → OpenCV 图像格式 的转换函数，
        // 将msg转换成OpenCV格式的图片对象，返回智能指针，
    }
    catch(const std::exception& e)// 转换失败时捕获异常
    {
        ROS_ERROR("cv_bridge exception: %s",e.what()); // 打印错误信息
        return; //退出回调函数
    }
    
    Mat imgOriginal = cv_ptr->image;// 提取 OpenCV 图像数据 (cv::Mat 类型)
    imshow("RGB",imgOriginal);      // 创建/更新名为 "RGB" 的窗口显示图像
    waitKey(1);                     // 等待 1ms，处理窗口事件（必须调用！）
}


int main(int argc ,char **argv)
{
   ros::init(argc,argv,"cv_image_node"); // 初始化 ROS 节点，命名为 cv_image_node

   ros::NodeHandle nh;             
   ros::Subscriber rgb_sub = nh.subscribe("/kinect2/qhd/image_color_rect",1, Cam_RGB_Callback);
//                                           相机名称/图像分辨率/相机节点话题  缓存1帧  回调函数      
   namedWindow("RGB");               // 创建 OpenCV 窗口，标题为 "RGB"
   ros::spin();                      // 等价于: while(ros::ok()) { ros::spinOnce(); }
}

