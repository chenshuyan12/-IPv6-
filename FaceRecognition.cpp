#include "FaceRecognition.h"


static			CvMemStorage* storage = 0; 
static			CvHaarClassifierCascade* cascade = 0;
const		char* cascade_name = "haarcascade_frontalface_alt.xml";	// ����������XML�ļ�
int				g_faceRadius = 0;																// ���������뾶
IplImage	*faces[20];																		// ��ǵ�������ͼƬ
char			*faceLibPathPrefix = "..\\faceLib\\";									// ������·��ǰ׺
char			*faceLibPathSuffix = ".jpg";												// ������·����׺
char			faceLibPath[256];																// ��ʽ�����������·��
int				g_allFaces = 0;																	// ��ǰ�������ж�����ģ��
int				g_thresholdValue = 1;														// ��ֵ���޸Ľ��е���
int				g_login_flag = 0;																// �Ƿ���֤�˺�����

//Ѱ���������������±�
int toolFindMaxIndex(int *arr){
	int max = arr[0];
	int max_index = 0;
	for(int i=1;i<FACE_FACE_LIB_MAX_PICTURE;i++){
		if(arr[i] > max){
			max_index = i;
			max = arr[i];
		}
	}
	return max_index;
}
//��ʾ��ʾ��Ϣ����
void Face_Show_Help_Info(){
	printf("ϵͳ����\r\n");
	printf("��ESC�˳�");
}
//��������ʶ���XML����
int Face_Recognition_Init(void){

	/* Load face XML file */
	cascade_name = FACE_PATHNAME_CASCADE_XML;
	cascade = (CvHaarClassifierCascade*)cvLoad( cascade_name, 0, 0, 0 ); 
	if( !cascade ) { 
		ERR("Could not load classifier cascade");
		return -1; 
	} 
	storage = cvCreateMemStorage(0); 

	/* Got how much face picture in faceLib */
	IplImage *libFace;
	for(int i=1;i<=FACE_FACE_LIB_MAX_PICTURE;i++){  
		sprintf(faceLibPath,"%s%d%s",faceLibPathPrefix,i,faceLibPathSuffix);	// ��ʽ��·��
		libFace = cvLoadImage(faceLibPath,1);													// ˳����ؿ�ͼ��
		if(!libFace){																							// ������ͼƬ��������
			break;
		}
		g_allFaces++;
		cvReleaseImage(&libFace);
	} 
#if FACE_DEBUG
	printf("[��Ϣ] �����⵱ǰ���� %3d ������\r\n",g_allFaces);
#endif	

	/* Create windows and show help infomation*/
	cvNamedWindow(FACE_WINDOWS_NAME_CAMERA_CAPTURE);		// ���������ڲɼ�
	cvNamedWindow(FACE_WINDOWS_NAME_FACE_OUTLINE);				// ���������ڰ�ͼ����
	Face_Show_Help_Info();																				// ��ʾ������Ϣ
	Face_Write_Flag_File("0");																			// дFLAG�ļ�0 ��ʶ��ģʽ
	return 0;
}
//����Ԥ����ģ��
IplImage *Face_Get_Face_Outline(IplImage *srcImg){
	int tmp = 0;
	IplImage *imgBkup = cvCloneImage(srcImg);

	double scale=1.0; 
	static CvScalar colors[] = { 
		{{0,0,255}},{{0,128,255}},{{0,255,255}},{{0,255,0}}, 
		{{255,128,0}},{{255,255,0}},{{255,0,0}},{{255,0,255}} 
	};	//Just some pretty colors to draw with

	// ͼ��׼��
	IplImage* gray = cvCreateImage(cvSize(srcImg->width,srcImg->height),8,1); // �����Ҷ�ͼ
	IplImage* small_img=cvCreateImage(cvSize(cvRound(srcImg->width/scale),cvRound(srcImg->height/scale)),8,1);// ����Сͼ 
	cvCvtColor(srcImg,gray, CV_BGR2GRAY);	// ��ɫ�ռ�ת��BGR - HSV
	cvResize(gray, small_img, CV_INTER_LINEAR);

#if FACE_HIST_SHOW
	cvNamedWindow("ֱ�����⻯ǰ");
	cvNamedWindow("ֱ�����⻯��");
	cvNamedWindow("ֱ�����⻯ǰ ֱ��ͼ");
	cvNamedWindow("ֱ�����⻯�� ֱ��ͼ");

	cvShowImage("ֱ�����⻯ǰ",small_img);

	/* ����ֱ��ͼ */
	int dims = 1;
	int sizes[1] = {256};
	float ranges_x[2] = {0,255};
	float *ranges[] = {ranges_x };
	int uniform = 1;
	CvHistogram *hist = cvCreateHist(dims,sizes,CV_HIST_ARRAY,ranges,uniform);
	cvClearHist(hist);
	cvCalcHist(&small_img,hist,0,0);	//�����Ѿ��õ������������bin��ֵ
	IplImage *hist_before = Face_Draw_Hist(hist,1,1);
	cvShowImage("ֱ�����⻯ǰ ֱ��ͼ",hist_before);
#endif

	cvEqualizeHist(small_img,small_img); //ֱ��ͼ����

#if FACE_HIST_SHOW
	cvShowImage("ֱ�����⻯��",small_img);
	cvClearHist(hist);
	cvCalcHist(&small_img,hist,0,0);
	IplImage *hist_after = Face_Draw_Hist(hist,1,1);
	cvShowImage("ֱ�����⻯�� ֱ��ͼ",hist_after);
#endif

	// ʶ���κο�����������λ��
	cvClearMemStorage(storage); 
	//double t = (double)cvGetTickCount(); 
	CvSeq* objects = cvHaarDetectObjects(small_img, 
		cascade, 
		storage, 
		1.1, 
		2, 
		0/*CV_HAAR_DO_CANNY_PRUNING*/, 
		cvSize(30,30));

	// ��ӡʶ���ʱ��
	//t = (double)cvGetTickCount() - t; 
	//printf( "detection time = %g ms\n", t/((double)cvGetTickFrequency()*1000.) );


	/* ��ʾ��λ������������ͼ */
	for( int i = 0; i < (objects? objects->total : 0); i++ ) { 
		// �õ����п��ܵ�Ŀ��
		CvRect* r = (CvRect*)cvGetSeqElem( objects, i ); 
		// ��ȡĿ��İ뾶
		g_faceRadius = cvRound((r->width + r->height)*0.25*scale);// ��double�����������뷵��int
		// �뾶����R_FACE_MIN_VAL��Ŀ����Ч ������Ŀ������ͼ
		if(g_faceRadius > FACE_MIN_RADIUS){ 
			/* ���α궨��ʾ��λ������������ */
			cvRectangle(srcImg, cvPoint((int)r->x*scale,(int)r->y*scale), cvPoint((int)(r->x+r->width)*scale,(int)(r->y+r->height)*scale), colors[i%8]); 
			/* Բ�α궨��ʾ��λ������������ */
			CvPoint center; 
			int radius; 
			center.x = cvRound((r->x + r->width*0.5)*scale); 
			center.y = cvRound((r->y + r->height*0.5)*scale); 
			radius = cvRound((r->width + r->height)*0.25*scale); 
			cvCircle( srcImg, center, radius, colors[i%8], 3, 8, 0 ); 
			/* ��ʾ��λ������������ͼ */
			int ymax = (int)(r->height*scale);	// ��������ͼ�ĳ���
			int xmax = (int)(r->width*scale);

			faces[i] = cvCreateImage(cvSize(xmax,ymax),8,3); // ����Ŀ������ͼ 3ͨ��
			tmp = i;
			for(int y=0;y<ymax;y++){ // д��Ŀ������ͼ������
				int y_py = (int)(r->y*imgBkup->widthStep*scale);
				char *psrc = imgBkup->imageData + y_py + y*imgBkup->widthStep;
				char *pdst = faces[i]->imageData + y*faces[i]->widthStep;
				for(int x=0;x<xmax;x++){
					int x_py = (int)(r->x*3*scale);
					*(pdst + x*3 + 0) = *(psrc + x*3 + 0 + x_py);
					*(pdst + x*3 + 1) = *(psrc + x*3 + 1 + x_py);
					*(pdst + x*3 + 2) = *(psrc + x*3 + 2 + x_py);
				}
			}
		}
	}

	cvShowImage(FACE_WINDOWS_NAME_CAMERA_CAPTURE, srcImg ); // ��ʾ��Ǻ��ԭͼ
	/*��ʾ״̬�Ľ���*/
	Mat stateimage(srcImg, true);
	putText(stateimage, "exist", cvPoint(100, 100), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 23, 0), 4, 8);
	imshow(FACE_WINDOWS_NAME_CAMERA_SHOW,stateimage);

