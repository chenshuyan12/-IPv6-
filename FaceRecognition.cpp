#include "FaceRecognition.h"


static			CvMemStorage* storage = 0; 
static			CvHaarClassifierCascade* cascade = 0;
const		char* cascade_name = "haarcascade_frontalface_alt.xml";	// 解析人脸的XML文件
int				g_faceRadius = 0;																// 人脸轮廓半径
IplImage	*faces[20];																		// 标记的轮廓脸图片
char			*faceLibPathPrefix = "..\\faceLib\\";									// 人脸库路径前缀
char			*faceLibPathSuffix = ".jpg";												// 人脸库路径后缀
char			faceLibPath[256];																// 格式化后的人脸库路径
int				g_allFaces = 0;																	// 当前人脸库有多少脸模型
int				g_thresholdValue = 1;														// 阀值，修改进行调节
int				g_login_flag = 0;																// 是否验证账号密码

//寻找最多次数的数组下标
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
//显示提示信息函数
void Face_Show_Help_Info(){
	printf("系统启动\r\n");
	printf("按ESC退出");
}
//加载人脸识别的XML函数
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
		sprintf(faceLibPath,"%s%d%s",faceLibPathPrefix,i,faceLibPathSuffix);	// 格式化路径
		libFace = cvLoadImage(faceLibPath,1);													// 顺序加载库图像
		if(!libFace){																							// 库所有图片遍历完了
			break;
		}
		g_allFaces++;
		cvReleaseImage(&libFace);
	} 
#if FACE_DEBUG
	printf("[信息] 人脸库当前共有 %3d 张人脸\r\n",g_allFaces);
