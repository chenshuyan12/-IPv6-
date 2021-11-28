#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h> 
#include <iostream>
#include <fstream>

using namespace std;

int main(int argc, char *argv[])
{
	int listenfd = 0;
	struct sockaddr_in serv_addr; 
	
	char parameter[256];
	ifstream in("/home/pi/out.txt");  
	if (!in.is_open())  
	{ cout << "Error opening file"; exit(1); }  	
	char value[20][30] = { "Ppeak=", "&Pmean=", "&PEEP=", "breath_rate=", "&Oxygen_density=", "&End_expiratory_flow=", "&TI=", "&MVI=", "&MVe=", "&VTI=", "&VTe=", "&MVesp=" };
	int n = 0;
	while (!in.eof())  
	{  		
		char buffer[20];
		in.getline(buffer, 20); 		
		strcat(value[n], buffer);
		//cout << buffer << endl;  
		strcat(parameter, value[n]);
		cout << value[n] << endl;
		n++;
	}  
	cout << parameter << endl;
	
	
	
	char sendBuff[1025] =  { "GET http://192.168.1.111/Health_detection/insert.php?"};
	strcat(sendBuff, parameter);
	strcat(sendBuff, " \r\nHTTP/1.1\r\n Host: 192.168.1.111 \r\n");
	cout <<"sendBuff:"<< sendBuff << endl;
	
	//&Pmean=2&PEEP=3&breath_rate=4&Oxygen_density=5&End_expiratory_flow=6&TITtot=7&MVI=8&MVe=9&VTI=10&VTe=11&MVesp=11
	
	//char sendBuff[1025];
	//strcpy(sendBuff, "GET http://192.168.1.111/Health_detection/insert.php?Ppeak=13&Pmean=2&PEEP=3&breath_rate=4&Oxygen_density=5&End_expiratory_flow=6&TITtot=7&MVI=8&MVe=9&VTI=10&VTe=11&MVesp=11 \r\nHTTP/1.1\r\n Host: 192.168.1.111 \r\n");
	time_t ticks; 

	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	memset(&serv_addr, '0', sizeof(serv_addr));
	//memset(sendBuff, '0', sizeof(sendBuff)); 

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr("192.168.1.111");	//服务器
	//serv_addr.sin_addr.s_addr = inet_addr("192.168.1.101");		//连接本地服务器
	serv_addr.sin_port = htons(80); 
	//serv_addr.sin_port = htons(5000); 
	
	int status=connect(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
	
	if (status != 0)
	{
		printf("[ERROR]Connecting failed!\n");		
		return -1;
	}

	
	ticks = time(NULL);
	//snprintf(sendBuff, sizeof(sendBuff)+100, "%.24s\r\n", ctime(&ticks));
	write(listenfd, sendBuff, strlen(sendBuff));	
}
