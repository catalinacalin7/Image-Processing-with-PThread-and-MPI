#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "homework.h"


int num_threads;
int resize_factor;
image *temp;

// matricea kernelului gaussian pentru resize factor 3
int gaussian_kernel[3][3] = {
  {1, 2, 1},
  {2, 4, 2},
  {1, 2, 1}
};


void readInput(const char * fileName, image *img) {
  FILE *fp = fopen(fileName, "rb");

  // citim toate informatiile referitoare la imaginea primita
  // ca input si le stocam intr-o structura de timp img
  img->format[0] = fgetc(fp);
  img->format[1] = fgetc(fp);
  fscanf(fp, "%d", &img->width);
  fscanf(fp, "%d", &img->height);
  fscanf(fp, "%d", &img->maxval);
  // skip "/n" de dupa maxvalue
  fgetc(fp);


  // daca imaginea e grayscale (P5)
  if (img->format[1] == '5') {

    // alocam memorie matricei ce va stoca pixelii imaginii grayscale
    img->gray_levels = (unsigned char**)malloc(sizeof(unsigned char*) * img->height);

    for(int i = 0; i < img->height; i++) {
      img->gray_levels[i] = (unsigned char*)malloc(sizeof(unsigned char) * img->width);
      // citim cate o linie
      fread(img->gray_levels[i], sizeof(unsigned char) *  img->width,1, fp);
    }

  } else { // daca imaginea de input este color

    // alocam memorie matricei ce va stoca pixelii imaginii color (pixeli de tip rgb)
    img->rgb_levels = (rgb**)malloc(sizeof(rgb*) * img->height);

    for(int i = 0; i < img->height; i++) {
      img->rgb_levels[i] = (rgb*)malloc(sizeof(rgb) * img->width);
      // citim cate o linie
      fread(img->rgb_levels[i], sizeof(rgb), img->width, fp);
    }
  }
  // inchidere fisier de intrare
  fclose(fp);
}



void writeData(const char * fileName, image *img) {

  FILE *fp = fopen(fileName, "wb");
  
  // scriem in fisierul de output informatiile aferente imaginii rezultante
  fprintf(fp, "%c%c\n%d %d\n%d\n",img->format[0], img->format[1], img->width,
                                                      img->height, img->maxval);

  // daca imaginea e grayscale, atunci scriem in fisierul de iesire pixelii din
  // matricea gray_levels (corespunzatoare imaginilor bw)
  if (img->format[1] == '5') {

    for(int i = 0; i < img->height; i++) {
      fwrite(img->gray_levels[i], sizeof(unsigned char), img->width, fp);
    }
  } else {
    // daca imaginea e color, atunci scriem in fisierul de iesire pixelii din
    // matricea rgb_levels (corespunzatoare imaginilor color)
    for(int i = 0; i < img->height; i++) {
      fwrite(img->rgb_levels[i], sizeof(rgb), img->width, fp);
    }
  }

  // inchidem fisierul de iesire
  fclose(fp);
}

