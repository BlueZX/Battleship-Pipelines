// BattleShip name pipelines
// Integrantes: Iván Castro Duran, Alejandro Figueroa

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#define FIFONAME_S "server_to_client"
#define FIFONAME_STATUS "status"
#define FIFONAME_ESTADO "estado"
#define FIFONAME_C "client_to_server"

//Tipos de puntos en el mapa
#define WATER 0
#define MISS 1
#define HIT 2
#define SHIP 3

void verMapa(int mapa[]){
    int i, j;

    printf(" y\\x ");
    for (i=0; i < 5; i++)
        if (i<10)
            printf("%i  ", i);
        else
            printf("%i ", i);

    printf("\n");

    for(i=0; i < 30; i++){
        if(i == 0){
            printf("  %i", i);
        }

        if(mapa[i] == -1){
            printf("\n");
            if(i / 5 < 5){
                printf("  %d", i / 5);
            }
        }
        else{
            switch(mapa[i]){
                case WATER:
                    printf("\x1b[36m%s\x1b[0m","  ~");
                break;    
                case MISS:
                    printf("\x1b[31m%s\x1b[0m","  ~");
                break;
                case SHIP:
                    printf("\x1b[32m%s\x1b[0m","  B");
                break;
                case HIT:
                    printf("\x1b[31m%s\x1b[0m","  X");
                break;
                default:
                    printf("  ?");
                break;
            }
        }
    }

    printf("\n");

}

int main (void){

    system("clear");

    printf("\x1b[33m%s\x1b[0m", " ___________________________________________________\n");
    printf("\x1b[33m%s\x1b[0m", "|  ____        _   _   _       _____ _     _        |\n");
    printf("\x1b[33m%s\x1b[0m", "| |  _ \\      | | | | | |     / ____| |   (_)       |\n");
    printf("\x1b[33m%s\x1b[0m", "| | |_) | __ _| |_| |_| | ___| (___ | |__  _ _ __   |\n");
    printf("\x1b[33m%s\x1b[0m", "| |  _ < / _` | __| __| |/ _ \\\\___ \\| '_ \\| | '_ \\  |\n");
    printf("\x1b[33m%s\x1b[0m", "| | |_) | (_| | |_| |_| |  __/____) | | | | | |_) | |\n");
    printf("\x1b[33m%s\x1b[0m", "| |____/ \\__,_|\\__|\\__|_|\\___|_____/|_| |_|_| .__/  |\n");
    printf("\x1b[33m%s\x1b[0m", "|                                           | |     |\n");
    printf("\x1b[33m%s\x1b[0m", "|                                           |_|     |\n");
    printf("\x1b[33m%s\x1b[0m", "|  Version: 0.1                                     |\n");
    printf("\x1b[33m%s\x1b[0m", "|___________________________________________________|\n\n");

	int n,fd, fifo_c, fifo_status, fifo_estado,ser, s;
	char buf[1024]; 
	char buf_s[1024]; 
    int mapa[30];

    int status[2], estado[2];

	if((fd = open(FIFONAME_S,O_RDWR))<0){ // Se abre el archivo FIFO con permisos de lectura y escritura. Se guarda su descriptor de archivo (fd)
		perror("open s"); // Si falla, error
		exit(1);
	}

	// if((fifo_c = open(FIFONAME_C,O_RDWR))<0){ // Se abre el archivo FIFO con permisos de lectura y escritura. Se guarda su descriptor de archivo (fd)
	// 	perror("open c"); // Si falla, error
	// 	exit(1);
	// }

    printf("xd %d", status[0]);
    status[0] = 1;

	if((fifo_status = open(FIFONAME_STATUS,O_RDWR))<0){ // Se abre el archivo FIFO con permisos de lectura y escritura. Se guarda su descriptor de archivo (fd)
		perror("open s"); // Si falla, error
		exit(1);
	}

	if((fifo_estado = open(FIFONAME_ESTADO,O_RDWR))<0){ // Se abre el archivo FIFO con permisos de lectura y escritura. Se guarda su descriptor de archivo (fd)
		perror("open e"); // Si falla, error
		exit(1);
	}

    printf("xd %d", fifo_estado);

    // if((s = read(fifo_estado,estado,sizeof(estado))) > 0){ // Lee desde teclado el mensaje que se quiere enviar al servidor.
    //     //write(1,estado,s); // Si se recibe más de 0 bytes, se escribe en la tuberia (fd) lo almacenado en buf con n bytes
    //     printf("estado: %d", estado[0]);
    //     fflush(stdout);    
    // }
    

    printf("---------- envio mensaje de conexion ------------ \n\n");
        // if((s = read(fifo_status,estado,sizeof(estado))) > 0){ // Lee desde teclado el mensaje que se quiere enviar al servidor.
        // //write(1,estado,s); // Si se recibe más de 0 bytes, se escribe en la tuberia (fd) lo almacenado en buf con n bytes
        //     printf("estado: %d", estado[0]);
        //     fflush(stdout);    
        // }
    write(fifo_status,status,sizeof(status));

    fflush(stdout);

    close(fifo_status);

    while(1){

        
        if((ser = read(fd,mapa,sizeof(mapa))) > 0){ 
            //write(1,mapa,ser);
            printf("Mapa \n\n");
            //for(int i=0; i < 30; i++){
                //printf(" %d ", mapa[i]);
            //}
            verMapa(mapa);


            printf("\n");
            fflush(stdout);

            printf("leo \n \n");

            if((fifo_c = open(FIFONAME_C,O_RDWR))<0){ // Se abre el archivo FIFO con permisos de lectura y escritura. Se guarda su descriptor de archivo (fd)
                perror("open c"); // Si falla, error
                exit(1);
            }
        }

        if((n=read(0,buf,sizeof(buf)))>0){ // Lee desde teclado el mensaje que se quiere enviar al servidor.
            write(fifo_c,buf,n); // Si se recibe más de 0 bytes, se escribe en la tuberia (fd) lo almacenado en buf con n bytes
            fflush(stdout);

            printf("escribo \n \n");
        }
        

       close(fifo_c);

    }

	close(fd);
	exit(0);
}
