#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "config.h"
#include "connection.h"


int socket_fd, uart_filestream, port;
char ip[16], msg[256]  = "null";
int connectionType = -1; // 0 for uart 1 for socket
char option;
char *sensorTip = "Hareket Sensoru";

int main(int argc, char **argv)
{

//check arguments 

	if(argc != 2 && argc != 5){
		printf("False arguments...\n");
		return 1;
	}
	else if (argc == 5)
	{
		int i;
		for(i=1;i<4;i++){
			if(!strncmp(argv[i],"-port",5)){
				port=atoi(argv[i+1]);
			}
			if(!strncmp(argv[i],"-ip",3)){
				strcpy(ip,argv[i+1]);
			}
		}
		

		if(connectServerSocket(ip, port, &socket_fd)== -1)
		{
			printf("\nBaglanti saglanilamadı.\nSunucuyu acik mi? Aciksa Port ve IP bilgilerini kontrol edin.\n");
			return 1;
		}
		connectionType = 1;
	}
	else if (argc == 2 && strcmp(argv[1], "-UART") == 0)
	{
		createUARTStream(&uart_filestream);
		connectionType = 0;
	}
	else
	{
		printf("False arguments...\n");
		return 1;
	}
	//check arguments end
	
	while(isRunning)
	{
	printMainMenu();
	printf("\nIP:%s\nPORT:%d\n",ip,port);
	option = getchar();
	int fflush_s;
	//printf("\n%c\n",option);
	while ((fflush_s = getchar()) != '\n' && fflush_s != EOF); //Flush stdin
	if(option == '1')
	{	
		//printf("\n%c\nA%d",option,connectionType);
		if(connectionType)
		{
			sendSocketMsg(socket_fd,"sensorDurum:");
			readSocketMsg(socket_fd,msg);
		}
		else
		{
			sendUARTMsg(uart_filestream,"sensorDurum:");
			readUARTMsg(uart_filestream,msg);

		}

		printf("\nSensor Durumu: ");
		if(strcmp(msg, "sensorDurum 1:") == 0)
			printf("ON\n");
		else if(strcmp(msg, "sensorDurum 0:")==0)
			printf("OFF\n");
		printf("\nDevam etmek icin enter\n");
		while ((fflush_s = getchar()) != '\n' && fflush_s != EOF); //Flush stdin

	}
	else if(option == '2')
	{
		if(connectionType)
		{
			sendSocketMsg(socket_fd,"surucuDurum:");
			readSocketMsg(socket_fd,msg);
		}
		else
		{
			sendUARTMsg(uart_filestream,"surucuDurum:");
			readUARTMsg(uart_filestream,msg);
		}
		printf("\nSurucu Durumu: ");
		//printf("%s\n",msg);
		if(strcmp(msg, "surucu 1:") == 0)
			printf("ON\n");
		else if(strcmp(msg, "surucu 0:")==0)
			printf("OFF\n");
		printf("\nDevam etmek icin enter\n");
		while ((fflush_s = getchar()) != '\n' && fflush_s != EOF); //Flush stdin
	}
	else if(option == '3')
	{
		if(connectionType)
		{
			sendSocketMsg(socket_fd,"sensorTip:");
			readSocketMsg(socket_fd,msg);
		}
		else
		{
			sendUARTMsg(uart_filestream,"sensorTip:");
			readUARTMsg(uart_filestream,msg);
		}
		int i;
		char type[32] = "";
		for (i = 10; msg[i] != ':'; i++)
			{
				int len = strlen(type);
				type[len] = msg[i];
				type[len+1] = '\0';
			}
		//printf("Sistemde bulunan sensorun tipi: %s\n",sensorTip);
		printf("Sensörün hangi durumunda sürücü ON olsun? (0-OFF/1-ON/2-Devre dışı bırak) ");
		option = getchar();
		while ((fflush_s = getchar()) != '\n' && fflush_s != EOF); //Flush stdin
		
		//printf("deneme%d\n",connectionType);
		if(option == '1' && connectionType == 0)
		{
			sendUARTMsg(socket_fd,"auto 1:");
			readUARTMsg(uart_filestream,msg);
			if(!strcmp(msg,"auto ok:"))
				printf("Auto Mode etkin. Sensor ON iken Role de ON olacak.\n");
		}
		else if(option == '1' && connectionType == 1)
		{	
			//printf("\nok!");
			sendSocketMsg(socket_fd,"auto 1:");
			readSocketMsg(socket_fd,msg);
			if(!strcmp(msg,"auto ok:"))
				printf("Auto Mode etkin. Sensor ON iken Role de ON olacak.\n");

		}
		else if(option == '0' && connectionType == 0)
		{
			sendUARTMsg(socket_fd,"auto 0:");
			readUARTMsg(uart_filestream,msg);
			printf("\n%s",msg);
			if(!strcmp(msg,"auto ok:"))
				printf("Auto Mode etkin. Sensor OFF iken Role  ON olacak.\n");

		}
		else if(option == '0' && connectionType == 1)
		{
			
			if(sendSocketMsg(socket_fd,"auto 0:"))
				printf("mesaj gonderilirken bir hata olustu\n");
			readSocketMsg(socket_fd,msg);
			printf("\n%s",msg);
			if(!strcmp(msg,"auto ok:"))
				printf("Auto Mode etkin. Sensor OFF iken Role  ON olacak.\n");
			
		}
		else if(option == '2' && connectionType == 0)
		{
			sendUARTMsg(socket_fd,"auto -1:");
			readUARTMsg(uart_filestream,msg);
			if(!strcmp(msg,"auto ok:"))
				printf("Auto Mode devre disi.\n");
			
		}
		else if(option == '2' && connectionType == 1)
		{
			sendSocketMsg(socket_fd,"auto -1:");
			readSocketMsg(socket_fd,msg);
			if(!strcmp(msg,"auto ok:"))
				printf("Auto Mode devre disi.\n");
			
		}
		printf("Devam etmek için Enter'a basın...\n");
		while ((fflush_s = getchar()) != '\n' && fflush_s != EOF); //Flush stdin
	}
	else if(option == 'x' || option == 'X')
	{	
		isRunning = false;
		CloseConnection(socket_fd);
		return 0;
	}

	}

}

void printMainMenu(){
	 //Clears the screen
	system("clear");        

		    //Clears the keyboard buffer
	fflush(stdin);
		        //Outputs the options to console
	puts("\n[1]Sensor Value"
		"\n[2]Driver Value"
		"\n[3]Auto Mode"
		"\n[x]Exit");
		    //Reads the user's option

		    //Selects the course of action specified by the option

	}
