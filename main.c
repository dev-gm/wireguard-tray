#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "tray.h"

#define TRAY_CONNECTED "mullvad-vpn"
#define TRAY_DISCONNECTED "stock_dialog-warning"

#define DEBUG

static struct tray tray;

static FILE *status_file;

#define LEN_CONN_TO 13
static char status[2][45] = {"Connected to \0", "Connected to \0"};
static int status_which = 0;

static bool status_changed = false;
static bool connected = false;
static bool update = false;

static void update_status() {
	fgets(status[!status_which] + LEN_CONN_TO, 32, status_file);
#ifdef DEBUG
	printf("Got mullvad status\n");
#endif
	if (strcmp(
		status[!status_which] + LEN_CONN_TO,
		status[status_which] + LEN_CONN_TO
	) != 0) {
		strncpy(status[status_which] + LEN_CONN_TO, status[!status_which] + LEN_CONN_TO, 45 - LEN_CONN_TO);
		status_which = !status_which;
		status_changed = true;
#ifdef DEBUG
		printf("status_changed is true\n");
#endif
	}
}

static void connect_cb(struct tray_menu *item) {
	(void)item;
	if (!connected) {
		system("sudo herd start wireguard-wg0");
#ifdef DEBUG
		printf("Connect\n");
#endif
	} else {
		system("sudo herd stop wireguard-wg0");
#ifdef DEBUG
		printf("Disconnect\n");
#endif
	}
	update = true;
}

static void quit_cb(struct tray_menu *item) {
#ifdef DEBUG
	printf("Quit tray\n");
#endif
	item->checked = true;
	tray_exit();
	tray_update(&tray);
}

static struct tray tray = {
	.icon = TRAY_DISCONNECTED,
	.menu = (struct tray_menu[]){
		{.text = status[0], .disabled = 1},
		{.text = "Connect", .cb = connect_cb, .checked = 0},
		{.text = "Quit tray", .cb = quit_cb}
	}
};

int main() {
	if (tray_init(&tray) < 0) {
		fprintf(stderr, "Failed to create tray\n");
		return 1;
	}
#ifdef DEBUG
	printf("Initialized tray\n");
#endif
	status_file = fopen("/var/lib/mullvad-status", "r");
	if (status_file == NULL) {
		fprintf(stderr, "Failed to get file pointer to mullvad-status\n");
		return 1;
	}
#ifdef DEBUG
	printf("Opened mullvad-status\n");
#endif
	while (tray_loop(0) == 0) {
		update_status();
#ifdef DEBUG
		printf("%s\n", status[status_which]);
#endif
		if (status_changed) {
			if (status[status_which] + LEN_CONN_TO == NULL) {
				tray.icon = TRAY_DISCONNECTED;
				tray.menu[0].text = "Disconnected";
				tray.menu[0].checked = tray.menu[1].checked = connected = false;
#ifdef DEBUG
				printf("Updated: Disconnected\n");
#endif
			} else {
			 	tray.icon = TRAY_CONNECTED;
				tray.menu[0].text = status[status_which];
				tray.menu[0].checked = tray.menu[1].checked = connected = true;
#ifdef DEBUG
				printf("Updated: Connected\n");
#endif
			}
			tray_update(&tray);
			status_changed = false;
		}
		if (!update)
			sleep(2);
	}
	return 0;
}
