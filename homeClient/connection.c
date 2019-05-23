#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include "connection.h"

#include <unistd.h>//Used for UART
#include <fcntl.h>//Used for UART
#include <termios.h>//Used for UART
#define COUNT 256


//socket_fd 

int connectServerSocket(char* ip, int port, int* socket_fd ){
	struct sockaddr_in caddr;
	caddr.sin_family = AF_INET;
	caddr.sin_port = htons(port);
	if(inet_aton(ip, &caddr.sin_addr) == 0){
		return (-1);
	}

	/*Sunucu olarak baglanti kuracak soketi yaratalim.*/
	*socket_fd = socket(PF_INET, SOCK_STREAM, 0);
	if(*socket_fd==-1){
		printf("Error on socket creation [%s]\n", strerror(errno));
		return (-1);
	}

	/*Suucuya baglanti istegi gonderip, baglantiyi kuralim.*/
	if(connect(*socket_fd, (const struct sockaddr *)&caddr, sizeof(struct sockaddr))==-1){
		printf("Error on socket connect [%s]\n", strerror(errno));
		return (-1);
	}
	
	return 1;

}

int sendSocketMsg(int socket_fd, char* msg){
	if (write(socket_fd, msg, strlen(msg)) != strlen(msg))
		return 1;
	return 0;
}

int readSocketMsg(int socket_fd, char* msg){
	char str[COUNT];
	memset(str, '\0', COUNT);
	if (read(socket_fd, str, COUNT) == 0)
		return 1;
	strcpy(msg, str);
	return 0;
}

int createUARTStream(int* uart0_filestream){
	*uart0_filestream = -1;

	*uart0_filestream = open("/dev/serial0", O_RDWR | O_NOCTTY | O_NDELAY);
	if (*uart0_filestream == -1)
	{
		printf("Error - Unable to open UART. Ensure it is not in use by another application\n");
		return 1;
	}

	struct termios options;
	tcgetattr(*uart0_filestream, &options);
	options.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
	options.c_iflag = IGNPAR;
	options.c_oflag = 0;
	options.c_lflag = 0;
	tcflush(*uart0_filestream, TCIFLUSH);
	tcsetattr(*uart0_filestream, TCSANOW, &options);
	return 0;
}

int readUARTMsg(int uart0_filestream, char *msg){
	if (uart0_filestream != -1)
	{
		unsigned char rx_buffer[256];
		int rx_length = read(uart0_filestream, (void *) rx_buffer, 255);
		if (rx_length > 0)
		{
			rx_buffer[rx_length] = '\0';
			strcpy(msg, rx_buffer);
			return 0;
		}
	}
	return 1;
}

int sendUARTMsg(int uart0_filestream, char *msg){
	char tx_buffer[COUNT];
	char *p_tx_buffer;
	
	p_tx_buffer = &tx_buffer[0];
	for (int i = 0; msg[i] != '\0'; i++)
		*p_tx_buffer++ = msg[i];

	if (uart0_filestream != -1)
	{
		int count = write(uart0_filestream, &tx_buffer[0], (p_tx_buffer - &tx_buffer[0]));
		if (count < 0)
		{
			printf("UART TX error\n");
			return 0;
		}
	}
	return 1;
}

int EndUARTStream(int uart0_filestream)
{
	if (close(uart0_filestream) != 0)
		return 0;
	return 1;
}

int CloseConnection(int socket_fd)
{
	if (close(socket_fd) != 0)
		return 1;
	return 0;
}