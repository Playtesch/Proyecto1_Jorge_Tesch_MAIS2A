#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	short int ETQ;
	short int Datos[8];
} T_LINEA_CACHE;

void leerFicheroRAM(unsigned char RAM[]);
int main(void) {
    unsigned char RAM[1024];
	T_LINEA_CACHE cache[4];
	int i, j;

	for (i = 0; i < 4; i++) {
		cache[i].ETQ = 0xFF;
		for (j = 0; j < 8; j++) {
			cache[i].Datos[j] = 0;
		}
	}

	leerFicheroRAM(RAM);
 
	return 0; 
}

void leerFicheroRAM(unsigned char RAM[]) {
	FILE* fichero;

	if ((fichero = fopen("RAM.bin", "rb")) == NULL) {
		printf("No existe el fichero de la RAM\n");
		exit(-1);
	}

	fread(RAM, 1024, 1, fichero);

	fclose(fichero);
	return;
}
