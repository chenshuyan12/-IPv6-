#include "FaceConfigs.h"


int main( int argc, char** argv ) { 
	IplImage *camera_frame;								// ����ͷ֡
	IplImage *face_outline_img;							// ��������ͼ
	IplImage *face_outline_canny_img;				// ���������ڰ�ͼ
	int cmpTimes = 0;											// ��ǰ�ɼ�����
	int trigger = 1;//�ж��Ƿ񱣴��һ��ͼƬ
	int PerformFace = 0;//�ж��Ƿ�ִ������ʶ��
	int FaceFind = 0;//����������֤
	int QiangGui = 1;//ǿ�ƹ���λ
	/* FaceRecognition init */
	Face_Recognition_Init();
	/* Create a camera */
	CvCapture *capture = cvCreateCameraCapture(0);
	cvNamedWindow(FACE_WINDOWS_NAME_CAMERA_SHOW);
	for (;;){

		/* got a camera frame */
		camera_frame = Camera_Get_A_Frame(capture);
		if (trigger){//�������һ��
			GetBackGround(camera_frame);//��ñ���ͼƬ
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
				if (FaceFind){//�����ṩһ������ȥ�ж��Ƿ�ִ��������֤
					cout << "FaceFind 1 == " << FaceFind << endl;
					cmpTimes = Face_Start_Recognition(face_outline_canny_img, cmpTimes);// ����canny���ӱ任 �õ����������ڰ�ͼ������������֤
					FaceFind = 0;//���й������
				}
			}
		}else{
			cvShowImage(FACE_WINDOWS_NAME_CAMERA_CAPTURE, camera_frame); // ��ʾ��Ǻ��ԭͼ
			/*��ʾ״̬�Ľ������ʾ*/
			Mat stateimage(camera_frame, true);
			putText(stateimage, "none", cvPoint(100, 100), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 23, 0), 4, 8);
			imshow(FACE_WINDOWS_NAME_CAMERA_SHOW,stateimage);
		}

		/* Key sacn event wait */
		switch (waitKeyEvent(2)){
		case 27:goto RELEASE_SOURSE;												// ESC	 �˳�								// H ��ʾ������Ϣ
			//case 32:Face_Write_Database(face_outline_canny_img,FACE_PATHNAME_FACE_LIB);break;	// SPACE �ɼ�¼������
			//case 83:																						// S ��ʼʶ��
			//IDENTIFY_MODE:			
			//	cmpTimes = Face_Start_Recognition(face_outline_canny_img,cmpTimes);break;
		default:break;
		}
	}
	/*�ͷ���Դ */ 
RELEASE_SOURSE:
	cvReleaseCapture( &capture );   
	cvReleaseImage(&face_outline_img);
	cvReleaseImage(&face_outline_canny_img);
	cvDestroyWindow(FACE_WINDOWS_NAME_CAMERA_CAPTURE); 
	cvDestroyWindow(FACE_WINDOWS_NAME_FACE_OUTLINE);


	return 0; 
}



