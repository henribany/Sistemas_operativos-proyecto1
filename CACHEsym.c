#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#define RAMSZ 1024

void acceso_mem(unsigned int* line);
void proceso_CACHEsym();
void  fijar(unsigned int num_bloque,unsigned int linea);
void imprimir();
void imprimirTexto(unsigned char* texto);

typedef struct {
	short int ETQ;
	short int Datos[8];
} T_LINEA_CACHE;

T_LINEA_CACHE cache[4];			// Cache 4  elementos 
int contaccesos = 0;
unsigned char texto[100];
unsigned char RAM[RAMSZ];

void proceso_CACHEsym() {		// Proceso principal de la cache
	int tiempoglobal = 0;
	int numfallos = 0;
	unsigned int a = 0xFF;
	unsigned int b = 0x00;
	int j, i, x, k;
	for (i = 0; i < 4; i++) {			// etq a ff y datos a 0 
		fflush(stdin);
		cache[i].ETQ = a;
		for (j = 0; j < 8; j++) {
			cache[i].Datos[j] = b;
		}
	}

	FILE* fpointer1;			// leemos fichero RAM.bin
	fpointer1 = fopen("RAM.bin", "rb");

	if (fpointer1 == NULL) {
		printf("No se ha podido cargar el fichero\n");
		exit(-1);
	}

	fread(RAM, sizeof(RAM), 1, fpointer1);		// Guardamos el fichero en el array ram 1024

	unsigned int cache_acceso_mem[11];			// Array de las direcciones del fichero ACCESOS_MEMORIA.txt
	acceso_mem(cache_acceso_mem);
			int f=0;
	for (x = 0; x < 12 ;x++) {
		unsigned int linea = (cache_acceso_mem[x] & 0b0000011000)/ pow(2,3);
		unsigned int bloque = (cache_acceso_mem[x] & 0b1111111000) / pow(2, 3);
		unsigned int etq = (cache_acceso_mem[x] & 0b1111100000) / pow(2, 5);;
		unsigned int palabra = cache_acceso_mem[x] & 0b0000000111;
		unsigned int palabrahex = (cache_acceso_mem[x] & 0x000F);
		if (cache[linea].ETQ != etq) {			//  Comparador etiquetas  
			numfallos++;
			printf("T: %d, Fallo de CACHE %d, ADDR %04X ETQ %X linea %02X palabra %02X bloque %02X", tiempoglobal, numfallos, cache_acceso_mem[x], etq, linea, palabra, bloque);
			tiempoglobal = tiempoglobal + 10;
			cache[linea].ETQ = etq;
			fijar(bloque,linea);
			printf("\nCargando el bloque %02X, en la linea %02X \n", bloque, linea);
			
		}
		else {
			printf("T: %d, Acierto de CACHE, ADDR %04X ETQ %X linea %02X palabra %02X DATO %02X \n", tiempoglobal, cache_acceso_mem[x], etq, linea, palabra, RAM[palabrahex]);
			tiempoglobal = tiempoglobal + 1;
			for (k = 0;k<8; k++) {
				texto[f] = cache[linea].Datos[k];
				f++;
			}
		}
	sleep(2);
	contaccesos++;
	}
	imprimir();
	printf("\nNumero accesos: %d\n", contaccesos);
	printf("Numero fallos: %d\n", numfallos);
	float tm = (float)(tiempoglobal) / (float)(contaccesos);
	printf("Tiempo medio de acceso: %f\n", tm);
	printf("Datos leidos  : ");
	imprimirTexto(texto);
}


void  fijar(unsigned int num_bloque, unsigned int linea) {					// Fijar los datos en cache
	int j;
	for (j = 0; j < 8; j++) {
		int x = (num_bloque * 8) + j ;
		cache[linea].Datos[j] = RAM[x];       // Buscar datos en RAM
	}
}

void imprimir() {			// Imprimir los datos y la etq de la cache
	int x, w;
	printf("\n");
	for (x = 0; x < 4; x++) {
		printf("ETQ:%02X Datos ", cache[x].ETQ);
		for (w = 7; w >=0; w--) {
			printf("%02X ", cache[x].Datos[w]);
		}
		printf("\n");
	}
}

void imprimirTexto(unsigned char* texto) {			// Imprimir Datos acierto
	int i;
	for (i = 0; i < 32; i++) {
		printf("%c", texto[i]);
	}
}


void acceso_mem(unsigned int* line) {		// Lectura fichero de las direcciones a memoria
	int w;
	FILE* fpointer2;
	fpointer2 = fopen("accesos_memoria.txt", "r");
	if (fpointer2 == NULL) {
		printf("No se ha podido cargar el fichero\n");
		exit(-1);
	}
	for (w = 0; w < 12; w++) {
		fscanf(fpointer2, "%X", &line[w]);
	}

	if (fclose(fpointer2) != 0) {
		printf("Problemas al cerrar el fichero\n");
	}
}


int main(int argc, char* argv[]) {
	proceso_CACHEsym();
	return 0;
}