#if 0
	cvNamedWindow("����������ȡ");
	cvShowImage("����������ȡ",faces[tmp]);
#endif

	/* �ͷžֲ���Դ */
	cvReleaseImage(&gray); 
	cvReleaseImage(&small_img); 
	cvReleaseImage(&imgBkup);
	return faces[tmp];
}
//canny������ͼ�����ݵı任��
IplImage *Face_Get_Face_Canny(IplImage *srcImg){ 
	IplImage  *cannyImg;  

	// ����ͼ��ĻҶ�ͼCV_LOAD_IMAGE_GRAYSCALE - �Ҷ�ͼ  
	cannyImg = cvCreateImage(cvGetSize(srcImg), IPL_DEPTH_8U, 1); //Ϊcannny����ռ� 

	cvCanny(srcImg, cannyImg, g_thresholdValue, g_thresholdValue * 3, 3); //����cannny�ļ���

	// תΪ3ɫ
	IplImage *dstImg = cvCreateImage(cvGetSize(srcImg),8,3);
	cvCvtColor(cannyImg, dstImg, CV_GRAY2BGR);
	cvReleaseImage(&cannyImg);

	// ��ʾ����
	cvShowImage(FACE_WINDOWS_NAME_FACE_OUTLINE,dstImg);	

	return dstImg;
}
//���������¼�
int waitKeyEvent(int delay){
	char keyVal = cvWaitKey(delay);
	return keyVal;
}





