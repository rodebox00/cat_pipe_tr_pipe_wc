/**
*   @author rodebox00
*/

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <strings.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#define BUF_SIZE 8192   //Establecemos este tamaño por defecto del buffer en base a los datos de la optimalidad del tiempo con respecto al tamaño obtenidos en la práctica de la semana anterior 
#define PARA_MIN 5     //numeros parametros minimos del programa

void writeall(int fdout, size_t bytes, char *buffer){
    ssize_t num_written;
    char *buf = buffer; //guardamos el puntero al buffer
    
    while (bytes > 0 && (num_written = write(fdout, buf, bytes)) != -1)
    {
        buf += num_written;
        bytes -= num_written;
    }
    
    if(num_written == -1){
        perror("write()");
        exit(EXIT_FAILURE);
    }

    assert(bytes==0);
}

void readall(int fdin, int fdout, char *buffer){
    
    ssize_t num_read;

    while ((num_read = read(fdin, buffer, BUF_SIZE)) > 0){
        writeall(fdout, num_read, buffer);
    }

    if (num_read == -1)
    {
        perror("read(fdin)");
        exit(EXIT_FAILURE);
    }
}

void catfd(int fdout,int *fdin, int tama){

    char *buffer = NULL;

    if((buffer= malloc(BUF_SIZE)) == NULL){
        perror("malloc(BUF_SIZE)");
        exit(EXIT_FAILURE);
    }
    
    
    if(tama == 0){
        readall(STDIN_FILENO, fdout, buffer);
    }else{
            for(int i = 0; i<tama; i++){    
                readall(fdin[i], fdout, buffer);
                
                if (close(fdin[i]) == -1)
                {
                    perror("close(fdin[])");
                    exit(EXIT_FAILURE);
                }
            }
    }
    
    free(buffer);
    
    if (close(fdout) == -1)
    {
            perror("fdout1");
            exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}


void trfd(int fdin, int fdout, char *cambio){
    
    char *buffer = NULL;
    ssize_t num_read;
    int i;

    if((buffer= malloc(BUF_SIZE)) == NULL){
        perror("malloc(BUF_SIZE)");
        exit(EXIT_FAILURE);
    }
    
    while ((num_read = read(fdin, buffer, BUF_SIZE)) > 0){
        for(i=0;i<num_read;i++){
            if(cambio[(int)buffer[i]] != (char)0) buffer[i] = cambio[(int)buffer[i]];   //Sustituimos el caracter si tiene sustituto
        }
        writeall(fdout, num_read, buffer);
    }


    free(buffer);

    if (num_read == -1)
    {
        perror("read(fdin)");
        exit(EXIT_FAILURE);
    }

    if (close(fdin) == -1)
    {
        perror("close(fdin)");
        exit(EXIT_FAILURE);
    }

    if (close(fdout) == -1)
    {
        perror("fdout2");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}

void wcfd(int fdin){
    
    char *buffer = NULL;
    ssize_t num_read = 0;
    ssize_t num_read_f = 0;
    int lineas = 0;

    if((buffer= malloc(BUF_SIZE)) == NULL){
        perror("malloc(BUF_SIZE)");
        exit(EXIT_FAILURE);
    }

    int i;

    while ((num_read = read(fdin, buffer, BUF_SIZE)) > 0){
        num_read_f += num_read;

        for(i=0;i<num_read;i++){
            if (buffer[i] == '\n') lineas++;       
        }
    }

    free(buffer);

    if (num_read == -1)
    {
        perror("read(fdin)");
        exit(EXIT_FAILURE);
    }

    printf("%d %ld\n",lineas, num_read_f);   //Mostramos por salida estándar 

    if (close(fdin) == -1)
    {
        perror("close(fdin)");
        exit(EXIT_FAILURE);
    }
    
    exit(EXIT_SUCCESS);
}

int reordenar(int longi, char *cadena){     //Reestructuramos la cadena de caracteres en base a introducir o no \n 

    int nlong = longi;
    int posi = 0;
    int cont = 0;
    int r = 0;

    for(int i=0;i<longi;i++){
        if((cadena[i] == '\\') && (cadena[i+1] =='n')){
            cadena[i+posi] ='\n'; //Sustituimos por un salto de linea
            posi--;
            nlong--;
            if(cont==1) r++;
            cont=1;

        }else{
            if(cont==1) cadena[i-r] = cadena[i+cont]; //Si entra en el if significa que hemos sustituido los caracteres "\" y "n" por un salto de linea al menos una vez
        }
    }
    return nlong;
}

int main(int argc, char **argv)
{

    int opcione = 0;
    int long1 = -1;
    int long2 = -1;
    char *cadena1 = NULL;
    char *cadena2 = NULL;
    int opt;


    while ((opt = getopt(argc, argv,"s:d:e")) != -1)
    {
        switch (opt)
        {
        case 's':
            cadena1=optarg;
            long1 = strlen(cadena1);

            if (opcione) long1 = reordenar(long1,cadena1);
            
            if((long2 > -1 && long1 != long2) || long1 == 0){
                fprintf(stderr, "Error: SRC y DST deben ser cadenas de caracteres de la misma longitud\n");
                exit(EXIT_FAILURE);
            }
            break;

        case 'd':
            cadena2=optarg;
            long2 = strlen(cadena2);

            if (opcione) long2 = reordenar(long2,cadena2);

            if((long1 > -1 && long1 != long2) || long2 == 0){
                fprintf(stderr, "Error: SRC y DST deben ser cadenas de caracteres de la misma longitud\n");
                exit(EXIT_FAILURE);
            }
            break;

        case 'e':
            opcione=1;
            break;

        default:
            fprintf(stderr, "Uso: %s [-e ] -s SRC -d DST [FILEIN1 FILEIN2 ... FILEINn]\n", argv[0]);
            exit(EXIT_FAILURE);
            break;
        }
    }

    if((long1 == -1) || (long2 == -1)){
        fprintf(stderr, "Uso: %s [-e ] -s SRC -d DST [FILEIN1 FILEIN2 ... FILEINn]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int pipefds1[2]; //Creamos la primera tuberia para el proceso que implementa cat y el que implementa tr

    if (pipe(pipefds1) == -1) /* Creación de la tubería */
    {
        perror("pipe()");
        exit(EXIT_FAILURE);
    }


    int tama = argc - (PARA_MIN+opcione);
    int fdin[tama]; //En este array vamos a almacenar los descriptores de ficheros del hijo que implementa cat


    switch (fork())    //Hijo que implementa cat
    {
    case -1:
        perror("fork(1)");
        exit(EXIT_FAILURE);
        break;
    case 0: /* Hijo izquierdo de la tubería */
        /* El extremo de lectura no se usa */
        if (close(pipefds1[0]) == -1)
        {
            perror("pipefds1[0]");
            exit(EXIT_FAILURE);
        }
        
        if(tama > 0){
            for(int i=0;i<tama;i++){
                fdin[i] = open(argv[PARA_MIN+opcione+i], O_RDONLY);
                if (fdin[i] == -1)
                {
                    perror("open(argv[])");
                    exit(EXIT_FAILURE);
                }
                
            }
        }

        catfd(pipefds1[1],fdin,tama);
        break;
    default: /* El proceso padre continúa... */
        break;
    }


    int pipefds2[2]; //Creamos la segunda tuberia para el proceso que implementa tr y el que implementa wc

    if (pipe(pipefds2) == -1) /* Creación de la tubería */
    {
        perror("pipe()");
        exit(EXIT_FAILURE);
    }



    char cambio[256]={0};   //Array de caracteres que vamos a usar para sustituir cada caracter por el que se desea empleando el numero ascii
    for(int c=0;c<long1;c++){
        cambio[(int)cadena1[c]] = cadena2[c];   //En la posicion del codigo ascii del caracter que se quiere sustituir guardamos el carácter a reemplazar
    }



    switch (fork())    //Hijo que implementa tr
    {
    case -1:
        perror("fork(2)");
        exit(EXIT_FAILURE);
        break;
    case 0: 
        /* El extremo de escritura de la primera tuberia no se usa */
        if (close(pipefds1[1]) == -1)
        {
            perror("pipefds1[1]");
            exit(EXIT_FAILURE);
        }
        /* El extremo de lectura de la segunda tuberia no se usa */
        if (close(pipefds2[0]) == -1)
        {
            perror("close(pipefds2[0])");
            exit(EXIT_FAILURE);
        }
        trfd(pipefds1[0],pipefds2[1],cambio);
        break;
    default: /* El proceso padre continúa... */
        break;
    }

    //Cerramos la primera tuberia
    if (close(pipefds1[0]) == -1)
    {
        perror("close(pipefds1[0])");
        exit(EXIT_FAILURE);
    }
    if (close(pipefds1[1]) == -1)
    {
        perror("close(pipefds1[1])");
        exit(EXIT_FAILURE);
    }


    switch (fork())    //Hijo que implementa wc
    {
    case -1:
        perror("fork(3)");
        exit(EXIT_FAILURE);
        break;
    case 0:
        // El extremo de escritura no se usa 
        if (close(pipefds2[1]) == -1)
        {
            perror("pipefds2[1]");
            exit(EXIT_FAILURE);
        }
        wcfd(pipefds2[0]);
        break;
    default: // El proceso padre continúa... 
        break;
    }

    //El proceso padre cierra los descriptores de ficheros de la segunda tuberia
     if (close(pipefds2[0]) == -1)
    {
        perror("close(pipefds2[0])");
        exit(EXIT_FAILURE);
    }
    if (close(pipefds2[1]) == -1)
    {
        perror("close(pipefds2[1])");
        exit(EXIT_FAILURE);
    }


    //El proceso padre espera que terminen los hijos
    if (wait(NULL) == -1)
    {
        perror("wait(1)");
        exit(EXIT_FAILURE);
    }
    if (wait(NULL) == -1)
    {
        perror("wait(2)");
        exit(EXIT_FAILURE);
    }
    if (wait(NULL) == -1)
    {
        perror("wait(3)");
        exit(EXIT_FAILURE);
    }


    return EXIT_SUCCESS;
}