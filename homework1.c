#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include "homework1.h"

int num_threads;
int resolution;
unsigned char **temp_gray_levels;


void initialize(image *im) {
  // initializam zona de memorie pentru renderer, alocand-o
  // vom avea o imagine cu resolution * resolution pixeli
  im->gray_levels = (unsigned char**)malloc(sizeof(unsigned char*) * resolution);

  for(int i = 0; i < resolution; i++) {
    im->gray_levels[i] = (unsigned char*)malloc(sizeof(unsigned char) * resolution);
  }
}


void render(image *im) {

  // declaram vectorul de threaduri si de id-uri ale threadurilor
  pthread_t tid[num_threads];
  int thread_id[num_threads];

  // in temp_gray_levels, care e declarat global pentru a fi vazut ulterior si de
  // threadFunction, vom stoca pixelii calculati de fiecare thread
  // (este o matrice de resolution * resolution pixeli)
  temp_gray_levels = (unsigned char**)malloc(sizeof(unsigned char*) * resolution);

  for(int i = 0; i < resolution; i++) {
    temp_gray_levels[i] = (unsigned char*)malloc(sizeof(unsigned char) * resolution);
  }

  // asociem fiecarui thread cate un id
  for (int i = 0; i < num_threads; i++) {
    thread_id[i] = i;
  }

  // cream threadurile si folosim threadFunction
  for (int i = 0; i < num_threads; i++) {
    pthread_create(&(tid[i]), NULL, threadFunction, &(thread_id[i]));
  }

  for (int i = 0; i < num_threads; i++) {
    pthread_join(tid[i], NULL);
  }

  // copiem continutul din matricea temp_gray_levels in imaginea "*im"
  // data ca parametru
  for(int i = 0; i < resolution; i++) {
    for(int j = 0; j < resolution; j++) {
      im->gray_levels[i][j] = temp_gray_levels[i][j];
    }
  }

  // dezalocam imaginea temp_gray_levels
  for (int i = 0; i < resolution; i++){
    free(temp_gray_levels[i]);
  }

  free(temp_gray_levels);
}


void writeData(const char * fileName, image *img) {
  FILE *fp = fopen(fileName, "wb");

  // imaginea rezultanta este bw(P5), are width si height = resolution
  // si maxvalue este 255
  fprintf(fp, "%s\n%d %d\n%d\n","P5", resolution, resolution, 255);

  // copiez din imaginea rezultata in fisierul de iesire
  for (int i = 0; i < resolution; i++) {
    fwrite(img->gray_levels[i], sizeof(unsigned char), resolution, fp);
  }

  // inchid fisierul de iesire
  fclose(fp);
}

void* threadFunction(void *var) {
  // calculez id-ul threadului curent 
  int thread_id = *(int*)var;

  // fiecare thread va avea linia de start de la start
  // si linia unde se va opri de facut verificarile va fi stop
  int start = resolution * thread_id / num_threads;
  int stop = resolution * (thread_id + 1) / num_threads;

  // daca stop e mai mare ca resolution, threadul va prelucra
  // datele pana la linia resolution
  if (stop > resolution) {
    stop = resolution;
  }


  for (int i = start; i < stop; i++) {
    for (int j = 0; j < resolution; j++) {
      // verificam daca pixelul din imagine trebuie
      // sa fie alb sau negru
      // indicii sunt j si resolution-i-1 pentru ca avem (0,0)
      // in partea stanga jos si (max,max) in dreapta sus
      // iar coordonatele normale ale axei de coordonate
      // sunt (0,0) in stanga jos si (max, max) in dreapta sus
      if (is_black(j, resolution - i - 1)) {
        temp_gray_levels[i][j] = 0;
      } else {
        temp_gray_levels[i][j] = 255;
      }
    }
  }
  return NULL;
}

int is_black(double x, double y) {
  double d;
  // ne deplasam in centrul pixelului
  x += 0.5;
  y += 0.5;
  // scalam pozitia in centimetri
  x *= 100.0 / (double)resolution;
  y *= 100.0 / (double)resolution;
  // calculam distanta de la pixel la linie
  d = abs(-1 * x  + 2 * y) / sqrt(5);
  // daca e mai mica ca 3, pixelul trebuie sa fie negru
  if (d <= 3) return 1;
  // altfel trebuie sa fie alb
  return 0;
}
