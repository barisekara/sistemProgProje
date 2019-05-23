#ifndef CNNCT_H
#define CNNCT_H

//SUNUCU İÇİN

typedef struct{
	int id;
	char tip[10];
	int pin;
	int deger;
}Sensor;


//İSTEMCİ İÇİN
int connectServerSocket(char*, int , int* );

int sendSocketMsg(int, char*);

int readSocketMsg(int, char*);


int createUARTStream(int* );

int readUARTMsg(int, char[256]);

int sendUARTMsg(int, char[20]);

int EndUARTStream(int);

int CloseConnection(int);
#endif
