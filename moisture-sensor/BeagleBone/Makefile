OUTFILE = dht_moisture
OUTDIR = $(HOME)/cmpt433/public/myApps
CROSS_COMPILE = arm-linux-gnueabihf-
CC_C = $(CROSS_COMPILE)gcc
CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror -pthread

all:
	$(CC_C) $(CFLAGS) src/main.c src/i2c.c src/general.c -lm src/dht_moisture.c -o $(OUTDIR)/$(OUTFILE)

noworky:
	$(CC_C) $(CFLAGS) noworky.c -o $(OUTDIR)/noworky

help:
	@echo "Build DHT moisture sensor program for BeagleBone Black"
	@echo "Targets include all, noworky, and clean"

clean:
	rm $(OUTDIR)/$(OUTFILE)
