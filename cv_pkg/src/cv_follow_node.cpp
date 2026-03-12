#include <ros/ros.h>
#include <cv_bridge/cv_bridge.h>//转换图像格式
#include <sensor_msgs/image_encodings.h>//图像编码格式
#include <opencv2/imgproc/imgproc.hpp> //opencv的图像处理函数
#include <opencv2/highgui/highgui.hpp>//opencv的图形化显示函数
#include <geometry_msgs/Twist.h> //数度控制文件

using namespace cv;
using namespace std;

static int iLowH=10;
static int iHighH=40;

static int iLowS=90;
static int iHighS=255;

static int iLowV=1;
static int iHighV=255;

geometry_msgs::Twist vel_cmd;  //速度消息包
ros::Publisher vel_pub;        //数度发布对象

void Cam_RGB_Callback(const sensor_msgs::Image& msg)
{
   cv_bridge ::CvImagePtr cv_ptr;
   try
   {
    cv_ptr = cv_bridge::toCvCopy(msg,sensor_msgs::image_encodings::BGR8);
   }
   catch(const std::exception& e)
   {
    ROS_ERROR("cv_bridge excetpion: %s", e.what());
    return;
   }

   Mat imgOriginal = cv_ptr ->image;
   
   //
   Mat imgHSV;
   vector<Mat> hsvSplit;
   cvtColor(imgOriginal ,imgHSV,COLOR_BGR2HSV);

   split(imgHSV,hsvSplit);
   equalizeHist(hsvSplit[2],hsvSplit[2]);//对V值（亮度）进行均衡化
   merge(hsvSplit,imgHSV);

   Mat imgThresholded;
   inRange(imgHSV,Scalar(iLowH,iLowS,iLowV),Scalar(iHighH,iHighS,iHighV),imgThresholded);

   //开操作(去除噪点)
   Mat element = getStructuringElement(MORPH_RECT,Size(5,5));
   morphologyEx(imgThresholded,imgThresholded,MORPH_OPEN,element);
  
  //闭操作（连接一些连通域）
   morphologyEx(imgThresholded,imgThresholded,MORPH_CLOSE,element);
   
   //遍历二值化后的图像数据
  int nTargetx = 0;
  int nTargety = 0;
  int nPixCount = 0;
  int nImgWidth = imgThresholded.cols;
  int nImgHeight = imgThresholded.rows;
  int nImgChannels = imgThresholded.channels();
  for (int y=0 ;y< nImgHeight ; y++)//对行数进行遍历
  {
    for (int x=0 ;x<nImgWidth ;x++)//对行里面的每一个数进行遍历
    {
        if(imgThresholded.data[y*nImgWidth + x] ==255)//记录白色像素点
        {
            nTargetx += x;
            nTargety += y;
            nPixCount++;
        }
    }
  }
  if(nPixCount>0)
  {
    nTargetx /= nPixCount;
    nTargety /= nPixCount;
    printf("颜色质心坐标(%d,%d),点数%d\n",nTargetx,nTargety,nPixCount);
    //画坐标
    Point line_begin = Point(nTargetx-10,nTargety);
    Point line_end = Point(nTargetx+10,nTargety);
    line(imgOriginal,line_begin,line_end,Scalar(255,0,0));
    line_begin.x = nTargetx;
    line_begin.y = nTargety-10;
    line_end.x   = nTargetx;
    line_end.y   = nTargety+10;
    line(imgOriginal,line_begin,line_end,Scalar(255,0,0));

    //计算机器人运动速度
    float fVelFoward = (nImgHeight/2-nTargety)*0.002;
    float fVelTurn   = (nImgWidth/2-nTargetx)*0.003;
    vel_cmd.linear.x = fVelFoward;
    vel_cmd.linear.y = 0;
    vel_cmd.linear.z = 0;
    vel_cmd.angular.x = 0;
    vel_cmd.angular.y = 0;
    vel_cmd.angular.z = fVelTurn;
  }
  else
  {
    printf("目标颜色消失。。。。\n");
    vel_cmd.linear.x = 0;
    vel_cmd.linear.y = 0;
    vel_cmd.linear.z = 0;
    vel_cmd.angular.x = 0;
    vel_cmd.angular.y = 0;
    vel_cmd.angular.z = 0;
  }
  vel_pub.publish(vel_cmd);
  printf("机器人运动速度 linear.x = %.2f  angular.z = %.2f \n",vel_cmd.linear.x,vel_cmd.angular.z);

  imshow("RGB",imgOriginal);
  imshow("Result",imgThresholded);
  imshow("HSV",imgHSV);
  cv::waitKey(1);
}


int main(int argc,char **argv)
{
    ros::init(argc,argv,"cv_follow_node");
    ros::NodeHandle nh;
    ros::Subscriber rgb = nh.subscribe("kinect2/qhd/image_color_rect",1,Cam_RGB_Callback);
    vel_pub = nh.advertise<geometry_msgs::Twist>("/cmd_vel",30);
    
    namedWindow("Threshold",WINDOW_AUTOSIZE);

    createTrackbar("LowH","Threshold",&iLowH,179);
    createTrackbar("HighH","Threshold",&iHighH,179);

    createTrackbar("LowS","Threshold",&iLowS,255);
    createTrackbar("HighS","Threshold",&iHighS,255);

    createTrackbar("LowV","Threshold",&iLowV,255);
    createTrackbar("HighV","Threshold",&iHighV,255);

    namedWindow("RGB");
    namedWindow("Result");
    namedWindow("HSV");

    ros::Rate loop_rate(30);

    while( ros::ok())
    {
      ros::spinOnce();
      loop_rate.sleep();
    }
}