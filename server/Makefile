# ********************************************************************** #
#                      	     PICTUREPORTAL  		                     #
#                              Makefile                                  #
#                                                                        #
# William Etter (MSE '12)                             		             #
# Kevin Conley (BSE '12)                             		             #
# Teddy Zhang (BSE/MSE '12)                             		         #
# ********************************************************************** #

PROGN := picture-portal

CC := gcc

CFLAGS := -Wall -g #-m32 -lpthread

PSOURCES := picture-portal.c linkedlist.c serialcomm.c portcomm.c

all: p1

p1: $(P1SOURCES)
	$(CC) $(CFLAGS) $(PSOURCES) -o $(PROGN)


clean :
	rm $(PROGN)