#include <stdio.h>
#include <omp.h>
#include <time.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/time.h>

#define MAX_NUM_THREADS	16

//int size; /*tamaño del vector*/
int numWorkers; /*numero de workers*/
int stripSize; /*tamaño del strip; size = numWorkers*stripSize*/


pthread_mutex_t lock;

char array[786432];
//char mensaje[2000];
double timeval_diff(struct timeval *a, struct timeval *b);

typedef struct bmpFileHeader
{
  /* 2 bytes de identificación */
  uint32_t size;        /* Tamaño del archivo */
  uint16_t resv1;       /* Reservado */
  uint16_t resv2;       /* Reservado */
  uint32_t offset;      /* Offset hasta hasta los datos de imagen */
} bmpFileHeader;

typedef struct bmpInfoHeader
{
  uint32_t headersize;      /* Tamaño de la cabecera */
  uint32_t width;           /* Ancho */
  uint32_t height;          /* Alto */
  uint16_t planes;          /* Planos de color (Siempre 1) */
  uint16_t bpp;             /* bits por pixel */
  uint32_t compress;        /* compresión */
  uint32_t imgsize;         /* tamaño de los datos de imagen */
  uint32_t bpmx;            /* Resolución X en bits por metro */
  uint32_t bpmy;            /* Resolución Y en bits por metro */
  uint32_t colors;          /* colors used en la paleta */
  uint32_t imxtcolors;      /* Colores importantes. 0 si son todos */
} bmpInfoHeader;



/*----------- Lectura de la img-----------*/

unsigned char *LoadBMP(char *filename, bmpInfoHeader *bInfoHeader, bmpFileHeader *fileHeader)
{

  FILE *f;
  unsigned char *imgdata;   /* datos de imagen */
  uint16_t type;        /* 2 bytes identificativos */

  f=fopen (filename, "r");
  if (!f)
    return NULL;        /* Si no podemos leer, no hay imagen*/

  /* Leemos los dos primeros bytes */
  fread(&type, sizeof(uint16_t), 1, f);
  if (type !=0x4D42)        /* Comprobamos el formato */
    {
      fclose(f);
      return NULL;
    }

  /* Leemos la cabecera de fichero completa */

  fread(fileHeader, sizeof(bmpFileHeader), 1, f);

  /* Leemos la cabecera de información completa */
  fread(bInfoHeader, sizeof(bmpInfoHeader), 1, f);

  /* Reservamos memoria para la imagen, ¿cuánta? 
     Tanto como indique imgsize */
  imgdata=(unsigned char*)malloc(bInfoHeader->imgsize);

  /* Nos situamos en el sitio donde empiezan los datos de imagen,
   nos lo indica el offset de la cabecera de fichero*/
  fseek(f, fileHeader->offset, SEEK_SET);

  /* Leemos los datos de imagen, tantos bytes como imgsize */
  fread(imgdata, bInfoHeader->imgsize,1, f);

  /* Cerramos */
  fclose(f);

  /* Devolvemos la imagen */
  return imgdata;
}

/*----------- imprimir los datos de la img-----------*/

void DisplayInfo(bmpInfoHeader *info)
{
  printf("Tamaño de la cabecera: %u\n", info->headersize);
  printf("Anchura: %d\n", info->width);
  printf("Altura: %d\n", info->height);
  printf("Planos (1): %d\n", info->planes);
  printf("Bits por pixel: %d\n", info->bpp);
  printf("Compresión: %d\n", info->compress);
  printf("Tamaño de datos de imagen: %u\n", info->imgsize);
  printf("Resolucón horizontal: %u\n", info->bpmx);
  printf("Resolucón vertical: %u\n", info->bpmy);
  printf("Colores en paleta: %d\n", info->colors);
  printf("Colores importantes: %d\n", info->imxtcolors);
}


bmpInfoHeader info;
unsigned char *img;
unsigned char *img2;
bmpFileHeader fileHeader;
/*--------------  MAIN  -----------------------------*/
void main(int argc,char* argv[]){
/*	Comenzamos a medir el tiempo*/
  	struct timeval t_ini, t_fin;
  	double secs;
  	gettimeofday(&t_ini, NULL);
/*------------------------*/
/*----- Leemos la imagen y asignamos la array*/
	img=LoadBMP("paisaje.bmp", &info, &fileHeader);
	
	unsigned char mens;
   	int i, turno;
	/*Lee parámetros de la línea de comando*/
	numWorkers = atoi(argv[1]);
	numWorkers = (numWorkers < MAX_NUM_THREADS ? numWorkers : MAX_NUM_THREADS);
	stripSize = info.imgsize/ numWorkers;
	/* crea los hilos */
	i=0;
	#pragma omp parallel shared(i,img,array,stripSize,mens) num_threads(numWorkers)

{
	int id = i++;
	int j=0,k=0,first,last;
	unsigned char mensaje[sizeof(array)];
	/*Determina la primera y ultima posición del bloque a procesar*/
	first = id*stripSize;
	last = first + stripSize - 1;
	
	for (j = first; j <= last; j=j+3){
		if((img[j] >= 65 && img[j] <= 90) ||
		(img[j] >= 97 && img[j] <= 122) ||
		(img[j] >= 160 && img[j] <= 163)|| 
		img[j]==32)
		//if(img[i] >=48 && img[i] <=164)
		{
			mensaje[k]=img[j];
			k++;
		}
		
	}
		
		#pragma omp critical
		for (j = 0; j <= sizeof(mensaje); j++){
			if(mensaje[j] >= 65 && mensaje[j] <= 90 || 
			mensaje[j] >= 97 && mensaje[j] <= 122||
			mensaje[j] >= 160 && mensaje[j] <= 163||
			mensaje[j]==32)
				printf("%c",mensaje[j]);
		}
		
} 
	
	/*Detiene el contador de tiempo*/
	gettimeofday(&t_fin, NULL);
	secs = timeval_diff(&t_fin, &t_ini);
	/*-------------------------------*/
}


/*función para medir el tiempo retorna "a -b" en segundos*/
double timeval_diff(struct timeval *a, struct timeval *b)
{
	return
	  (double)(a->tv_sec + (double)a->tv_usec/1000000) -
	  (double)(b->tv_sec + (double)b->tv_usec/1000000);
}

