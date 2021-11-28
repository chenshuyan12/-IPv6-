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


CvRect box;   //���νṹ x,y��height,width
vector<CvRect>boxes;
bool drawing_box = false;

//������Ȥ������
void draw_box(IplImage* img, CvRect box)
{
	cvRectangle(img, cvPoint(box.x, box.y), cvPoint(box.x + box.width, box.y + box.height),
		cvScalar(0xff, 0x00, 0x00));

}


void my_mouse_callback(int event, int x, int y, int flag, void * param)//����¼�
{
	IplImage *img = (IplImage *)param;
	switch (event)
	{
	case CV_EVENT_LBUTTONDOWN:   ///���������
	{
								 drawing_box = true;
								 box = cvRect(x, y, 0, 0);
	}
		break;

	case CV_EVENT_MOUSEMOVE:   //��껬��
	{
							   if (drawing_box)
								   box.width = x - box.x;
							   box.height = y - box.y;
	}
		break;
	case CV_EVENT_LBUTTONUP:   /////�������ͷ� 
	{
							   drawing_box = false;
							    if (box.width<0)
							   {
									

								   cout << "��ȣ�" << box.width << endl;
								   box.x += box.width;   //x
								   box.width *= -1;	     //width																					
							   }
							   if (box.height<0)
							   {
								   box.y += box.height;  //y
								   box.height *= -1;     //height
							   }
							   draw_box(img, box);    //��ͼ box.x = x; box.y = y; width  height
							   //cout << box.x << box.y << box.width << box.height << endl;
							   boxes.push_back(box);
	}
		break;
	default:
		break;  
	}
}

//�ֶ�ѡ��
void manualPostioning(IplImage *src)   
{
	IplImage *cloneimge = cvCloneImage(src);   //��ȫ����src  ��¡��
	cvNamedWindow("Box", 1);
	cvMoveWindow("Box", 200, 200);   //�ı䴰��λ��
	cvSetMouseCallback("Box", my_mouse_callback, (void*)src);   //��ص�����  ��ѡ��

	while (1)
	{
		cvCopyImage(src, cloneimge);   ///ֻ�Ḵ��ROI����ȡ���clone����֮��Դͼ�����ڴ�����ʧ�󣬸��Ƶ�ͼ��Ҳ���ˣ�
		                               //����copy���ƣ�Դͼ����ʧ�󣬸��Ƶ�ͼ�񲻱�
		if (drawing_box)
		{ 
			draw_box(cloneimge,box);    //ѡ��ͼ
		}
		cvShowImage("src", src);
		cvShowImage("Box", cloneimge);
		if (27 == cvWaitKey(15))
			break;
	}
	//cvReleaseImage(&src);
	cvReleaseImage(&cloneimge);  //�ͷ�
	cvDestroyWindow("Box");      //���ٴ���
	//cvDestroyWindow("src");
	cvWaitKey(0);

}

vector<CvRect> select(IplImage *src)
{ 
	cvNamedWindow("src", 1);
	cvMoveWindow("src", 100, 100);
	//IplImage *src = cvLoadImage("test.jpg");   //C��ʽ��ȡͼƬ
	cout << "test1!!!!!!!!!!!!!!!!!!";
	if (src == NULL)
		cout << "empty image!" << endl;
	IplImage *img = cvCloneImage(src);   //?????????????????????????//
	manualPostioning(img);               //�ֶ�ѡ��  boxes
	cvWaitKey();
	return boxes;
}


IplImage *g_pGrayImage = NULL;    //ԭͼ��
IplImage *g_pBinaryImage = NULL;  //��ֵ��֮������ͼ��  ���ǵ�ͨ����
IplImage *pSrcImage = NULL;
const char *pstrWindowsBinaryTitle = "binary";

int nThreshold = 0;

void on_trackbar(int pos)//�����ʾ         pos����ֵ
{

	cvThreshold(g_pGrayImage, g_pBinaryImage, pos, 255, CV_THRESH_BINARY);  //�Ҷ�ͼ�������ֵ�����õ���ֵͼ��
	//cvShowImage(pstrWindowsBinaryTitle, g_pBinaryImage);
}

