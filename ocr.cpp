#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv/cv.h>
#include <opencv2/ml/ml.hpp>
//#include <opencv2/opencv.hpp>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <conio.h>
#include <vector>
#define SHOW 600//all show image

using namespace std;
using namespace cv;


CvRect box;
vector<CvRect>boxes;
bool drawing_box = false;

//画感兴趣的区域
void draw_box(IplImage* img, CvRect box)
{
	cvRectangle(img, cvPoint(box.x, box.y), cvPoint(box.x + box.width, box.y + box.height),
		cvScalar(0xff, 0x00, 0x00));


}


void my_mouse_callback(int event, int x, int y, int flag, void * param)//鼠标事件
{
	IplImage *img = (IplImage *)param;
	switch (event)
	{
	case CV_EVENT_LBUTTONDOWN:
	{
								 drawing_box = true;
								 box = cvRect(x, y, 0, 0);
	}
		break;

	case CV_EVENT_MOUSEMOVE:
	{
							   if (drawing_box)
								   box.width = x - box.x;
							   box.height = y - box.y;
	}
		break;
	case CV_EVENT_LBUTTONUP:
	{
							   drawing_box = false;
							   if (box.width<0)
							   {
								   box.x += box.width;
								   box.width *= -1;
							   }
							   if (box.height<0)
							   {
								   box.y += box.height;
								   box.height *= -1;
							   }
							   draw_box(img, box);

							   //cout << box.x << box.y << box.width << box.height << endl;
							   boxes.push_back(box);


	}
		break;

	default:
		break;
	}
}

//手动选区
void manualPostioning(IplImage *src)
{
	IplImage *cloneimge = cvCloneImage(src);


	cvNamedWindow("Box", 1);
	cvMoveWindow("Box", 200, 200);
	cvSetMouseCallback("Box", my_mouse_callback, (void*)src);

	while (1)
	{
		cvCopyImage(src, cloneimge);





		if (drawing_box)
		{
			draw_box(cloneimge, box);

		}


		cvShowImage("src", src);
		cvShowImage("Box", cloneimge);
		if (27 == cvWaitKey(15))
			break;

	}

	//cvReleaseImage(&src);
	cvReleaseImage(&cloneimge);
	cvDestroyWindow("Box");
	//cvDestroyWindow("src");
	cvWaitKey(0);

}

vector<CvRect> select(IplImage *src)
{
	cvNamedWindow("src", 1);
	cvMoveWindow("src", 100, 100);
	//IplImage *src = cvLoadImage("test.jpg");
	cout << "test1!!!!!!!!!!!!!!!!!!";
	if (src == NULL)
		cout << "empty image!" << endl;
	IplImage *img = cvCloneImage(src);
	manualPostioning(img);


	cvWaitKey();
	return boxes;
}


IplImage *g_pGrayImage = NULL;
IplImage *g_pBinaryImage = NULL;
IplImage *pSrcImage = NULL;
const char *pstrWindowsBinaryTitle = "binary";

int nThreshold = 0;

void on_trackbar(int pos)//结果显示
{

	cvThreshold(g_pGrayImage, g_pBinaryImage, pos, 255, CV_THRESH_BINARY);
	//cvShowImage(pstrWindowsBinaryTitle, g_pBinaryImage);
}

