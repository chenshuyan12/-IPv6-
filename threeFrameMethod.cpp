#include "threeFrameMethod.h"

using namespace cv;
//���˶�����ֹ��һ��״̬��ת������ֵ����ǰ�ı�����
IplImage *GetBackGround(IplImage *before,IplImage *now){
	IplImage *background = NULL;
	
	background = cvCloneImage(before);
	return background;
}
int main(int argc, char ** argv){
	IplImage *camera = NULL;
	CvCapture *capture = cvCreateCameraCapture(0);
	while (1){
		camera = cvQueryFrame(capture);
		if (!camera){
			printf("Camera capture frame is NULL");
			return 0;
		}
	}
}
