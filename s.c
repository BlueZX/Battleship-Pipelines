// BattleShip name pipelines
// Integrantes: Iván Castro Duran, Alejandro Figueroa



#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/sem.h>

#include <signal.h>
#include <sys/wait.h>

#define FIFONAME_S "server_to_client"
#define FIFONAME_STATUS "status"
#define FIFONAME_ESTADO "estado"
#define FIFONAME_C "client_to_server"


//Tipos de puntos en el mapa
#define WATER 0
#define MISS 1
#define HIT 2
#define SHIP 3

typedef struct map{
    int width;
    int height;
    int **map;
    int total;
} Map;

struct sembuf p = {0, -1, SEM_UNDO}; // Estrutura que define la operación atomica sem_wait().
struct sembuf v = {0, 1, SEM_UNDO}; // Estructura que define la operación atomica sem_post().


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

//generamos el mapa
Map *iniciarMapa(){
    Map *m = malloc(sizeof(Map));
    int i, j;

    m->width = 5;
    m->height = 5;

    m->map = malloc(m->width * sizeof(int *));

    for(i=0; i<m->width; i++){
        m->map[i] = malloc(m->height * sizeof(int));
        for(j=0; j<m->height; j++)
            m->map[i][j] = 0;
    }

    return m;
}

// se pone los barcos en el mapa
void colocarMapa(Map *m){
    int a = 0;
    int n = 5;

    time_t t;

    srand((unsigned) time(&t));

    int i = rand() % m->height;
    int j = rand() % m->width;

    while(a < 5){

        if(m->map[i][j] == 3){
            i = rand() % m->height;
            j = rand() % m->width;
        
        }
        else{
            m->map[i][j] = 3;
            a++;
        }

    }

}

