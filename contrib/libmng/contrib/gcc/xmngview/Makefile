CFLAGS=-g -Wall
MOTIFLIB=/usr/lib
MOTIFINC=/usr/include
MNGLIB=-lmng
LIBS=-L/usr/lib -L$(MOTIFLIB) -lXm -lXt -lX11 $(MNGLIB) -lm
INC=-I/usr/include -I$(MOTIFINC)
CC=gcc
LDFLAGS=

all: clean compile

compile:
	$(CC) $(CFLAGS) $(INC) color.c xmngview.c -o xmngview $(LIBS)

clean:
	rm -f xmngview core 

install:
	cp -a xmngview  /usr/local/bin/xmngview
