Tema 1 - APD : Antialiasing sii micro renderer
	Nume, prenume: Calin Catalina
	Grupa, seria: 334CA
	Data : 28 octombrie 2018


Mod de lucru:


**** Prima parte : Super Sampling Anti Aliasing

	In aceasta parte a temei am folosit structura image, declarata in homework.h,
care stocheaza informatiile importante ale unui fisier de tip imagine pnm/pgm, cum ar
fi membrii format(P5/P6), width, height, maxvalue si cate 2 matrixi, gray_levels(tipul
unsigned char) si rgb_levels(tip rgb), pe care le aloc si le folosesc in dependenta de 
tipul imaginii curente cu care lucrezi.
	In functia readInput am salvat toate informatiile din fisierul cu numele
fileName in structura de tip image cu denumirea img. In cazul imaginii alb-negru, 
salvez valorile pixelilor in membrul matrice gray_levels a imaginii, in cazul
imaginii color, salvez valorile pixelilor in membrul matrice rgb_levels a imaginii.
	In functia resize, efectuez tot algoritmul necesar antialiasingului. Folosesc
o structura temp, retinuta global, pentru a fi vazuta de threadFunction. In ea copiez
datele din imaginea initiala, care trebuie micsorata. Threadurile, in numar de 
num_threads, efectueaza algoritmul din functia threadFunction, care imparte imaginea
in mai multe parti, ca fiecare thread sa prelucreze de la linia start pana la linia
stop corespunzatoare acestuia. Pentru a calcula media valorilor unui patrat de dimensiunea
resize_factor * resize_factor, am facut 4 functii, in dependenta de resize_factor par si
tipul imaginii, pe care le apelez in cadrul threadFunction, salvand media rezultata
aferenta unui patrat, in pixelul din stanga sus a acestuia din matricea de pixeli a imaginii
temp. Dupa care, salvez toti pixelii din stanga sus a fiecarui patrat din imaginea temp,
in matricea imaginii out, care este cea finala.
	In functia writeData copiez toti membrii din imaginea prelucrata si micsorata
in fisierul de output cu denumirea fileName, data ca parametru.
	Scalabilitatea algoritmului este observabila din urmatoarele output-uri, obtinute
la rularea in cadrul cluster-ului. Observam ca o data cu marirea numarului de threaduri,
functia resize efectueaza intr-un timp mai scurt executia, lucru ce putem sa-l urmarim mai jos:



Grayscale image:

Resize_factor = 2, nr_threads = 1
./homework lenna_bw.pgm output.pgm 2 1
0.007812

Resize_factor = 2, nr_threads = 2
./homework lenna_bw.pgm output.pgm 2 2
0.005607

Resize_factor = 2, nr_threads = 4
./homework lenna_bw.pgm output.pgm 2 4
0.005195

Resize_factor = 2, nr_threads = 8
./homework lenna_bw.pgm output.pgm 2 8
0.004772

*****************************************
Grayscale image:

Resize_factor = 8, nr_threads = 1
./homework lenna_bw.pgm output.pgm 8 1
0.005698

Resize_factor = 8, nr_threads = 2
./homework lenna_bw.pgm output.pgm 8 2
0.004471

Resize_factor = 8, nr_threads = 4
./homework lenna_bw.pgm output.pgm 8 4
0.004130

Resize_factor = 8, nr_threads = 8
./homework lenna_bw.pgm output.pgm 8 8	
0.002880

*****************************************

Grayscale image:

Resize_factor = 3, nr_threads = 1
./homework lenna_bw.pgm output.pgm 3 1
0.005555

Resize_factor = 3, nr_threads = 2
./homework lenna_bw.pgm output.pgm 3 2
0.005317

Resize_factor = 3, nr_threads = 4
./homework lenna_bw.pgm output.pgm 3 4
0.004891

Resize_factor = 3, nr_threads = 8
./homework lenna_bw.pgm output.pgm 3 8	
0.004228

****************************************
Color image:

Resize_factor = 2, nr_threads = 1
./homework lenna_color.pnm output.pnm 2 1
0.012499

Resize_factor = 2, nr_threads = 2
./homework lenna_color.pnm output.pnm 2 2
0.008681

Resize_factor = 2, nr_threads = 4
./homework lenna_color.pnm output.pnm 2 4
0.006860


Resize_factor = 2, nr_threads = 8
./homework lenna_color.pnm output.pnm 2 8
0.006585

****************************************
Color image:

Resize_factor = 8, nr_threads = 1
./homework lenna_color.pnm output.pnm 8 1
0.008229

Resize_factor = 8, nr_threads = 2
./homework lenna_color.pnm output.pnm 8 2
0.005781

Resize_factor = 8, nr_threads = 4
./homework lenna_color.pnm output.pnm 8 4
0.004776

Resize_factor = 8, nr_threads = 8
./homework lenna_color.pnm output.pnm 8 8
0.004560


****************************************
Color image:

Resize_factor = 3, nr_threads = 1
./homework lenna_color.pnm output.pnm 3 1
0.017698


Resize_factor = 3, nr_threads = 2
./homework lenna_color.pnm output.pnm 3 2
0.010911


Resize_factor = 3, nr_threads = 4
./homework lenna_color.pnm output.pnm 3 4
0.007394


Resize_factor = 3, nr_threads = 8
./homework lenna_color.pnm output.pnm 3 8
0.005995



********Partea 2: Micro renderer
	
	
	In functia initialize, initializez zona de memorie pentru renderer, alocand memorie
pentru matricea de pixeli a imaginii, dimensiunea acesteia fiind resolution*resolution.
	In functia render, folosesc structura temp_gray_levels, in care voi stoca pixelii
prelucrati de fiecare thread. Fiecare thread foloseste threadFunction, pentru a colora
pixelii in alb sau negru. In acesta functie, am calculam linia de start si linia de finish
a fiecarui thread in care trebuie el sa lucreze si folosind functia is_black, care vede daca
centrul pixelului este la o distanta mai mica decat 3 fata de linia data de formula din tema, colorand
astfel pixelii in alb sau negru. Tot in functia render, copiez apoi continutul din temp_gray_levels
in imaginea im. 
	In functia writeData scriu datele pentru imaginea pgm si matricea de pixeli rezultata(din imagine img).

	Scalabilitatea algoritmului este observabila din urmatoarele output-uri, obtinute
la rularea in cadrul cluster-ului. Observam ca o data cu marirea numarului de threaduri,
functia render efectueaza intr-un timp mai scurt executia, lucru ce putem sa-l urmarim mai jos:



Render:

Resolution = 1000:

1 Thread: 0.054326
2 Thread-uri: 0.041201
4 Thread-uri: 0.021548
8 Thread-uri: 0.013699

Resolution = 8000:

1 Thread: 2.051055
2 Thread-uri: 1.222675
4 Thread-uri: 0.812453
8 Thread-uri: 0.621599


