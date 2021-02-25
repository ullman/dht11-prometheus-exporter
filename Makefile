PREFIX= /usr/local
BINDIR= $(PREFIX)/bin
TARGET= dht11-prometheus
CC	= gcc
SRCDIR = src
CFLAGS = -c -O2 -Wall
LDFLAGS =-lprom -lpromhttp -lmicrohttpd -lwiringPi
SOURCES= $(SRCDIR)/dht11.c $(SRCDIR)/prometheus.c
OBJECTS=$(SOURCES:.c=.o)


all: $(TARGET)

dht11-prometheus: $(SRCDIR)/dht11.o $(SRCDIR)/prometheus.o
	$(CC) $(LDFLAGS) $(SRCDIR)/dht11.o $(SRCDIR)/prometheus.o -o $@

install: all
	install -D $(TARGET) $(BINDIR)/$(TARGET)

uninstall: all
	  -rm $(BINDIR)/$(TARGET)
.c.o:
	$(CC) $(CFLAGS) $(XCFLAGS) $< -o $@

clean:
	rm -f $(TARGET) $(TEST_TARGET)
	rm -f $(OBJECTS)

.PHONY: all clean
