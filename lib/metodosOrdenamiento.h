#ifndef METODOSORDENAMIENTO_H
#define METODOSORDENAMIENTO_H

#include <stdio.h>
#include <stdlib.h>

//PROTOTIPOS 
int* Burbuja(int array[], int n, int *pasadas);
int* Seleccion(int array[], int n, int *pasadas);
int BuscarMenor(int array[], int n, int inicio);
int* insercion(int arr[], int n);
int* Quicksort(int arr[], int n);
int* ShellSort(int arr[], int n);
int* RadixSort(int arr[], int n);
//generador de arreglo con numeros aleatorios
int * get_data_array(int size);
    


#endif