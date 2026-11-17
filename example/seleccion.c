#include<stdio.h>
#include<stdlib.h>
#include <time.h>
#include <pthread.h>
#include <stdatomic.h>

#define ANCHO_BARRA 40
#define LLENO "▓"                        // caracter 178 de ASCII extendido (CP437), en UTF-8
#define VACIO "░"                        // caracter 176 de ASCII extendido (CP437), en UTF-8
#define REFRESCO_NS 50000000L            // redibuja las barras cada 50 ms

// Datos que comparte cada hilo con el hilo principal
typedef struct {
    const char *nombre;
    int *arr;
    int n;
    int* (*ordenar)(int*, int, atomic_llong*);
    long long total;                     // comparaciones que hara el algoritmo en total
    atomic_llong hecho;                  // comparaciones realizadas hasta ahora
    double segundos;
} Tarea;

int* generate_random_array(int n){
    int *arr = (int*)malloc(n * sizeof(int));
    if(arr == NULL){
        return NULL;
    }
    for(int i=0; i<n; i++){
        arr[i] = rand() % 10000;
    }
    return arr;
}

int* burbuja(int *arr, int n, atomic_llong *hecho){
int temp;
for(int j=0;j<n;j++){
    for(int i=0; i<n-1; i++){
        if(arr[i]>arr[i+1]){
            temp= arr[i+1];
            arr[i+1]=arr[i];
            arr[i]=temp;
        }
    }
    atomic_fetch_add(hecho, n - 1);
}
return arr;
}


int* Seleccion(int arr[], int n, atomic_llong *hecho){
    for (int i = 0; i < n - 1; i++) {
        int min = i;                     // suponemos que el mínimo está en i

        for (int j = i + 1; j < n; j++) {
            if (arr[j] < arr[min]) {
                min = j;                 // guardamos la posición del menor
            }
        }

        if (min != i) {                  // intercambiamos solo si hace falta
            int temp = arr[i];
            arr[i] = arr[min];
            arr[min] = temp;
        }
        atomic_fetch_add(hecho, n - 1 - i);
    }

    return arr;
}

// Funcion que ejecuta cada hilo: ordena y mide su propio tiempo de CPU.
// clock() ya no sirve aqui porque suma el tiempo de todos los hilos del proceso.
void* ejecutar_tarea(void *arg){
    Tarea *t = (Tarea*)arg;
    struct timespec inicio, fin;

    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &inicio);
    t->ordenar(t->arr, t->n, &t->hecho);
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &fin);

    t->segundos = (double)(fin.tv_sec - inicio.tv_sec) + (double)(fin.tv_nsec - inicio.tv_nsec) / 1e9;
    return NULL;
}

void dibujar_barra(Tarea *t){
    double avance = t->total > 0 ? (double)atomic_load(&t->hecho) / (double)t->total : 1.0;
    int llenos = (int)(avance * ANCHO_BARRA);

    printf("%-10s[", t->nombre);
    for(int i=0; i<ANCHO_BARRA; i++){
        fputs(i < llenos ? LLENO : VACIO, stdout);
    }
    printf("]%3d%%\n", (int)(avance * 100));
}

void dibujar_barras(Tarea *a, Tarea *b){
    dibujar_barra(a);
    dibujar_barra(b);
    fflush(stdout);
}

//main function
int main(){

    int numero;
    srand((unsigned int)time(NULL));

    printf("Dame el numero de elementos del arreglo: ");
    if(scanf("%d", &numero) != 1 || numero <= 0){
        fprintf(stderr, "Error: debes escribir un numero entero mayor que 0.\n");
        return 1;
    }

    printf("Generando arreglo aleatorio de %d elementos para Burbuja y Seleccion...\n", numero);
    int* arrBurbuja = generate_random_array(numero);
    int* arrSeleccion = generate_random_array(numero);
    if(arrBurbuja == NULL){
        fprintf(stderr, "Error: no se pudo reservar memoria para %d elementos.\n", numero);
        return 1;
    }
    if(arrSeleccion == NULL){
        fprintf(stderr, "Error: no se pudo reservar memoria para %d elementos.\n", numero);
        return 1;
    }

    /*printf("Arreglo generado para burbuja: ");
    for(int i=0; i<numero; i++){
        printf("%d ", arrBurbuja[i]);
    }*/
    printf("\n");

    long long n = numero;
    Tarea tareaBurbuja = { .nombre = "Burbuja", .arr = arrBurbuja, .n = numero,
                           .ordenar = burbuja, .total = n * (n - 1) };
    Tarea tareaSeleccion = { .nombre = "Seleccion", .arr = arrSeleccion, .n = numero,
                             .ordenar = Seleccion, .total = n * (n - 1) / 2 };
    atomic_init(&tareaBurbuja.hecho, 0);
    atomic_init(&tareaSeleccion.hecho, 0);

    //inicia
    pthread_t hiloBurbuja, hiloSeleccion;
    if(pthread_create(&hiloBurbuja, NULL, ejecutar_tarea, &tareaBurbuja) != 0){
        fprintf(stderr, "Error: no se pudo crear el hilo de Burbuja.\n");
        return 1;
    }
    if(pthread_create(&hiloSeleccion, NULL, ejecutar_tarea, &tareaSeleccion) != 0){
        fprintf(stderr, "Error: no se pudo crear el hilo de Seleccion.\n");
        pthread_join(hiloBurbuja, NULL);
        return 1;
    }

    // El hilo principal solo dibuja las barras mientras los otros dos ordenan
    struct timespec espera = { 0, REFRESCO_NS };
    dibujar_barras(&tareaBurbuja, &tareaSeleccion);
    while(atomic_load(&tareaBurbuja.hecho) < tareaBurbuja.total ||
          atomic_load(&tareaSeleccion.hecho) < tareaSeleccion.total){
        nanosleep(&espera, NULL);
        printf("\033[2A");               // sube el cursor 2 lineas para redibujar encima
        dibujar_barras(&tareaBurbuja, &tareaSeleccion);
    }

    pthread_join(hiloBurbuja, NULL);
    pthread_join(hiloSeleccion, NULL);
    //finaliza

    /*printf("Arreglo ordenado: ");
    for(int i=0; i<numero; i++){
        printf("%d ", arr_ordenado[i]);
    }*/
    printf("\n");
    printf("Tiempo de ordenamiento por burbuja:   %.6f segundos\n", tareaBurbuja.segundos);
    printf("Tiempo de ordenamiento por seleccion: %.6f segundos\n", tareaSeleccion.segundos);

    free(arrBurbuja);
    free(arrSeleccion);
    return 0;
}
