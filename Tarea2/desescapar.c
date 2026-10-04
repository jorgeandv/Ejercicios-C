#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "desescapar.h"

//En el desarrollo usé switch para no tener muchos else if seguidos y porque lo que hacia era comparar la misma variable *explorador con distintos valores :).
//Recordar que el escritor siempre va atrás o en la misma posición que el lector

//Recibe el dígito que sabemos que es un hexadecimal y devuelve su entero respectivo. Luego usamos eso para pasar de hexadecimal a entero.
/*
En hexadecimal
A -> 10 (Le restamos 55 a su versión ASCII, 'A' - 55 = 10)
B -> 11 (Le restamos 55 a su versión ASCII, 'B' - 55 = 11)
C -> 12 ...
D -> 13
E -> 14
F ->15


En hexadecimal
a -> 10 (Le restamos 87 a su versión ASCII, 'a' - 87 = 10)
b -> 11 (Le restamos 87 a su versión ASCII, 'b' - 87 = 11)
c -> 12 ...
d -> 13
e -> 14
f ->15

Lo mismo del 0 al pero con -48
*/

int pasarHexadecimal(char e) { //Recibe un char que es un dígito hexadecimal (0 a 9, a a f, A a F) y devuelve su valor hexadecimal
    if (e>='0' && e<='9') {
        int valorHexadecimal = e - 48;  //Según la tabla ASCII, al restar 48 se llega a su valor entero de hexadecimal
        return valorHexadecimal;
    }
    else if (e>='a' && e<='f') {
        int valorHexadecimal = e - 87;  //Según la tabla ASCII, al restar 87 se llega a su valor entero de hexadecimal
        return valorHexadecimal;
    }
    else if (e>='A' && e<='F'){
        int valorHexadecimal = e - 55; //Según la tabla ASCII, al restar 55 se llega a su valor entero de hexadecimal
        return valorHexadecimal;
    }
    else {  //Nunca debería caer acá porque significa que ingrese un digito que no es hexadecimal
        return 0;
    }
}


//Devuelve el largo que debe tener el string que es retornado por char *desescapado(const char *str), esto es necesario para usar en el malloc de esa función pues el enunciado pide 
//"En desescapado sí debe usar malloc para pedir el espacio ocupado por el resultado. Debe pedir exactamente la cantidad de bytes requeridos por el resultado, 
//ni más ni menos. Si no, el test de prueba rechazará su solución."

