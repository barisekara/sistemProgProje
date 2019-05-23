/*
152120141099
Baris Erenkara
Sistem Programlama Projesi




 */

#ifndef SERVER
#define SERVER

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>

//GPIO
#include "gpioDIO.h"

//UART
#include <fcntl.h>
#include <termios.h>

//Keypad
#include "Keypad.h"

//configuration variables and function prototypes
#include "config.h"


//variables
char *SENSOR_NAME = "Egim";  // Sensor Tipi
int sVal, rVal, aVal = -1; //Sensor and relay values, also automode state
int port;
bool bye = false;


//function prototypes
bool IsPasswordCorrect(char*);
void DoKeypadOperations(char*);

//threads
pthread_t listenKeypad;
pthread_t threadSocket;
pthread_t uart;
pthread_t readSensor;
pthread_t autoMode;

void signal_handler(int no){   //signal handler SIGINT
    char str[100];
    sprintf(str,"bye");

    if(write(client,str,strlen(str))!=strlen(str)){
        printf("Error writing data [%s]\n",strerror(errno));
    }
    close(server);
    close(client);
    exit(0);
}

void *threadListenKeypad(void *arg) {   //keypad thread

char keypadInput[11];

while (1){

        // keypadInput icerigi temizlenir.
	memset(&keypadInput[0], '\0', sizeof(keypadInput));
	sleep(1);

        // Keypad'ten maximum 11 karakter okuma yapar.
	ReadInputFromKeypad(10, keypadInput);

        // Gelen Input Formatını kontrol eder ve uygunsa
        // Parola degistirir veya Röle Acıp/Kapar
	DoKeypadOperations(keypadInput);

    }// while(1)
} // end threadListenKeypad(void *arg)

void *threadReadSensor(void *arg)   //sensor thread
{
    while (1)
    {
        sVal = GPIORead(SENSOR);
        rVal = GPIORead(RELAY);
    }
    return;
}

void *threadSocketService(void *arg)  //socket listener thread
{
    /*Baglanti olustur.*/
    /*Adresi olusturalim.*/
    struct sockaddr_in saddr,caddr;
    saddr.sin_family=AF_INET;
    saddr.sin_port=htons(port);
    saddr.sin_addr.s_addr=htonl(INADDR_ANY);

    /*Sunucu olarak baglanti isteklerini dinleyecek soketi yaratalim.*/
    server=socket(PF_INET, SOCK_STREAM, 0);
    if(server==-1){
        printf("Error on socket creation [%s]\n",strerror(errno));
        return false;
    }

    /*Sokete olusturdugumuz adresi ve ayarlari baglayalim.*/
    if(bind(server,(struct sockaddr *)&saddr,sizeof(struct sockaddr))==-1){
        printf("Error on socket bind [%s]\n",strerror(errno));
        return false;
    }
    
    while (1)
    {   
        /*Baglanti istekleri icin dinlemeye baslayalim.*/
        if(listen(server, 5)==-1){
            printf("Error on socket listen [%s]\n", strerror(errno));
            return (-1);
        }

        /*Baglanti talebi geldi, baglantinin idamesini yeni bir sokete tahsis edelim.*/
        uint32_t inet_len=sizeof(caddr);
        if((client = accept(server,(struct sockaddr *)&caddr, &inet_len))==-1){
            printf("Error on socket accept [%s]\n",strerror(errno));
            return (-1);
        }

        sleep(1);
        char str[100];
        while (!bye) {
            memset(str, '\0', 100);
            read(client, str, count);
            
            if (strcmp(str, "sensorDurum:") == 0) {
                sprintf(str, "sensorDurum %d:", sVal);
                if (write(client, str, strlen(str)) != strlen(str))
                    printf("Error sending sensor value!\n");
            }
            else if (strcmp(str, "surucu 1:") == 0) {
                strcpy(str, "surucu ok:");
                aVal = -1;
                if (GPIOWrite(27, HIGH) != 0)
                    strcpy(str, "surucu err:");
                if (write(client, str, strlen(str)) != strlen(str))
                    printf("Error sending relay change message!\n");
            }
            else if (strcmp(str, "surucu 0:") == 0) {
                strcpy(str, "surucu ok:");
                aVal = -1;
                if (GPIOWrite(27, LOW) != 0)
                    strcpy(str, "surucu err:");
                if (write(client, str, strlen(str)) != strlen(str))
                    printf("Error sending relay change message!\n");
            }
            else if (strcmp(str, "sensorTip:") == 0) {
                strcpy(str, "sensorTip HAREKET:");
                if (write(client, str, strlen(str)) != strlen(str))
                    printf("Error sending sensor type!\n");
            }
            else if (strcmp(str, "surucuDurum:") == 0) {
                sprintf(str, "surucu %d:", rVal);
                if (write(client, str, strlen(str)) != strlen(str))
                    printf("Error sending relay value!\n");
            }
            else if (strcmp(str, "auto 1:") == 0) {
                strcpy(str, "auto ok:");
                aVal = 1;
                if (write(client, str, strlen(str)) != strlen(str))
                    printf("Error sending auto-mode message!\n");
            }
            else if (strcmp(str, "auto 0:") == 0) {
                strcpy(str, "auto ok:");
                aVal = 0;
                if (write(client, str, strlen(str)) != strlen(str))
                    printf("Error sending auto-mode message!\n");
            }
            else if (strcmp(str, "auto -1:") == 0) {
                strcpy(str, "auto ok:");
                aVal = -1;
                if (write(client, str, strlen(str)) != strlen(str))
                    printf("Error sending auto-mode message!\n");
            }
            else if (strcmp(str, "kapat:") == 0) {
                strcpy(str, "kapat ok:");
                if (write(client, str, strlen(str)) != strlen(str))
                    printf("Error sending shutdown message!\n");
                bye = true;
            }
        }
        close(client);
        client = -1;
        bye = false;
    }
    return;
}

