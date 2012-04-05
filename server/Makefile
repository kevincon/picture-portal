# ********************************************************************** #
#                      	     PICTUREPORTAL  		                     #
#                              Makefile                                  #
#                                                                        #
# William Etter (MSE '12)                             		             #
# Kevin Conley (BSE '12)                             		             #
# Teddy Zhang (BSE/MSE '12)                             		         #
# ********************************************************************** #

PROGN := picture-portal

CC := clang

CFLAGS := -Wall -g #-m32 -lpthread

PSOURCES := picture-portal.c linkedlist.c serialcomm.c

all: p1

p1: $(P1SOURCES)
	$(CC) $(CFLAGS) $(PSOURCES) -o $(PROGN)


clean :
	rm $(PROGN)