TRAY_CFLAGS := -DTRAY_APPINDICATOR=1 $(shell pkg-config --cflags appindicator3-0.1) $(shell pkg-config --cflags gtk+-3.0)
TRAY_LDFLAGS := $(shell pkg-config --libs appindicator3-0.1) $(shell pkg-config --libs gtk+-3.0)


CFLAGS := -Wall $(TRAY_CFLAGS) -Wall -Wextra -std=c99 -pedantic -O3
LDFLAGS := $(TRAY_LDFLAGS)

all: wireguard-tray
wireguard-tray: main.o
	$(CC) $^ $(LDFLAGS) -o $@
	
main.o: main.c tray.h

clean:
	rm -f main.o wireguard-tray