#endif	

	/* Create windows and show help infomation*/
	cvNamedWindow(FACE_WINDOWS_NAME_CAMERA_CAPTURE);		// 创建主窗口采集
	cvNamedWindow(FACE_WINDOWS_NAME_FACE_OUTLINE);				// 人脸轮廓黑白图窗口
	Face_Show_Help_Info();																				// 显示帮助信息
	Face_Write_Flag_File("0");																			// 写FLAG文件0 非识别模式
	return 0;
}
//进行预处理模块
IplImage *Face_Get_Face_Outline(IplImage *srcImg){
	int tmp = 0;
	IplImage *imgBkup = cvCloneImage(srcImg);

	double scale=1.0; 
	static CvScalar colors[] = { 
		{{0,0,255}},{{0,128,255}},{{0,255,255}},{{0,255,0}}, 
		{{255,128,0}},{{255,255,0}},{{255,0,0}},{{255,0,255}} 
	};	//Just some pretty colors to draw with

	// 图像准备
	IplImage* gray = cvCreateImage(cvSize(srcImg->width,srcImg->height),8,1); // 创建灰度图
	IplImage* small_img=cvCreateImage(cvSize(cvRound(srcImg->width/scale),cvRound(srcImg->height/scale)),8,1);// 创建小图 
	cvCvtColor(srcImg,gray, CV_BGR2GRAY);	// 颜色空间转换BGR - HSV
	cvResize(gray, small_img, CV_INTER_LINEAR);

#if FACE_HIST_SHOW
	cvNamedWindow("直方均衡化前");
	cvNamedWindow("直方均衡化后");
	cvNamedWindow("直方均衡化前 直方图");
	cvNamedWindow("直方均衡化后 直方图");

	cvShowImage("直方均衡化前",small_img);

	/* 创建直方图 */
	int dims = 1;
	int sizes[1] = {256};
	float ranges_x[2] = {0,255};
	float *ranges[] = {ranges_x };
	int uniform = 1;
	CvHistogram *hist = cvCreateHist(dims,sizes,CV_HIST_ARRAY,ranges,uniform);
	cvClearHist(hist);
	cvCalcHist(&small_img,hist,0,0);	//到此已经得到各个区间各个bin的值
	IplImage *hist_before = Face_Draw_Hist(hist,1,1);
	cvShowImage("直方均衡化前 直方图",hist_before);
#endif

	cvEqualizeHist(small_img,small_img); //直方图均衡

#if FACE_HIST_SHOW
	cvShowImage("直方均衡化后",small_img);
	cvClearHist(hist);
	cvCalcHist(&small_img,hist,0,0);
	IplImage *hist_after = Face_Draw_Hist(hist,1,1);
	cvShowImage("直方均衡化后 直方图",hist_after);
#endif

	// 识别任何可能是人脸的位置
	cvClearMemStorage(storage); 
	//double t = (double)cvGetTickCount(); 
	CvSeq* objects = cvHaarDetectObjects(small_img, 
		cascade, 
		storage, 
		1.1, 
		2, 
		0/*CV_HAAR_DO_CANNY_PRUNING*/, 
		cvSize(30,30));

	// 打印识别的时间
	//t = (double)cvGetTickCount() - t; 
	//printf( "detection time = %g ms\n", t/((double)cvGetTickFrequency()*1000.) );


	/* 显示定位到的人脸轮廓图 */
	for( int i = 0; i < (objects? objects->total : 0); i++ ) { 
		// 得到所有可能的目标
		CvRect* r = (CvRect*)cvGetSeqElem( objects, i ); 
		// 获取目标的半径
		g_faceRadius = cvRound((r->width + r->height)*0.25*scale);// 对double进行四舍五入返回int
		// 半径大于R_FACE_MIN_VAL的目标有效 并绘制目标轮廓图
		if(g_faceRadius > FACE_MIN_RADIUS){ 
			/* 矩形标定显示定位到的人脸轮廓 */
			cvRectangle(srcImg, cvPoint((int)r->x*scale,(int)r->y*scale), cvPoint((int)(r->x+r->width)*scale,(int)(r->y+r->height)*scale), colors[i%8]); 
			/* 圆形标定显示定位到的人脸轮廓 */
			CvPoint center; 
			int radius; 
			center.x = cvRound((r->x + r->width*0.5)*scale); 
			center.y = cvRound((r->y + r->height*0.5)*scale); 
			radius = cvRound((r->width + r->height)*0.25*scale); 
			cvCircle( srcImg, center, radius, colors[i%8], 3, 8, 0 ); 
			/* 显示定位到的人脸轮廓图 */
			int ymax = (int)(r->height*scale);	// 人脸轮廓图的长宽
			int xmax = (int)(r->width*scale);

			faces[i] = cvCreateImage(cvSize(xmax,ymax),8,3); // 创建目标轮廓图 3通道
			tmp = i;
			for(int y=0;y<ymax;y++){ // 写入目标轮廓图像数据
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

	cvShowImage(FACE_WINDOWS_NAME_CAMERA_CAPTURE, srcImg ); // 显示标记后的原图
	/*显示状态的界面*/
	Mat stateimage(srcImg, true);
	putText(stateimage, "exist", cvPoint(100, 100), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 23, 0), 4, 8);
	imshow(FACE_WINDOWS_NAME_CAMERA_SHOW,stateimage);

#if 0
	cvNamedWindow("人脸轮廓提取");
	cvShowImage("人脸轮廓提取",faces[tmp]);
#endif

	/* 释放局部资源 */
	cvReleaseImage(&gray); 
	cvReleaseImage(&small_img); 
	cvReleaseImage(&imgBkup);
	return faces[tmp];
}
//canny函数的图像数据的变换。
IplImage *Face_Get_Face_Canny(IplImage *srcImg){ 
	IplImage  *cannyImg;  

	// 载入图像的灰度图CV_LOAD_IMAGE_GRAYSCALE - 灰度图  
	cannyImg = cvCreateImage(cvGetSize(srcImg), IPL_DEPTH_8U, 1); //为cannny申请空间 

	cvCanny(srcImg, cannyImg, g_thresholdValue, g_thresholdValue * 3, 3); //进行cannny的计算

	// 转为3色
	IplImage *dstImg = cvCreateImage(cvGetSize(srcImg),8,3);
	cvCvtColor(cannyImg, dstImg, CV_GRAY2BGR);
	cvReleaseImage(&cannyImg);

	// 显示出来
	cvShowImage(FACE_WINDOWS_NAME_FACE_OUTLINE,dstImg);	

	return dstImg;
}
//操作鼠标的事件
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
	printf("[信息] 当前阀值 = %3d\r\n",g_thresholdValue);
#endif
}
//Face_Threshold_Value_Change控制canny函数中的两个的范围的变量.大于零的话g_thresholdValue增加三，小于零的话，g_thresholdValue减三。




//加载摄像头
IplImage *Camera_Get_A_Frame(CvCapture *capture){
	IplImage *frame = cvQueryFrame(capture);
	if(!frame){
		ERR("Camera capture frame is NULL");
		exit(-1);
	}
	return frame;
}


//平方差匹配法
int Face_TempLate_Campare(IplImage *tmp,int methed){  
	if(!tmp){
		INFO("模板图片为空");
		return -1;
	}

	int tmp2change = 0;								// 用来交换数据 如果模板比原图大 则交换角色
	IplImage *libFace;   
	double faceShapeVal[FACE_FACE_LIB_MAX_PICTURE] = {0.0};	// 存取数据库所有人脸的匹配结果数据

	IplImage *result;								// 创建匹配的结果数组
	int srcW, srcH, templatW, templatH, resultH, resultW;  
	double minValue, maxValue;						// 用来存取匹配结果的最大值和最小值
	CvPoint minLoc, maxLoc;							// 用来存取匹配结果的坐标位置

	/* 模板匹配人脸库中所有的图片 */
    #pragma omp parallel for//并行计算openMp
	for(int i=1;i<=FACE_FACE_LIB_MAX_PICTURE;i++){  

		sprintf(faceLibPath,"%s%d%s",faceLibPathPrefix,i,faceLibPathSuffix);	// 格式化路径
		libFace = cvLoadImage(faceLibPath,1);									// 顺序加载库图像
		if(!libFace){															// 库所有图片遍历完了
			//printf("[信息] 人脸库当前共有 %3d 张人脸\r\n",i-1);
			cvReleaseImage(&libFace);
			break;
		}
		srcW = libFace->width;  
		srcH = libFace->height;  
		templatW = tmp->width;  
		templatH = tmp->height;

		if(srcW < templatW || srcH < templatH)  {  // 模板不能比原图像小 交换角色
			//INFO("模板不能比原图像小");
			// 交换匹配
			tmp2change = srcW;
			srcW = templatW;
			templatW = tmp2change;
			tmp2change = srcH;
			srcH = templatH;
			templatH = tmp2change; 
		}  
		resultW = srcW - templatW + 1;  
		resultH = srcH - templatH + 1;  
		result = cvCreateImage(cvSize(resultW, resultH), 32, 1);	// 创建匹配的结果数组
		cvMatchTemplate(libFace, tmp, result, methed);				// 开始模板匹配   
		cvMinMaxLoc(result, &minValue, &maxValue, &minLoc, &maxLoc);// 获取匹配的最值和坐标点
		//cvRectangle(srcResult, minLoc, cvPoint(minLoc.x + templatW, minLoc.y+ templatH), cvScalar(0,0,255));// 矩形标记

		if(CV_TM_SQDIFF == methed){				// 平方差匹配法：该方法采用平方差来进行匹配；最好的匹配值为0；匹配越差，匹配值越大。
			faceShapeVal[i-1] = minValue;
			//printf("minVal = %f\n",minValue);
		}

		if(CV_TM_CCORR == methed){				// 相关匹配法：该方法采用乘法操作；数值越大表明匹配程度越好。
			faceShapeVal[i-1] = maxValue;
			printf("maxVal = %f\n",maxValue);
		}

		cvReleaseImage(&libFace);				// 释放当前图片内存

	}  

	int who = 0;	// 存取最相似图片的下标
	double tmpVal = faceShapeVal[0];
	int n = 1;
	if(CV_TM_SQDIFF == methed){		// 找最大值
		for(n=1;n<g_allFaces;n++){
			if(faceShapeVal[n] < 0)
				continue;
			if(faceShapeVal[n] < tmpVal){
				tmpVal = faceShapeVal[n];
				who = n;
			}
		}
	}

	if(CV_TM_CCORR == methed){		// 找最小值
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

	if(tmpVal < FACE_CRITICAL_VALUE)	// 该人不存在
		return -1;

	return (int)(who/FACE_CAP_TIMES); // 返回人物数据库行的下标
}  
int Face_Start_Recognition(IplImage *sampImg,int cmpTimes){ //进行人脸库的匹配，找到目标
	int faceLibObjIndex[2];			// 存取匹配的人脸数据库行下标
	int faceObjArr[FACE_FACE_LIB_MAX_PICTURE] = {0};// 每个人的匹配次数数组
	int faceObjIndex = 0;			// 匹配的人脸库行下标

	printf("\n===================================\n");
	printf("\n[消息]	识别中，请稍等···\n");	
	/* 开始模板比较 返回人脸数据库的下角标 */ 
	faceLibObjIndex[0] = Face_TempLate_Campare(sampImg,CV_TM_CCORR);		// 平方差匹配法	
	// faceLibObjIndex[0] = Face_TempLate_Campare(blackWhiteImg,CV_TM_SQDIFF);	// 相关匹配法

	if(faceLibObjIndex[0] != -1){									// 如果符合临界值
		faceObjArr[faceLibObjIndex[0]] += 1;						// 锁定的人次数 +1
	}
	if(cmpTimes == FACE_IDENTIFY_TIMES){								// 识别规定的次数识别完成

		faceObjIndex = toolFindMaxIndex(faceObjArr);			// 寻找锁定的人脸最多次数的数组下标
		if(faceObjArr[faceObjIndex] == 0){						// 就是说所有的人匹配率都是0
			printf("[识别结果] ---> 不存在该家伙\n");			// 该人不存在
			Face_Write_Flag_File("2");// 写FLAG文件2
		}else{													// 存在该人
			char *objNmae = Face_Read_Database_File_inLine(faceObjIndex + 1);	// 读取文件中的对应行 就是该人
			//printf("[识别结果] ---> %s \n",objNmae);			// 打印显示该人
			printf("[欢迎光临] ---> %s \n",objNmae);		
			Face_Write_Flag_File("1");// 写FLAG文件1
		}
		printf("===================================\n");

		for(int i=0;i<FACE_FACE_LIB_MAX_PICTURE;i++){
			// printf("faceObjArr[%d] = %d\n",i,faceObjArr[i]);
			faceObjArr[i] = 0;									// 清空锁定人脸暂时存取的数组
		}
		cmpTimes = 0;											// 复位比较次数
	}else{														// 还没识别完成
		cmpTimes++;												// 比较次数 +1
	}

	return cmpTimes;
}


// 画直方图
IplImage *Face_Draw_Hist(CvHistogram *hist,float scaleX,float scaleY){
	float histMax;
	int histImgWideth = 256;
	int histImgHeight = 64;

	//获取直方图数据bin的最大值 以防止溢出图片大小
	cvGetMinMaxHistValue(hist,NULL,&histMax,0,0);

	IplImage *imghist = cvCreateImage(cvSize(histImgWideth * scaleX,histImgHeight * scaleY),8,1);
	cvZero(imghist);// 图片清背景色为黑色

	for(int i=0;i<255;i++){
		float curHistValue = cvQueryHistValue_1D(hist,i);
		float nextHistValue = cvQueryHistValue_1D(hist,i+1);

		//得到相邻的点
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

		// 绘制凸多边形
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

	//分别复制每个通道到多个单通道图像
	cvSplit(srcImg,r,g,b,NULL);

	//Temporary storage
	IplImage* s= cvCreateImage(cvGetSize(srcImg),IPL_DEPTH_8U,1);

	//计算两两数组加权值的和

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







