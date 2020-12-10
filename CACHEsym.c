#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define TAMANORAM 1024
#define LINEASCACHE 4
#define DATOSCACHE 8
#define BITETIQUETA 5
#define BITLINEA 2
#define BITPALABRA 3


typedef struct {
	short int ETQ;
	short int Datos[DATOSCACHE];
} T_LINEA_CACHE;

void leerFicheroRAM(unsigned char RAM[]);
void dividirRAM(unsigned char RAM[], unsigned char RAMOrdenada[][DATOSCACHE]);
void datosDireccionDeMemoria(char direccionHex[], short int *direccion, short int* bloque, short int* etiqueta, short int* linea, short int* palabra);
void traduccionBinario(char direccionHex[], char direccionBinario[]);
void aciertoCache(T_LINEA_CACHE cache[], short int direccion, short int bloque, short int etiqueta, short int linea, short int palabra, float* tiempo, char texto[], int numaccesos);
void falloCache(unsigned char RAM[][DATOSCACHE], T_LINEA_CACHE cache[], short int direccion, short int bloque, short int etiqueta, short int linea, short int palabra, float* tiempo, int* numfallos);
void imprimirCache(T_LINEA_CACHE cache[]);

//Jorge Tesch Torres MAIS2A
int main(void) {
    unsigned char RAM[TAMANORAM];
    unsigned char RAMOrdenada[TAMANORAM/DATOSCACHE][DATOSCACHE];
	T_LINEA_CACHE cache[LINEASCACHE];
    FILE* ficheroAccesosDeMemoria;
    char valorAccesoDeMemoria[5];
    short int direccion, bloque, etiqueta, linea, palabra;
    char texto[100];
    int numaccesos = 0;
	float tiempoglobal = 0;
	int numfallos = 0;
    int acierto = 0;
	int i, j;

    //Inicializamos el array de la cache.
	for (i = 0; i < LINEASCACHE; i++) {
		cache[i].ETQ = 0xFF;
		for (j = 0; j < DATOSCACHE; j++) {
			cache[i].Datos[j] = 0;
		}
	}

    //Leemos el fichero de la RAM y lo dividimos en los bloques coreespondientes.
	leerFicheroRAM(RAM);
    dividirRAM(RAM, RAMOrdenada);

    //Comprobamos si existe el fichero de los accesos de memoria y, si existe, lo abrimos y si no alertamos al usuario y salimos del programa.
    if ((ficheroAccesosDeMemoria = fopen("accesos_memoria.txt", "r")) == NULL) {
        printf("No existe el fichero de Accesos de Memoria\n");
        exit(-1);
    }

    //Leemos las lineas del fichero de accesos de memoria de 1 en 1 hasta el final.
    while ((fread(valorAccesoDeMemoria, 5, 1, ficheroAccesosDeMemoria)) != NULL) {
        acierto = 0;
        numaccesos += 1;

        //Obtenemos el bloque, etiqueta, linea y palabra de la direccion de memoria leida.
        datosDireccionDeMemoria(valorAccesoDeMemoria, &direccion, &bloque, &etiqueta, &linea, &palabra);

        //Realizamos la comprobacion de la coincidecia de las etiquetas hasta que obtengamos el acierto.
        while (acierto == 0) {
            if (cache[linea].ETQ == etiqueta) {
                //Si las etiquetas coinciden llamamos a la funcion de acierto.
                aciertoCache(cache, direccion, bloque, etiqueta, linea, palabra, &tiempoglobal, texto, numaccesos);
                acierto = 1;
            }
            else {
                //Si las etiquetas no coinciden llamamos a la función de fallo.
                falloCache(RAMOrdenada, cache, direccion, bloque, etiqueta, linea, palabra, &tiempoglobal, &numfallos);
            }
            sleep(2);
        }
    }

    //Finalmente, imprimimos el tiempo global, el numero de accesos, el numero de fallos, el tiempo medio de acceso y el texto.
    printf("------------------------------------------------------------------------\n");
    printf("Tiempo Global: %.0f\nNumero de Accesos: %d\nNumero de Fallos: %d\nTiempo Medio de Acceso: %.2f\nTexto: ", tiempoglobal, numaccesos, numfallos, tiempoglobal / numaccesos);
    for (i = 0; i < numaccesos; i++) {
        printf("%c", texto[i]);
    }
    printf("\n------------------------------------------------------------------------\n");
	return 0; 
}

//Función para leer el fichero de la RAM.
void leerFicheroRAM(unsigned char RAM[]) {
	FILE* fichero;

    //Comprobamos si existe el fichero de la RAM y, si existe, lo abrimos y si no alertamos al usuario y salimos del programa.
	if ((fichero = fopen("RAM.bin", "rb")) == NULL) {
		printf("No existe el fichero de la RAM\n");
		exit(-1);
	}

    //Leemos todo el fichero y lo almacenamos en nuestro array RAM.
	fread(RAM, TAMANORAM, 1, fichero);

	fclose(fichero);
	return;
}

//Función para dividir el array de RAM en los bloques correspondientes.
void dividirRAM(unsigned char RAM[], unsigned char RAMOrdenada[][DATOSCACHE]) {
    int i, j;

    //Dividimos el array RAM segun el numero de bloques correspondientes a la cantidad de datos de la cache y los vamos almacenando en el nuevo array RAMOrdenada.
    for (i = 0; i < (TAMANORAM / DATOSCACHE); i++) {
        for (j = 0; j < DATOSCACHE; j++) {
            RAMOrdenada[i][j] = RAM[(i * DATOSCACHE) + j];
        }
    }
}