int memoriaRequerida(const char *str) { //Se usa const char *str porque como usamos esta función en char *desescapado(const char *str), da warning si no le ponemos el const
    int largoFinal = strlen(str)+1; //largoFinal empieza siendo el largo del string original
    const char *lector = str;   //como str es const char *str, lector tambien debe ser const char *lector para que no de warning
    while (*lector != '\0') {
        if (*lector == '\\') {
            const char *explorador = lector;    //Mismo caso que el lector, con const para que no de warning
            explorador++;
            switch(*explorador)
            {
                //Del case n al " se resta 1 porque encontramos una secuencia de escape y esa secuencia al final se representa con un solo char, pasando de ocupar dos char a solo uno. 
                //Ej: \n son dos char pero eso al final queda 0x0A que es un solo char
                case 'n':
                largoFinal-=1;
                lector++;
                lector++;
                break;

                case 't':
                largoFinal-=1;
                lector++;
                lector++;
                break;

                case '\\':
                largoFinal-=1;             
                lector++;
                lector++;
                break;

                case '"':
                largoFinal-=1;             
                lector++;
                lector++;
                break;

                case 'x':
                explorador++;
                //Caso: El primer dígito es hexadecimal
                //Avanzamos el explorador para ver el siguiente dígito 
                if ((*explorador>='0' && *explorador<='9')||
                (*explorador>='a' && *explorador<='f')||(*explorador>='A' && *explorador<='F')) {
                    explorador++;   
                    //Caso: Ambos dígitos son hexadecimales, encontramos una secuencia de escape del caso \xhh
                    //Se resta 3 porque encontramos una secuencia de escape de la forma \xhh y esa secuencia al final se representa con un solo char, pasando de ocupar cuatro char a sol uno.
                    //\x41 son cuatro char pero eso al final queda A que es un solo char
                    if ((*explorador>='0' && *explorador<='9')||
                    (*explorador>='a' && *explorador<='f')||(*explorador>='A' && *explorador<='F')) {
                        largoFinal-=3;
                        lector++;
                        lector++;
                        lector++;
                        lector++;
                    }
                    //Caso: Falló el segundo dígito (ese dígito no es hexadecimal)
                    //Se resta 1 porque como no encontramos la secuencia de escape, al final se elimina la \ y se conservan los otros char.
                    else {
                        largoFinal-=1;
                        lector++;
                    }
                }
                //Caso: Falló el primer dígito (ese dígito no es hexadecimal)
                //Se resta 1 porque como no encontramos la secuencia de escape, al final se elimina la \ y se conservan los otros char.
                else {
                    largoFinal-=1;
                    lector++;              
                }
                break;

                //Cuando caemos en default, también se resta 1 porque significa que encontramos una \ que no forma una secuencia de escape así que al final se elimina, pasando de ocupar un char a ocupar cero char.
                //Ej: \z son dos char pero eso al final queda z que es un solo char
                default:
                largoFinal-=1;
                lector++;   
                
            }
        }
        //No resto nada porque *lector=char que al final se conserva (Ej: *lector=a,b,c,...) así que no resta al largo que tendrá el string que es retornado por char *desescapado(const char *str)
        else {
            lector++;
        }
    }
    return largoFinal;
}



