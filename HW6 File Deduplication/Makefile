
TARGET = main

SOURCES = main.cpp

CC = g++

CFLAGS = -Wall -lssl -lcrypto -Wno-deprecated-declarations

$(TARGET): $(SOURCES)
	$(CC) -o $(TARGET) $(SOURCES) $(CFLAGS)

clean:
	rm -f $(TARGET)
