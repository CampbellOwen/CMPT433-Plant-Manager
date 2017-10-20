DESKTOP_CC = gcc
BB_CC = arm-linux-gnueabihf-gcc

CC = $(BB_CC)
CFLAGS = -I./ -Wall -Werror -std=c99 -g -lm -lpthread -D _POSIX_C_SOURCE=199309L
SRC = $(wildcard include/*.c) $(wildcard src/*.c)
OUTNAME = plantManager
OUTDIR = bin
TARGET_DIR = $(HOME)/cmpt433/public/myApps
TARGET_NAME=$(OUTNAME)

all: 
	$(CC) $(CFLAGS) $(SRC) -o $(OUTDIR)/$(OUTNAME)
	cp $(OUTDIR)/$(OUTNAME) $(TARGET_DIR)/$(TARGET_NAME)

clean:
	rm $(OUTDIR)/$(OUTNAME) 
	rm $(TARGET_DIR)/$(TARGET_NAME) 

