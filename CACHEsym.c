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
void aciertoCache(T_LINEA_CACHE cache[], short int direccion, short int bloque, short int etiqueta, short int linea, short int palabra, float* tiempo, int numaccesos);
void falloCache(unsigned char RAM[][8], T_LINEA_CACHE cache[], short int direccion, short int bloque, short int etiqueta, short int linea, short int palabra, float* tiempo, int* numfallos);
void imprimirCache(T_LINEA_CACHE cache[]);
int main(void) {
    unsigned char RAM[1024];
    unsigned char RAMOrdenada[128][8];
	T_LINEA_CACHE cache[4];
    FILE* ficheroAccesosDeMemoria;
    char valorAccesoDeMemoria[5];
    short int direccion, bloque, etiqueta, linea, palabra;
    int numaccesos = 0;
	float tiempoglobal = 0;
	int numfallos = 0;
    int acierto = 0;
	int i, j;

	for (i = 0; i < 4; i++) {
		cache[i].ETQ = 0xFF;
		for (j = 0; j < 8; j++) {
			cache[i].Datos[j] = 0;
		}
	}

	leerFicheroRAM(RAM);
    dividirRAM(RAM, RAMOrdenada);

    if ((ficheroAccesosDeMemoria = fopen("accesos_memoria.txt", "r")) == NULL) {
        printf("No existe el fichero de Accesos de Memoria\n");
        exit(-1);
    }

    while ((fread(valorAccesoDeMemoria, 5, 1, ficheroAccesosDeMemoria)) != NULL) {
        acierto = 0;
        numaccesos += 1;

        datosDireccionDeMemoria(valorAccesoDeMemoria, &direccion, &bloque, &etiqueta, &linea, &palabra);

        while (acierto == 0) {
            if (cache[linea].ETQ == etiqueta) {
                aciertoCache(cache, direccion, bloque, etiqueta, linea, palabra, &tiempoglobal, numaccesos);
                acierto = 1;
            }
            else {
                falloCache(RAMOrdenada, cache, direccion, bloque, etiqueta, linea, palabra, &tiempoglobal, &numfallos);
            }
        }
    }

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
    int i;

    traduccionBinario(direccionHex, direccionBinario);

    for (i = 0; i < 5; i++) {
        valorEtiqueta[i] = direccionBinario[i];
    }
    valorEtiqueta[5] = '\0';

    for (i = 0; i < 2; i++) {
        valorLinea[i] = direccionBinario[i + 5];
    }
    valorLinea[2] = '\0';

    for (i = 0; i < 3; i++) {
        valorPalabra[i] = direccionBinario[i + (7)];
    }
    valorPalabra[3] = '\0';

    strcpy(valorBloque, valorEtiqueta);
    strcat(valorBloque, valorLinea);

    *direccion = (short int)strtol(direccionHex, NULL, 16);
    *bloque = (short int)strtol(valorBloque, NULL, 2);
    *etiqueta = (short int)strtol(valorEtiqueta, NULL, 2);
    *linea = (short int)strtol(valorLinea, NULL, 2);
    *palabra = (short int)strtol(valorPalabra, NULL, 2);
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



void aciertoCache(T_LINEA_CACHE cache[], short int direccion, short int bloque, short int etiqueta, short int linea, short int palabra, float* tiempo, int numaccesos) {

    printf("T: %.0f, Acierto de CACHE, ADDR %04X ETQ %X linea %02X palabra %02X DATO %02X\n", *tiempo, direccion, etiqueta, linea, palabra, cache[linea].Datos[7-palabra]);

    imprimirCache(cache);

    *tiempo += 1;
}


void falloCache(unsigned char RAM[][8], T_LINEA_CACHE cache[], short int direccion, short int bloque, short int etiqueta, short int linea, short int palabra, float* tiempo, int* numfallos) {
    int i;

    *numfallos += 1;

    printf("T: %.0f, Fallo de CACHE %d, ADDR %04X ETQ %X linea %02X palabra %02X bloque %02X\n", *tiempo, *numfallos, direccion, etiqueta, linea, palabra, bloque);
    printf("Cargando el bloque %02X en la linea %02X\n", bloque, linea);

    *tiempo += 10;

    cache[linea].ETQ = etiqueta;
    for (i = 0; i < 8; i++) {
        cache[linea].Datos[i] = RAM[bloque][7 - i];
    }
}

void imprimirCache(T_LINEA_CACHE cache[]) {
    int i, j;

    for (i = 0; i < 4; i++) {
        printf("ETQ: %X\tDatos ", cache[i].ETQ);
        for (j = 0; j < 8; j++) {
            printf("%02X ", cache[i].Datos[j]);
        }
        printf("\n");
    }
    printf("\n");
}
