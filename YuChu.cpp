#include "YuChu.h"

static IplImage* background=NULL;
void GetBackGround(IplImage* image){
	background = cvCloneImage(image);//存取背景图片
	cvNamedWindow("background",1);
	cvShowImage("background",background);
}
bool equal(double a, double b) {
	if ((a - b> -0.000001) && (a - b) < 0.000001)
		return true;
	else
		return false;
}
int BackgroundMothed(Mat background, Mat Front) {
	int flag = 0;
	Mat Bresult = Front.clone();
	Mat Dobad;//做差后的图像
	Mat Bcgray, Frgray;//灰度化后的图像
	cvtColor(background, Bcgray, CV_BGR2GRAY);//转为灰度化图像
	cvtColor(Front, Frgray, CV_BGR2GRAY);
	absdiff(Bcgray, Frgray, Dobad);//图像做差运算
	Mat Dobad_thresh;
	threshold(Dobad, Dobad_thresh, 50, 255, CV_THRESH_BINARY);//对差值图像做阈值化的操作
	Mat Kernel_ero = getStructuringElement(MORPH_RECT, Size(3, 3));
	Mat Kernel_dil = getStructuringElement(MORPH_RECT, Size(18, 18));
	erode(Dobad_thresh, Dobad_thresh, Kernel_ero);//腐蚀
	dilate(Dobad_thresh, Dobad_thresh, Kernel_dil);//膨胀
	namedWindow("dilate", 0);
	imshow("dilate", Dobad_thresh);
	double minv = 0.0, maxv = 0.0;
	double* minp = &minv;
	double* maxp = &maxv;
	minMaxIdx(Dobad_thresh, minp, maxp);
	cout << "Mat minv = " << minv << endl;
	cout << "Mat maxv = " << maxv << endl;
	if (!equal(minv,maxv)){
		flag = 1;
	}
	cout << "Flag 0 == " << flag << endl;
	return flag;
}
int ThreeFrameDifferenceMethod(IplImage* now){
	Mat one(background, true);
	Mat two(now, true);
	return BackgroundMothed(one, two);
}