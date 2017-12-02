DESKTOP_CC = gcc
BB_CC = arm-linux-gnueabihf-gcc

CC = $(BB_CC)
CFLAGS = -I./ -Wall -Werror -std=c99 -g -ldl -lm -lpthread -D _POSIX_C_SOURCE=199309L -pthread
LFLAGS = -Llib
INCLUDE = $(wildcard include/*.c)
PROD_MAIN = main.c
SRC = $(wildcard src/*.c)
OBJECTS=$(patsubst %.c, %.o, $(SRC))
OUTDIR = bin
OUT_NAME = plantManager
TARGET_DIR = $(HOME)/cmpt433/public/myApps
TARGET_NAME=$(OUT_NAME)
DB_NAME = plants.db

EXEC=$(OUTDIR)/$(OUT_NAME)

all: build $(EXEC)

deploy: all init_db

$(EXEC): $(OBJECTS)
	$(CC) $(INCLUDE) $(OBJECTS) -o $@ $(CFLAGS) $(LFLAGS)
	cp $(EXEC) $(TARGET_DIR)/$(TARGET_NAME)

$(OBJECTS): src/%.o : src/%.c
	$(CC) $(INCLUDE) -c $< -o $@ $(CFLAGS) $(LFLAGS)

build:
	@mkdir -p $(OUTDIR)

clean:
	rm -rf $(OUTDIR)
	rm -f $(OBJECTS)
	rm -f $(TARGET_DIR)/$(TARGET_NAME)

init_db:
	rm -f $(OUTDIR)/$(DB_NAME)
	cat sql/init.sql | sqlite3 $(OUTDIR)/$(DB_NAME)
	cp $(OUTDIR)/$(DB_NAME) $(TARGET_DIR)/$(DB_NAME)