void  Binary()
{

	const char *pstrWindowsSrcTitle = "source";
	const char *pstrWindowsToolBarName = "threshold";
	g_pGrayImage = cvCreateImage(cvGetSize(pSrcImage), IPL_DEPTH_8U, 1);

	cvCvtColor(pSrcImage, g_pGrayImage, CV_BGR2GRAY);

	g_pBinaryImage = cvCreateImage(cvGetSize(g_pGrayImage), IPL_DEPTH_8U, 1);

	/*if (n == SHOW)
	{
	cvNamedWindow(pstrWindowsSrcTitle, CV_WINDOW_AUTOSIZE);
		cvShowImage(pstrWindowsSrcTitle, pSrcImage);

		cvNamedWindow(pstrWindowsBinaryTitle, CV_WINDOW_AUTOSIZE);
		cvMoveWindow(pstrWindowsBinaryTitle, 100, 100);
	}*/
	cvCreateTrackbar(pstrWindowsToolBarName, pstrWindowsBinaryTitle, &nThreshold, 254, on_trackbar);
	on_trackbar(160);


	//cvWaitKey(0);
	//std::cout << "hi";
	cvDestroyWindow(pstrWindowsSrcTitle);
	cvDestroyWindow(pstrWindowsBinaryTitle);
	//cvReleaseImage(&pSrcImage);
	cvReleaseImage(&g_pGrayImage);
	//cvReleaseImage(&g_pBinaryImage);


}

CvSeq* contours = NULL;
void CharacterSegment(int num)//图片分割处
{

	static int n = 1;
	//char image_name[25];//图片的名称
	CvMemStorage* storage = cvCreateMemStorage(0);

	int count = cvFindContours(g_pBinaryImage, storage, &contours, sizeof(CvContour), CV_RETR_EXTERNAL);
	printf("contours's number:%d \n", count);

	int idx = 0;
	char szName[56] = { 0 };
	int tempCount = 0;
	for (CvSeq* c = contours; c != NULL; c = c->h_next) {

		CvRect rc = cvBoundingRect(c, 0);

		printf("coordinate: x:%d y:%d  width:%d height:%d windowID:%d \n", rc.x, rc.y, rc.width, rc.height, idx);

		cvDrawRect(pSrcImage, cvPoint(rc.x, rc.y), cvPoint(rc.x + rc.width, rc.y + rc.height), CV_RGB(255, 0, 0));
		IplImage* imgNo = cvCreateImage(cvSize(rc.width, rc.height), IPL_DEPTH_8U, 3);
		cvSetImageROI(pSrcImage, rc);

		cvCopyImage(pSrcImage, imgNo);
		cvResetImageROI(pSrcImage);

		sprintf(szName, "wnd_%d", idx++);

		cvReleaseImage(&imgNo);
	}
	//sprintf(image_name, "%s%d%s", "B:\092716102\ocr视频测试\\测试\\a", n, ".bmp");//保存的图片名
	//cvSaveImage(image_name, pSrcImage);   //保存分割图片
	if (n == SHOW||num==1)
	{
		cvShowImage("t_src", pSrcImage);//分割的图像显示

	}
	n++;

}

CvSeq* segment(IplImage *src, int &threshold,int sum)
{
	//std::cout << "depth:" << src->depth;
	puts("");

	pSrcImage = cvCreateImage(cvGetSize(src), src->depth, src->nChannels);
	cvCopyImage(src, pSrcImage);
	//pSrcImage = cvCloneImage(src);
	//pSrcImage = src;

	Binary();
	printf("threhold:%d", nThreshold);
	threshold = nThreshold;
	//threshold = 150;
	CharacterSegment(sum);
	return contours;
	//	cvWaitKey(0);
}


int predict(IplImage *test)
{

	char result[300];
	float ret;
	CvSVM svm;
	svm.load("HOG_SVM_DATA.xml");


	if (!test)
	{
		cout << "not exist" << endl;
		return -1;
	}
	cout << "svm load" << endl;
	IplImage* trainTempImg = cvCreateImage(cvSize(28, 28), 8, 3);
	cvZero(trainTempImg);
	cvResize(test, trainTempImg);
	HOGDescriptor *hog = new HOGDescriptor(cvSize(28, 28), cvSize(14, 14), cvSize(7, 7), cvSize(7, 7), 9);
	vector<float>descriptors;
	hog->compute(trainTempImg, descriptors, Size(1, 1), Size(0, 0));
	cout << "HOG dims: " << descriptors.size() << endl;
	CvMat* SVMtrainMat = cvCreateMat(1, descriptors.size(), CV_32FC1);
	int n = 0;
	//for ( vector<float>::iterator iter = descriptors.begin(); iter != descriptors.end(); iter++)
	for (int i = 0; i < descriptors.size(); i++)
	{
		//cvmSet(SVMtrainMat, 0, n, *iter);
		cvmSet(SVMtrainMat, 0, n, descriptors[i]);
		n++;
	}

	ret= svm.predict(SVMtrainMat);
	sprintf(result, "%s\r\n", ret);
	//分割图显示


	//cvNamedWindow("dst", 1);
	//cvShowImage("dst", test);


	//cvReleaseImage(&test);
	cvReleaseImage(&trainTempImg);

	return ret;
}


