#include "FaceFileOp.h"



void Face_Write_Database_File(const char *data){
	FILE *pfile=fopen(FACE_PATHNAME_DATABASE_PATH,"a+");
	fwrite(data,1,strlen(data),pfile);			// 将数据写入文件
	fwrite("\n",1,strlen("\n"),pfile);			// 写入新行
	fflush(pfile);										// 刷新缓冲区 将缓冲区数据写入文件 
	fclose(pfile);										// 关闭文件
}

char *Face_Read_Database_File_inLine(int line){
	int i;
	char data[256];
	memset(data,0,256);
	char *pData = data;
	FILE *pfile=fopen(FACE_PATHNAME_DATABASE_PATH,"r");
	for(i=0;i<line;i++){
		fgets(data,256,pfile);
	}
	fclose(pfile);
	return pData;
}

void Face_Write_Flag_File(char *_flag){
	FILE *pfile=fopen(FACE_PATHNAME_FLAGFILE_PATH,"w+");
	fwrite(_flag,1,strlen(_flag),pfile);	// 将数据写入文件
	fwrite("\n",1,strlen("\n"),pfile);		// 写入新行
	fflush(pfile);							// 刷新缓冲区 将缓冲区数据写入文件 
	fclose(pfile);							// 关闭文件
}




