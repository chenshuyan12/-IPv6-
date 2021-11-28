#ifndef		_FACE_FILEOP_H_H_
#define		_FACE_FILEOP_H_H_

#include "FaceConfigs.h"




void Face_Write_Database_File(const char *data);
char *Face_Read_Database_File_inLine(int line);
void Face_Write_Flag_File(char *_flag);

#endif


