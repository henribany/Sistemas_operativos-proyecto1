#include <stdio.h>
#include <stdlib.h>

void acceso_mem(unsigned int* line);
void proceso_CACHEsym();
void  fijar(unsigned int num_bloque);
void imprimir();
void imprimirTexto(unsigned char* texto);

typedef struct {
	short int ETQ;
	short int Datos[8];
} T_LINEA_CACHE;


T_LINEA_CACHE cache[4];			// array 4 elementos 

int contaccesos=0;
unsigned char texto[100];
unsigned char RAM[1024];
	
void proceso_CACHEsym() {
	int tiempoglobal = 0;
	int numfallos = 0;
	unsigned int a = 0xFF;
	unsigned int b = 0x00;
	int j, i,y,x,k,f;	
	f=0;
	for (i = 0; i < 4; i++) {			// poner etq a ff y datos a 0 
		fflush(stdin);
		cache[i].ETQ = a;
		for (j = 0; j < 8; j++) {
			cache[i].Datos[j] = b;
		}
	}

	FILE* fpointer1;			// ram
	fpointer1 = fopen("RAM.bin","rb");						// se lee y printea acceso a ram (cambiar dinamico)
	
	if (fpointer1 == NULL) {
		printf("No se ha podido cargar el fichero\n");
		exit(-1);
	}

	fread(RAM, sizeof(RAM), 1, fpointer1);		// guardo ram bin el array 1024

	

	unsigned int cache_acceso_mem[11];			// todas direcciones de acceso a mem 
	acceso_mem(cache_acceso_mem);
			
	for (x=0; x<12; x++){
		unsigned int linea = cache_acceso_mem[x] & 0b0000011000 >> 3;				// asignamos el numero de linea 
		unsigned int bloque = cache_acceso_mem[x] &0b1111111000 >> 3;
		unsigned int etq = cache_acceso_mem[x] & 0b1111100000 >> 5;
		unsigned int palabra = cache_acceso_mem[x] &0b0000000111;
		unsigned int palabraRAM= 0x000F;
		if (cache[linea].ETQ != etq) {			// ff = 10
			numfallos ++;
			printf("T: %d, Fallo de CACHE %d, ADDR %04X ETQ %X linea %02X palabra %02X bloque %02X", tiempoglobal, numfallos ,cache_acceso_mem[x], etq, linea, palabra, bloque);
			tiempoglobal = tiempoglobal + 10;
		    cache[linea].ETQ = etq;					// ff = 10 
			fijar(bloque);
		    printf("\nCargando el bloque %d, en la linea %d \n", bloque,linea);
		} 
		else {
			printf("T: %d, Acierto de CACHE, ADDR %04X ETQ %X linea %02X palabra %02X DATO %02X \n", tiempoglobal, cache_acceso_mem[x], etq, linea, palabra, RAM[palabra]);	 
			tiempoglobal = tiempoglobal + 1;	
			for (k=0;k < 8; k++){
				texto[f] = cache[linea].Datos[k];  
				f++;
			}
		} 
		contaccesos++;
	}
 	imprimir();
 	printf("\nNumero accesos: %d\n", contaccesos);
 	printf("Numero fallos: %d\n", numfallos);
 	float tm= (float)(tiempoglobal) / (float)(contaccesos);
 	printf("Tiempo medio de acceso: %f\n", tm);
 	printf("Datos leidos  : ");
 	imprimirTexto(texto);
}


void  fijar(unsigned int num_bloque) {
	unsigned int linea= num_bloque & 0b0000000011;
	int j;
	for (j = 0; j < 8; j++) {
		int x=(num_bloque*8)+j-1;
		cache[linea].Datos[j] = RAM[x];       // datos array x eso incrementa
	}
}

void imprimir() {			// HACER FUNCION 	printf("Texto leido de la cache: \n");
	int x,w;
	int j=0;
	printf("\n");
	for ( x = 0; x < 4; x++) {
		printf("ETQ:%X Datos ", cache[x].ETQ);
		for(w=0; w<7; w++){
			printf("%02X ", cache[x].Datos[w]);			// hacer 1 a 1 datoos para evitar el salto de linea 
		}
		printf("\n");
	} 
}

void imprimirTexto(unsigned char* texto){
	int i;
	for ( i = 0; i < 32; i++){
		printf("%c", texto[i]);
	}
}


void acceso_mem(unsigned int* line) {
	int w;
	FILE* fpointer2;			// mem 
	fpointer2 = fopen("ACCESOS_MEMORIA.txt", "r");				// se lee y printea acceso a mem (cambiar dinamico)
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
