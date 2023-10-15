CC = gcc
SRCDIR = ./src
BUILD_DIR = ./build

# Add flags if needed
CFLAGS = -lpthread

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
	touch $(SRCDIR)/main.c

build: monitor simulador

monitor: $(MONITOR_OBJ)

$(MONITOR_OBJ): $(MONITOR_SRC)
	$(CC) $< $(CFLAGS) -o $@

simulador: $(SIMULADOR_OBJ)

$(SIMULADOR_OBJ): $(SIMULADOR_SRC)
	$(CC) $< $(CFLAGS) -o $@

run-monitor: monitor
	$(MONITOR_OBJ)

run-simulador: simulador
	$(SIMULADOR_OBJ)

clean:
	rm -f $(BUILD_DIR)/*
