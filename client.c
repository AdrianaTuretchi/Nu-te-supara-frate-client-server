#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#define BUFFER_SIZE 600
extern int errno;
int main(int argc, char *argv[]) {
    int sd;
    struct sockaddr_in server;
    char buffer[BUFFER_SIZE];
    char response[BUFFER_SIZE];
    if (argc != 3) {
        printf("Sintaxa: %s <adresa_server> <port>\n", argv[0]);
        return -1;
    }
    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("[client] Eroare la socket().");
        return errno;
    }
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(argv[1]);
    server.sin_port = htons(atoi(argv[2]));
    if (connect(sd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1) {
        perror("[client] Eroare la connect().");
        return errno;
    }
    printf("[client] Conexiune reușită. Puteți trimite mesaje. Scrieți 'exit' pentru a închide conexiunea.\n");
    while (1) {
        printf(">> ");
        fflush(stdout);
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = 0;
        if (strcmp(buffer, "exit") == 0) {
            printf("[client] Deconectare.\n");
            break;
        }
        if (write(sd, buffer, strlen(buffer) + 1) <= 0) {
            perror("[client] Eroare la write().");
            break;
        }
        system("clear");
        while (1) {
            memset(response, 0, sizeof(response));
            int bytes_read = read(sd, response, sizeof(response));
            
            if (bytes_read <= 0) {
                perror("[client] Eroare la read().");
                goto close_connection;
            }
            
            if ( strcmp(response,"STOP") == 0) {
                break;
            }
            printf("[server] : %s\n", response);
        }
        
    }
close_connection:
    close(sd);
    return 0;
}
