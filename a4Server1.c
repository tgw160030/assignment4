#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <pthread.h>
#include <sqlite3.h> 
#include <unistd.h>
#include <time.h>

/*
    CONCURRENT SERVER: THREAD EXAMPLE
    Must be linked with the "pthread" library also, e.g.:
       cc -o example example.c -lnsl -lsocket -lpthread 

    This program creates a connection socket, binds a name to it, then
    listens for connections to the sockect.  When a connection is made,
    it accepts messages from the socket until eof, and then waits for
    another connection...

    This is an example of a CONCURRENT server -- by creating threads several
    clients can be served at the same time...

    This program has to be killed to terminate, or alternately it will abort in
    120 seconds on an alarm...
*/

#define PORTNUMBER 16030

static int callback(void *NotUsed, int argc, char **argv, char **azColName){
   int i;
   for(i=0; i<argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}

struct serverParm {
           int connectionDesc;
       };

void *serverThread(void *parmPtr) {

#define PARMPTR ((struct serverParm *) parmPtr)
    int recievedMsgLen;
    char messageBuf[1025];
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	char *sql;
	const char* data = "Callback function called";
	FILE * fp;
	

    /* Server thread code to deal with message processing */
    printf("DEBUG: connection made, connectionDesc=%d\n",
            PARMPTR->connectionDesc);
    if (PARMPTR->connectionDesc < 0) {
        printf("Accept failed\n");
        return(0);    /* Exit thread */
    }
    
    /* Receive messages from sender... */
    while ((recievedMsgLen=
            read(PARMPTR->connectionDesc,messageBuf,sizeof(messageBuf)-1)) > 0) 
    {
		
        recievedMsgLen[messageBuf] = '\0';
        printf("Message: %s\n",messageBuf);
		/* Open database */
		rc = sqlite3_open("book.db", &db);
		if( rc ){
			fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
			exit(0);
		}else{
		fprintf(stderr, "Opened database successfully\n");
		}
		
		sql = messageBuf;
		
		/* Execute SQL statement */
		rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);
		if( rc != SQLITE_OK ){
			fprintf(stderr, "SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
		}else{
			fprintf(stdout, "Operation done successfully\n");
		}
		sqlite3_close(db);
		/* open the file for writing*/
	 if (fp = fopen("a4ServerLog.txt", "w"))
	 {
		 //time stuff
		 time_t t = time(NULL);
		 struct tm *tm = localtime(&t);
		 char s[64];
		 strftime(s, sizeof(s), "%c", tm);
		 
		 //write to console
		printf("Time : %s ", s);
		printf("PID : %d ", getpid());
		printf("Thread ID : %d ", pthread_self());
		printf("Processing client request\n");
		 
		 /* write to log*/
		 fprintf(fp, "Time : %s ", s);
		 fprintf(fp, "PID : %d ", getpid());
		 fprintf(fp, "Thread ID : %d ", pthread_self());
		 fprintf(fp, "Processing client request\n");
	 }
	 else
	 {
		 perror("File Write Error");
		 exit(1);
	 }

	/* close the file*/
	fclose(fp);
		
		
        if (write(PARMPTR->connectionDesc,"GOT IT\0",7) < 0) {
               perror("Server: write error");
               return(0);
           }
    }
    close(PARMPTR->connectionDesc);  /* Avoid descriptor leaks */
    free(PARMPTR);                   /* And memory leaks */
    return(0);                       /* Exit thread */
}

main () {
    int listenDesc;
    struct sockaddr_in myAddr;
    struct serverParm *parmPtr;
    int connectionDesc;
    pthread_t threadID;
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	char *sql;
	
	rc = sqlite3_open("book.db", &db);
	
	if( rc ){
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
   }else{
      fprintf(stderr, "Opened database successfully\n");
   }
   
   /* Create SQL statement */
   sql = "CREATE TABLE IF NOT EXISTS `classics` ("
	   "`author` varchar(128) DEFAULT NULL,"
	   "`title` varchar(128) DEFAULT NULL,"
	   "`category` varchar(16) DEFAULT NULL,"
	   "`year` char(4) DEFAULT NULL,"
	   "`isbn` char(13) NOT NULL DEFAULT '',"
	   "PRIMARY KEY(`isbn`)"
	   ");";
	   
	/* Execute SQL statement */   
	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
	if( rc != SQLITE_OK ){
	fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
	}else{
      fprintf(stdout, "Table created successfully\n");
	}
	
	sql = "INSERT INTO `classics` (`author`, `title`, `category`, `year`, `isbn`) VALUES"
	   "('Mark Twain', 'The Adventures of Tom Sawyer', 'Fiction', '1876', '9781598184891'),"
	   "('Jane Austen', 'Pride and Prejudice', 'Fiction', '1811', '9780582506206'),"
	   "('Charles Darwin', 'The Origin of Species', 'Non-Fiction', '1856', '9780517123201'),"
	   "('Charles Dickens', 'The Old Curiosity Shop', 'Fiction', '1841', '9780099533474'),"
	   "('William Shakespeare', 'Romeo and Juliet', 'Fiction', '1594', '9780192814968'),"
	   "('Herman Melville', 'Moby Dick', 'Fiction', '1851', '9780199535729');";


   /* Execute SQL statement */
   rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
   if( rc != SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }else{
      fprintf(stdout, "Records created successfully\n");
   }
   sqlite3_close(db);

    /* For testing purposes, make sure process will terminate eventually */
    alarm(120);  /* Terminate in 120 seconds */

    /* Create socket from which to read */
    if ((listenDesc = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("open error on socket");
        exit(1);
    }

    /* Create "name" of socket */
    myAddr.sin_family = AF_INET;
    myAddr.sin_addr.s_addr = INADDR_ANY;
    myAddr.sin_port = htons(PORTNUMBER);
        
    if (bind(listenDesc, (struct sockaddr *) &myAddr, sizeof(myAddr)) < 0) {
        perror("bind error");
        exit(1);
    }

    /* Start accepting connections.... */
    /* Up to 5 requests for connections can be queued... */
    listen(listenDesc,5);

    while (1) /* Do forever */ {
        /* Wait for a client connection */
        connectionDesc = accept(listenDesc, NULL, NULL);

        /* Create a thread to actually handle this client */
        parmPtr = (struct serverParm *)malloc(sizeof(struct serverParm));
        parmPtr->connectionDesc = connectionDesc;
        if (pthread_create(&threadID, NULL, serverThread, (void *)parmPtr) 
              != 0) {
            perror("Thread create error");
            close(connectionDesc);
            close(listenDesc);
            exit(1);
        }

        printf("Parent ready for another connection\n");
    }

}
