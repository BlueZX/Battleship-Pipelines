#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <sys/sem.h>

#define FIFONAME_S "server_to_client"
#define FIFONAME_STATUS "status"
#define FIFONAME_C "client_to_server"

struct sembuf p = {0, -1, SEM_UNDO}; // Estrutura que define la operación atomica sem_wait(). 0 significa que se va a operar en el indice 0 del arreglo
									 // de semáforos. -1 significa que se va a disminuir el contador del semáforo, SEM_UNDO significa que la operación
									 // aplicada será mientras viva el proceso, es decir, si el proceso muere antes de liberar el recurso o sección
									 // critica, automáticamente se liberar dicho recurso, esto para evitar deadlock (Ver clase 7)
struct sembuf v = {0, 1, SEM_UNDO}; // Estructura que define la operación atomica sem_post(). Mismo caso que la operación anterior, con la diferencia
									// que 1 significa que se va a aumentar el contador del semáforo.

void welcome(){
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
}

void wait_semaphore(int semid){
    if(semop(semid, &p, 1) < 0) { // semop realiza una operación sobre un arreglo de semaforos. Recibe la id del arreglo de semaforos,
                                // el puntero de la operación (en este caso, p, sem_wait()) y la dimensión del arreglo. Se ocupa el recurso.
        perror("semop p"); exit(1);
    }
}

void post_semaphore(int semid){
    if(semop(semid, &v, 1) < 0) { // Luego se libera el recurso para que otro proceso lo pueda ocupar
        perror("semop v"); exit(1);
    }
}

int main(void){

    welcome();

	int n,fifo,fifo_c, fifo_status, len, temp, s_len;
    int semid;
    //char buffer_s[1024];
	char buf[1024]; // Cadena de char usado para guardar lo que se recibe desde el cliente.
    int status[2];
    //char client[20];

    pid_t pid;


	unlink(FIFONAME_STATUS); // Elimina "myfifo" si existe.
	unlink(FIFONAME_S); // Elimina "myfifo" si existe.
	unlink(FIFONAME_C); // Elimina "myfifo" si existe.
    
	if(mkfifo(FIFONAME_S,666)<0){ // Crea que archivo "myfifo" con permisos 666, si no existe lo crea.
		perror("mkfifo"); // Si no se puede crear, imprime el error causado.
		exit(1);
	}

	if(mkfifo(FIFONAME_STATUS,666)<0){ // Crea que archivo "myfifo" con permisos 666, si no existe lo crea.
		perror("mkfifo"); // Si no se puede crear, imprime el error causado.
		exit(1);
	}

	if(mkfifo(FIFONAME_C,666)<0){ // Crea que archivo "myfifo" con permisos 666, si no existe lo crea.
		perror("mkfifo"); // Si no se puede crear, imprime el error causado.
		exit(1);
	}


    fifo = open(FIFONAME_S,O_RDWR);
    fifo_c = open(FIFONAME_C,O_RDWR);
    fifo_status = open(FIFONAME_STATUS,O_RDWR);

	if(fifo < 0){ // Se abre el archivo FIFO con permisos de lectura y escritura. Se guarda su descriptor de archivo (fd)
		perror("open"); // Si falla, error
		exit(1);
	}

	if(fifo_c < 0){ // Se abre el archivo FIFO con permisos de lectura y escritura. Se guarda su descriptor de archivo (fd)
		perror("open"); // Si falla, error
		exit(1);
	}

	if(fifo_status < 0){ // Se abre el archivo FIFO con permisos de lectura y escritura. Se guarda su descriptor de archivo (fd)
		perror("open"); // Si falla, error
		exit(1);
	}

    semid = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT); // Obtiene la id de un arreglo de semáforos, el primero argumento de semget()
													  // define la visibilidad del arreglo con respecto a otro procesos que distinta jerarquia, en
													  // este caso, es privado, solo procesos de la misma jerarquia podrá visualizar el semáforo.
													  // 1 significa la dimensión del arreglo que semáforos
													  // El ultimo parametro da los permisos.
    
    if(semid < 0) { // Si no se pudo crear el arreglo, retorna un valor negativo
        perror("semget"); exit(1);
    }
    if(semctl(semid, 0, SETVAL, 1) < 0) { // semctl inicializa un semaforo, para un arreglo de semaforos (semid), en su posición 0, se va a inicializar
										  // el valor del contador (SETVAL) el valor 1.
        perror("semctl"); exit(1); // Si falla, retorna un valor negativo.
    }


    printf("\x1b[32m%s\x1b[0m", "Esperando a los jugadores...\n\n");

    while(1) {


        if((s_len = read(fifo_status, status, sizeof(status))) > 0){

            printf("mensaje del status: %d \n\n",status[0]);

            if(status[0] > 0){

                pid = fork();
                switch(pid){
                    case -1:
                        perror("fork");
                    break;
                    case 0:
                        wait_semaphore(semid);
                        printf("\x1b[36m%s\x1b[0m", "Entro al hijo... \n \n");
                        fflush(stdout);
                        if(( n = read(fifo_c,buf,sizeof(buf)))>0){ // Lee desde la tuberia los mensajes que va escribiendo el cliente, lo almacena en buf y retorna en número de bytes escritos

                            write(1,buf,n); // Escribe por pantalla los almacenado en buf (escribe n bytes).
                            //printf("%s: ",buf);
                            fflush(stdout);

                        }

                        printf("\x1b[32m%s\x1b[0m", "Salio hijo... \n \n");

                        printf("pid: %d \n \n",pid);
                        post_semaphore(semid);

                        close(fifo_c);
                        exit(0);
                    break;
                    default:
                        wait_semaphore(semid);
                        printf("\x1b[36m%s\x1b[0m","Entro el padre... \n \n");

                        if(status[0] == 1){
                            printf("\x1b[34m%s\x1b[0m", "Ha ingresado un jugador \n \n");
                            printf("\x1b[32m%s\x1b[0m", "Enviando mapa...\n\n");

                            char mapa[] = "este es el mapa xd";
                            write(fifo,mapa, sizeof(mapa));

                            printf("\x1b[34m%s\x1b[0m", "El mapa fue enviado exitosamente al primer jugador! \n \n");
                        }

                        if(status[0] == 2){
                            printf("\x1b[34m%s\x1b[0m", "Ha ingresado el opnente \n \n");
                            printf("\x1b[32m%s\x1b[0m", "Enviando mapa...\n\n");

                            char mapa[] = "este es el mapa xdxdxd";
                            write(fifo,mapa, sizeof(mapa));

                            printf("\x1b[34m%s\x1b[0m", "El mapa fue enviado exitosamente al segundo jugador! \n \n");
                        }

                        printf("ppid: %d \n \n",pid);
                        post_semaphore(semid);
                    break;

                }
            }


        }
    }

    //close(fifo);
    close(fifo_c);
    close(fifo);
    close(fifo_status);
    exit(0);

}
