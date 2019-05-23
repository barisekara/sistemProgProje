#ifndef CONFIG_H
#define CONFIG_H
#define SENSOR 23         // SENSOR_1    GPIO23 pinine bağlı.
#define RELAY 4             // RELAY       GPIO4 pinine bağlı.
char PASSWORD[5]= "0000";   // Sistemin default parolasi
void printMainMenu();
bool isRunning = true;

#endif
