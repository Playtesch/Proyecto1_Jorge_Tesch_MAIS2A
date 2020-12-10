#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	short int ETQ;
	short int Datos[8];
} T_LINEA_CACHE;

void leerFicheroRAM(unsigned char RAM[]);
void dividirRAM(unsigned char RAM[], unsigned char RAMOrdenada[][8]);
void datosDireccionDeMemoria(char direccionHex[], short int *direccion, short int* bloque, short int* etiqueta, short int* linea, short int* palabra);
void traduccionBinario(char direccionHex[], char direccionBinario[]);
int main(void) {
    unsigned char RAM[1024];
    unsigned char RAMOrdenada[128][8];
	T_LINEA_CACHE cache[4];
    char valorAccesoDeMemoria[5];
    short int direccion, bloque, etiqueta, linea, palabra;
	int i, j;

	for (i = 0; i < 4; i++) {
		cache[i].ETQ = 0xFF;
		for (j = 0; j < 8; j++) {
			cache[i].Datos[j] = 0;
		}
	}

	leerFicheroRAM(RAM);
    dividirRAM(RAM, RAMOrdenada);

    valorAccesoDeMemoria = "02E4";
    datosDireccionDeMemoria(valorAccesoDeMemoria, &direccion, &bloque, &etiqueta, &linea, &palabra);
  

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

void dividirRAM(unsigned char RAM[], unsigned char RAMOrdenada[][8]) {
    int i, j;

    for (i = 0; i < 128; i++) {
        for (j = 0; j < 8; j++) {
            RAMOrdenada[i][j] = RAM[(i * 8) + j];
        }
    }
}

void datosDireccionDeMemoria(char direccionHex[], short int *direccion, short int *bloque, short int *etiqueta, short int* linea, short int *palabra){
	char direccionBinario[11];
    char valorBloque[8], valorEtiqueta[6], valorLinea[3], valorPalabra[4];

    traduccionBinario(direccionHex, direccionBinario);
}

void traduccionBinario(char direccionHex[], char direccionBinario[]) {
    char valorBinario[5];
    int i;

    for (i = 1; i < 4; i++) {
        switch (direccionHex[i]) {
        case '0':
            strcpy(valorBinario, "0000"); break;
        case '1':
            strcpy(valorBinario, "0001"); break;
        case '2':
            strcpy(valorBinario, "0010"); break;
        case '3':
            strcpy(valorBinario, "0011"); break;
        case '4':
            strcpy(valorBinario, "0100"); break;
        case '5':
            strcpy(valorBinario, "0101"); break;
        case '6':
            strcpy(valorBinario, "0110"); break;
        case '7':
            strcpy(valorBinario, "0111"); break;
        case '8':
            strcpy(valorBinario, "1000"); break;
        case '9':
            strcpy(valorBinario, "1001"); break;
        case 'A':
            strcpy(valorBinario, "1010"); break;
        case 'B':
            strcpy(valorBinario, "1011"); break;
        case 'C':
            strcpy(valorBinario, "1100"); break;
        case 'D':
            strcpy(valorBinario, "1101"); break;
        case 'E':
            strcpy(valorBinario, "1110"); break;
        case 'F':
            strcpy(valorBinario, "1111"); break;
        }

        if (i == 1) {
            direccionBinario[0] = valorBinario[2];
            direccionBinario[1] = valorBinario[3];
            direccionBinario[2] = '\0';
        }

        else {
            strcat(direccionBinario, valorBinario);
        }
    }
}
