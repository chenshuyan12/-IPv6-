#include <iostream>
#include <fstream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/ml/ml.hpp>

using namespace std;
using namespace cv;
//出现乱码，所以中文注释没了。

Mat frame;
Mat org, img, tmp;
int x1=0, x2=0, y11=0, y22=0;
class MySVM : public CvSVM
{
public:
	double * get_alpha_vector()
	{
		return this->decision_func->alpha;
	}
	float get_rho()
	{
		return this->decision_func->rho;
	}
};
int choose(int x, int y)
{
	if (((x > x1) && (x<x2)) && ((y>y11) && (y < y22)))
		return 1;
	return 0;
}
void on_mouse(int event, int x, int y, int flags, void *ustc)//event鼠标事件代号，x,y鼠标坐标，flags拖拽和键盘操作的代号  
{
	static Point pre_pt = (-1, -1);//初始坐标  
	static Point cur_pt = (-1, -1);//实时坐标  
	char temp[16];
	if (event == CV_EVENT_LBUTTONDOWN)//左键按下，读取初始坐标，并在图像上该点处划圆  
	{
		org.copyTo(img);//将原始图片复制到img中  
		sprintf(temp, "(%d,%d)", x, y);
		pre_pt = Point(x, y);
		putText(img, temp, pre_pt, FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0, 255), 1, 8);//在窗口上显示坐标  
		circle(img, pre_pt, 2, Scalar(255, 0, 0, 0), CV_FILLED, CV_AA, 0);//划圆  
		imshow("img", img);
	}
	else if (event == CV_EVENT_MOUSEMOVE && !(flags & CV_EVENT_FLAG_LBUTTON))//左键没有按下的情况下鼠标移动的处理函数  
	{
		img.copyTo(tmp);//将img复制到临时图像tmp上，用于显示实时坐标  
		sprintf(temp, "(%d,%d)", x, y);
		cur_pt = Point(x, y);
		putText(tmp, temp, cur_pt, FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0, 255));//只是实时显示鼠标移动的坐标  
		imshow("img", tmp);
	}
	else if (event == CV_EVENT_MOUSEMOVE && (flags & CV_EVENT_FLAG_LBUTTON))//左键按下时，鼠标移动，则在图像上划矩形  
	{
		img.copyTo(tmp);
		sprintf(temp, "(%d,%d)", x, y);
		cur_pt = Point(x, y);
		putText(tmp, temp, cur_pt, FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0, 255));
		rectangle(tmp, pre_pt, cur_pt, Scalar(0, 255, 0, 0), 1, 8, 0);//在临时图像上实时显示鼠标拖动时形成的矩形  
		imshow("img", tmp);
	}
	else if (event == CV_EVENT_LBUTTONUP)//左键松开，将在图像上划矩形  
	{
		org.copyTo(img);
		sprintf(temp, "(%d,%d)", x, y);
		cur_pt = Point(x, y);
		putText(img, temp, cur_pt, FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0, 255));
		circle(img, pre_pt, 2, Scalar(255, 0, 0, 0), CV_FILLED, CV_AA, 0);
		rectangle(img, pre_pt, cur_pt, Scalar(0, 255, 0, 0), 1, 8, 0);//根据初始点和结束点，将矩形画到img上  
		imshow("img", img);
		img.copyTo(tmp);
		//截取矩形包围的图像，并保存到dst中  
		x1 = pre_pt.x;
		cout << x1 << endl;
		x2 = cur_pt.x;
		cout << x2 << endl;
		y11 = pre_pt.y;
		cout << y11 << endl;
		y22 = cur_pt.y;
		cout << y22 << endl;
	}
}
int main(int argc, char** argv)
{
	HOGDescriptor hog(Size(64, 128), Size(16, 16), Size(8, 8), Size(8, 8), 9);
	int DescriptorDim;
	MySVM svm;
	svm.load("2.xml");
	DescriptorDim = svm.get_var_count();
	int supportVectorNum = svm.get_support_vector_count();
	Mat alphaMat = Mat::zeros(1, supportVectorNum, CV_32FC1);
	Mat supportVectorMat = Mat::zeros(supportVectorNum, DescriptorDim, CV_32FC1);
	Mat resultMat = Mat::zeros(1, DescriptorDim, CV_32FC1);
	for (int i = 0; i<supportVectorNum; i++)
	{
		const float * pSVData = svm.get_support_vector(i);
		for (int j = 0; j<DescriptorDim; j++)
		{
			supportVectorMat.at<float>(i, j) = pSVData[j];
		}
	}
	double * pAlphaData = svm.get_alpha_vector();
	for (int i = 0; i<supportVectorNum; i++)
	{
		alphaMat.at<float>(0, i) = pAlphaData[i];
	}
	resultMat = -1 * alphaMat * supportVectorMat;
	vector<float> myDetector;
	for (int i = 0; i<DescriptorDim; i++)
	{
		myDetector.push_back(resultMat.at<float>(0, i));
	}
	myDetector.push_back(svm.get_rho());
	int i = 0;
	int x=0,y=0;
	VideoCapture cap("1.wmv");
	if (!cap.isOpened())
	{
		return -1;
	}
	//Mat frae;
	vector<Point> vectorp;
	while (1)
	{
		cap >> frame;
		if (i == 0)
		{
			frame.copyTo(org);
			org.copyTo(img);
			org.copyTo(tmp);
			namedWindow("img");//定义一个img窗口  
			setMouseCallback("img", on_mouse, 0);//调用回调函数  
			imshow("img", img);
			cv::waitKey(0);
		}
		if (frame.empty()) break;
		HOGDescriptor myHOG;
		myHOG.setSVMDetector(myDetector);
		vector<Rect> found, found_filtered;
		myHOG.detectMultiScale(frame, found, 0, Size(8, 8), Size(32, 32), 1.05, 2);
		for (int i = 0; i < found.size(); i++)
		{
			Rect r = found[i];
			int j = 0;
			for (; j < found.size(); j++)
			if (j != i && (r & found[j]) == r)
			break;
			if (j == found.size())
			found_filtered.push_back(r);
		}
		for (int i = 0; i<found_filtered.size(); i++)
		{
		Rect r = found_filtered[i];
		r.x += cvRound(r.width*0.1);
		r.width = cvRound(r.width*0.8);
		r.y += cvRound(r.height*0.07);
		r.height = cvRound(r.height*0.8);
		x=r.x+(r.width)/2.0;
		y=r.y+(r.height)/2.0;
		if (choose(x, y) == 1)
			cout << "in the room" << endl;
		else
			cout << "out the room" << endl;
		Point p = Point(x,y);
		circle(frame,p,8,Scalar(0,255,0),-1);
		rectangle(frame, r.tl(), r.br(), Scalar(0, 255, 0), 3);	
		vectorp.push_back(p);
		}
		x=y=0;
		imshow("now", frame);
		cout << "The"<< i++ << "times"<< endl;
		if (waitKey(2)==1)
			break;
	}
	cvDestroyWindow("now");
	return 0;
}

