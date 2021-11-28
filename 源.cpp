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
	//��������ֵ�Ƶ��
	int sum = image.cols*image.rows;
	float p[256];
	for (int i = 0; i<256; i++){
		p[i] = hist.at<float>(i) / (1.0*sum);
		//cout<<p[i]<<" ";	
	}
	double max_val;  //ֱ��ͼ�����ֵ 
	minMaxLoc(hist, 0, &max_val, 0, 0); //����ֱ��ͼ���ֵ
	//����ֱ��ͼ
	int maxheight = 256;
	Mat image2 = Mat::zeros(256, 2 * 256, CV_8UC3);
	for (int i = 0; i<256; i++){
		//����߶�
		double height = (maxheight*hist.at<float>(i)) / (1.0*max_val);
		//������Ӧ�ĸ߶�ͼ
		//������ϵ�е�������궨��ΪͼƬ�����Ͻǣ�X��Ϊͼ����ε���������ˮƽ�ߣ��������ң�Y��Ϊͼ�������ߵ�������ֱ�ߣ��������¡���Point(x,y)��Rect(x,y)�У���һ������x�������Ԫ������ͼ����������ڶ�������y�������Ԫ������ͼ�������������at(x,y)�����෴�ġ�
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
	minMaxLoc(hist, &minValue, &maxValue, 0, 0);//�ҵ�ȫ����С����������ֵ��Ŀ
	cout << "max: " << maxValue << "min: " << minValue << endl;
	int histSize = hist.rows;
	Mat histImage(histSize, histSize, CV_8UC3, Scalar(255, 255, 255));

	int hpt = static_cast<int>(0.9*histSize);
	int total = 0;
	Scalar color(172, 172, 150);//BGR
	for (int h = 0; h < histSize; h++)
	{
		float binVal = hist.at<float>(h);//��ȡ��Ӧ�Ҷȼ������ظ���,һ��1000000��
		//cout << h << ": " << binVal << endl;
		total += binVal;
		int intensity = static_cast<int>(binVal*hpt / maxValue);//���������㣬��ǰ��Ŀ*230/�����Ŀ,���������ֻ�Ǳ�����ͬ
		line(histImage, Point(h, histSize), Point(h, histSize - intensity), color);
		//rectangle(histImage, Point(h, histSize), Point(h + 1, histSize - intensity), color);
	}
	cout << total << endl;//total = 1000000
	return histImage;
}
void HuiJieZhiFangTu(Mat image){
	int a = image.channels();//����ͨ����
	int  image_count = 1; //Ҫ����ֱ��ͼ��ͼ��ĸ���
	int channels[1] = { 0 };//ͼ���ͨ��
	Mat out;//����õ���ֱ��ͼ
	int dims = 1;//����õ���ֱ��ͼ��ά��
	int histsize[1] = { 256 };//ֱ��ͼ���������������
	float hrange[2] = { 0, 255 };//������ܷ�Χ
	const float *ranges[1] = { hrange };//ָ������
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
	Mat framePrePre; //����һ֡  
	Mat framePre; //��һ֡  
	Mat frameNow; //��ǰ֡  
	Mat frameDet; //�˶�����  
	Mat showNow;//��ʾ��ǰ��֡
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
		absdiff(framePrePre, framePre, Det1);  //֡��1  
		absdiff(framePre, frameNow, Det2);     //֡��2  
		threshold(Det1, Det1, 0, 255, CV_THRESH_OTSU);  //����Ӧ��ֵ��  
		threshold(Det2, Det2, 0, 255, CV_THRESH_OTSU);
		Mat element = getStructuringElement(0, Size(3, 3));  //���ͺ�  
		dilate(Det1, Det1, element);    //����  
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
			rectangle(showNow, boundRect[i], Scalar(0, 255, 0), 2);//��ǳ����
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