class Digit			//单独的数字字符
{
public:
	Digit(CvRect rect) :rect(rect){};
	CvRect rect;
};

class Value		//从选区内识别的数值
{
public:
	Value(CvRect rect) :rect(rect){};
	CvRect rect;
};


bool SortByOrdinates(const Value &value1, const Value &value2);		//按纵坐标排序
bool SortByHorizontal(const Digit &digit1, const Digit &digit2);	//按横坐标排序



bool SortByOrdinates(const Value &value1, const Value &value2)
{
	return value1.rect.y <value2.rect.y;
}



bool SortByHorizontal(const Digit &digit1, const Digit &digit2)
{
	return digit1.rect.x < digit2.rect.x;
}


string num2str(double i)
{
	stringstream ss;
	ss << i;
	return ss.str();
}

IplImage *ocr(IplImage *src, int num)
{

	vector<CvRect>t_rois;//保存选区
	static vector<CvRect>t_srois;
	//IplImage *src = cvLoadImage("CPAP1.png");
	if (num == 1)//判断
		t_rois = select(src), t_srois = t_rois;
		//选区,并判断是否为对第一帧的处理
	else
	{
		t_rois = t_srois;
	}
	ofstream out("out.txt",ios::app);
	vector<Value>values;

	// auto it = t_rois.begin();

	//while (it != t_rois.end())
	for (int i = 0; i < t_rois.size(); i++)
	{
		Value value(t_rois[i]);
		values.push_back(value);
		//it++;
	}

	sort(values.begin(), values.end(), SortByOrdinates);
	cout << "the number of values:" << values.size() << endl;

	//auto it_value = values.begin();
	int n = 0;
	//while (it_value != values.end())
	for (int i = 0; i < values.size(); i++)
	{
		string ch = num2str(n);
		string str = "ROI";
		str += ch;
		const char *windowName = str.c_str();

		IplImage *img = cvCloneImage(src);
		//cvSetImageROI(img, it_value->rect);
		cvSetImageROI(img, values[i].rect);
		//cvNamedWindow(windowName, 1);
		//cvShowImage(windowName, img);
		cout << "n:" << n << endl;
		n++;
	}



	//it_value = values.begin();
	 (int)n = 0;
	//while (it_value != values.end())
	for (int i = 0; i < values.size(); i++)
	{
		//ROIn
		string ch = num2str(n);
		string str = "ROI";
		str += ch;
		const char *windowName = str.c_str();


		IplImage *img = cvCloneImage(src);
		//cout << "test2" << endl;
		//cvSetImageROI(img, it_value->rect);
		cvSetImageROI(img, values[i].rect);
		if (i == SHOW)
		{
			cvNamedWindow(windowName, 1);
			cvShowImage(windowName, img);
		}

		CvSeq* cha_con;
		int thre = 125;
		cha_con = segment(img, thre,num);// 1）二值化 2） 字符分割
		vector<Digit>digits;
		for (CvSeq* c = cha_con; c != NULL; c = c->h_next)
		{
			CvRect rc = cvBoundingRect(c, 0);
			Digit digit(rc);
			digits.push_back(digit);

		}
		sort(digits.begin(), digits.end(), SortByHorizontal);


		string result;
		//auto it_cha = digits.begin();
		//while (it_cha != digits.end())
		for (int i = 0; i < digits.size(); i++)
		{
			//ROIn
			string ch = num2str(n);
			string str = "ROI";

			str += ch;
			const char *windowName = str.c_str();
			cout << "img_value's depth:" << img->depth << "channels:" << img->nChannels << endl;
			//pSrcImage = cvCreateImage(cvGetSize(src), src->depth, src->nChannels);
			IplImage* img_digit = cvCreateImage(cvGetSize(img), img->depth, img->nChannels);
			cvCopyImage(img, img_digit);

			//cvSetImageROI(img_digit, it_cha->rect);
			cvSetImageROI(img_digit, digits[i].rect);
			cout << "width:" << img_digit->width << "height:" << img_digit->height << endl;
			//cout << "coordinate:" << it_cha->rect.x << "," << it_cha->rect.y << endl;
			cout << "coordinate:" << digits[i].rect.x << "," << digits[i].rect.y << endl;
			//cvNamedWindow(windowName, 1);
			//cvShowImage(windowName, img_digit);

			float ret;
			string ch_ret;
			//	if (rc.width < 6 || rc.width>26 || rc.height < 16 || rc.height>35)

			if (digits[i].rect.width < 8 && digits[i].rect.height < 8)
			{
				ch_ret += ".";
			}
			else
			{
				ret = predict(img_digit);
				ch_ret = num2str(ret);
			}



			result = result + ch_ret;

			cvResetImageROI(img_digit);				//severances
			//cout << "test!!!!!!!!!!!!!!!!" << endl;
			//cvReleaseImage(&img_digit);
			n++;
			//it_cha++;

		}
		cvResetImageROI(img);
		cvReleaseImage(&img);
		out << "第"<<num<<"次结果为:    "<<result << "\n";
		cout << "the value is : " << result << endl;

		//it_value++;

	}


	out.close();
	return src;
	cvWaitKey(0);
}


