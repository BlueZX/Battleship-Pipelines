#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#define FIFONAME_S "server_to_client"
#define FIFONAME_STATUS "status"
#define FIFONAME_C "client_to_server"

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

	int n,fd, fifo_c, fifo_status, ser;
	char buf[1024]; // Cadena de char usado para guardar lo que se escribe en el cliente.
	char buf_s[1024]; // Cadena de char usado para guardar lo que se escribe en el cliente.

    int status[2];
    status[0] = 1;

	if((fd=open(FIFONAME_S,O_RDWR))<0){ // Se abre el archivo FIFO con permisos de lectura y escritura. Se guarda su descriptor de archivo (fd)
		perror("open"); // Si falla, error
		exit(1);
	}

	if((fifo_c = open(FIFONAME_C,O_RDWR))<0){ // Se abre el archivo FIFO con permisos de lectura y escritura. Se guarda su descriptor de archivo (fd)
		perror("open"); // Si falla, error
		exit(1);
	}

	if((fifo_status = open(FIFONAME_STATUS,O_RDWR))<0){ // Se abre el archivo FIFO con permisos de lectura y escritura. Se guarda su descriptor de archivo (fd)
		perror("open"); // Si falla, error
		exit(1);
	}
    

    printf("---------- envio mensaje por defecto ------------ \n\n");
    write(fifo_status,status,sizeof(status));

    fflush(stdout);

    close(fifo_status);

    while(1){

        if((ser=read(fd,buf_s,sizeof(buf_s))) > 0){ // Lee desde teclado el mensaje que se quiere enviar al servidor.
            write(1,buf_s,ser); // Si se recibe más de 0 bytes, se escribe en la tuberia (fd) lo almacenado en buf con n bytes
            fflush(stdout);
        }

        while((n=read(0,buf,sizeof(buf)))>0){ // Lee desde teclado el mensaje que se quiere enviar al servidor.
            write(fifo_c,buf,n); // Si se recibe más de 0 bytes, se escribe en la tuberia (fd) lo almacenado en buf con n bytes
            fflush(stdout);
        }

    }

    close(fifo_c);
	close(fd);
	exit(0);
}
