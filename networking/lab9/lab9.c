#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>


#define NUM_ROUTES 7

typedef struct {
    uint32_t ip;
    uint32_t maskLength;
    char nexthop;
} Route;

Route routingTable[NUM_ROUTES];

uint32_t ipStrToNum(const char *ip) {
    uint32_t num = 0;
    int value;
    for (int i = 0; i < 4; i++) {
        sscanf(ip, "%x", &value);
        num = (num << 8) | value;
        while (*ip && *ip++ != '.');
    }
    return num;
}

void loadRoutingTable() {
    char *routes[] = {
        "C4.50.0.0/12",   "A",
        "C4.5E.10.0/20",  "B",
        "C4.60.0.0/12",   "C",
        "C4.68.0.0/14",   "D",
        "80.0.0.0/1",     "E",
        "40.0.0.0/2",     "F",
        "00.0.0.0/2",     "G"
    };

    for (int i = 0; i < NUM_ROUTES; i++) {
        char *route = routes[i*2];
        char *slash = strchr(route, '/');
        routingTable[i].ip = ipStrToNum(route);
        routingTable[i].maskLength = atoi(slash + 1);
        routingTable[i].nexthop = *routes[i*2 + 1];
    }
}

char getNexthopForPacket(const char *packetIP) {
    uint32_t ip = ipStrToNum(packetIP);
    char nexthop = '?';
    int maxMaskLength = -1;
    for (int i = 0; i < NUM_ROUTES; i++) {
        uint32_t mask = ~((1 << (32 - routingTable[i].maskLength)) - 1);
if ((ip & mask) == routingTable[i].ip && routingTable[i].maskLength > maxMaskLength) {
    nexthop = routingTable[i].nexthop;
    maxMaskLength = routingTable[i].maskLength;
}

    }
    return nexthop;
}

int main() {
    loadRoutingTable();

    char packetIP[16];
    printf("Enter IP address: ");
    scanf("%15s", packetIP);

    char nexthop = getNexthopForPacket(packetIP);
    printf("Nexthop for %s is: %c\n", packetIP, nexthop);

    return 0;
}