void desescapar(char *str) {
    char *lector = str;     //Con el puntero lector leo el str (por eso inicialmente le doy la dirección del str)
    char *escritor = str;   //Con el puntero escritor voy a escribir sobre el mismo str (por eso inicialmente le doy la dirección del str)

    while (*lector != '\0') {
        //Caso: Encontré una barra invertida, dependiende del siguiente char, puedo o no haber encontrado una secuencia de escape
        if (*lector == '\\') { 
            char *explorador = lector;  //Defino un puntero explorador para leer el siguiente char (y saber si encontré una secuencia de escape o no) sin tener que usar ni el puntero lector ni el puntero escritor
            explorador++;

            switch(*explorador)
            {
                //Caso: Encontré la secuencia de escape \n
                //Se escribe el 0x0A donde está apuntando el escritor, se avanza el escritor para preparar la siguiente sobrescritura
                //Se avanza dos veces el lector para que en la siguiente iteración se lea un char útil (que no sea ni la \ ni el n, porque ambos quedaron represenados en 0x0A) 
                case 'n':
                *escritor=0x0A;   
                escritor++;    
                lector++;   
                lector++; 
                break;

                //Caso: Encontré la secuencia de escape \t
                //Analogo al \n                
                case 't':
                *escritor=0x09;    
                escritor++;   
                lector++;  
                lector++; 
                break;

                //Caso: Encontré la secuencia de escape \\.
                //Analogo al \n
                case '\\':
                *escritor=0x5c;    
                escritor++;    
                lector++;   
                lector++; 
                break;

                //Caso: Encontré la secuencia de escape \"
                //Analogo al \n
                case '"':
                *escritor=0x22;   
                escritor++;    
                lector++;   
                lector++; 
                break;

                //Caso: Quizás encontré la secuencia de escape \xhh, me faltan revisar los proximos dos char
                case 'x':
                explorador++;
                //Caso: El primer dígito es hexadecimal 
                //Se guarda en valor1 el valor hexadecimal de ese dígito y se avanza el explorador para ver el siguiente dígito
                if ((*explorador>='0' && *explorador<='9')||
                (*explorador>='a' && *explorador<='f')||(*explorador>='A' && *explorador<='F')) {
                    int valor1=pasarHexadecimal(*explorador);
                    explorador++;
                    //Caso: Ambos dígitos son hexadecimales, encontramos una secuencia de escape del caso \xhh
                    //Se guarda en valor2 el valor hexadecimal de ese segundo dígito, se calcula el numero ASCII del caracter conformado por ambos dígitos (pasando de hexadecimal a entero),
                    //luego se castea este caracter (para que me dé su representación en ASCII y no su número asociado) y se escribe en donde está apuntando el escritor (recordar que este no lo hemos movido).
                    //Se avanza el escritor para preparar la siguiente sobrescritura.
                    //Se avanza cuatro veces el lector para que en la siguiente iteración se lea un char útil (que no sea ni la \ ni el x ni los dos dígitos hexadecimales, porque todo quedo representado por caracter)
                    if ((*explorador>='0' && *explorador<='9')||
                    (*explorador>='a' && *explorador<='f')||(*explorador>='A' && *explorador<='F')) {
                        int valor2=pasarHexadecimal(*explorador);
                        int caracter = valor1*16 + valor2*1;
                        *escritor = (char)caracter;
                        escritor++;
                        lector++;
                        lector++;
                        lector++;
                        lector++;
                    }
                    //Caso: Falló el segundo dígito (ese dígito no es hexadecimal)
                    //Se avanza el lector para que en la siguiente iteración no se caiga en el if (*lector == '\0'), logrando que se ejecute *escritor=*lector escribiendo el primer dígito
                    //sin la \ y luego en la siguiente iteración pase lo mismo, escribiendo el segundo dígito sin la \.
                    else {
                        lector++;  
                    }
                }
                //Caso: Falló el primer dígito (ese dígito no es hexadecimal)
                //Se avanza el lector para que en la siguiente iteración no se caiga en el if (*lector == '\0'), logrando que se ejecute *escritor=*lector escribiendo el primer dígito
                //sin la \.
                else {
                    lector++;              
                }
                break;

                default:    
                //Caso: La \ que leyó el lector es el último char ("El string se termina antes")
                //Se avanza el lector para que en la siguiente iteración se salga del While y se escriba el \0 (así no escribimos la \)
                if (*explorador == '\0') {
                    lector++;
                }
                //Caso: No encontré una secuencia de escape, lo que venía después de la \ no era n,t,\,",x ni tampoco la \ que leyó el lector es el último char
                //Se ejecuta *escritor=*explorador, donde se escribe este char que no es n,t,\,",x y luego se avanza el escritor para preparar la siguiente sobreescritura y 
                //se avanza dos veces el lector para leer el siguiente char útil (que no sea ni la \ porque no se quiere copiar ni el char que no es n,t,\,",x porque ya se copió)
                else {
                    *escritor=*explorador;
                    escritor++;
                    lector++;
                    lector++;
                } 
            }

        }
        //Caso: No encontré una barra invertida
        //Se ejecuta *escritor=*lector, donde se escribe el char que no es \ y luego se avanza el escritor para preparar la siguiente sobrescritura y se avanza el lector para leer el siguiente
        //char útil (que no sea el char que no es \ porque ya se escribió)
        else {  
            *escritor = *lector;  //El valor donde apunta escritor ahora es el valor donde apunta lector (así me aseguro de sobreescribir el char que venia después del \)
            escritor++;
            lector++;
        }
    }
    //Cuando se sale del While escribimos todos los char pero para terminar se escribir hay que poner el \0 al final
    *escritor='\0';    
}