void resize(image *in, image *out) {

  // copiez intr-o structura temp, declarata global
  // informatiile din imagine de input
  temp = (image*)malloc(sizeof(image));
  temp->format[0] = in->format[0];
  temp->format[1] = in->format[1];
  temp->maxval = in->maxval;

  // scalam height si width ale lui temp, ca aceste 2 marimi sa 
  // se imparta fara rest la resize_factor
  temp->height = in->height - in->height % resize_factor;
  temp->width = in->width - in->width % resize_factor;

  // scriem in imaginea de output informatiile necesare
  // tipul si maxval raman la fel
  out->format[0] = in->format[0];
  out->format[1] = in->format[1];
  out->maxval = in->maxval;
  // height si width se micsoreaza de resize_factor ori
  out->height = in->height / resize_factor;
  out->width = in->width / resize_factor;


  // daca imaginea e bw, salvam pixelii in matricea gray_levels din structura
  if (in->format[1] == '5') {
    temp->gray_levels = (unsigned char**)malloc(sizeof(unsigned char*) * temp->height);

    for(int i = 0; i < temp->height; i++) {
      temp->gray_levels[i] = (unsigned char*)malloc(sizeof(unsigned char) * temp->width);

      // copiem pixelii din imaginea de input in temp
      for(int j = 0; j < temp->width; j++) {
        temp->gray_levels[i][j] = in->gray_levels[i][j];
      }
    }


  } else {
    // daca imaginea e color, salvam pixelii in matricea rgb_levels din structura
    temp->rgb_levels = (rgb**)malloc(sizeof(rgb*) * temp->height);

    for(int i = 0; i < temp->height; i++) {
      temp->rgb_levels[i] = (rgb*)malloc(sizeof(rgb) * temp->width);

      // copiem pixelii din imaginea de input in temp
      for(int j = 0; j < temp->width; j++) {
        temp->rgb_levels[i][j] = in->rgb_levels[i][j];
      }
    }
  }

  // declaram vectorul de threaduri si de id-uri ale threadurilor
  pthread_t tid[num_threads];
  int thread_id[num_threads];

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

  // daca imaginea de output e grayscale
  if (out->format[1] == '5') {
    // scriem pixelii obtinuti in matricea gray_levels aferenta imaginii out
    out->gray_levels = (unsigned char**)malloc(sizeof(unsigned char*) * out->height);
    for (int i = 0; i < out->height; i++){
        out->gray_levels[i] = (unsigned char*)malloc(sizeof(unsigned char) * out->width);
    }

    // parcurgem din resize_factor in resize_factor, pentru a scrie pixelii obtinuti
    // in urma calculelor mediilor in imaginea de output
    // pixelii rezultati vor fi mereu in partea stanga sus a patratelor
    // de dimensiunea resize_factor * resize_factor
    for(int i = 0; i < temp->height; i += resize_factor) {
        for(int j = 0; j < temp->width; j += resize_factor) {
          out->gray_levels[i / resize_factor][j / resize_factor] = temp->gray_levels[i][j];
          }
      }


    // dezalocam matricea graylevels
    for (int i = 0; i < temp->height; i++){
      free(temp->gray_levels[i]);
    }
    free(temp->gray_levels);

  } else {
    // daca imaginea de output e color
    // scriem pixelii obtinuti in matricea rgb_levels aferenta imaginii out
    out->rgb_levels = (rgb**)malloc(sizeof(rgb*) * out->height);

    for (int i = 0; i < out->height; i++){
        out->rgb_levels[i] = (rgb*)malloc(sizeof(rgb) * out->width);
    }

    for(int i = 0; i < temp->height; i += resize_factor) {
        for(int j = 0; j < temp->width; j += resize_factor) {
          out->rgb_levels[i / resize_factor][j / resize_factor] = temp->rgb_levels[i][j];
          }
      }
    
    // dezalocam matricea graylevels
    for (int i = 0; i < temp->height; i++){
      free(temp->rgb_levels[i]);
    }
    free(temp->gray_levels);
  }

  // dezalocam imaginea temp
  free(temp);
}


void* threadFunction(void *var) {
  // calculam id-ul threadului curent
  int thread_id = *(int*)var;

  // calculam de unde incepe si unde termina de prelucrat
  // informatiile threadul curent
  int start = temp->height * thread_id / num_threads;

  // start si stop trebuie sa se imparta exact la resize_factor
  // ca sa nu obtinem segfault
  start = start - start % resize_factor;
  int stop = temp->height * (thread_id + 1) / num_threads;
  stop = stop - stop % resize_factor;


  // daca ultimul thread are stopul mai mare ca inaltimea imaginii
  // nu parcurgem mai mult decat inaltimea
  if (stop > temp->height) {
    stop = temp->height;
  }

  // daca imaginea e bw
  if (temp->format[1] == '5') {

    // mergem pe liniile de la start pana la stop(din resize_factor in resize_factor)
    // (din patrat in patrat)
    for (int i = start; i < stop; i += resize_factor) {
      for (int j = 0; j < temp->width; j += resize_factor) {

        // daca resize_factorul este par
        if (resize_factor % 2 == 0) {
          // folosim algoritmul de calculare a mediei pentru bw,resize par
          temp->gray_levels[i][j] = get_media_even(i, j);
        } else {
          // altfel folosim algoritmul de calculare a mediei 
          // pentru bw,resize impar (adica 3)
          temp->gray_levels[i][j] = get_media_odd(i, j);
        }
      }
    }

  } else { // daca imaginea e color
    // mergem iarasi din patrat in patrat
    for (int i = start; i < stop; i += resize_factor) {
      for (int j = 0; j < temp->width; j += resize_factor) {

        // si apelam functiile in dependenta de resize par/impar
        // pentru imagini color
        if (resize_factor % 2 == 0) {
          temp->rgb_levels[i][j] = get_media_rgb_even(i, j);
        } else {
          temp->rgb_levels[i][j] = get_media_rgb_odd(i, j);
        }
      }
    }
  }
  return NULL;
}