void  Binary()
{

	const char *pstrWindowsSrcTitle = "source";
	const char *pstrWindowsToolBarName = "threshold";
	g_pGrayImage = cvCreateImage(cvGetSize(pSrcImage), IPL_DEPTH_8U, 1);   //����ͼ���׵�ַ��������洢�ռ�    cvGetSize����width��height
	//�õ�ͬ��С�Ҷ�ͼ                                                    //cvCreateImage(CvSize cvSize(int width, int height), int depth, int channels);

	cvCvtColor(pSrcImage, g_pGrayImage, CV_BGR2GRAY);                      //����cvCvtColorʵ��ɫ�ʿռ�ת��   ע��ͼƬͨ��˳����BGR

	g_pBinaryImage = cvCreateImage(cvGetSize(g_pGrayImage), IPL_DEPTH_8U, 1);

	/*if (n == SHOW)
	{
	cvNamedWindow(pstrWindowsSrcTitle, CV_WINDOW_AUTOSIZE);
	cvShowImage(pstrWindowsSrcTitle, pSrcImage);
	cvNamedWindow(pstrWindowsBinaryTitle, CV_WINDOW_AUTOSIZE);
	cvMoveWindow(pstrWindowsBinaryTitle, 100, 100);
	}*/
	cvCreateTrackbar(pstrWindowsToolBarName, pstrWindowsBinaryTitle, &nThreshold, 254, on_trackbar);
	/*
	createTrackbar(trackbarname,window_name,value,count,onchange,userdate);
	����1���켣������
	����2����������
	����3�������ʼλ��
	����4����ʾ����ﵽ���λ�õ�ֵ
	����5��Ĭ��ֵΪ0��ָ��ص�����
	����6��Ĭ��ֵΪ0���û������ص�����������ֵ
	*/
	on_trackbar(160);   //�켣���Ļص�����
	

	 
	//cvWaitKey(0);
	//std::cout << "hi";
	cvDestroyWindow(pstrWindowsSrcTitle);
	cvDestroyWindow(pstrWindowsBinaryTitle);
	//cvReleaseImage(&pSrcImage);
	cvReleaseImage(&g_pGrayImage);
	//cvReleaseImage(&g_pBinaryImage);


}

CvSeq* contours = NULL;
void CharacterSegment(int num)//ͼƬ�ָ
{

	static int n = 1;
	//char image_name[25];//ͼƬ������
	CvMemStorage* storage = cvCreateMemStorage(0);  //�ڴ�� 0ΪĬ��ֵ Ϊ64K
	/*
	cvFindContours(CvArr* image, CvMemStorage* storage, CvSeq** first_contour,
int header_size=sizeof(CvContour), int mode=CV_RETR_LIST,
int method=CV_CHAIN_APPROX_SIMPLE, CvPoint offset=cvPoint(0,0) );
---
1��
2���õ��������Ĵ洢����
3�����������������һ�����������ָ��
4��header_size 
��� method=CV_CHAIN_CODE��������ͷ�Ĵ�С >=sizeof(CvChain)������ >=sizeof(CvContour)
5��method 
�ƽ����� (�����нڵ�, ������ʹ���ڲ��ƽ��� CV_RETR_RUNS).   CV_RETR_EXTERNAL - ֻ��ȡ���������� 
	*/
	int count = cvFindContours(g_pBinaryImage, storage, &contours, sizeof(CvContour), CV_RETR_EXTERNAL);   //��������  ���ؼ������������ĸ���
	printf("contours's number:%d \n", count);

	int idx = 0;
	char szName[56] = { 0 };
	int tempCount = 0;
	for (CvSeq* c = contours; c != NULL; c = c->h_next) {         //contours�����У�ָ���һ���������

		CvRect rc = cvBoundingRect(c, 0);     //���㲢���ر߽����   

		printf("coordinate: x:%d y:%d  width:%d height:%d windowID:%d \n", rc.x, rc.y, rc.width, rc.height, idx);

		cvDrawRect(pSrcImage, cvPoint(rc.x, rc.y), cvPoint(rc.x + rc.width, rc.y + rc.height), CV_RGB(255, 0, 0));
		IplImage* imgNo = cvCreateImage(cvSize(rc.width, rc.height), IPL_DEPTH_8U, 3);   //��ͨ��ͼ��
		cvSetImageROI(pSrcImage, rc);    //��ȡ����Ȥ������ROI��region of interesting�� ��rc����ROI����ü���Сͼ��
		     ///��ʱpSrcImage�ѱ�ɽ�ͼ���򣬵�ԭͼ����Ϣ��Ȼ����
		cvCopyImage(pSrcImage, imgNo);      
		cvResetImageROI(pSrcImage);

		sprintf(szName, "wnd_%d", idx++);

		cvReleaseImage(&imgNo);
	}
	//sprintf(image_name, "%s%d%s", "B:\092716102\ocr��Ƶ����\\����\\a", n, ".bmp");//�����ͼƬ��
	//cvSaveImage(image_name, pSrcImage);   //����ָ�ͼƬ 
	if (n == SHOW || num == 1)
	{
		cvShowImage("t_src", pSrcImage);//�ָ��ͼ����ʾ

	}
	n++;

} 

