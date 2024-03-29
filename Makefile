CC = gcc
SRCDIR = ./src
BUILD_DIR = ./build

# Add flags if needed
CFLAGS = -lpthread

# Add common files if needed
COMMONFILES = $(SRCDIR)/common/*.c  $(SRCDIR)/common/cjson/*.c  $(SRCDIR)/common/socketUtils/*.c 
SIMULADORFOLDERS =  $(SRCDIR)/simulador/socketServer/*.c

# List of source files for monitor and simulador
MONITOR_SRC = $(SRCDIR)/monitor/*.c
SIMULADOR_SRC = $(SRCDIR)/simulador/*.c

# List of object files for monitor and simulador
MONITOR_OBJ = $(BUILD_DIR)/monitor
SIMULADOR_OBJ = $(BUILD_DIR)/simulador

.PHONY: build run-simulador run-monitor clean

default: build 

init:
	mkdir -p $(BUILD_DIR) $(SRCDIR)

build: monitor simulador

monitor:
	$(CC) -g $(MONITOR_SRC) $(COMMONFILES) $(CFLAGS) -o $(MONITOR_OBJ)

simulador:
	$(CC) -g $(SIMULADOR_SRC) $(COMMONFILES) $(SIMULADORFOLDERS) $(CFLAGS) -o $(SIMULADOR_OBJ)

run-monitor:
	$(MONITOR_OBJ)

run-simulador:
	$(SIMULADOR_OBJ) $(FILE)

clean:
	rm -f $(BUILD_DIR)/*
