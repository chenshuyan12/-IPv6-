#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/imgproc/imgproc_c.h>
#include<opencv2/core/core.hpp>
#include<opencv2/core/operations.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/objdetect/objdetect.hpp>
#include<stdio.h>
#include <ml.h>  
#include <fstream>  
#include <string>  
#include <vector>  
#include<iostream>

using namespace cv;
using namespace std;

char temp[16]="patient";
int j = 0;

class MySVM : public CvSVM
{
public:
	//获得SVM的决策函数中的alpha数组
	double * get_alpha_vector()
	{
		return this->decision_func->alpha;
	}

	//获得SVM的决策函数中的rho参数,即偏移量
	float get_rho()
	{
		return this->decision_func->rho;
	}
};

//IplImage *testc=NULL;

int k = 1;
void Person(Mat test)
{
	if (k == 1)
	{
		//IplImage *test;
		HOGDescriptor hog(Size(16, 16), Size(8, 8), Size(8, 8), Size(4, 4), 9);
		//HOGDescriptor hog(Size(300, 300), Size(250, 250), Size(250, 250), Size(100, 100), 9);
		int DescriptorDim;
		if (test.data == NULL)
		{
			cout << " can not load the image: " << endl;
			//return -1;
		}
		MySVM svm;
		svm.load("SVMTWO.xml");
		printf("Xml is success\n");
		DescriptorDim = svm.get_var_count();
		int supportVectorNum = svm.get_support_vector_count();
		Mat alphaMat = Mat::zeros(1, supportVectorNum, CV_32FC1);
		Mat supportVectorMat = Mat::zeros(supportVectorNum, DescriptorDim, CV_32FC1);
		Mat resultMat = Mat::zeros(1, DescriptorDim, CV_32FC1);
		for (int i = 0; i < supportVectorNum; i++)
		{
			const float * pSVData = svm.get_support_vector(i);
			for (int j = 0; j < DescriptorDim; j++)
			{
				supportVectorMat.at<float>(i, j) = pSVData[j];
			}
		}
		double * pAlphaData = svm.get_alpha_vector();
		for (int i = 0; i < supportVectorNum; i++)
		{
			alphaMat.at<float>(0, i) = pAlphaData[i];
		}
		resultMat = -1 * alphaMat * supportVectorMat;
		vector<float> myDetector;
		for (int i = 0; i < DescriptorDim; i++)
		{
			myDetector.push_back(resultMat.at<float>(0, i));
		}
		myDetector.push_back(svm.get_rho());
		k = 0;
	}
	HOGDescriptor myHOG;
	myHOG.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());
	vector<Rect> found, found_filtered;
	myHOG.detectMultiScale(test, found, 0, Size(8, 8), Size(32, 32), 1.04, 3);//对图片进行多尺度行人检测
	//myHOG.detectMultiScale(test, found, 0, Size(250, 250), Size(300, 300), 1.05, 2);
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
		//r.x += cvRound(r.width*0.1);
		r.x += cvRound(r.width*0.5);
		r.width = cvRound(r.width*0.8);/*
		r.y += cvRound(r.height*0.07);
		r.height = cvRound(r.height*0.8);*/
		rectangle(test, r.tl(), r.br(), Scalar(0, 255, 0), 3);
		Rect rect;//选择矩形框区域内的内容
		rect = Rect(r.x, r.y, r.width, r.height);
		CvPoint textPos = cvPoint(r.x, r.y);
		putText(test, temp, textPos, 1, 1, Scalar(0, 0, 255), 1);
	}
	imshow("test", test);
	//waitKey(0);
}
int main(int argc, char** argv)
{
	int count = 0;
	Mat frame;
	//VideoCapture capture("onlypatient.wmv");
	VideoCapture capture("onlypatient.wmv");
	if (!capture.isOpened())
	{
		cout << "fail to open!" << endl;
		return -1;
	}
	while (1)
	{
		capture >> frame;
		if (frame.empty())
			break;
		//if (1)
		//{
		//	printf("YES\n");
			Person(frame);
			//cvWaitKey(0);
		//}
		//else
		//{
			//printf("NO\n");
		//}
		//if (waitKey(2) == 1)
			//break;
		/*if (count == 180)
			break;*/
		cvWaitKey(5);
		printf("%d\n",count);
		
		count++;
	}
	capture.release();
	//waitKey(0);
	return 0;
}