char *desescapado(const char *str) {
    char *copia = malloc(memoriaRequerida(str)); //El puntero copia apunta al inicio de la memoria que reservé
    char *escritor= copia;      //Con el puntero escritor escribo en la memoria reservada (por eso inicialmente le doy la dirección de copia)
    const char *lector=str;   //Con el puntero lector leo el str (por eso inicialmente le doy la dirección del str), se define como const char *lector=str porque sino da warning (lo mismo que pasa en memoriaRequerida() )
    
    while (*lector != '\0') {
        //Caso: Encontré una barra invertida, dependiende del siguiente char, puedo o no haber encontrado una secuencia de escape
        if (*lector == '\\') { 
            const char *explorador = lector;    //Mismo caso que el lector, con const para que no de warning
            explorador++;

            switch(*explorador)
            {
                //Caso: Encontré la secuencia de escape \n
                //Se escribe el 0x0A en la copia, se avanza el escritor para preparar la siguiente copia
                //Se avanza dos veces el lector para que en la siguiente iteración se lea un char útil (que no sea ni la \ ni el n, porque ambos quedaron represenados en 0x0A)
                case 'n':
                *escritor=0x0A;    
                escritor++;   
                lector++;      
                lector++;
                break;

                //Caso: Encontré la secuencia de escape \t
                //Analogo al \n
                case 't':
                *escritor=0x09;    
                escritor++;    
                lector++;
                lector++;
                break;

                //Caso: Encontré la secuencia de escape \\.
                //Analogo al \n
                case '\\':
                *escritor=0x5c;    
                escritor++;    
                lector++;
                lector++;                
                break;

                //Caso: Encontré la secuencia de escape \"
                //Analogo al \n
                case '"':
                *escritor=0x22;    
                escritor++;   
                lector++;
                lector++;                
                break;

                //Caso: Quizás encontré la secuencia de escape \xhh, me faltan revisar los proximos dos char
                case 'x':
                explorador++;
                if ((*explorador>='0' && *explorador<='9')||
                (*explorador>='a' && *explorador<='f')||(*explorador>='A' && *explorador<='F')) {
                    int valor1=pasarHexadecimal(*explorador);
                    explorador++;
                    if ((*explorador>='0' && *explorador<='9')||
                    (*explorador>='a' && *explorador<='f')||(*explorador>='A' && *explorador<='F')) {
                        int valor2=pasarHexadecimal(*explorador);
                        int caracter = valor1*16 + valor2*1;
                        *escritor = (char)caracter;
                        escritor++;
                        lector++;
                        lector++;
                        lector++;
                        lector++;
                    }
                    else {
                        lector++;  //Solo eliminamos la barra
                    }
                }
                //Caso: Falló el primer dígito (ese dígito no es hexadecimal)
                //Sólo avanzamos el lector, en la siguiente iteración se leera este dígito que no es hexadecimal, NO se cae en el if (*lector == '\\'), ejecutando *escritor=*lector, donde
                //se copia este dígito que no es hexadecimal pero sin \ (pues avancé el lector)
                else {
                    lector++;  
                }
                break;
                

                default:
                //Caso: La \ que leyó el lector es el último char ("El string se termina antes")
                //Se avanza el lector para que en la siguiente iteración se salga del While y se copie el \0 (así no copiamos la \)
                if (*explorador == '\0') {
                    lector++;
                }
                //Caso: No encontré una secuencia de escape, lo que venía después de la \ no era n,t,\,",x ni tampoco la \ que leyó el lector es el último char
                //Se ejecuta *escritor=*explorador, donde se copia este char que no es n,t,\,",x y luego se avanza el escritor para preparar la siguiente copia y 
                //se avanza dos veces el lector para leer el siguiente char útil (que no sea ni la \ porque no se quiere copiar ni el char que no es n,t,\,",x porque ya se copió)
                else {
                    *escritor=*explorador;
                    escritor++;
                    lector++;
                    lector++;
                } 
            }
        }

        //Caso: No encontré una barra invertida
        //Se ejecuta *escritor=*lector, donde se copia el char que no es \ y luego se avanza el escritor para preparar la siguiente copia y se avanza el lector para leer el siguiente
        //char útil (que no sea el char que no es \ porque ya se copió)
        else {  
            *escritor=*lector;
            escritor++;
            lector++;
        }
    }
    //Cuando se sale del While copiamos todos los char pero para terminar esta copia hay que poner el \0 al final
    *escritor='\0';
    return copia;
}