void *threadUARTService(void *arg){   //uart thread
    int uart0_filestream = -1;

    uart0_filestream = open("/dev/serial0", O_RDWR | O_NOCTTY | O_NDELAY);
    if (uart0_filestream == -1)
        printf("Error - Unable to open UART. Ensure it is not in use by another application\n");

    struct termios options;
    tcgetattr(uart0_filestream, &options);
    options.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
    options.c_iflag = IGNPAR;
    options.c_oflag = 0;
    options.c_lflag = 0;
    tcflush(uart0_filestream, TCIFLUSH);
    tcsetattr(uart0_filestream, TCSANOW, &options);

    char msg[256] = "";
    unsigned char tx_buffer[256];
    unsigned char *p_tx_buffer;

    while (1)
    {
        if (uart0_filestream != -1)
        {
            unsigned char rx_buffer[256];
            int rx_length = read(uart0_filestream, (void *) rx_buffer, 255);
            if (rx_length > 0)
            {
                rx_buffer[rx_length] = '\0';
                if (strcmp(rx_buffer, "sensorDurum:") == 0)
                    sprintf(msg, "sensorDurum %d:", sVal);
                else if (strcmp(rx_buffer, "surucu 1:") == 0) {
                    strcpy(msg, "surucu ok:");
                    aVal = -1;
                    if (GPIOWrite(SENSOR, HIGH) != 0)
                        strcpy(msg, "surucu err:");
                }
                else if (strcmp(rx_buffer, "surucu 0:") == 0) {
                    strcpy(msg, "surucu ok:");
                    aVal = -1;
                    if (GPIOWrite(SENSOR, LOW) != 0)
                        strcpy(msg, "surucu err:");
                }
                else if (strcmp(rx_buffer, "sensorTip:") == 0)
                    strcpy(msg, "sensorTip HAREKET:");
                else if (strcmp(rx_buffer, "surucuDurum:") == 0)
                    sprintf(msg, "surucu %d:", rVal);
                else if (strcmp(rx_buffer, "auto 1:") == 0)
                {
                    strcpy(msg, "auto ok:");
                    aVal = 1;
                }
                else if (strcmp(rx_buffer, "auto 0:") == 0)
                {
                    sprintf(msg, "auto ok:", rVal);
                    aVal = 0;
                }
                else if (strcmp(rx_buffer, "auto -1:") == 0)
                {
                    sprintf(msg, "auto ok:", rVal);
                    aVal = -1;
                }
            }
        }
        
        sleep(1);
        
        p_tx_buffer = &tx_buffer[0];
        for (int i = 0; msg[i] != '\0'; i++)
            *p_tx_buffer++ = msg[i];

        if (uart0_filestream != -1)
        {
            int count = write(uart0_filestream, &tx_buffer[0], (p_tx_buffer - &tx_buffer[0]));
            if (count < 0)
                printf("UART TX error\n");
        }
    }

    close(uart0_filestream);
    return;
}

