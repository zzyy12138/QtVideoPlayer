.SUFFIXES: .c .o

CC=gcc


SRCS=src/server.c\
		src/pub.c\
		src/work.c
		
WDSRCS=src/s.c
PASSSRCS=src/pass.c
		
OBJS=$(SRCS:.c=.o)
WDOBJS=$(WDSRCS:.c=.o)
PASSOBJS=$(PASSSRCS:.c=.o)

EXEC=src/myhttpd
WDEXEC=s.cgi
PASSEXEC=pass.cgi

all: $(OBJS) $(WDOBJS) $(PASSOBJS) 
	$(CC) -o $(EXEC) $(OBJS) -lpthread
	$(CC) -o $(WDEXEC) $(WDOBJS)
	$(CC) -o $(PASSEXEC) $(PASSOBJS)
	chmod a+x myhttp
	@echo '-------------ok--------------'

.c.o: 
	$(CC) -Wall -g -o $@ -c $<


clean:
	rm -f $(OBJS) $(WDOBJS) $(PASSOBJS)
	rm -f core*

