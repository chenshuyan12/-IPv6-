#include "FaceConfigs.h"


int main( int argc, char** argv ) { 
	IplImage *camera_frame;								// 摄像头帧
	IplImage *face_outline_img;							// 人脸轮廓图
	IplImage *face_outline_canny_img;				// 人脸轮廓黑白图
	int cmpTimes = 0;											// 当前采集次数
	int trigger = 1;//判断是否保存第一张图片
	int PerformFace = 0;//判断是否执行人脸识别
	int FaceFind = 0;//进行人脸验证
	int QiangGui = 1;//强制归零位
	/* FaceRecognition init */
	Face_Recognition_Init();
	/* Create a camera */
	CvCapture *capture = cvCreateCameraCapture(0);
	cvNamedWindow(FACE_WINDOWS_NAME_CAMERA_SHOW);
	for (;;){

		/* got a camera frame */
		camera_frame = Camera_Get_A_Frame(capture);
		if (trigger){//这个仅仅一次
			GetBackGround(camera_frame);//获得背景图片
			trigger = 0;
		}
		PerformFace=ThreeFrameDifferenceMethod(camera_frame);
		cout << "FaceFind 1 == " << PerformFace << endl;
		if (PerformFace) {
			FaceFind = 1;
		}
		if (FaceFind == 1){
			face_outline_img = Face_Get_Face_Outline(camera_frame);
			/* got a cannyOutline from a raw outline image*/
			if (face_outline_img){
				face_outline_canny_img = Face_Get_Face_Canny(face_outline_img);
				if (FaceFind){//这里提供一个方法去判断是否执行人脸验证
					cout << "FaceFind 1 == " << FaceFind << endl;
					cmpTimes = Face_Start_Recognition(face_outline_canny_img, cmpTimes);// 进行canny算子变换 得到人脸轮廓黑白图，进行人脸认证
					FaceFind = 0;//进行归零操作
				}
			}
		}else{
			cvShowImage(FACE_WINDOWS_NAME_CAMERA_CAPTURE, camera_frame); // 显示标记后的原图
			/*显示状态的界面的显示*/
			Mat stateimage(camera_frame, true);
			putText(stateimage, "none", cvPoint(100, 100), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 23, 0), 4, 8);
			imshow(FACE_WINDOWS_NAME_CAMERA_SHOW,stateimage);
		}

		/* Key sacn event wait */
		switch (waitKeyEvent(2)){
		case 27:goto RELEASE_SOURSE;												// ESC	 退出								// H 显示帮助信息
			//case 32:Face_Write_Database(face_outline_canny_img,FACE_PATHNAME_FACE_LIB);break;	// SPACE 采集录入人脸
			//case 83:																						// S 开始识别
			//IDENTIFY_MODE:			
			//	cmpTimes = Face_Start_Recognition(face_outline_canny_img,cmpTimes);break;
		default:break;
		}
	}
	/*释放资源 */ 
RELEASE_SOURSE:
	cvReleaseCapture( &capture );   
	cvReleaseImage(&face_outline_img);
	cvReleaseImage(&face_outline_canny_img);
	cvDestroyWindow(FACE_WINDOWS_NAME_CAMERA_CAPTURE); 
	cvDestroyWindow(FACE_WINDOWS_NAME_FACE_OUTLINE);


	return 0; 
}