void *threadAutoMode(void *parameter)
{
    while (1)
    {
        if (aVal == 1)
        {
            if (sVal == 1)
                GPIOWrite(SENSOR, HIGH);
            else
                GPIOWrite(SENSOR, LOW);
        }
        else if (aVal == 0)
        {
            if (sVal == 1)
                GPIOWrite(SENSOR, LOW);
            else
                GPIOWrite(SENSOR, HIGH);
        }
    }
}


int main(int argc, char **argv){
    /*CTRL^C handler.*/
    struct sigaction newAction, oldAction;
    newAction.sa_handler=signal_handler;
    newAction.sa_flags=SA_NODEFER |SA_ONSTACK;
    sigaction(SIGINT, &newAction, &oldAction);
    /*Argumanları kontrol et */
        if(argc!=3){
        printf("False arguments...\n");
        return 0;
    }

    
    int i;
    for(i=1;i<2;i++){
        if(!strncmp(argv[i],"-port",5)){
            port=atoi(argv[i+1]);
        }
    }
    /* create and join threads */
	pthread_create(&listenKeypad, NULL, threadListenKeypad, NULL);
    pthread_create(&threadSocket, NULL, threadSocketService, NULL);
    pthread_create(&uart, NULL, threadUARTService, NULL);
    pthread_create(&autoMode, NULL, threadAutoMode, NULL);
    pthread_create(&readSensor, NULL, threadReadSensor, NULL);
    pthread_join(listenKeypad, NULL);
    pthread_join(threadSocket, NULL);
    pthread_join(autoMode, NULL);
    pthread_join(readSensor, NULL);
    pthread_join(uart, NULL);

    return 0;
}

void DoKeypadOperations(char* keypadInput){

   	char currentPasswd[5];  // Mevcut Parolayi tutar.
    char newPasswd[5];      // Yeni Parolayi gecici olarak tutar.

    char chStar[2];         // * karaterini tutar
    char chSharp[2];        // # karakterini tutar.
    char relayStatus[2];
    //printf("%s\n",keypadInput);

   // bool isParse;           // keypad'ten girilen girdi parse edildi mi?

    if (strlen(keypadInput) == 10){

        // XXXX*YYYY#
    	sscanf(keypadInput,"%4s%1s%4s%1s",currentPasswd,chStar,newPasswd,chSharp);
    	if (IsPasswordCorrect(currentPasswd) && !strcmp(chStar,"*")){

            // Keypad girdilerinin boyutunu kontrol et
            // 10 ise Parola Degisimi
            // 7  ise Röle Islemleri
    		if(strlen(keypadInput) == 10){

    			strcpy( PASSWORD, newPasswd);
    			printf("\nPassword has changed!\n");

    		}

    	}
    	else
    		printf("\nWARNING - Authentication Failed, Wrong Password\n");
    }


   else if(strlen(keypadInput) == 7)
    {
        
    	sscanf(keypadInput,"%4s%1s%1s%1s",currentPasswd,chStar,relayStatus,chSharp);
        /*printf("%4s\n",currentPasswd);
        printf("%1s\n",chStar);
        printf("%1s\n",relayStatus);
        printf("%1s\n",chSharp);*/
    	if (IsPasswordCorrect(currentPasswd) && !strcmp(chStar,"*")){

            // Keypad girdilerinin boyutunu kontrol et
            // 10 ise Parola Degisimi
            // 7  ise Röle Islemleri
    		if (strlen(keypadInput) == 7){

    			int ret = GPIOWrite(RELAY, atoi(relayStatus));

    			if (ret)
    				printf("\nNow Relay [%s]\n", atoi(relayStatus) == 1 ? "ON" : "OFF");

    		}
    	}
    	else
    		printf("\nWARNING - Authentication Failed, Wrong Password\n");
    }


} // DoKeypadOperations

//Parola doğru mu?
bool IsPasswordCorrect(char* password) {

	return (!strcmp(password,PASSWORD)) ? true : false;
}

#endif