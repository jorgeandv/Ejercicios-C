#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "pss.h"


int main(int argc, char *argv[]) {
    /*
    argv[1] es el diccionario, argv[2] es la llave, argv[3] es la definición (valor)
    Además, llave y definición se pasan como strings (a la defición la llamaremos valor)
    */

    //Diagnostica error: Cantidad de parámetros incorrecta
    if (argc!=4) {
        fprintf(stderr, "Uso: ./definir <diccionario> <llave> <definicion>\n");
        exit(1);
    }

    //Guardamos la llave y valor, y sus respectivos largos
    char *llave = argv[2];
    char *valor = argv[3];
    int tamLlave = strlen(llave);
    int tamValor = strlen(valor);

    //Diagnostica error: La llave y la definición no caben en la fila
    if (tamLlave + tamValor > 99) {
        fprintf(stderr, "Tamanno de la llave mas tamanno de la definicion (%d) exceden maximo permitido (%d)\n", tamLlave + tamValor, 99);
        exit(1);
    }

    //Abrimos el archivo en modo lectura y escritura mediante el puntero dicc
    FILE *dicc = fopen(argv[1], "r+");

    //Diagnostica error: No se pudo abrir el archivo
    if (dicc == NULL) {
        perror(argv[1]);
        exit(1);
    }

    /*
    Para calcular la cantidad de filas, hay que calcular el tamaño del archivo (en bytes) y luego dividirlo entre 100
    pues cada fila tiene 100 bytes
    */

    //Tamaño del archivo
    //Diagnostica error: No se pudo mover el cursor
    //El cursor queda al final del archivo
    if (fseek(dicc, 0, SEEK_END) != 0) {
        perror(argv[1]);
        exit(1);
    }
    int tamArchivo = ftell(dicc);  //Devuelve el tamaño del archivo (en bytes)

    //Diagnostica error: No se pudo contar los bytes
    if (tamArchivo == -1) {
        perror(argv[1]);
        exit(1);
    }

    //Cantidad de filas
    int numFilas = tamArchivo / 100;

    //Fila n, se calcula según el enunciado
    int n = hash_string(llave) % numFilas;

    //Se parte en la fila n
    int fila = n;

    //Variable que se usará para controlar el recorrido de las filas
    int quedan = numFilas;


    /*Se debe recorrer secuencialmente a partir de la fila n, hasta
    *Encontrar una fila vacía -> Insertar la llave y valor en esa posición
    *Encontrar una fila que ya contenga la llave que se está insertando -> Reportar error

    Si no se cae en alguno de los casos anteriores, es decir, se llega al final del archivo, hay que buscar desde el comienzo porque
    la tabla es circular

    Luego, puede darse el caso de que la tabla este llena (no hayan filas vacías) y no contenga la llave (ninguna fila contiene la llave),
    caemos en este caso si es que se llega nuevamente a la fila n, ahí hay que reportar un error
    */

    /*
    Para recorrer secuencialmente se usa un while con quedan>0. quedan parte en numFilas y
    baja en 1 en cada iteración, así se revisan exactamente numFilas filas y se termina
    al volver a la fila n (si no, el ciclo sería infinito cuando la tabla está llena).

    En cada iteración se avanza a la siguiente fila con fila = (fila+1)%numFilas:
    fila+1 pasa a la fila siguiente y %numFilas hace que, al llegar al final de la tabla, se vuelva a la fila 0 (la tabla es circular).
    */

    while(quedan>0) {
        //Diagnostica error: No se pudo mover el cursor
        //El cursor queda al inicio de la fila n (el tam)
        if (fseek(dicc, fila*100, SEEK_SET) != 0) {
            perror(argv[1]);
            exit(1);
        }

        /*
        Obtenemos tam
        No usamos fgetc porque eso obtiene el caracter y acá tam es un byte, así que usamos fread
        El cursor queda en el primer byte de la llave de la fila n
        */
        unsigned char tam;
        
        //Diagnostica error: No se pudo leer el archivo
        //El cursor queda en el primer byte de la llave de la fila n
        if (fread(&tam, 1, 1, dicc) != 1) {
            perror(argv[1]);
            exit(1);
        }

        //Fila vacía
        //Para este if necesito que el cursor esté en el tam
        if (tam == 0) {

            //Diagnostica error: No se pudo mover el cursor
            //El cursor queda en el primer byte de la fila n (el tam)
            if (fseek(dicc, -1, SEEK_CUR) != 0) {
                perror(argv[1]);
                exit(1);
            }

            //Se inserta el tam
            //Diagnostica error: No se pudo escribir en el archivo
            //El cursor queda en el primer byte de la llave de la fila n
            unsigned char t = tamLlave;
            if (fwrite(&t, 1, 1, dicc) != 1) {
                perror(argv[1]);
                exit(1);
            }

            //Se inserta la llave
            //Diagnostica error: No se pudo escribir en el archivo, abarcamos el caso de tamLlave es 0
            //El cursor queda en el primer byte del valor de la fila n
            if (tamLlave > 0 && fwrite(llave, tamLlave, 1, dicc) != 1) {
                perror(argv[1]);
                exit(1);
            }
            
            //Se inserta el valor
            //Diagnostica error: No se pudo escribir en el archivo, abarcamos el caso de tamValor es 0
            //El cursor queda en el primer byte de la siguiente fila
            if (tamValor > 0 && fwrite(valor, tamValor, 1, dicc) != 1) {
                perror(argv[1]);
                exit(1);
            }

            //Si valor no completa los 99-tam bytes que debe usar, entonces debo rellenar con ' ' hasta compeltar los bytes que faltan
            char espacio = ' ';
            for (int i = 0; i < 99 - tamLlave - tamValor; i++) {
                //Diagnostica error: No se pudo escribir en el archivo
                if (fwrite(&espacio, 1, 1, dicc) != 1) {
                    perror(argv[1]);
                    exit(1);
                }
            }

            //Vacía el buffer a disco
            fclose(dicc);

            //Retorna 0, todo bien
            return 0;
        }


        //Obtenemos la llave de la fila n, poniendola en un arreglo, donde se deja espacio para el '\0'
        char llaveFila[tam+1];
        //Diagnostica error: No se pudo leer el archivo
        //El puntero queda en el primer byte del valor de la fila n
        if (fread(llaveFila, tam, 1, dicc) != 1) {
            perror(argv[1]);
            exit(1);
        }

        //LlaveFila se convierte en un string
        llaveFila[tam]='\0';

        //Vemos si la fila que ya contiene a la llave que se está insertando
        if (strcmp(llaveFila, llave) == 0) {
            fprintf(stderr, "La llave %s ya se encuentra en el diccionario\n", llave);
            exit(1);
        }

        //Se pasa a la fila siguiente, si era la última, se vuelve a la 0
        fila = (fila+1)%numFilas;

        //Se retrocede en uno el quedan
        quedan--;
    }

    //Si salí del while anterior es porque llegue al final del archivo sin caer en ninguno de los dos casos, es decir, la tabla está llena
    
    fprintf(stderr, "%s: el diccionario esta lleno\n", argv[1]);
    exit(1);
}
