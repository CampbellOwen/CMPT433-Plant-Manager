DESKTOP_CC = gcc
BB_CC = arm-linux-gnueabihf-gcc

CC = $(BB_CC)
CFLAGS = -I./ -Wall -Werror -std=c99 -g -lm -lpthread -ldl -D _POSIX_C_SOURCE=199309L
LFLAGS = -Llib
INCLUDE = $(wildcard include/*.c) 
PROD_MAIN = main.c
SRC = $(wildcard src/*.c)
TEST = $(wildcard test/*.c)
TESTNAME = test
OUTNAME = plantManager
OUTDIR = bin
TARGET_DIR = $(HOME)/cmpt433/public/myApps
TARGET_NAME=$(OUTNAME)
DB_NAME = plants.db

all: prod
runtest: 
	$(DESKTOP_CC) $(LFLAGS) $(CFLAGS) $(INCLUDE) $(SRC) $(TEST) -o $(OUTDIR)/$(TESTNAME)
	$(OUTDIR)/$(TESTNAME)

prod:
	$(CC) $(CFLAGS) $(LFLAGS) $(INCLUDE) $(SRC) $(PROD_MAIN) -o $(OUTDIR)/$(OUTNAME)
	cp $(OUTDIR)/$(OUTNAME) $(TARGET_DIR)/$(TARGET_NAME)

clean:
	rm $(OUTDIR)/$(TESTNAME)
	rm $(OUTDIR)/$(OUTNAME) 
	rm $(TARGET_DIR)/$(TARGET_NAME) 

init_db:
	rm $(OUTDIR)/$(DB_NAME)
	cat sql/init.sql | sqlite3 $(OUTDIR)/$(DB_NAME)
	cp $(OUTDIR)/$(DB_NAME) $(TARGET_DIR)/$(DB_NAME)