// functie de calcularea mediei unui patrat cu pixelul din stanga
// sus in (x,y) (pentru imagine bw cu resize par)
int get_media_even(int x, int y) {
  int sum = 0;

  // calculam media pixelilor din patratul cu dimensiunea
  // resize_factor * resize_factor si o returnam
  for (int i = x; i < x + resize_factor; i++) {
    for (int j = y; j < y + resize_factor; j++) {
      sum += temp->gray_levels[i][j];
    }
  }
  return sum / (resize_factor * resize_factor);
}

// functie de calcularea mediei unui patrat cu pixelul din stanga
// sus in (x,y) (pentru imagine rgb cu resize par)
rgb get_media_rgb_even(int x, int y) {

  // calculam media pixelilor din patratul cu dimensiunea
  // resize_factor * resize_factor (de tip rgb) si o returnam
  rgb media;
  int sum_red = 0;
  int sum_green = 0;
  int sum_blue = 0;
  for(int i = x; i < x + resize_factor; i++) {
    for(int j = y; j < y + resize_factor; j++) {
      sum_red += temp->rgb_levels[i][j].red;
      sum_green += temp->rgb_levels[i][j].green;
      sum_blue += temp->rgb_levels[i][j].blue;
      }
  }
  media.red = sum_red / (resize_factor * resize_factor);
  media.green = sum_green /  (resize_factor * resize_factor);
  media.blue = sum_blue /  (resize_factor * resize_factor);
  return media;
}


// functie de calcularea mediei unui patrat cu pixelul din stanga
// sus in (x,y) (pentru imagine bw cu resize impar)
int get_media_odd(int x, int y) {
  int sum = 0;

  // facem inmultirea pixelilor din patrat cu valorile din matricea 
  // kernelului gaussian, le facem suma si impartim la 16
  for(int i = x; i < x + resize_factor; i++) {
    for(int j = y; j < y + resize_factor; j++) {
      // la gaussian_kernel mereu parcurgem de la 0...3 liniile si coloanele
      sum += temp->gray_levels[i][j] * gaussian_kernel[i % resize_factor][j % resize_factor];
    }
  }
  return sum / 16;
}

// functie de calcularea mediei unui patrat cu pixelul din stanga
// sus in (x,y) (pentru imagine color cu resize impar)
rgb get_media_rgb_odd(int x, int y) {
  rgb media;
  int sum_red = 0;
  int sum_green = 0;
  int sum_blue = 0;

  for(int i = x; i < x + resize_factor; i++) {
    for(int j = y; j < y + resize_factor; j++) {
      sum_red += temp->rgb_levels[i][j].red * gaussian_kernel[i % resize_factor][j % resize_factor];
      sum_green += temp->rgb_levels[i][j].green * gaussian_kernel[i % resize_factor][j % resize_factor];
      sum_blue += temp->rgb_levels[i][j].blue * gaussian_kernel[i % resize_factor][j % resize_factor];
    }
  }
  media.red = sum_red / 16;
  media.green = sum_green / 16;
  media.blue = sum_blue / 16;
  return media;
}