CvSeq* segment(IplImage *src, int &threshold, int sum)
{
	//std::cout << "depth:" << src->depth;
	puts("");

	pSrcImage = cvCreateImage(cvGetSize(src), src->depth, src->nChannels);    //��С��ͬ��ͨ�������  ֵ�� ͼ����ͬ��
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
	cvZero(trainTempImg);   //ȫ��ͼ��
	cvResize(test, trainTempImg);   //cvResize������IplImage������resize������Mat����  �ı�ͼ���С
	HOGDescriptor *hog = new HOGDescriptor(cvSize(28, 28), cvSize(14, 14), cvSize(7, 7), cvSize(7, 7), 9);
	vector<float>descriptors;
	hog->compute(trainTempImg, descriptors, Size(1, 1), Size(0, 0));    //������������  trainTempingΪ����ͼƬ  descriptors Ϊ������������vector
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

	ret = svm.predict(SVMtrainMat);      //ָ�����ֵ��ĸ���
	sprintf(result, "%s\r\n", ret);
	//�ָ�ͼ��ʾ


	//cvNamedWindow("dst", 1);
	//cvShowImage("dst", test);


	//cvReleaseImage(&test);
	cvReleaseImage(&trainTempImg);

	return ret;
}


class Digit			//�����������ַ�
{
public:
	Digit(CvRect rect) :rect(rect){};
	CvRect rect;
};

class Value		//��ѡ����ʶ�����ֵ
{
public:
	Value(CvRect rect) :rect(rect){};
	CvRect rect;
};


bool SortByOrdinates(const Value &value1, const Value &value2);		//������������
bool SortByHorizontal(const Digit &digit1, const Digit &digit2);	//������������



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

	vector<CvRect>t_rois;//����ѡ��
	static vector<CvRect>t_srois;
	//IplImage *src = cvLoadImage("CPAP1.png");
	if (num == 1)//�ж�
		t_rois = select(src), t_srois = t_rois;
	//ѡ��,���ж��Ƿ�Ϊ�Ե�һ֡�Ĵ���
	else
	{
		t_rois = t_srois;
	}
	ofstream out("out.txt", ios::app);
	vector<Value>values;

	// auto it = t_rois.begin();

	//while (it != t_rois.end())
	for (int i = 0; i < t_rois.size(); i++)
	{
		Value value(t_rois[i]);
		values.push_back(value);  //β����������
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
		const char *windowName = str.c_str();  //c_str ��ʱָ��
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
		cha_con = segment(img, thre, num);// 1����ֵ�� 2�� �ַ��ָ�
		vector<Digit>digits;
		for (CvSeq* c = cha_con; c != NULL; c = c->h_next)
		{
			CvRect rc = cvBoundingRect(c, 0);  //�߽�
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

			cvResetImageROI(img_digit);		//severances
			//cout << "test!!!!!!!!!!!!!!!!" << endl;
			//cvReleaseImage(&img_digit);
			n++;
			//it_cha++;

		}
		cvResetImageROI(img);
		cvReleaseImage(&img);
		out << "��" << num << "�ν��Ϊ:    " << result << "\n";
		cout << "the value is : " << result << endl;

		//it_value++;

	}


	out.close();
	return src;
	cvWaitKey(0);
}
  

int main(int argc, char** argv)
{
	CvCapture* capture = cvCaptureFromAVI("1.mp4");//��ȡ��Ƶ
	static int i = 1;//ͼƬ���
	IplImage* src;//���ͼƬ
	///char image_name[25];//ͼƬ������
	//��ȡ����ʾ
	char y;
	int num = 1;
	float time1, time2;
	time1 = getTickCount();
	int frames = (int)cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_COUNT);     //��ȡ��Ƶ��֡��
	cout << "��֡��Ϊ" << frames << endl;
	src = cvQueryFrame(capture); //֡��
	if (src == NULL)
	{
		cout << "src is empty!" << endl;
		exit(0);
	}
	do
	{


		cout << "�Ƿ�����������֡������y��������֡,����ֵ������:" << endl;
		cin >> y;//�ж��Ƿ񱣴�����֡
		if (num == 1)
		{
			printf("\n*************************��%d�δ���***************************\n", i);
			ocr(src, num);
			if (y != 'y')waitKey(0);
			num++;
			i++;

		}
		if (y != 'y')
		{
			time2 = getTickCount();
			cout << "����ʱ��Ϊ:" << (time2 - time1) / getTickFrequency() << "��" << endl;
			exit(0);
		}
		if (num != 1 && y == 'y')
		{
			while (1)
			{
				while (1)
				{
					src = cvQueryFrame(capture);
					if (waitKey(100) == -1)break;
				}
				printf("\n*************************��%d�δ���***************************\n", i);
				ocr(src, num);
				if (!src)
					break;
				Mat image(src, true);
				imshow("���ڴ�����Ƶ����Ƶ֡", image);
				i++;
				if (i > SHOW){
					time2 = getTickCount();
					cout << "����ʱ��Ϊ:" << (time2 - time1) / getTickFrequency() << "��" << endl;
					cvReleaseCapture(&capture);//�ͷ���Ƶ
					cvReleaseImage(&src);
					cvDestroyWindow("��Ƶ");
					waitKey(0);
					exit(0); //break;
				}

				num++;
				//sprintf(image_name, "%s%d%s", "B:\\092716102\\����\\����\\", ++i, ".bmp");//�����ͼƬ��
				//cvSaveImage(image_name, src);   //����һ֡ͼƬ
			}


		}



	} while (1);
	//getTemplate();
	return 0;

}
