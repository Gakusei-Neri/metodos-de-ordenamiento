# metodos-de-ordenamiento
# Librería de Métodos de Ordenamiento
### creado por: Daniel Neri Ramírez

La librería metodosOrdenamiento.h contiene los siguentes métodos:

* Burbuja
* selección
* Inserción
* QuickSort
* ShellSort
* Radix 

Para agregar la librería a tu proyecto solo debes agregarla como:

```c
#include "lib/metodosOrdenamiento.h"
````

el archivo test.c muestra el funcionamiento de la librería de metosdos de ordenamiento y usa hilos para desplegar todos los métodos al mismo tiempo mediante theads.

para compilar no debes compilar tambien la libreria como se muestra:

```bash
gcc test.c lib/metodosOrdenamiento.c -o test
````
el siguente es el código de test:

```c
#include "lib/metodosOrdenamiento.h"
#include <pthread.h>
#include <stdatomic.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define ROJO   "\033[1;31m"
#define RESET  "\033[0m"
#define NUM_METODOS 6
#define ANCHO_BARRA 30

/* ───────────── Estructura de cada tarea (un hilo por metodo) ───────────── */
typedef struct Tarea {
    const char *nombre;
    void (*ordenar)(struct Tarea *t);
    int *arr;
    int n;
    _Atomic int progreso;     /* 0 .. 1000 (por mil) */
    _Atomic int terminado;
    double inicio, fin;
} Tarea;

static double ahora(void){
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

static void set_progreso(Tarea *t, double p){
    if (p > 1.0) p = 1.0;
    atomic_store(&t->progreso, (int)(p * 1000));
}

/* ───────────── Metodos de ordenamiento con reporte de progreso ───────────── */
static void burbuja(Tarea *t){
    int *a = t->arr, n = t->n;
    for (int i = 0; i < n; i++){
        int bandera = 0;
        for (int j = 0; j < n - 1 - i; j++){
            if (a[j] > a[j+1]){
                int aux = a[j]; a[j] = a[j+1]; a[j+1] = aux;
                bandera = 1;
            }
        }
        /* trabajo hecho ~ 1 - ((n-i)/n)^2 */
        double r = (double)(n - i - 1) / n;
        set_progreso(t, 1.0 - r * r);
        if (!bandera) break;
    }
}

static void seleccion(Tarea *t){
    int *a = t->arr, n = t->n;
    for (int i = 0; i < n - 1; i++){
        int menor = i;
        for (int j = i + 1; j < n; j++)
            if (a[j] < a[menor]) menor = j;
        int tmp = a[i]; a[i] = a[menor]; a[menor] = tmp;
        double r = (double)(n - i - 1) / n;
        set_progreso(t, 1.0 - r * r);
    }
}

static void insercion_p(Tarea *t){
    int *a = t->arr, n = t->n;
    for (int i = 1; i < n; i++){
        int clave = a[i], j = i - 1;
        while (j >= 0 && a[j] > clave){ a[j+1] = a[j]; j--; }
        a[j+1] = clave;
        double r = (double)(i + 1) / n;
        set_progreso(t, r * r);
    }
}

static _Thread_local int colocados;   /* elementos ya en su posicion final */

static void quick_rec(Tarea *t, int bajo, int alto){
    int *a = t->arr;
    if (bajo < alto){
        /* pivote central: se mueve al final y se parte igual que antes */
        int medio = bajo + (alto - bajo) / 2;
        int m = a[medio]; a[medio] = a[alto]; a[alto] = m;
        int pivote = a[alto], i = bajo - 1;
        for (int j = bajo; j < alto; j++){
            if (a[j] <= pivote){ i++; int x = a[i]; a[i] = a[j]; a[j] = x; }
        }
        int x = a[i+1]; a[i+1] = a[alto]; a[alto] = x;
        int pi = i + 1;
        colocados++;
        set_progreso(t, (double)colocados / t->n);
        quick_rec(t, bajo, pi - 1);
        quick_rec(t, pi + 1, alto);
    } else if (bajo == alto){
        colocados++;
    }
}

static void quicksort_p(Tarea *t){
    colocados = 0;
    quick_rec(t, 0, t->n - 1);
}

static void shellsort(Tarea *t){
    int *a = t->arr, n = t->n;
    int total_gaps = 0;
    for (int g = n / 2; g > 0; g /= 2) total_gaps++;
    int k = 0;
    for (int gap = n / 2; gap > 0; gap /= 2, k++){
        for (int i = gap; i < n; i++){
            int tmp = a[i], j;
            for (j = i; j >= gap && a[j-gap] > tmp; j -= gap) a[j] = a[j-gap];
            a[j] = tmp;
            if ((i & 1023) == 0)
                set_progreso(t, (k + (double)i / n) / total_gaps);
        }
        set_progreso(t, (double)(k + 1) / total_gaps);
    }
}

static void radix_p(Tarea *t){
    int *a = t->arr, n = t->n;
    int max = a[0];
    for (int i = 1; i < n; i++) if (a[i] > max) max = a[i];
    int digitos = 0;
    for (int m = max; m > 0; m /= 10) digitos++;
    if (digitos == 0) digitos = 1;

    int *salida = malloc(n * sizeof(int));
    int d = 0;
    for (int exp = 1; max / exp > 0; exp *= 10, d++){
        int cuenta[10] = {0};
        for (int i = 0; i < n; i++) cuenta[(a[i] / exp) % 10]++;
        for (int i = 1; i < 10; i++) cuenta[i] += cuenta[i-1];
        for (int i = n - 1; i >= 0; i--){
            salida[--cuenta[(a[i] / exp) % 10]] = a[i];
            if ((i & 1023) == 0)
                set_progreso(t, (d + (double)(n - i) / n) / digitos);
        }
        memcpy(a, salida, n * sizeof(int));
        set_progreso(t, (double)(d + 1) / digitos);
    }
    free(salida);
}

static void *ejecutar(void *arg){
    Tarea *t = arg;
    t->inicio = ahora();
    t->ordenar(t);
    t->fin = ahora();
    set_progreso(t, 1.0);
    atomic_store(&t->terminado, 1);
    return NULL;
}

/* ───────────── Dibujo ───────────── */
static void encabezado(void){
    system("clear");
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║               METODOS DE ORDENAMIENTO                      ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");
}

static void banner(void){
    printf(ROJO);
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║  ##  ###  ###  #### #  #  ##  #   # # #### #  # ####  ##   ║\n");
    printf("║ #  # #  # #  # #    ## # #  # ## ## # #    ## #   #  #  #  ║\n");
    printf("║ #  # ###  #  # ###  # ## #### # # # # ###  # ##   #  #  #  ║\n");
    printf("║ #  # # #  #  # #    #  # #  # #   # # #    #  #   #  #  #  ║\n");
    printf("║  ##  #  # ###  #### #  # #  # #   # # #### #  #   #   ##   ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");
    printf(RESET);
}

/* Cada fila mide exactamente 60 columnas entre los bordes ║ */
static void dibujar_barras(Tarea *tareas, int redibujar){
    if (redibujar) printf("\033[%dA", NUM_METODOS + 2);   /* sube el cursor */
    printf("╔════════════════════════════════════════════════════════════╗\n");
    for (int k = 0; k < NUM_METODOS; k++){
        Tarea *t = &tareas[k];
        int p = atomic_load(&t->progreso);
        int llenos = p * ANCHO_BARRA / 1000;
        double seg = atomic_load(&t->terminado) ? t->fin - t->inicio
                                                : ahora() - t->inicio;

        printf("║  %-10s " ROJO "[", t->nombre);
        for (int i = 0; i < ANCHO_BARRA; i++) printf(i < llenos ? "█" : "░");
        printf("]" RESET " %5.1f%% %6.2fs║\n", p / 10.0, seg);
    }
    printf("╚════════════════════════════════════════════════════════════╝\n");
    fflush(stdout);
}

static int todos_terminaron(Tarea *tareas){
    for (int k = 0; k < NUM_METODOS; k++)
        if (!atomic_load(&tareas[k].terminado)) return 0;
    return 1;
}

/* ───────────── Programa principal ───────────── */
int main(void){
    int n = 0;
    encabezado();
    printf("\n  Cuantos numeros deseas generar? ");
    if (scanf("%d", &n) != 1 || n <= 0){
        printf("  Cantidad invalida.\n");
        return 1;
    }

    /* Arreglo base con numeros aleatorios */
    int *base = malloc(n * sizeof(int));
    srand(time(NULL));
    for (int i = 0; i < n; i++) base[i] = 1 + rand() % 10000;

    Tarea tareas[NUM_METODOS] = {
        { .nombre = "Burbuja",   .ordenar = burbuja     },
        { .nombre = "Seleccion", .ordenar = seleccion   },
        { .nombre = "Insercion", .ordenar = insercion_p },
        { .nombre = "Quicksort", .ordenar = quicksort_p },
        { .nombre = "ShellSort", .ordenar = shellsort   },
        { .nombre = "RadixSort", .ordenar = radix_p     },
    };

    /* Cada metodo ordena su propia copia del mismo arreglo */
    pthread_t hilos[NUM_METODOS];
    double t0 = ahora();
    for (int k = 0; k < NUM_METODOS; k++){
        tareas[k].n = n;
        tareas[k].arr = malloc(n * sizeof(int));
        memcpy(tareas[k].arr, base, n * sizeof(int));
        tareas[k].inicio = t0;
        atomic_init(&tareas[k].progreso, 0);
        atomic_init(&tareas[k].terminado, 0);
    }
    for (int k = 0; k < NUM_METODOS; k++)
        pthread_create(&hilos[k], NULL, ejecutar, &tareas[k]);

    encabezado();
    printf("\033[?25l");                      /* oculta el cursor */
    dibujar_barras(tareas, 0);
    while (!todos_terminaron(tareas)){
        usleep(100000);                       /* 10 cuadros por segundo */
        dibujar_barras(tareas, 1);
    }
    dibujar_barras(tareas, 1);
    printf("\033[?25h");                      /* muestra el cursor */

    for (int k = 0; k < NUM_METODOS; k++){
        pthread_join(hilos[k], NULL);
        free(tareas[k].arr);
    }
    free(base);

    banner();
    return 0;
}
````


