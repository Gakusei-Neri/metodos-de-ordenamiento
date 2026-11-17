#include "metodosOrdenamiento.h"
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
//implementamos burbuja
int* Burbuja(int array[], int n, int *pasadas){
    int aux;
    int bandera=0;
    *pasadas=0;
    for(int i=0; i<n; i++){
        bandera=0;
        for(int j=0; j<n-1; j++){
         if(array[j]>array[j+1]){
                aux=array[j+1]; //paso 1
                array[j+1]=array[j]; //paso2
                array[j]=aux;  //paso3
                bandera=1;
         }
        }
        (*pasadas)++;
        if(bandera == 0){
            break;
        }
        
        
    } return array;
}

//Regresa la posicion del menor elemento entre array[inicio] y array[n-1]
int BuscarMenor(int array[], int n, int inicio){
    int pos_menor = inicio;
    for(int j = inicio + 1; j < n; j++){
        if(array[j] < array[pos_menor]){
            pos_menor = j;
        }
    }
    return pos_menor;
}

//implementamos seleccion
int* Seleccion(int array[], int n, int *pasadas){
    int temp,pos_menor;
    *pasadas=0;
    for(int i=0; i<n-1; i++){
        pos_menor=BuscarMenor(array,n,i);
        temp=array[i];
        array[i]=array[pos_menor];
        array[pos_menor]=temp;
        (*pasadas)++;
    }
    return array;
}

//implementamos insercion
int* insercion(int arr[], int n) {
    for (int i = 1; i < n; i++) {
        int clave = arr[i];
        int j = i - 1;

        // Desplaza los elementos de arr[0..i-1] que son mayores que la clave
        // a una posición adelante de su posición actual
        while (j >= 0 && arr[j] > clave) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = clave;
    }

    return arr;
}

// Implementación de funciones auxiliares para quicksort
static void intercambiar(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

static int particion(int arr[], int bajo, int alto) {
    // Tomamos el elemento central como pivote y lo movemos al final
    int medio = bajo + (alto - bajo) / 2;
    intercambiar(&arr[medio], &arr[alto]);
    int pivote = arr[alto];
    int i = bajo - 1;       // Índice del elemento menor

    for (int j = bajo; j < alto; j++) {
        if (arr[j] <= pivote) {
            i++;
            intercambiar(&arr[i], &arr[j]);
        }
    }
    intercambiar(&arr[i + 1], &arr[alto]);
    return i + 1;
}
// Función recursiva de quicksort
static void quicksort_recursivo(int arr[], int bajo, int alto) {
    if (bajo < alto) {
        int pi = particion(arr, bajo, alto);

        quicksort_recursivo(arr, bajo, pi - 1);
        quicksort_recursivo(arr, pi + 1, alto);
    }
}

//Funcion publica de quicksort: ordena todo el arreglo
int* Quicksort(int arr[], int n) {
    quicksort_recursivo(arr, 0, n - 1);
    return arr;
}

//implementamos shellsort
int* ShellSort(int arr[], int n) {
    // Empieza comparando elementos separados por n/2 y reduce el salto a la mitad
    int paso, i,j, temp;
    paso = n/2;
    while(n>0){
        for(i=paso; i<n ; i++){
            temp= arr[i];
            for(j=i; j>=paso; j-=paso){
                if(arr[j-paso]>temp){
                    arr[j]= arr[j-paso];
                }else{
                    break;
                }
            }
            arr[j]=temp;
        }
      paso=paso/2;  
    }
    
    return arr;
}

int* mergeSort2(int arr[], int n, int left, int right){
    if (n <= 1) return; // Caso base

    int middle = n / 2;
    int right_size = n - middle;

    // 1. Equivalente a: left_array = arr[:middle]
    int *left_array = (int *)malloc(middle * sizeof(int));
    for (int i = 0; i < middle; i++) {
        left_array[i] = arr[i];
    }

    // 2. Equivalente a: right_array = arr[middle:]
    int *right_array = (int *)malloc(right_size * sizeof(int));
    for (int i = 0; i < right_size; i++) {
        right_array[i] = arr[middle + i];
    }

    // Llamadas recursivas
    mergeSortLiteral(left_array, middle);
    mergeSortLiteral(right_array, right_size);

    // Mezclar de vuelta en 'arr'
    merge(arr, left_array, middle, right_array, right_size);

    // En C debes liberar manualmente la memoria que pediste
    free(left_array);
    free(right_array);

}

//implementamos radixsort (LSD, base 10, solo enteros no negativos)
int* RadixSort(int arr[], int n) {
    if (n <= 1) return arr;

    // Busca el maximo para saber cuantos digitos hay que procesar
    int max = arr[0];
    for (int i = 1; i < n; i++) {
        if (arr[i] > max) max = arr[i];
    }

    int *salida = (int*)malloc(n * sizeof(int));

    // Ordena por cada digito: unidades, decenas, centenas...
    for (int exp = 1; max / exp > 0; exp *= 10) {
        int cuenta[10] = {0};

        // Cuenta cuantos numeros tienen cada digito
        for (int i = 0; i < n; i++) {
            cuenta[(arr[i] / exp) % 10]++;
        }
        // Acumula para obtener la posicion final de cada digito
        for (int i = 1; i < 10; i++) {
            cuenta[i] += cuenta[i - 1];
        }
        // Coloca de atras hacia adelante para que sea estable
        for (int i = n - 1; i >= 0; i--) {
            int digito = (arr[i] / exp) % 10;
            salida[--cuenta[digito]] = arr[i];
        }
        // Copia el resultado al arreglo original
        for (int i = 0; i < n; i++) {
            arr[i] = salida[i];
        }
    }

    free(salida);
    return arr;
}

//Funcion para generar un arreglo con numeros aleatorios
int * get_data_array(int size){
    int min = 1;
    int max = 10000;
    int *arr = (int*)malloc(size * sizeof(int));
    srand(time(NULL));
    for(int i=0; i<size; i++){
        arr[i] = min + rand() % (max - min + 1);
    }
    return arr;
}