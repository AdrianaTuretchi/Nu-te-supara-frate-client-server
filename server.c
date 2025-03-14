#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include <time.h>
#include <stdbool.h>
#include <pthread.h>
#define PORT 2908
#define BUFFER_SIZE 256
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int number = 0,play = 0,turn = 1,save=1;
typedef struct thData {
    int idThread;
    int cl;   
    char* name;
    int id;
} thData;
//1 rosu 2 albastru 3 galben 4 verde
typedef struct poz {int i,j;}poz;
typedef struct player {
    int id , cl , turn , baza;
    int p[5];
    poz uni[5];
}player;
player j[5];
char board[16][16];
int road[5][16][16];//straturi linii coloane
char stop[BUFFER_SIZE];
void *raspunde(void *arg);
void *waiting_room(void *arg);
void *control_room(void *arg);
void *play_room(void *arg);
void zero(){
    for(int i=0;i<5;i++){
        for(int j=0;j<16;j++)
        for(int k=0;k<16;k++){
            road[i][j][k]=0;
        }}
}
void init_roads(){
    zero();
    //1 rosu
    road [1][6][1]=1;   road [1][6][2]=2;   road [1][6][3]=3;   road [1][6][4]=4;   road [1][6][5]=5;
    road [1][5][6]=6;   road [1][4][6]=7;   road [1][3][6]=8;   road [1][2][6]=9;   road [1][1][6]=10;   road [1][0][6]=11;
    road [1][0][7]=12;
    road [1][0][8]=13;   road [1][1][8]=14;   road [1][2][8]=15;   road [1][3][8]=16;   road [1][4][8]=17;   road [1][5][8]=18;
    road [1][6][9]=19;   road [1][6][10]=20;   road [1][6][11]=21;   road [1][6][12]=22;   road [1][6][13]=23;   road [1][6][14]=24;
    road [1][7][14]=25;
    road [1][8][14]=26;   road [1][8][13]=27;   road [1][8][12]=28;   road [1][8][11]=29;   road [1][8][10]=30;   road [1][8][9]=31;
    road [1][9][8]=32;   road [1][10][8]=33;   road [1][11][8]=34;   road [1][12][8]=35;   road [1][13][8]=36;   road [1][14][8]=37;
    road [1][14][7]=38;  
    road [1][14][6]=39;   road [1][13][6]=40;   road [1][12][6]=41;   road [1][11][6]=42;   road [1][10][6]=43;   road [1][9][6]=44;
    road [1][8][5]=45;   road [1][8][4]=46;   road [1][8][3]=47;   road [1][8][2]=48;   road [1][8][1]=48;   road [1][8][0]=50;
    road [1][7][0]=51;   road [1][7][1]=52;   road [1][7][2]=53;   road [1][7][3]=54;   road [1][7][4]=55;   road [1][7][5]=56;
    //2 albastru
    road [2][1][8]=1;   road [2][2][8]=2;   road [2][3][8]=3;   road [2][4][8]=4;   road [2][5][8]=5; 
    road [2][6][9]=6;   road [2][6][10]=7;   road [2][6][11]=8;   road [2][6][12]=9;   road [2][6][13]=10;   road [2][6][14]=11;
    road [2][7][14]=12;
    road [2][8][14]=13;   road [2][8][13]=14; road [2][8][12]=15;   road [2][8][11]=16; road [2][8][10]=17;   road [2][8][9]=18;
    road [2][9][8]=19;   road [2][10][8]=20; road [2][11][8]=21;   road [2][12][8]=22; road [2][13][8]=23;   road [2][14][8]=24;
    road [2][14][7]=25;
    road [2][14][6]=26;   road [2][13][6]=27; road [2][12][6]=28;   road [2][11][6]=29; road [2][10][6]=30;   road [2][9][6]=31;
    road [2][8][5]=32;   road [2][8][4]=33; road [2][8][3]=34;   road [2][8][2]=35; road [2][8][1]=36;   road [2][8][0]=37;
    road [2][7][0]=38;   
    road [2][6][0]=39;   road [2][6][1]=40;   road [2][6][2]=41;   road [2][6][3]=42;   road [2][6][4]=43;   road [2][6][5]=44;
    road [2][5][6]=45;   road [2][4][6]=46;   road [2][3][6]=47;   road [2][2][6]=48;   road [2][1][6]=49;   road [2][0][6]=50;
    road [2][0][7]=51;   road [2][1][7]=52;   road [2][2][7]=53;   road [2][3][7]=54;   road [2][4][7]=55;   road [2][5][7]=56;
    //3 galben
    road [3][8][13]=1;   road [3][8][12]=2;   road [3][8][11]=3;   road [3][8][10]=4;   road [3][8][9]=5;
    road [3][9][8]=6;   road [3][10][8]=7;   road [3][11][8]=8;   road [3][12][8]=9;   road [3][13][8]=10;   road [3][14][8]=11;
    road [3][14][7]=12;
    road [3][14][6]=13;   road [3][13][6]=14;   road [3][12][6]=15;   road [3][11][6]=16;   road [3][10][6]=17;   road [3][9][6]=18;
    road [3][8][5]=19;   road [3][8][4]=20; road [3][8][3]=21;   road [3][8][2]=22; road [3][8][1]=23;   road [3][8][0]=24;
    road [3][7][0]=25;
    road [3][6][0]=26;   road [3][6][1]=27; road [3][6][2]=28;   road [3][6][3]= 29; road [3][6][4]=30;   road [3][6][5]=31;
    road [3][5][6]=32;   road [3][4][6]=33; road [3][3][6]=34;   road [3][2][6]=35; road [3][1][6]=36;   road [3][0][6]=37;
    road [3][0][7]=38;   
    road [3][0][8]=39; road [3][1][8]=40;   road [3][2][8]=41; road [3][3][8]=42;   road [3][4][8]=43; road [3][5][8]=44;
    road [3][6][9]=45;   road [3][6][10]=46; road [3][6][11]=47;   road [3][6][12]=48; road [3][6][13]=49;   road [3][6][14]=50;
    road [3][7][14]=51;  road [3][7][13]=52;    road [3][7][12]=53;     road [3][7][11]=54; road [3][7][10]=55; road [3][7][9]=56;
    //4 verde
    road [4][13][6]=1;    road [4][12][6]=2;   road [4][11][6]=3;   road [4][10][6]=4;   road [4][9][6]=5;
    road [4][8][5]=6;   road [4][8][4]=7;   road [4][8][3]=8;   road [4][8][2]=9;   road [4][8][1]=10;   road [4][8][0]=11;
    road [4][7][0]=12;
    road [4][6][0]=13;   road [4][6][1]=14;   road [4][6][2]=15;   road [4][6][3]=16; road [4][6][4]=17;   road [4][6][5]=18;
    road [4][5][6]=19;   road [4][4][6]=20; road [4][3][6]=21;   road [4][2][6]=22; road [4][1][6]=23;   road [4][0][6]=24;
    road [4][0][7]=25;   
    road [4][0][8]=26; road [4][1][8]=27;   road [4][2][8]=28; road [4][3][8]=29;   road [4][4][8]=30; road [4][5][8]=31;
    road [4][6][9]=32;   road [4][6][10]=33; road [4][6][11]=34;   road [4][6][12]=35; road [4][6][13]=36;   road [4][6][14]=37;
    road [4][7][14]=38;  
    road [4][8][14]=39; road [4][8][13]=40; road [4][8][12]=41; road [4][8][11]=42; road [4][8][10]=43; road [4][8][9]=44;
    road [4][9][8]=45;   road [4][10][8]=46; road [4][11][8]=47;   road [4][12][8]=48;   road [4][13][8]=49;   road [4][14][8]=50;
    road [4][14][7]=51;   road [4][13][7]=52;   road [4][12][7]=53;   road [4][11][7]=54;   road [4][10][7]=55;   road [4][9][7]=56;
}
void init_nou(){
    for(int i=0;i<15;i++)
        for(int j=0;j<15;j++){
                if(i==0 || i==14){
                    if(j<6 || j>8 )     board[i][j]='_';
                    else                board[i][j]=' '; 
                }else if(j==0 || j==14){
                        if(i<6 ||i>8 )  board[i][j]='|';
                            else        board[i][j]=' ';
                }else if(j==5||j==9){
                        if(i<6 || i>8)  board[i][j]='|';
                        else if(i!=7)   board[i][j]=' ';
                        else {          board[7][5]='r';   board[7][9]='g'; }
                }else if(i==5||i==9){
                        if(j<6 || j>8)  board[i][j]='_';           
                        else if(j!=7)   board[i][j]=' ';                
                        else           {board[5][7]='a';   board[9][7]='v'; }   
                }
                else if(i==7){
                    if(j<6){board[i][j]='r';  }
                    else if(j>8){board[i][j]='g';   }
                    else {board[i][j]='0';   }
                }else if(j==7){
                    if(i<6){board[i][j]='a'; }
                    else if(i>8){board[i][j]='v';  }
                    else {board[i][j]='0';  }
                }else board[i][j]=' ';
        }
        board[1][8]='s';
        board[2][6]='s';
        board[6][1]='s';
        board[8][2]='s';
        board[13][6]='s';
        board[12][8]='s';
        board[8][13]='s';
        board[6][12]='s';
        board[7][7]=' ';
        init_roads();
}
void init_jucator(void *arg){
     struct thData tdL = *((struct thData *)arg);
         for(int i=1;i<=number;i++){
            j[i].p[1]=0;  j[i].p[2]=0;      j[i].p[3]=0;     j[i].p[4]=0;
                    if(j[i].baza==1){
                                    j[i].uni[1].i=2;   j[i].uni[1].j=2;
                                    j[i].uni[2].i=2;   j[i].uni[2].j=3;
                                    j[i].uni[3].i=3;   j[i].uni[3].j=2;
                                    j[i].uni[4].i=3;   j[i].uni[4].j=3;  }
                    else if(j[i].baza==2){
                                    j[i].uni[1].i=2;   j[i].uni[1].j=11;
                                    j[i].uni[2].i=2;   j[i].uni[2].j=12;
                                    j[i].uni[3].i=3;   j[i].uni[3].j=11;
                                    j[i].uni[4].i=3;   j[i].uni[4].j=12;  }
                    else if(j[i].baza==3){
                                    j[i].uni[1].i=11;   j[i].uni[1].j=11;
                                    j[i].uni[2].i=11;   j[i].uni[2].j=12;
                                    j[i].uni[3].i=12;   j[i].uni[3].j=11;
                                    j[i].uni[4].i=12;   j[i].uni[4].j=12;  }
                    else if(j[i].baza==4){
                                    j[i].uni[1].i=11;   j[i].uni[1].j=2;
                                    j[i].uni[2].i=11;   j[i].uni[2].j=3;
                                    j[i].uni[3].i=12;   j[i].uni[3].j=2;
                                    j[i].uni[4].i=12;   j[i].uni[4].j=3;  }
                    
                    if(j[i].baza==1){
                                    board[2][2]='R';    board[2][3]='R';
                                    board[3][2]='R';    board[3][3]='R';
                    }else if(j[i].baza==2){
                                    board[2][12]='A';   board[2][11]='A';
                                    board[3][12]='A';   board[3][11]='A';
                    }else if(j[i].baza==3){
                                    board[11][11]='G';  board[11][12]='G';
                                    board[12][11]='G';  board[12][12]='G';
                    }else if(j[i].baza==4)  {
                                    board[11][2]='V';   board[11][3]='V';
                                    board[12][2]='V';   board[12][3]='V';
                    }
         }
}
bool win(int i){
    for(int k=1;k<=4;k++)
        if(j[i].p[k]!=57) return 0;
    return 1;
}
bool valid_mutare(int zar,int nr_pion,int i){
        if(j[i].p[nr_pion]==0 && zar!=6)
            return 0;
        else {
            if(57-j[i].p[nr_pion]-zar<0)
                return 0;  
            else  
                return 1; }
}
bool exista_mutari_posiblile(int i,int zar){
    for(int j=1;j<=4;j++)
    {
        if(valid_mutare(zar,j,i)==1)
        return 1;
    }
    return 0;
}
void muta(int zar, int nr_pion, int i){
    char aux;
    aux=board[j[i].uni[nr_pion].i][j[i].uni[nr_pion].j];//carcter
    board[j[i].uni[nr_pion].i][j[i].uni[nr_pion].j]=' ';//gol
    if( j[i].p[nr_pion]==0&& zar ==6){  j[i].p[nr_pion]=1; //board[6][1]='R'; 
    }
    else      j[i].p[nr_pion]= j[i].p[nr_pion]+zar;
    //muta universal pionul
        for(int k=0;k<15;k++)
            for(int l=0;l<15;l++)
                if(road[i][k][l]==j[i].p[nr_pion])
                {
                        //board[k][l]=aux;
                        j[i].uni[nr_pion].i=k;
                        j[i].uni[nr_pion].j=l;
                        if(i==1)      board[k][l]='R';
                        else if(i==4) board[k][l]='V';
                        else if(i==2) board[k][l]='A';
                        else if(i==3) board[k][l]='G';
                        printf(" %d %d",k,l);
                        
                        break;
                }
                
                
            if(board[j[i].uni[nr_pion].i][j[i].uni[nr_pion].j]!='s'){
                for(int m=1;m<=number;m++)
                    for(int n=1;n<=4;n++)
                        if(j[m].uni[n].i==j[i].uni[nr_pion].i && j[m].uni[n].j==j[i].uni[nr_pion].j  && m!=i)
                        {  
                            j[m].p[n]=0;
                            if(j[i].baza==1){
                                if(n==1)   {  j[i].uni[1].i=2;   j[i].uni[1].j=2;}
                                if(n==2)   {  j[i].uni[2].i=2;   j[i].uni[2].j=3;}
                                if(n==3)   {  j[i].uni[3].i=3;   j[i].uni[3].j=2;}
                                if(n==4)     {j[i].uni[4].i=3;   j[i].uni[4].j=3; } }
                            else if(j[i].baza==2){
                                    if(n==1)   { j[i].uni[1].i=2;   j[i].uni[1].j=11;}
                                    if(n==2)   { j[i].uni[2].i=2;   j[i].uni[2].j=12;}
                                    if(n==3)   { j[i].uni[3].i=3;   j[i].uni[3].j=11;}
                                    if(n==4)  {  j[i].uni[4].i=3;   j[i].uni[4].j=12; } }
                            else if(j[i].baza==3){
                                    if(n==1){   j[i].uni[1].i=11;   j[i].uni[1].j=11;}
                                    if(n==2){   j[i].uni[2].i=11;   j[i].uni[2].j=12;}
                                    if(n==3){   j[i].uni[3].i=12;   j[i].uni[3].j=11;}
                                    if(n==4){   j[i].uni[4].i=12;   j[i].uni[4].j=12;}  }
                            else if(j[i].baza==4){
                                    if(n==1){   j[i].uni[1].i=11;   j[i].uni[1].j=2;}
                                    if(n==2){   j[i].uni[2].i=11;   j[i].uni[2].j=3;}
                                    if(n==3){   j[i].uni[3].i=12;   j[i].uni[3].j=2;}
                                    if(n==4){   j[i].uni[4].i=12;   j[i].uni[4].j=3; } }
                    
                            if(j[i].baza==1)
                                    board[j[i].uni[n].i][j[i].uni[n].j]='R';   
                            else if(j[i].baza==2)
                                    board[j[i].uni[n].i][j[i].uni[n].j]='A'; 
                            else if(j[i].baza==3)
                                    board[j[i].uni[n].i][j[i].uni[n].j]='G';  
                            else if(j[i].baza==4)  
                                    board[j[i].uni[n].i][j[i].uni[n].j]='V';
                    
                         }}
}
void scrie(char *msg,void *arg){
    struct thData tdL = *((struct thData *)arg);
    if (write(tdL.cl, msg, strlen(msg) + 1) <= 0) 
            printf("[thread %d] Eroare la write().\n", tdL.idThread);
        
}
void scrie_tabla(void *arg){
    struct thData tdL = *((struct thData *)arg);
    char board_str[1024] = "";
    strcat(board_str, "\n");
    for(int i=0;i<15;i++)
    {
        for(int j=0;j<15;j++)
            sprintf(board_str + strlen(board_str), "%c ", board[i][j]);
        strcat(board_str, "\n");
    }
    scrie(board_str,arg);
}
int main() {
    struct sockaddr_in server; 
    struct sockaddr_in from;   
    int sd;                    
    pthread_t th;             
    int i = 0;
    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("[server] Eroare la socket().");
        return errno;
    }
    int on = 1;
    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    bzero(&server, sizeof(server));
    bzero(&from, sizeof(from));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(PORT);
    if (bind(sd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1) {
        perror("[server] Eroare la bind().");
        return errno;
    }
    if (listen(sd, 5) == -1) {
        perror("[server] Eroare la listen().");
        return errno;
    }
    snprintf(stop, sizeof(stop), "STOP");
    printf("[server] Asteptam conexiuni la portul %d...\n", PORT);
    fflush(stdout);
    while (1) {
        int client;
        thData *td;
        int length = sizeof(from);
        if ((client = accept(sd, (struct sockaddr *)&from, &length)) < 0) {
            perror("[server] Eroare la accept().");
            continue;
        }
        td = (struct thData *)malloc(sizeof(struct thData));
        td->idThread = i++;
        td->cl = client;
        pthread_create(&th, NULL, raspunde, td); 
    }
    close(sd);
    return 0;
}
void *play_wait(void *arg){
    struct thData tdL = *((struct thData *)arg);
    char response[BUFFER_SIZE];
    snprintf(response, sizeof(response), " Am ajuns in functia play_wait ");
    scrie(response,arg);
    while(play==1)
        sleep(0.1);
    play_room(arg);
}
void pre_play(void *arg){
    struct thData tdL = *((struct thData *)arg);
    //if(number==2)
   // {
   // j[2].baza=3;
   // }
    init_jucator(arg);
    //sleep(2);
    pthread_mutex_lock(&mutex);
    play=2;
    pthread_mutex_unlock(&mutex);
    play_room(arg);
}
bool handle_add_user(thData *tdL, const char *username) {
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int rc = sqlite3_open("database.db", &db);
    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return false;
    }
    const char *sql = "INSERT INTO PLAYER (NAME, SCORE) VALUES (?, 0)";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to insert data: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return false;
    }
    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
    if (sqlite3_step(stmt) == SQLITE_DONE) {
        printf("[thread %d] User %s added successfully.\n", tdL->idThread, username);
        return true;
    } else {
        printf("[thread %d] Error adding user %s: %s\n", tdL->idThread, username, sqlite3_errmsg(db));
        return false;
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
}
void handle_ranking(thData *tdL) {
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int c=1;
    char stop[BUFFER_SIZE];
    char response[BUFFER_SIZE];
    int rc = sqlite3_open("database.db", &db);
    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return;
    }
    const char *sql = "SELECT NAME, SCORE FROM PLAYER ORDER BY SCORE DESC";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to fetch data: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }
   
    bzero(response, BUFFER_SIZE);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char *name = (const char *)sqlite3_column_text(stmt, 0);
        int score = sqlite3_column_int(stmt, 1);
        char row[BUFFER_SIZE];
        snprintf(row, sizeof(row), "%s: %d\n", name, score);
        strcat(response, row);
    }
    if (write(tdL->cl, response, strlen(response) + 1) <= 0) {
            printf("[thread %d] Eroare la write().\n", tdL->idThread);
            return;
        }
        sleep(1);
        snprintf(stop, sizeof(stop), "STOP");
        if (write(tdL->cl, stop, strlen(stop) + 1) <= 0) {
            printf("[thread %d] Eroare la write().\n", tdL->idThread);
            
        }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
}
bool handle_login(thData *tdL, const char *username) {
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int rc = sqlite3_open("database.db", &db);
    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return false;
    }
    const char *sql = "SELECT ID FROM PLAYER WHERE NAME = ?";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to fetch data: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return false;
    }
    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        tdL->id = sqlite3_column_int(stmt, 0);
        tdL->name = strdup(username);
        printf("[thread %d] User %s logged in with ID %d.\n", tdL->idThread, username, tdL->id);
        return true;
    } else {
        return false;
        printf("[thread %d] User %s not found.\n", tdL->idThread, username);
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
}
void handle_score(thData *tdL) {
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int rc = sqlite3_open("database.db", &db);
    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return;
    }
    const char *sql = "SELECT NAME, SCORE, WINS, LOSES, SECONDS, THIRDS FROM PLAYER WHERE ID = ?";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to fetch data: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }
    sqlite3_bind_int(stmt, 1, tdL->id);
    char response[BUFFER_SIZE];
    bzero(response, BUFFER_SIZE);
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const char *name = (const char *)sqlite3_column_text(stmt, 0);
        int score = sqlite3_column_int(stmt, 1);
        int wins = sqlite3_column_int(stmt, 2);
        int losses = sqlite3_column_int(stmt, 3);
        int seconds = sqlite3_column_int(stmt, 4);
        int thirds = sqlite3_column_int(stmt, 5);
        snprintf(response, sizeof(response),
                 "Name: %s\nScore: %d\nWins: %d\nLosses: %d\nSeconds: %d\nThirds: %d\n",
                 name, score, wins, losses, seconds, thirds);
    } else {
        snprintf(response, sizeof(response), "Player details not found.");
    }

   char stop[BUFFER_SIZE];
   snprintf(stop, sizeof(stop), "STOP");
    if (write(tdL->cl, response, strlen(response) + 1) <= 0) {
        printf("[thread %d] Eroare la write().\n", tdL->idThread);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

void *raspunde(void *arg) {
    struct thData tdL = *((struct thData *)arg);
    char buffer[BUFFER_SIZE];
    char stop[BUFFER_SIZE];
    tdL.id = -1;
    printf("[thread %d] Client conectat. Asteptam mesaje...\n", tdL.idThread);
    fflush(stdout);
    while (1) {
        bzero(buffer, BUFFER_SIZE);
        int bytesRead = read(tdL.cl, buffer, sizeof(buffer));
       if (bytesRead <= 0) {
            printf("[thread %d] Clientul a deconectat.\n", tdL.idThread);
            break;
        }
        printf("[thread %d] Mesaj primit: %s\n", tdL.idThread, buffer);
        char response[BUFFER_SIZE];
        int c=1;
        int ok;
        if(strcmp(buffer,"Ranking")==0){
            handle_ranking(&tdL);
        }else if (strncmp(buffer, "AddUser : ", 10) == 0) {
                char *username = buffer + 10;
                if(handle_add_user(&tdL, username))
                    snprintf(response, sizeof(response), "User %s added successfully.", username);
                else
                    snprintf(response, sizeof(response), "User not added.");
        }else if(strcmp(buffer,"Play")==0){
                    if(tdL.id!=-1){
                        if (number == 0) {
                              pthread_mutex_lock(&mutex);
                              number++;
                              pthread_mutex_unlock(&mutex);
                            //  j[0].id=tdL.id; 
                              j[1].cl = tdL.cl;
                               j[1].baza=1; j[1].turn = 1;
                             // sleep(1);
                              control_room(arg);
                        }else if (number < 4) { ok=1;
                            for(int i = 0; i < number; i++) {
                                if (j[i].id == tdL.id) {  snprintf(response, sizeof(response), "Nu poti juca cu tine insuti"); ok=0; break; }}
                            if(ok){int aux;
                            pthread_mutex_lock(&mutex);
                            number++;  aux = number; j[number].cl = tdL.cl;
                             j[number].baza=number; j[number].turn = number;
                            pthread_mutex_unlock(&mutex);
                            //j[aux-1].id=tdL.id;    
                            
                            sleep(1);
                            waiting_room(arg);}
                        }else 
                            snprintf(response, sizeof(response), "Nu mai sunt locuri disponibile în waiting_room.");
                    }else 
                         snprintf(response, sizeof(response), "Nu puteti juca daca nu sunteti logat");
        }else if(strcmp(buffer,"Score")==0){
            if(tdL.id==-1){  
                snprintf(response, sizeof(response), "Nu va putem comunoica scorul fara a fi logat.");}
            else 
                handle_score(&tdL);
        }else if (strncmp(buffer, "Login : ", 8) == 0){
                    if(tdL.id!=-1){ 
                            snprintf(response, sizeof(response), " Sunteti deja logat ");
                    }else{
                            char *username = buffer + 8;
                            if (handle_login(&tdL, username)) { 
                                    snprintf(response, sizeof(response), " Ati fost logat ");}
                            else
                                    snprintf(response, sizeof(response), " Wrong Username ");
                        }
        }else if (strcmp(buffer, "Logout") == 0){
                    tdL.id = -1;
                    snprintf(response, sizeof(response), " Ati fost deconectat ");
        }else{
                snprintf(response, sizeof(response), "%s : Nu e o comanda valida ", buffer);
             }
        //scrie
        
        if (write(tdL.cl, response, strlen(response) + 1) <= 0) {
            printf("[thread %d] Eroare la write().\n", tdL.idThread);
            break;
        }
        sleep(1);
        snprintf(stop, sizeof(stop), "STOP");
        if (write(tdL.cl, stop, strlen(stop) + 1) <= 0) {
            printf("[thread %d] Eroare la write().\n", tdL.idThread);
            break;
        }
    }
    close(tdL.cl);
    free(arg);
    pthread_exit(NULL);
}


void *waiting_room(void *arg){
    struct thData tdL = *((struct thData *)arg);
    char response[BUFFER_SIZE];
    snprintf(response, sizeof(response), " Am ajuns in functia waiting_room .");
    scrie(response,arg);

    while(play==0)
            sleep(0.1); 
    play_wait(arg);
}
void *control_room(void *arg){
    struct thData tdL = *((struct thData *)arg);
    char buffer2[256];
    char response[BUFFER_SIZE];
    snprintf(response, sizeof(response), "Am ajuns in functia control_room .");
    scrie(response,arg);    sleep(1);
    scrie(stop,arg);
    while(1){
            int bytesRead = read(tdL.cl, buffer2, sizeof(buffer2));
            if (strcmp(buffer2,"Play")==0){
                    if(number>=2)
                            {   pthread_mutex_lock(&mutex);
                                play=1;
                                pthread_mutex_unlock(&mutex);
                                init_nou();
                                pre_play(arg);
                                break;
                            }
                    else    {
                                snprintf(response, sizeof(response), " A fost identificata comanda Play dar nu sunt minim 2 jucatori pregatiti sa joace ");
                                scrie(response,arg);
                                scrie(stop,arg);
                            }   
                }
            else{       
                    snprintf(response, sizeof(response), "%s : Nu e o comanda valida ", buffer2);
                    scrie(response,arg);
                    sleep(0.2);
                    scrie(stop,arg);
                }   
    } 
}
void manage_turns(void *arg) {
    struct thData tdL = *((struct thData *)arg);
    char response[BUFFER_SIZE];
    char buffer[BUFFER_SIZE];
    int myturn = 0;
    int color;
    srand(time(NULL));
    for(int k = 1; k <= number; k++) {
        if(tdL.cl == j[k].cl) {
            myturn = j[k].turn;
            color = k;
            break;
        }
    }
    while(1) {
        if(turn == myturn) {
            pthread_mutex_lock(&mutex);
            int zar = rand() % 6 + 1;
            scrie_tabla(arg);
            sleep(1);
            snprintf(response, sizeof(response), "Este randul tau! Ai dat %d\n", zar);
            scrie(response, arg);
            sleep(1);
            if(exista_mutari_posiblile(color, zar)) {
                snprintf(response, sizeof(response), "Alege ce pion muti (1-4):\n");
                scrie(response, arg);
                sleep(1);
                 snprintf(stop, sizeof(stop), "STOP");
                scrie(stop,arg);
                int moved;
               // while(moved==0){
                bzero(buffer, BUFFER_SIZE);
                if(read(tdL.cl, buffer, sizeof(buffer)) > 0) {
                    sleep(1);
                    int pion = atoi(buffer);
                    if(pion >= 1 && pion <= 4 && valid_mutare(zar, pion, color)) {
                        muta(zar, pion, color); moved = 1;
                          //  scrie_tabla(arg);
                            if(win(color))
                            {
                                sleep(1);
                                snprintf(response, sizeof(response), "Felicitari! Ai castigat!\n");
                                scrie(response, arg);
                                 for(int k = 1; k <= number; k++) {
                                     if(j[k].turn != turn) {
                                            sleep(1);
                                            snprintf(response, sizeof(response), " Ai pierdut!\n");
                                            scrie(response, arg);
                                     }
                                     }
                                break;
                            }
                    }else{
                        sleep(1);
                        snprintf(response, sizeof(response), "Pozitia pionului este invalida!\n");
                        scrie(response, arg);
                    }
                }//}
            } else {
                snprintf(response, sizeof(response), "Nu puteti muta\n");
                scrie(response, arg);
            }
            
        
            //if(zar != 6) {  
                turn = (turn % number) + 1;//}
                /*
                for(int k = 1; k <= number; k++) {
                    if(j[k].turn != turn) {
                        char board_str[1024] = "";
                        strcat(board_str, "\n");
                        for(int i=0;i<15;i++)
                        {
                        for(int j=0;j<15;j++)
                        sprintf(board_str + strlen(board_str), "%c ", board[i][j]);
                        strcat(board_str, "\n");
                        }
                         if (write(j[k].cl, board_str, strlen(board_str) + 1) <= 0) 
                                printf("[thread %d] Eroare la write().\n", tdL.idThread);
        
                    }
                }*/
           
            pthread_cond_broadcast(&cond);  
            pthread_mutex_unlock(&mutex);
        } else {
            pthread_mutex_lock(&mutex);
            while(turn!=myturn)
            pthread_cond_wait(&cond, &mutex);
            pthread_mutex_unlock(&mutex);
        }
        if(win(color)) {
            snprintf(response, sizeof(response), "Felicitari! Ai castigat!\n");
            scrie(response, arg);
            break;
        }
        sleep(0.5); 
    }
}
void init_turns(void *arg) {
    pthread_mutex_lock(&mutex);
    turn = 1;  
    pthread_mutex_unlock(&mutex);
}
bool is_my_turn(void *arg) {
    struct thData tdL = *((struct thData *)arg);
    int myturn = 0;
    
    for(int k = 1; k <= number; k++) {
        if(tdL.cl == j[k].cl) {
            myturn = k;
            break;
        }
    }
    
    return (turn == myturn);
}
void next_turn(void *arg) {
    pthread_mutex_lock(&mutex);
    turn = (turn % number) + 1;
    pthread_mutex_unlock(&mutex);
}
void *play_room(void *arg) {
    struct thData tdL = *((struct thData *)arg);
    char response[BUFFER_SIZE];
   // scrie_tabla(arg);
    sleep(0.2);
    int myturn =0;
    for(int k=1;k<number;k++)
        if(tdL.cl==j[k].cl)
                 myturn=j[k].turn;
   // printf("%d",myturn);
    manage_turns(arg);
}
