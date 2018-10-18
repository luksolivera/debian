#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/time.h>

#define MAX_NUM_WORKERS 8
#define MAX_SIZE 2000
//int size; /*tamaño del vector*/
int numWorkers; /*numero de workers*/
int stripSize; /*tamaño del strip; size = numWorkers*stripSize*/


pthread_mutex_t lock;

char array[786432];
//char mensaje[2000];
void * worker (void * ptr);
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



/*-----------  Carga de bytes modificados-----------*/
unsigned char *EditImage(bmpInfoHeader *info, unsigned char *img,int *array[]){
unsigned char *img2;
int i,j=0;

img2=img;
for(i=0; i<info->imgsize; i=i+3){
	array[j]=img2[i];
	j++;
}

return img2;
}


bmpInfoHeader info;
unsigned char *img;
unsigned char *img2;
bmpFileHeader fileHeader;
pthread_t workers[MAX_NUM_WORKERS];

/*--------------  MAIN  -----------------------------*/
void main(int argc,char* argv[]){
/*	Comenzamos a medir el tiempo*/
  	struct timeval t_ini, t_fin;
  	double secs;
  	gettimeofday(&t_ini, NULL);
/*------------------------*/
/*----- Leemos la imagen y asignamos la array*/
	img=LoadBMP("paisaje.bmp", &info, &fileHeader);
	
	
   	int i, ids[MAX_NUM_WORKERS];
	pthread_attr_t attr;
	
	pthread_attr_init(&attr);
	pthread_mutex_init(&lock,NULL);
	/*Lee parámetros de la línea de comando*/
	numWorkers = atoi(argv[1]);
	numWorkers = (numWorkers < MAX_NUM_WORKERS ? numWorkers : MAX_NUM_WORKERS);
	stripSize = info.imgsize/ numWorkers;
	/* crea los hilos */
	for (i = 0; i < numWorkers; i++) {
		ids[i] = i;
		pthread_create(&workers[i], &attr, worker, &ids[i]);
	}
	/*Espera a que los hilos terminen*/
	for (i = 0; i < numWorkers; i++)
		pthread_join(workers[i], NULL);
	
	printf("%d \n",array);
	/*Detiene el contador de tiempo*/
	gettimeofday(&t_fin, NULL);
	secs = timeval_diff(&t_fin, &t_ini);
	/*-------------------------------*/
  	DisplayInfo(&info);

}


void * worker (void *arg) {
	int *ptr_id = (int*) arg;
	int id = *ptr_id;
	int local_sum, i, first, last;
	unsigned char mensaje[sizeof(array)];

	/*Determina la primera y ultima posición del bloque a procesar*/
	first = id*stripSize;
	last = first + stripSize - 1;
	printf("Primer bloque %d ,%d \n",first,id);
	printf("ultimo bloque %d ,%d \n",last,id);
	/*Suma los valores de su bloque*/
	int j= 0;
	for (i = first; i <= last; i=i+3){
		if((img[i] >= 65 && img[i] <= 90) || (img[i] >= 97 && img[i] <= 122) ||(img[i] >= 160 && img[i] <= 163)|| img[i]==32)
			mensaje[j]=img[i];
		
		j++;
	}
	if(id==0)
		for (i = 0; i <= sizeof(mensaje); i++)
			if(mensaje[i] >= 65 && mensaje[i] <= 90 || mensaje[i] >= 97 && mensaje[i] <= 122||mensaje[i] >= 160 && mensaje[i] <= 163|| mensaje[i]==32)
				printf("%c",mensaje[i]);
	else{
		pthread_join(workers[id-1],NULL);
		for (i = 0; i <= sizeof(mensaje); i++)
			if(mensaje[i] >= 65 && mensaje[i] <= 90 || mensaje[i] >= 97 && mensaje[i] <= 122||mensaje[i] >= 160 && mensaje[i] <= 163|| mensaje[i]==32)
				printf("%c",mensaje[i]);
	}
		
		
	/*pthread_mutex_lock(&lock);
	for (i = first; i <= last; i=i+3){
		array[i]=mensaje[j];
		j++;
	}
	pthread_mutex_unlock(&lock);*/
	pthread_exit (0);

}

/*función para medir el tiempo retorna "a -b" en segundos*/
double timeval_diff(struct timeval *a, struct timeval *b)
{
	return
	  (double)(a->tv_sec + (double)a->tv_usec/1000000) -
	  (double)(b->tv_sec + (double)b->tv_usec/1000000);
}


