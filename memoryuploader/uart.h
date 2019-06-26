
#include <stdio.h>

void UARTInit();
int UARTPutchar(char c, FILE *stream);
char UARTGetchar(FILE *stream);
void UARTFlush(void);