int main(int argc, char** argv)
{

	CvCapture* capture = cvCaptureFromAVI("1.mp4");//获取视频
	static int i = 1;//图片编号
	IplImage* src;//存放图片
	///char image_name[25];//图片的名称
	//读取和显示
	char y;
	int num = 1;
	float time1,time2;
	time1 = getTickCount();
	int frames = (int)cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_COUNT);     //获取视频总帧数
	cout << "总帧数为" << frames << endl;
	src = cvQueryFrame(capture);
	if (src == NULL)
	{
		cout << "src is empty!" << endl;
		exit(0);
	}
	do
	{


		cout << "是否作用于所有帧（输入y保存所有帧,其余值结束）:" << endl;
		cin >> y;//判断是否保存所有帧
		if (num == 1 )
		{
			printf("\n*************************第%d次处理***************************\n", i);
			ocr(src, num);
			if (y != 'y')waitKey(0);
			num++;
			i++;

		}
		if (y != 'y')
		{
			time2 = getTickCount();
			cout << "运行时间为:" << (time2 - time1) / getTickFrequency() << "秒" << endl;
			exit(0);
		}
		if (num!=1&&y == 'y')
		{
			while (1)
			{
				while (1)
				{
					src = cvQueryFrame(capture);
					if(waitKey(100)==-1)break;
				}
				printf("\n*************************第%d次处理***************************\n", i);
				ocr(src, num);
				if (!src)
					break;
				Mat image(src, true);
				imshow("正在处理视频的视频帧", image);
				i++;
				if (i > SHOW){
					time2 = getTickCount();
					cout << "运行时间为:" << (time2 - time1) / getTickFrequency() << "秒" << endl;
					cvReleaseCapture(&capture);//释放视频
					cvReleaseImage(&src);
					cvDestroyWindow("视频");
					waitKey(0);
					exit(0); //break;
				}

				num++;
				//sprintf(image_name, "%s%d%s", "B:\\092716102\\测试\\测试\\", ++i, ".bmp");//保存的图片名
				//cvSaveImage(image_name, src);   //保存一帧图片
			}


		}



	} while (1);
	//getTemplate();
	return 0;

}
