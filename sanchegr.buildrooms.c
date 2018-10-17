#include <stdio.h>
#include <unistd.h>
#include <string.h>

void main() {
char dirRooms[] = "sanchegr.rooms.";
char pid[10];
snprintf(pid, 10, "%d", (int)getpid());
strcat(dirRooms, pid);
mkdir(dirRooms, 0755);
}
