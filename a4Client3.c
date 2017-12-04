#include <stdlib.h>
#include <stdio.h>
#include <sqlite3.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>


#define MAXLINE 4096 /*max text line length*/
#define SERV_PORT 16030 /*port*/
#define LEN 256

int
main(int argc, char **argv) 
{
 int sockfd;
 struct sockaddr_in servaddr;
 char sendline[MAXLINE], recvline[MAXLINE];
 char * sql;
 FILE * fp;
 time_t t = time(NULL);
 struct tm *tm = localtime(&t);
 char s[64];
 int i;
 

 alarm(300);  // to terminate after 300 seconds
	
 //basic check of the arguments
 //additional checks can be inserted
 if (argc !=3) {
  perror("Usage: TCPClient <Server IP> <Server Port>"); 
  exit(1);
 }
	
 //Create a socket for the client
 //If sockfd<0 there was an error in the creation of the socket
 if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) <0) {
  perror("Problem in creating the socket");
  exit(2);
 }
	
 //Creation of the socket
 memset(&servaddr, 0, sizeof(servaddr));
 servaddr.sin_family = AF_INET;
 servaddr.sin_addr.s_addr= inet_addr(argv[1]);
 servaddr.sin_port = htons((int)strtol(argv[2], (char **)NULL, 10));
 servaddr.sin_port =  htons(SERV_PORT); //convert to big-endian order
	
 //Connection of the client to the socket 
 if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr))<0) {
  perror("Problem in connecting to the server");
  exit(3);
 }
	
 while (1) {
	 for (i = 0; i < 2; i++)
	 {
		 if (i == 0)
		 {
			 sql = "DELETE from classics where title='Moby Dick'; ";
			 strcpy(sendline, sql);
			 send(sockfd, sendline, strlen(sendline), 0);

			 if (recv(sockfd, recvline, MAXLINE, 0) == 0) {
				 //error: server terminated prematurely
				 perror("The server terminated prematurely");
				 exit(4);
			 }
			 //time stuff
			 t = time(NULL);
			 tm = localtime(&t);
			 strftime(s, sizeof(s), "%c", tm);

			 //write to console
			 printf("Time : %s ", s);
			 printf("PID : %d ", getpid());
			 printf("String received from the server: %s \n", recvline);
			 /* open the file for writing*/
			 if (fp = fopen("a4Client1Log.txt", "w"))
			 {
				 /* write to log*/
				 fprintf(fp, "Time : %s ", s);
				 fprintf(fp, "PID : %d ", getpid());
				 fprintf(fp, "String received from the server: %s \n", recvline);
			 }
			 else
			 {
				 perror("File Write Error");
				 exit(1);
			 }

			 /* close the file*/
			 fclose(fp);
			 fputs(recvline, stdout);
			 sleep(2);

			 //wake up to display message
			 t = time(NULL);
			 tm = localtime(&t);
			 strftime(s, sizeof(s), "%c", tm);

			 //write to console
			 printf("Time : %s ", s);
			 printf("PID : %d ", getpid());
			 printf("Just woke up");
		 }
		 else
		 {
			 sql = "INSERT INTO `classics` (`author`, `title`, `category`, `year`, `isbn`) VALUES"
				 "('Herman Melville', 'Moby Dick', 'Fiction', '1851', '9780199535729');";
			 strcpy(sendline, sql);
			 send(sockfd, sendline, strlen(sendline), 0);

			 if (recv(sockfd, recvline, MAXLINE, 0) == 0) {
				 //error: server terminated prematurely
				 perror("The server terminated prematurely");
				 exit(4);
			 }
			 //time stuff
			 t = time(NULL);
			 tm = localtime(&t);
			 strftime(s, sizeof(s), "%c", tm);

			 //write to console
			 printf("Time : %s ", s);
			 printf("PID : %d ", getpid());
			 printf("String received from the server: %s \n", recvline);
			 /* open the file for writing*/
			 if (fp = fopen("a4Client1Log.txt", "w"))
			 {
				 /* write to log*/
				 fprintf(fp, "Time : %s ", s);
				 fprintf(fp, "PID : %d ", getpid());
				 fprintf(fp, "String received from the server: %s \n", recvline);
			 }
			 else
			 {
				 perror("File Write Error");
				 exit(1);
			 }

			 /* close the file*/
			 fclose(fp);
			 fputs(recvline, stdout);
			 sleep(2);

			 //wake up to display message
			 t = time(NULL);
			 tm = localtime(&t);
			 strftime(s, sizeof(s), "%c", tm);

			 //write to console
			 printf("Time : %s ", s);
			 printf("PID : %d ", getpid());
			 printf("Just woke up");
		 }
	 }
}

 exit(0);
}
