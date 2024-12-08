CC = gcc
CFLAGS = -Wall -g
LDFLAGS = -lncurses

TARGET = simulation
SOURCES = main.c init.c lives.c
HEADERS = init.h lives.h

all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)
