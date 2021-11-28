#ifndef		_FACE_CONFIGS_H_H_
#define		_FACE_CONFIGS_H_H_

/* Head Files */
// OpenCV
#include "cv.h"
#include "cxcore.h"
#include "highgui.h"
// Standard
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h> 
#include <math.h> 
#include <float.h> 
#include <limits.h> 
#include <time.h> 
#include <ctype.h>
#include <omp.h>

// Define by Myself
#include "FaceRecognition.h"
#include "FaceFileOp.h"
#include "FaceVerificationJudgment.h"
#include "YuChu.h"

/* Config Macros */
#define FACE_DEBUG									1			// ������Ϣ�Ƿ����
#define FACE_HIST_SHOW							0			// ֱ���Ƿ�ͼ��ʾ
#define FACE_FACE_LIB_MAX_PICTURE	500		// ��ȡ���������
#define FACE_CAP_TIMES							8			// ÿ���˲ɼ�����������
#define FACE_MIN_RADIUS							40			// �ɼ������������뾶����ֵ
#define FACE_IDENTIFY_TIMES					0			// ʶ�����
#define FACE_CRITICAL_VALUE				80000000	// ���ڸüһ���ٽ�ֵ
/* Windows Macros */
#define FACE_WINDOWS_NAME_CAMERA_CAPTURE		"���ӽ���"
#define FACE_WINDOWS_NAME_CAMERA_SHOW           "״̬����"
#define FACE_WINDOWS_NAME_FACE_OUTLINE			"��������"
/* Pathname Macros */
#define FACE_PATHNAME_CASCADE_XML						"../faceXml/haarcascade_frontalface_alt2.xml"
#define FACE_PATHNAME_FACE_LIB									"../faceLib/"
#define FACE_PATHNAME_DATABASE_PATH						"../faceDatabase/faceDatabase.txt"
#define FACE_PATHNAME_FLAGFILE_PATH						"../faceFlag/faceFlag.txt"

/* Function Macros */
#define INFO(info)	printf("[��Ϣ] %s.\n",info)
#define ERR(err)		printf("[����] %s.\n",err)

using namespace cv;
using namespace std;

#endif