void Face_Threshold_Value_Change(int _flag){
#define STEP_VAL	3
	if(_flag >= 0){
		g_thresholdValue += STEP_VAL;
		if(g_thresholdValue > 255){
			g_thresholdValue = 0;
		}
	}
	else{
		g_thresholdValue -= STEP_VAL;
		if(g_thresholdValue < 0){
			g_thresholdValue = 255;
		}
	}
#if FACE_DEBUG
	printf("[��Ϣ] ��ǰ��ֵ = %3d\r\n",g_thresholdValue);
#endif
}
//Face_Threshold_Value_Change����canny�����е������ķ�Χ�ı���.������Ļ�g_thresholdValue��������С����Ļ���g_thresholdValue������




//��������ͷ
IplImage *Camera_Get_A_Frame(CvCapture *capture){
	IplImage *frame = cvQueryFrame(capture);
	if(!frame){
		ERR("Camera capture frame is NULL");
		exit(-1);
	}
	return frame;
}


//ƽ����ƥ�䷨
int Face_TempLate_Campare(IplImage *tmp,int methed){  
	if(!tmp){
		INFO("ģ��ͼƬΪ��");
		return -1;
	}

	int tmp2change = 0;								// ������������ ���ģ���ԭͼ�� �򽻻���ɫ
	IplImage *libFace;   
	double faceShapeVal[FACE_FACE_LIB_MAX_PICTURE] = {0.0};	// ��ȡ���ݿ�����������ƥ��������

	IplImage *result;								// ����ƥ��Ľ������
	int srcW, srcH, templatW, templatH, resultH, resultW;  
	double minValue, maxValue;						// ������ȡƥ���������ֵ����Сֵ
	CvPoint minLoc, maxLoc;							// ������ȡƥ����������λ��

	/* ģ��ƥ�������������е�ͼƬ */
    #pragma omp parallel for//���м���openMp
	for(int i=1;i<=FACE_FACE_LIB_MAX_PICTURE;i++){  

		sprintf(faceLibPath,"%s%d%s",faceLibPathPrefix,i,faceLibPathSuffix);	// ��ʽ��·��
		libFace = cvLoadImage(faceLibPath,1);									// ˳����ؿ�ͼ��
		if(!libFace){															// ������ͼƬ��������
			//printf("[��Ϣ] �����⵱ǰ���� %3d ������\r\n",i-1);
			cvReleaseImage(&libFace);
			break;
		}
		srcW = libFace->width;  
		srcH = libFace->height;  
		templatW = tmp->width;  
		templatH = tmp->height;

		if(srcW < templatW || srcH < templatH)  {  // ģ�岻�ܱ�ԭͼ��С ������ɫ
			//INFO("ģ�岻�ܱ�ԭͼ��С");
			// ����ƥ��
			tmp2change = srcW;
			srcW = templatW;
			templatW = tmp2change;
			tmp2change = srcH;
			srcH = templatH;
			templatH = tmp2change; 
		}  
		resultW = srcW - templatW + 1;  
		resultH = srcH - templatH + 1;  
		result = cvCreateImage(cvSize(resultW, resultH), 32, 1);	// ����ƥ��Ľ������
		cvMatchTemplate(libFace, tmp, result, methed);				// ��ʼģ��ƥ��   
		cvMinMaxLoc(result, &minValue, &maxValue, &minLoc, &maxLoc);// ��ȡƥ�����ֵ�������
		//cvRectangle(srcResult, minLoc, cvPoint(minLoc.x + templatW, minLoc.y+ templatH), cvScalar(0,0,255));// ���α��

		if(CV_TM_SQDIFF == methed){				// ƽ����ƥ�䷨���÷�������ƽ����������ƥ�䣻��õ�ƥ��ֵΪ0��ƥ��Խ�ƥ��ֵԽ��
			faceShapeVal[i-1] = minValue;
			//printf("minVal = %f\n",minValue);
		}

		if(CV_TM_CCORR == methed){				// ���ƥ�䷨���÷������ó˷���������ֵԽ�����ƥ��̶�Խ�á�
			faceShapeVal[i-1] = maxValue;
			printf("maxVal = %f\n",maxValue);
		}

		cvReleaseImage(&libFace);				// �ͷŵ�ǰͼƬ�ڴ�

	}  

	int who = 0;	// ��ȡ������ͼƬ���±�
	double tmpVal = faceShapeVal[0];
	int n = 1;
	if(CV_TM_SQDIFF == methed){		// �����ֵ
		for(n=1;n<g_allFaces;n++){
			if(faceShapeVal[n] < 0)
				continue;
			if(faceShapeVal[n] < tmpVal){
				tmpVal = faceShapeVal[n];
				who = n;
			}
		}
	}

	if(CV_TM_CCORR == methed){		// ����Сֵ
		for(n=1;n<g_allFaces;n++){
			if(faceShapeVal[n] < 0)
				continue;
			if(faceShapeVal[n] > tmpVal){
				tmpVal = faceShapeVal[n];
				who = n;
			}
		}
	}

	cvReleaseImage(&result);  

	if(tmpVal < FACE_CRITICAL_VALUE)	// ���˲�����
		return -1;

	return (int)(who/FACE_CAP_TIMES); // �����������ݿ��е��±�
}  
int Face_Start_Recognition(IplImage *sampImg,int cmpTimes){ //�����������ƥ�䣬�ҵ�Ŀ��
	int faceLibObjIndex[2];			// ��ȡƥ����������ݿ����±�
	int faceObjArr[FACE_FACE_LIB_MAX_PICTURE] = {0};// ÿ���˵�ƥ���������
	int faceObjIndex = 0;			// ƥ������������±�

	printf("\n===================================\n");
	printf("\n[��Ϣ]	ʶ���У����Եȡ�����\n");	
	/* ��ʼģ��Ƚ� �����������ݿ���½Ǳ� */ 
	faceLibObjIndex[0] = Face_TempLate_Campare(sampImg,CV_TM_CCORR);		// ƽ����ƥ�䷨	
	// faceLibObjIndex[0] = Face_TempLate_Campare(blackWhiteImg,CV_TM_SQDIFF);	// ���ƥ�䷨

	if(faceLibObjIndex[0] != -1){									// ��������ٽ�ֵ
		faceObjArr[faceLibObjIndex[0]] += 1;						// �������˴��� +1
	}
	if(cmpTimes == FACE_IDENTIFY_TIMES){								// ʶ��涨�Ĵ���ʶ�����

		faceObjIndex = toolFindMaxIndex(faceObjArr);			// Ѱ�������������������������±�
		if(faceObjArr[faceObjIndex] == 0){						// ����˵���е���ƥ���ʶ���0
			printf("[ʶ����] ---> �����ڸüһ�\n");			// ���˲�����
			Face_Write_Flag_File("2");// дFLAG�ļ�2
		}else{													// ���ڸ���
			char *objNmae = Face_Read_Database_File_inLine(faceObjIndex + 1);	// ��ȡ�ļ��еĶ�Ӧ�� ���Ǹ���
			//printf("[ʶ����] ---> %s \n",objNmae);			// ��ӡ��ʾ����
			printf("[��ӭ����] ---> %s \n",objNmae);		
			Face_Write_Flag_File("1");// дFLAG�ļ�1
		}
		printf("===================================\n");

		for(int i=0;i<FACE_FACE_LIB_MAX_PICTURE;i++){
			// printf("faceObjArr[%d] = %d\n",i,faceObjArr[i]);
			faceObjArr[i] = 0;									// �������������ʱ��ȡ������
		}
		cmpTimes = 0;											// ��λ�Ƚϴ���
	}else{														// ��ûʶ�����
		cmpTimes++;												// �Ƚϴ��� +1
	}

	return cmpTimes;
}


