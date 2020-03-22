#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#define FIFONAME_S "server_to_client"
#define FIFONAME_C "client_to_server"

int main(void){

    system("clear");

    printf("\x1b[33m%s\x1b[0m", " _____________________________________________________________________________________\n");
    printf("\x1b[33m%s\x1b[0m", "|  ____        _   _   _       _____ _     _          _____                           |\n");
    printf("\x1b[33m%s\x1b[0m", "| |  _ \\      | | | | | |     / ____| |   (_)        / ____|                          |\n");
    printf("\x1b[33m%s\x1b[0m", "| | |_) | __ _| |_| |_| | ___| (___ | |__  _ _ __   | (___   ___ _ ____   _____ _ __  |\n");
    printf("\x1b[33m%s\x1b[0m", "| |  _ < / _` | __| __| |/ _ \\\\___ \\| '_ \\| | '_ \\   \\___ \\ / _ \\ '__\\ \\ / / _ \\ '__| |\n");
    printf("\x1b[33m%s\x1b[0m", "| | |_) | (_| | |_| |_| |  __/____) | | | | | |_) |  ____) |  __/ |   \\ V /  __/ |    |\n");
    printf("\x1b[33m%s\x1b[0m", "| |____/ \\__,_|\\__|\\__|_|\\___|_____/|_| |_|_| .__/  |_____/ \\___|_|    \\_/ \\___|_|    |\n");
    printf("\x1b[33m%s\x1b[0m", "|                                           | |                                       |\n");
    printf("\x1b[33m%s\x1b[0m", "|                                           |_|                                       |\n");
    printf("\x1b[33m%s\x1b[0m", "|_____________________________________________________________________________________|\n\n");

	int n,fifo,fifo_c, len, temp, s_len;

	char buf[1024]; // Cadena de char usado para guardar lo que se recibe desde el cliente.
    char status[2];
    //char client[20];

    pid_t pid;


	unlink(FIFONAME_S); // Elimina "myfifo" si existe.
	unlink(FIFONAME_C); // Elimina "myfifo" si existe.
    
	if(mkfifo(FIFONAME_S,666)<0){ // Crea que archivo "myfifo" con permisos 666, si no existe lo crea.
		perror("mkfifo"); // Si no se puede crear, imprime el error causado.
		exit(1);
	}

	if(mkfifo(FIFONAME_C,666)<0){ // Crea que archivo "myfifo" con permisos 666, si no existe lo crea.
		perror("mkfifo"); // Si no se puede crear, imprime el error causado.
		exit(1);
	}


    fifo = open(FIFONAME_S,O_RDWR);
    fifo_c = open(FIFONAME_C,O_RDWR);

	if(fifo < 0){ // Se abre el archivo FIFO con permisos de lectura y escritura. Se guarda su descriptor de archivo (fd)
		perror("open"); // Si falla, error
		exit(1);
	}

	if(fifo_c < 0){ // Se abre el archivo FIFO con permisos de lectura y escritura. Se guarda su descriptor de archivo (fd)
		perror("open"); // Si falla, error
		exit(1);
	}

    printf("\x1b[32m%s\x1b[0m", "Esperando a los jugadores...\n\n");

    while(1) {

        while((s_len = read(fifo_c, status, sizeof(status))) > 0){

            printf("mensaje del status: %s \n\n",status);

            if(status[0] == '1'){

                printf("------------- ingreso un jugador ----------------------- \n \n");

                if((pid = fork()) == 0){
                    //temp = open(buf, O_RDONLY);
                    printf("------------- creo hijo ----------------------- \n \n");
                    printf("%s",buf);
                    while((n=read(fifo,buf,sizeof(buf)))>0){ // Lee desde la tuberia los mensajes que va escribiendo el cliente, lo almacena en buf y retorna en número de bytes escritos
                        write(1,buf,n); // Escribe por pantalla los almacenado en buf (escribe n bytes).
                        //printf("%s: ",buf);
                        fflush(stdout);

                    }

                close(fifo);
                exit(0);

                }
            }


        }
    }

    close(fifo_c);
    exit(0);

}