//mostrar el mapa en la pantalla
void verMapa(Map *m){
    int i, j;

    printf(" y\\x ");
    for (i=0; i<m->height; i++)
        if (i<10)
            printf("%i  ", i);
        else
            printf("%i ", i);

    printf("\n");

    for (i=0; i<m->width; i++)
    {
        if (i<10)
            printf("  %i", i);
        else
            printf(" %i", i);

        for (j=0; j<m->height; j++)
        {
            switch(m->map[i][j])
            {
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
        printf("\n");
    }
    printf("\n");
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

void clientSend(){
    int fifo, n;
    char buf[1024];

    fifo = open(FIFONAME_C,O_RDWR);

    if(fifo < 0){ // Se abre el archivo FIFO con permisos de lectura y escritura. Se guarda su descriptor de archivo (fd)
		perror("open"); // Si falla, error
		exit(1);
	}

    printf("\n Jugador %d va a enviar un mensaje... \n\n", getpid());


    if(( n = read(fifo,buf,sizeof(buf)))>0){
        write(1,buf,n); // Escribe por pantalla los almacenado en buf (escribe n bytes).
        fflush(stdout);
    }

    close(fifo);
    kill(getppid(),SIGALRM);
}

void serverSend(){
    int fifo;

    fifo = open(FIFONAME_S,O_RDWR);

    if(fifo < 0){ // Se abre el archivo FIFO con permisos de lectura y escritura. Se guarda su descriptor de archivo (fd)
		perror("open"); // Si falla, error
		exit(1);
	}

    char buf[] = "espere a que el oponente haga su jugada...";

    write(fifo,buf,sizeof(buf));
    fflush(stdout);

    close(fifo);
}

int main() 
{ 
    pid_t player1, player2; 

    int n,fifo,fifo_c, fifo_status, len, temp, s_len, fifo_estado;
    int semid;
	char buf[1024]; 
    int status[2];
    int estado = 0;

    welcome();

    unlink(FIFONAME_STATUS); // Elimina "status" si existe.
	unlink(FIFONAME_ESTADO); // Elimina "estado" si existe.
	unlink(FIFONAME_S); // Elimina "server_to_client" si existe.
	unlink(FIFONAME_C); // Elimina "client_to_server" si existe.
    
	if(mkfifo(FIFONAME_S,666) < 0){ // Crea que archivo "server_to_client" con permisos 666, si no existe lo crea.
		perror("mkfifo"); // Si no se puede crear, imprime el error causado.
		exit(1);
	}

	if(mkfifo(FIFONAME_STATUS,666) < 0){ // Crea que archivo "status" con permisos 666, si no existe lo crea.
		perror("mkfifo"); // Si no se puede crear, imprime el error causado.
		exit(1);
	}

	if(mkfifo(FIFONAME_ESTADO,666) < 0){ // Crea que archivo "estado" con permisos 666, si no existe lo crea.
		perror("mkfifo"); // Si no se puede crear, imprime el error causado.
		exit(1);
	}

	if(mkfifo(FIFONAME_C,666) < 0){ // Crea que archivo "client_to_server" con permisos 666, si no existe lo crea.
		perror("mkfifo"); // Si no se puede crear, imprime el error causado.
		exit(1);
	}

    fifo = open(FIFONAME_S,O_RDWR);
    // fifo_c = open(FIFONAME_C,O_RDWR);
    fifo_status = open(FIFONAME_STATUS,O_RDWR);
    fifo_estado = open(FIFONAME_ESTADO,O_RDWR);

	if(fifo < 0){ // Se abre el archivo FIFO con permisos de lectura y escritura. Se guarda su descriptor de archivo (fd)
		perror("open"); // Si falla, error
		exit(1);
	}

	if(fifo_status < 0){ // Se abre el archivo FIFO con permisos de lectura y escritura. Se guarda su descriptor de archivo (fd)
		perror("open"); // Si falla, error
		exit(1);
	}

	if(fifo_estado < 0){ // Se abre el archivo FIFO con permisos de lectura y escritura. Se guarda su descriptor de archivo (fd)
		perror("open"); // Si falla, error
		exit(1);
	}

    semid = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT); 
    
    if(semid < 0) { // Si no se pudo crear el arreglo, retorna un valor negativo
        perror("semget"); exit(1);
    }
    if(semctl(semid, 0, SETVAL, 1) < 0) { // semctl inicializa un semaforo, para un arreglo de semaforos (semid), en su posición 0, se va a inicializar
										  // el valor del contador (SETVAL) el valor 1.

        perror("semctl"); exit(1); // Si falla, retorna un valor negativo.
    }

    Map *m1 = iniciarMapa();
    Map *m2 = iniciarMapa();


    printf("\x1b[32m%s\x1b[0m", "Esperando a los jugadores...\n\n");

    //si el cliente se conecto enviando un estado, genera un hijo

    if((s_len = read(fifo_status, status, sizeof(status))) > 0){
        player1 = fork(); 
        if (player1 == 0) { 
            wait_semaphore(semid);

            printf("\x1b[36m%s\x1b[0m", "Ha ingresado el primer jugador \n \n");

            printf("child[1] --> pid = %d and ppid = %d\n\n", getpid(), getppid()); 

            printf("\x1b[32m%s\x1b[0m", "Enviando mapa del primer jugador...\n\n");

            colocarMapa(m1);

            //se genera el mapa como un arreglo de numeros donde -1 significa salto de linea
            int i, j, k;
            int mapa[30];
            k = 0;

            for(i=0; i<m1->width; i++){
                for(j=0; j<m1->height; j++){
                    mapa[k] = m1->map[i][j];
                    k++;
                }
                mapa[k] = -1;
                k++;
            }

            verMapa(m1);

            write(fifo,mapa, sizeof(mapa));

            printf("\x1b[34m%s\x1b[0m", "El mapa fue enviado exitosamente al primer jugador! \n \n");

            post_semaphore(semid);

            wait_semaphore(semid);

            signal(SIGHUP,clientSend);

            post_semaphore(semid);

            while(1);
        }
        else{
            wait_semaphore(semid);

            printf("\x1b[34m%s\x1b[0m","Entro el padre del primer player... \n \n");

            printf("parent --> pid = %d \n\n", getpid()); 

            post_semaphore(semid);
        }
        
    }

    //si se conecta otro cliente genera otro hijo

    if((s_len = read(fifo_status, status, sizeof(status))) > 0){

        player2 = fork(); 
        if (player2 == 0) { 
            wait_semaphore(semid);

            printf("\x1b[36m%s\x1b[0m", "Ha ingresado el oponente \n \n");

            printf("child[2] --> pid = %d and ppid = %d\n\n", getpid(), getppid()); 

            printf("\x1b[32m%s\x1b[0m", "Enviando mapa del segundo jugador...\n\n");

            colocarMapa(m2);

            //se genera el mapa como un arreglo de numeros donde -1 significa salto de linea
            int i, j, k;
            int mapa[30];
            k = 0;

            for(i=0; i<m2->width; i++){
                for(j=0; j<m2->height; j++){
                    mapa[k] = m2->map[i][j];
                    k++;
                }
                mapa[k] = -1;
                k++;
            }

            verMapa(m2);
            write(fifo,mapa, sizeof(mapa));

            printf("\x1b[34m%s\x1b[0m", "El mapa fue enviado exitosamente al segundo jugador! \n \n");

            post_semaphore(semid);

            kill(player1,SIGHUP); 
        } 
        else {  
            wait_semaphore(semid);

            printf("\x1b[34m%s\x1b[0m","Entro el padre del segundo player... \n \n");

            printf("parent --> pid = %d \n\n", getpid()); 

            signal(SIGALRM,serverSend);

            post_semaphore(semid);

            // // wait_semaphore(semid);
            // printf("\x1b[34m%s\x1b[0m","fuera semaforo padre... \n \n");
            // //signal(SIGHUP,clientSend);
            // signal(SIGALRM,serverSend);
            // // post_semaphore(semid);

            while(1);
        } 
    }

    close(fifo_c);
    close(fifo);
    close(fifo_status);
    exit(0);
  
    return 0; 
} 