#include<stdio.h>
#include<stdlib.h>
#include<time.h>

int array[]={3,5,2,4,8,1,8,9};

int * Data(int size){
    int min = 1;
    int max = 10000;
    int *arr = (int*)malloc(size * sizeof(int));
    srand(time(NULL));
    for(int i=0; i<size; i++){
        arr[i] = min + rand() % (max - min + 1);
    }
    return arr;
}

int BuscarMenor(int array[], int n, int inicio){
    int pos_menor=inicio;
    for(int j=inicio+1; j<n; j++){
        if(array[j]<array[pos_menor]){
            pos_menor=j;
        }
    }
    return pos_menor;
}

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

int main(){
    int i;
    printf("****Generando lista de números aleatorios*****\n\n");
    printf("Dame el núero de elementos a generar: \n");
    scanf("%d", &i);
    int* numeros=Data(i);
    printf("****Lista original Generada con exito*****\n\n");
    printf("se generaron %d números\n", i);
    int pasadas;
    printf("****Ordenando*****\n\n");
    clock_t inicio = clock();
    int* array_ordenado=Burbuja(numeros,i,&pasadas);
    clock_t fin = clock();
    double tiempo = ((double)(fin - inicio)) / CLOCKS_PER_SEC;
    printf("****Lista ordenada*****\n\n");
    printf("\nPasadas: %d\n",pasadas);
    printf("pasadas que evito: %d\n",i-pasadas);
    printf("Porcentaje de pasadas que evito: %f\n",((float)(i-pasadas)/i)*100);
    printf("Tiempo de ejecución: %f segundos\n", tiempo);
    return 0;
}