// ��ֱ��ͼ
IplImage *Face_Draw_Hist(CvHistogram *hist,float scaleX,float scaleY){
	float histMax;
	int histImgWideth = 256;
	int histImgHeight = 64;

	//��ȡֱ��ͼ����bin�����ֵ �Է�ֹ���ͼƬ��С
	cvGetMinMaxHistValue(hist,NULL,&histMax,0,0);

	IplImage *imghist = cvCreateImage(cvSize(histImgWideth * scaleX,histImgHeight * scaleY),8,1);
	cvZero(imghist);// ͼƬ�屳��ɫΪ��ɫ

	for(int i=0;i<255;i++){
		float curHistValue = cvQueryHistValue_1D(hist,i);
		float nextHistValue = cvQueryHistValue_1D(hist,i+1);

		//�õ����ڵĵ�
		CvPoint pt1 = cvPoint(		i * scaleX,histImgHeight * scaleY);
		CvPoint pt2 = cvPoint((i + 1) * scaleX,histImgHeight * scaleY);
		CvPoint pt3 = cvPoint((i + 1) * scaleX,(histImgHeight * scaleY) - (nextHistValue / histMax) * (histImgHeight * scaleY));
		CvPoint pt4 = cvPoint(		i * scaleX,(histImgHeight * scaleY) - (curHistValue / histMax) * (histImgHeight * scaleY));

		int numPts = 5;
		CvPoint pts[5];
		pts[0] = pt1;
		pts[1] = pt2;
		pts[2] = pt3;
		pts[3] = pt4;
		pts[4] = pt1;

		// ����͹�����
		cvFillConvexPoly(imghist,pts,numPts,cvScalar(255));

	}

	return imghist;
}


