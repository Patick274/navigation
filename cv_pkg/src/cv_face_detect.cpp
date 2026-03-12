#include <ros/ros.h>
#include <cv_bridge/cv_bridge.h>//转换图像格式
#include <sensor_msgs/image_encodings.h>//图像编码格式
#include <opencv2/imgproc/imgproc.hpp> //opencv的图像处理函数
#include <opencv2/highgui/highgui.hpp>//opencv的图形化显示函数
#include <opencv2/objdetect/objdetect.hpp>//OpenCV目标检测（级联分类器）

using namespace cv;
using namespace std;

static CascadeClassifier face_cascade;// Haar级联分类器（人脸检测器）
static Mat frame_gray;                // 灰度图像（检测用）
static vector<Rect> faces;            // 检测到的人脸矩形框数组
static vector<Rect>::const_iterator face_iter;// 遍历人脸的迭代器

void callbackRGB(const sensor_msgs::Image& msg)
{
   cv_bridge::CvImagePtr cv_ptr;
   try
   {
    cv_ptr = cv_bridge::toCvCopy(msg,sensor_msgs::image_encodings::BGR8);
   }
   catch(const std::exception& e)
   {
    ROS_ERROR("cv_briidge exception : %s",e.what());
    return ;
   }
   
   Mat imgOriginal = cv_ptr->image; // 获取原始彩色图像
   
   //转换成黑白图像
   cvtColor(imgOriginal , frame_gray , CV_BGR2GRAY);// BGR转灰度
   equalizeHist( frame_gray,frame_gray);// 直方图均衡（增强对比度）
   
   //人连检测
   face_cascade.detectMultiScale( frame_gray , faces ,      1.1 ,                      9 ,                  0|CASCADE_SCALE_IMAGE , Size(30,30));
   //                             输入灰度图   输出人脸矩形数组 缩放因子（每次窗口缩放10%） 最小邻居数（过滤误检）          标志位              最小检测窗口尺寸

   // 绘制检测结果
   if(faces.size()>0)
   {
    //遍历所以人脸
    for(face_iter = faces.begin() ; face_iter != faces.end() ; ++face_iter)
    {
        rectangle(
            imgOriginal,
            Point(face_iter->x , face_iter->y),
            Point(face_iter->x + face_iter->width , face_iter->y + face_iter->height),
            CV_RGB(255,0,255),
            2
        );
    }
   }
   imshow("faces",imgOriginal);//将标注好的人脸显示到faces的窗口中W
   waitKey(1);
}

int main(int argc,char **argv)
{
  ros::init(argc,argv,"cv_face_detect");

  namedWindow("faces");            // 创建显示窗口
  
  std::string strLoadFile;         //定义字符串strLoadFile
  char const* home =getenv("HOME");
  //char const*：指针指向的内容不能改，但指针本身可以指向其他地址；
  //char* const：指针本身不能改（固定指向某个地址），但指向的内容可以改。
  //getenv 是 C 标准库（<stdlib.h>）中的函数，作用是读取系统环境变量：
  //getenv("HOME") 返回的字符指针，被赋值给 home 变量
  strLoadFile = home;
  strLoadFile += "/catkin_ws"; //工作空间目录
  strLoadFile += "/src/wpr_simulation/config/haarcascade_frontalface_alt.xml";//文件位置及文件名

  bool res = face_cascade.load(strLoadFile);
  //              调用人脸分类器的load（）函数，把文件file传进去，face_cascade从中加载人脸特征信息
  if(res == false)
  {
    ROS_ERROR("fail to load haarcascade)frontalface_alt.xml");//报错信息
    return 0;
  }

  ros::NodeHandle nh;
  ros::Subscriber rgb_sub = nh.subscribe("kinect2/qhd/image_color_rect",1,callbackRGB);

  ros::spin();
  return 0;
}