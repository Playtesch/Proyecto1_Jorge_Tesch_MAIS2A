#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	short int ETQ;
	short int Datos[8];
} T_LINEA_CACHE;

int main(void) {
	T_LINEA_CACHE cache[4];
	int i, j;

	for (i = 0; i < 4; i++) {
		cache[i].ETQ = 0xFF;
		for (j = 0; j < 8; j++) {
			cache[i].Datos[j] = 0;
		}
	}
 
	return 0; 
}