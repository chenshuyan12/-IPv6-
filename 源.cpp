#include "core/core.hpp"  
#include "highgui/highgui.hpp"  
#include "imgproc/imgproc.hpp"  
#include<iostream>
#include<cv.hpp>

using namespace cv;
using namespace std;
void mycalhist(Mat image){
	MatND hist;
	int bins = 256;
	int hist_size[] = { bins };
	float range[] = { 0, 256 };
	const float* ranges[] = { range };
	int channels[] = { 0 };
	calcHist(&image, 1, channels, Mat(), hist, 1, hist_size, ranges, true, false);
	//计算出出现的频率
	int sum = image.cols*image.rows;
	float p[256];
	for (int i = 0; i<256; i++){
		p[i] = hist.at<float>(i) / (1.0*sum);
		//cout<<p[i]<<" ";	
	}
	double max_val;  //直方图的最大值 
	minMaxLoc(hist, 0, &max_val, 0, 0); //计算直方图最大值
	//画出直方图
	int maxheight = 256;
	Mat image2 = Mat::zeros(256, 2 * 256, CV_8UC3);
	for (int i = 0; i<256; i++){
		//计算高度
		double height = (maxheight*hist.at<float>(i)) / (1.0*max_val);
		//画出对应的高度图
		//坐标体系中的零点坐标定义为图片的左上角，X轴为图像矩形的上面那条水平线，从左往右；Y轴为图像矩形左边的那条垂直线，从上往下。在Point(x,y)和Rect(x,y)中，第一个参数x代表的是元素所在图像的列数，第二个参数y代表的是元素所在图像的行数，而在at(x,y)中是相反的。
		rectangle(image2, Point(i * 2, 255),Point((i + 1) * 2 - 1, 255 - height),CV_RGB(255, 255, 255));
	}
	imshow("hist", image2);
	//cout<<endl;
}
Mat getHistImage(const MatND& hist)
{
	double maxValue = 0;
	double minValue = 0;
	int a = 0;
	for (int i = 0; i < hist.rows; i++)
	{
		for (int j = 0; j < hist.cols; j++)
		{
			float b = hist.at<float>(i, j);
			a += 1;
			//cout << b << endl;
		}
	}
	minMaxLoc(hist, &minValue, &maxValue, 0, 0);//找到全局最小、最大的像素值数目
	cout << "max: " << maxValue << "min: " << minValue << endl;
	int histSize = hist.rows;
	Mat histImage(histSize, histSize, CV_8UC3, Scalar(255, 255, 255));

	int hpt = static_cast<int>(0.9*histSize);
	int total = 0;
	Scalar color(172, 172, 150);//BGR
	for (int h = 0; h < histSize; h++)
	{
		float binVal = hist.at<float>(h);//读取对应灰度级的像素个数,一共1000000个
		//cout << h << ": " << binVal << endl;
		total += binVal;
		int intensity = static_cast<int>(binVal*hpt / maxValue);//按比例运算，当前数目*230/最大数目,与除以总数只是比例不同
		line(histImage, Point(h, histSize), Point(h, histSize - intensity), color);
		//rectangle(histImage, Point(h, histSize), Point(h + 1, histSize - intensity), color);
	}
	cout << total << endl;//total = 1000000
	return histImage;
}
void HuiJieZhiFangTu(Mat image){
	int a = image.channels();//计算通道数
	int  image_count = 1; //要计算直方图的图像的个数
	int channels[1] = { 0 };//图像的通道
	Mat out;//计算得到的直方图
	int dims = 1;//计算得到的直方图的维数
	int histsize[1] = { 256 };//直方图横坐标的子区间数
	float hrange[2] = { 0, 255 };//区间的总范围
	const float *ranges[1] = { hrange };//指针数组
	calcHist(&image, image_count, channels, Mat(), out, dims, histsize, ranges);
	Mat last = getHistImage(out);
	imshow("ZhiFangTu", last);
}
int main()
{
	VideoCapture videoCap;
	videoCap.open(0);
	if (!videoCap.isOpened())
	{
		return -1;
	}
	Mat framePrePre; //上上一帧  
	Mat framePre; //上一帧  
	Mat frameNow; //当前帧  
	Mat frameDet; //运动物体  
	Mat showNow;//显示当前的帧
	videoCap >> framePrePre;
	videoCap >> framePre;
	cvtColor(framePrePre, framePrePre, CV_RGB2GRAY);
	cvtColor(framePre, framePre, CV_RGB2GRAY);
	int save = 0;
	while (true)
	{
		videoCap >> frameNow;
		cvtColor(frameNow, frameNow, CV_RGB2GRAY);
		Mat Det1;
		Mat Det2;
		absdiff(framePrePre, framePre, Det1);  //帧差1  
		absdiff(framePre, frameNow, Det2);     //帧差2  
		threshold(Det1, Det1, 0, 255, CV_THRESH_OTSU);  //自适应阈值化  
		threshold(Det2, Det2, 0, 255, CV_THRESH_OTSU);
		Mat element = getStructuringElement(0, Size(3, 3));  //膨胀核  
		dilate(Det1, Det1, element);    //膨胀  
		dilate(Det2, Det2, element);
		bitwise_and(Det1, Det2, frameDet);
		framePrePre = framePre;
		framePre = frameNow;
		vector<vector<Point>> Point;
		findContours(frameDet, Point, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
		vector<Rect> boundRect(Point.size());
		showNow = frameNow.clone();
		for (int i = 0; i < Point.size(); i++)
		{
			boundRect[i] = boundingRect(Point[i]);
			rectangle(showNow, boundRect[i], Scalar(0, 255, 0), 2);//标记出人物。
		}
		HuiJieZhiFangTu(frameDet);
		mycalhist(frameDet);
		imshow("Video", frameNow);
		imshow("Detection", frameDet);
		imshow("VideoShow", showNow);
		if (cvWaitKey(27) == 27)
			break;
	}
	return 0;
}
