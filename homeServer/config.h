#ifndef CONFIG_H
#define CONFIG_H
#define SENSOR 22         	// SENSOR   GPIO22 pinine bağlı.
#define RELAY 27           // RELAY       GPIO27 pinine bağlı.
#define TRUE 1
#define FALSE 0
#define true 1
#define false 0
char PASSWORD[5]= "0000";   // Sistemin default parolasi
void printMainMenu();
bool isRunning = true;
int client = -1, server;

int count=100;
int rCount=0;

#endif
