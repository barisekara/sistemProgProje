#include <stdio.h>      //  printf()
#include <unistd.h>     //  sleep()
#include <string.h>     //  strlen(), strcmp()
#include "Keypad.h"     //  readKeypad(),
#include "gpioDIO.h"    //  GPIORead(), GPIOWrite()


char key[4][4]={{'1','2','3','A'},
    {'4','5','6','B'},
    {'7','8','9','C'},
    {'*','0','#','D'}};


/*  Keypad Column Pins: 6  13 19 26  out    */
/*  Keypad Row    Pins: 12 16 20 21  in     */
int keyGPIOCol[4] = {  6,  13, 19, 26 };
int keyGPIORow[4] = {  12, 16, 20, 21 };

char ReadCharacterFromKeypad()
{
    
    int k,i,j;
    
    for(i=0;i<4;i++){
        //Set high first column
        if(GPIOWrite(keyGPIOCol[i],1)==-1)
            return 'e';
        
        for(j=0;j<4;j++){
            if(( k=GPIORead(keyGPIORow[j]) )==-1)
                return 'e';
            if(k==1)
                return key[j][i];
        }
        if(GPIOWrite(keyGPIOCol[i],0)==-1)
            return 'e';
    }
    return 'n';
}


bool ReadInputFromKeypad(int size, char keypadInput[])
{
    
    char keypressed='n';
    char prevKeypressed = keypressed;

    sleep(1);
    for(int index=0; keypressed != '#' && index < size; )
    {
        prevKeypressed = keypressed;
        keypressed = ReadCharacterFromKeypad();
        
        if( keypressed == 'e' || keypressed == 'n' )
            continue;
        
        if( prevKeypressed == 'n' )
            keypadInput[index++] = keypressed;
    }
    
    printf("\nINFO - Keypad Input: [%s], MessageSize: [%d]\n\n", keypadInput, strlen(keypadInput) );
    return true;
}// end ReadInputFromKeypad()

/*bool ParseKeypadInput(char* keypadInput, char* currentPasswd, char* newPasswd , char* chStar , char* chSharp, char* relayStatus){

    // Parola Değişimi
    if (strlen(keypadInput) == 10){

        // XXXX*YYYY#
        sscanf(keypadInput,"%4s%1s%4s%1s",currentPasswd,chStar,newPasswd,chSharp);
        return true;
    }

    // Relay ON/OFF
    else if(strlen(keypadInput) == 7){
        // XXXX*Y#
        sscanf(keypadInput,"%4s%1s%1s%1s",currentPasswd,chStar,relayStatus,chSharp);
        return true;
    }
    return false;
}*/


