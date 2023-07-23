TRAY_CFLAGS := -DTRAY_APPINDICATOR=1 $(shell pkg-config --cflags appindicator3-0.1) $(shell pkg-config --cflags gtk+-3.0)
TRAY_LDFLAGS := $(shell pkg-config --libs appindicator3-0.1) $(shell pkg-config --libs gtk+-3.0)


CFLAGS := -g -Wall $(TRAY_CFLAGS) -Wall -Wextra -std=c99 -pedantic -O3
LDFLAGS := -g $(TRAY_LDFLAGS)

all: main
main: main.o
	$(CC) $^ $(LDFLAGS) -o $@
	
main.o: main.c tray.h

clean:
	rm -f main.o main main.exe
