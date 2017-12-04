default:	all

all:	a4Client1 a4Client2 a4Client3 a4Server1

a4Client1: 
	gcc -o a4Client1 a4Client1.c -lnsl -lpthread	
	
a4Client2:
	gcc -o a4Client2 a4Client2.c -lnsl -lpthread	
	
a4Client3:
	gcc -o a4Client3 a4Client3.c -lnsl -lpthread	

a4Server1:
	gcc -o a4Server1 a4Server1.c -lnsl -lpthread	

clean:
	rm -f a4Client1	
	rm -f a4Client2	
	rm -f a4Client3	
	rm -f a4Server1	