//Función para extraer el bloque, etiqueta, linea y palabra de la direccion de memoria.
void datosDireccionDeMemoria(char direccionHex[], short int *direccion, short int *bloque, short int *etiqueta, short int* linea, short int *palabra){
	char direccionBinario[BITETIQUETA+BITLINEA+BITPALABRA+1];
    char valorBloque[BITETIQUETA+BITLINEA+1], valorEtiqueta[BITETIQUETA+1], valorLinea[BITLINEA+1], valorPalabra[BITPALABRA+1];
    int i;

    //Primero traducimos la dirección de memoria de hexadecimal a binario.
    traduccionBinario(direccionHex, direccionBinario);

    //Copiamos los bits correspondientes de la direccion de memoria en binario en el array de etiqueta.
    for (i = 0; i < BITETIQUETA; i++) {
        valorEtiqueta[i] = direccionBinario[i];
    }
    valorEtiqueta[BITETIQUETA] = '\0';

    //Copiamos los bits correspondientes de la direccion de memoria en binario en el array de linea.
    for (i = 0; i < BITLINEA; i++) {
        valorLinea[i] = direccionBinario[i + BITETIQUETA];
    }
    valorLinea[BITLINEA] = '\0';

    //Copiamos los bits correspondientes de la direccion de memoria en binario en el array de palabra.
    for (i = 0; i < BITPALABRA; i++) {
        valorPalabra[i] = direccionBinario[i + (BITETIQUETA + BITLINEA)];
    }
    valorPalabra[BITPALABRA] = '\0';
    
    //Obtenemos el bloque concatenando la etiqueta y la linea.
    strcpy(valorBloque, valorEtiqueta);
    strcat(valorBloque, valorLinea);

    //Transformamos los strings de direccion, bloque, etiqueta, linea y palabra a short int.
    *direccion = (short int)strtol(direccionHex, NULL, 16);
    *bloque = (short int)strtol(valorBloque, NULL, 2);
    *etiqueta = (short int)strtol(valorEtiqueta, NULL, 2);
    *linea = (short int)strtol(valorLinea, NULL, 2);
    *palabra = (short int)strtol(valorPalabra, NULL, 2);
}

//Funcion para traducir la direccion de memoria de hexadecimal a binario.
void traduccionBinario(char direccionHex[], char direccionBinario[]) {
    char valorBinario[5];
    int i;

    //Transformamos de 1 en 1 los bytes de la dirección hexadecimal a binario.
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
        //Ya que el bus de memoria es de 10 bits, inicialmente solo copiamos los 2 primeros bits del primer byte.
        if (i == 1) {
            direccionBinario[0] = valorBinario[2];
            direccionBinario[1] = valorBinario[3];
            direccionBinario[2] = '\0';
        }
        //El resto de bytes los concatenamos completamente.
        else {
            strcat(direccionBinario, valorBinario);
        }
    }
}


//Funcion para manejar un acierto de la cache.
void aciertoCache(T_LINEA_CACHE cache[], short int direccion, short int bloque, short int etiqueta, short int linea, short int palabra, float* tiempo, char texto[], int numaccesos) {
    //Imprimimos las datos del acierto.
    printf("T: %.0f, Acierto de CACHE, ADDR %04X ETQ %X linea %02X palabra %02X DATO %02X\n", *tiempo, direccion, etiqueta, linea, palabra, cache[linea].Datos[7-palabra]);
    //Imprimimos todos los elementos de la cache.
    imprimirCache(cache);
    //Guardamos el dato en la variable texto.
    texto[numaccesos - 1] = cache[linea].Datos[7 - palabra];
    *tiempo += 1;
}

//Funcion para manejar un fallo de la cache.
void falloCache(unsigned char RAM[][DATOSCACHE], T_LINEA_CACHE cache[], short int direccion, short int bloque, short int etiqueta, short int linea, short int palabra, float* tiempo, int* numfallos) {
    int i;

    *numfallos += 1;

    //Imprimimos las datos del fallo.
    printf("T: %.0f, Fallo de CACHE %d, ADDR %04X ETQ %X linea %02X palabra %02X bloque %02X\n", *tiempo, *numfallos, direccion, etiqueta, linea, palabra, bloque);
    printf("Cargando el bloque %02X en la linea %02X\n", bloque, linea);

    *tiempo += 10;

    //Copiamos la etiqueta y los datos del bloque en la cache.
    cache[linea].ETQ = etiqueta;
    for (i = 0; i < DATOSCACHE; i++) {
        cache[linea].Datos[i] = RAM[bloque][7 - i];
    }
}

//Funcion para imprimir todos los datos de la cache.
void imprimirCache(T_LINEA_CACHE cache[]) {
    int i, j;

    for (i = 0; i < LINEASCACHE; i++) {
        printf("ETQ: %X\tDatos ", cache[i].ETQ);
        for (j = 0; j < DATOSCACHE; j++) {
            printf("%02X ", cache[i].Datos[j]);
        }
        printf("\n");
    }
    printf("\n");
}
