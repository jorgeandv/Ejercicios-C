#include <stdio.h>

typedef unsigned int uint;
uint comprimir(uint a[], int nbits);

uint comprimir(uint a[], int nbits) {  
    //Contamos el k (pues no podemos usar / ni *)
    uint totalBits = sizeof(uint) << 3; 
    uint acumulado = 0;
    uint k=0;

    while (acumulado + nbits <= totalBits) {
        acumulado = acumulado + nbits;
        k = k + 1;
    }


    //Creamos la mascara para hacer el truncamiento a nbits en cada elemento del arreglo
    uint mask = ~(-1U <<(nbits-1)<<1);  //La mascara tiene 1's en los primeros nbits de derecha a izquierda, y 0's en el resto de bits

    //Aplicamos la mascara a cada elemento del arreglo a[], cada elemento del arreglo queda truncado a nbits
    for (int i=0; i<(int)k; i++) {  //Acá hay que hacer un cast a k pues k es uint e i es int
        a[i]= mask&a[i];   //Lo mismo que *(a+i)=*(a+i)&iMask
    } 


    //Metemos cada elemento del arreglo en el uint r
    uint r=0;   //r tiene 0's en todos sus bits
    for (int i=0; i<(int)k-1; i++) {  //se hace k veces (del primer hasta el penultimo elemento del arreglo a[])
        r=(r|a[i])<<(nbits-1)<<1;   //Usando | metemos el elemento del arreglo a[] a r y lo desplazamos para dejar espacio para meter el siguiente elemento en la siguiente iteración
    }
    r=(r|a[k-1]);   //Metemos el último elemento del arreglo a[] a r pero no desplazamos

    
    //Devolvemos el r
    return r;
}