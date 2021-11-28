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
#define FACE_DEBUG									1			// 调试信息是否输出
#define FACE_HIST_SHOW							0			// 直方是否图显示
#define FACE_FACE_LIB_MAX_PICTURE	500		// 存取最大人脸数
#define FACE_CAP_TIMES							8			// 每个人采集的人脸次数
#define FACE_MIN_RADIUS							40			// 采集的人脸轮廓半径最少值
#define FACE_IDENTIFY_TIMES					0			// 识别次数
#define FACE_CRITICAL_VALUE				80000000	// 存在该家伙的临界值
/* Windows Macros */
#define FACE_WINDOWS_NAME_CAMERA_CAPTURE		"监视界面"
#define FACE_WINDOWS_NAME_CAMERA_SHOW           "状态界面"
#define FACE_WINDOWS_NAME_FACE_OUTLINE			"人脸轮廓"
/* Pathname Macros */
#define FACE_PATHNAME_CASCADE_XML						"../faceXml/haarcascade_frontalface_alt2.xml"
#define FACE_PATHNAME_FACE_LIB									"../faceLib/"
#define FACE_PATHNAME_DATABASE_PATH						"../faceDatabase/faceDatabase.txt"
#define FACE_PATHNAME_FLAGFILE_PATH						"../faceFlag/faceFlag.txt"

/* Function Macros */
#define INFO(info)	printf("[信息] %s.\n",info)
#define ERR(err)		printf("[错误] %s.\n",err)

using namespace cv;
using namespace std;

#endif

