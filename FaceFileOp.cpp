#include "FaceFileOp.h"



void Face_Write_Database_File(const char *data){
	FILE *pfile=fopen(FACE_PATHNAME_DATABASE_PATH,"a+");
	fwrite(data,1,strlen(data),pfile);			// ������д���ļ�
	fwrite("\n",1,strlen("\n"),pfile);			// д������
	fflush(pfile);										// ˢ�»����� ������������д���ļ� 
	fclose(pfile);										// �ر��ļ�
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
	fwrite(_flag,1,strlen(_flag),pfile);	// ������д���ļ�
	fwrite("\n",1,strlen("\n"),pfile);		// д������
	fflush(pfile);							// ˢ�»����� ������������д���ļ� 
	fclose(pfile);							// �ر��ļ�
}