#if 0
IplImage *Face_Get_Black_And_White_Img(IplImage * srcImg){

	IplImage * dstImg=cvCreateImage(cvGetSize(srcImg),srcImg->depth,1);

	IplImage* r=cvCreateImage(cvGetSize(srcImg),IPL_DEPTH_8U,1);
	IplImage* g=cvCreateImage(cvGetSize(srcImg),IPL_DEPTH_8U,1);
	IplImage* b=cvCreateImage(cvGetSize(srcImg),IPL_DEPTH_8U,1);

	//�ֱ���ÿ��ͨ���������ͨ��ͼ��
	cvSplit(srcImg,r,g,b,NULL);

	//Temporary storage
	IplImage* s= cvCreateImage(cvGetSize(srcImg),IPL_DEPTH_8U,1);

	//�������������Ȩֵ�ĺ�

	cvAddWeighted(r,1./3. ,g, 1./3. ,0.0, s);
	cvAddWeighted(s, 2./3., b, 1./3., 0.0,s);

	cvThreshold(s,dstImg,g_thresholdValue,255,CV_THRESH_BINARY);// 100 255
	//cvShowImage("b",dstImg);


	cvReleaseImage(&r);
	cvReleaseImage(&g);
	cvReleaseImage(&b);
	cvReleaseImage(&s);

	IplImage *dstImg3c = cvCreateImage(cvGetSize(srcImg),8,3);
	cvCvtColor(dstImg, dstImg3c, CV_GRAY2BGR);
	return dstImg3c;
}
#endif







