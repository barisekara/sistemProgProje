#ifndef KEYPAD_H
#define KEYPAD_H

#include <stdbool.h>

char ReadCharacterFromKeypad();                                 // GPIO pinlerinden karakter okur
bool ReadInputFromKeypad(int, char[]);                          // Keypad'ten girilen degeri okur
//bool ParseKeypadInput(char *, char *, char *, char *, char *, char *); // Verilen kalıplari ayristirir.

#endif /* KEYPAD_H */
