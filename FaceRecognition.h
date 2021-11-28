#ifndef		_FACE_RECOGNITION_H_H_
#define		_FACE_RECOGNITION_H_H_


#include "FaceConfigs.h"


int toolFindMaxIndex(int *arr);
IplImage *Camera_Get_A_Frame(CvCapture *capture);
int waitKeyEvent(int delay);
void Face_Show_Help_Info();
int Face_Recognition_Init(void);
void Face_Write_Database(IplImage *frame,const char *mainPath);
void Face_Threshold_Value_Change(int _flag);
IplImage *Face_Get_Face_Canny(IplImage *srcImg);
IplImage *Face_Get_Face_Outline(IplImage *srcImg);
int Face_TempLate_Campare(IplImage *tmp,int methed);
int Face_Start_Recognition(IplImage *sampImg,int cmpTimes);
IplImage *Face_Draw_Hist(CvHistogram *hist,float scaleX,float scaleY);
//IplImage *Face_Get_Black_And_White_Img(IplImage * srcImg);
#endif

