#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

struct room {
	char room_name[8];
	char room_type[10];
	char room_connections[6][8];
	int room_num_connection;
};

struct room rooms[7];
int start;
/*
 * Name: init_room_dir
 * Description: initialize a room directory with the PID number attached to the end (e.g. /sanchegr.rooms.99999)
 */
void init_room_dir() {
	char dirRooms[] = "sanchegr.rooms.";
	char pid[10];
	
	sprintf(pid, "%d", (int)getpid());
	strcat(dirRooms, pid);
	mkdir(dirRooms, 0755);
}

/*
 * Name: contains_connection
 * Params: index of the room, room to compare
 * Description: Compares a room name to the array of room connections to see if it is in there
 * Return: returns 1 if the room was found in the array, or 0 if that room is not in the array
 */
int contains_connection(const int room_index, const char *room) {
	int i;
	if(strcmp(rooms[room_index].room_name, room) == 0) {
		return 1;
	}
	for(i=0; i < rooms[room_index].room_num_connection; i++) {
		if(strcmp(rooms[room_index].room_connections[i], room) == 0) {
			return 1;
		}
	}
	return 0;
}

/*
 * Name: init_room_connection
 * Params: name of hardcoded rooms
 * Description: Randomly assigns room connections to the array making sure there is only a 3 to 6 connections
 */
void init_room_connection(const char *names[]) {
	srand(time(0));
	int i, rand_room;
	for(i=0; i < 7; i++) {
		int num_of_connec = rooms[i].room_num_connection;
		for(num_of_connec; num_of_connec < (rand() % (6-3+1) + 3); num_of_connec++) {
			rand_room = rand() % 7;
			if(contains_connection(i, names[rand_room]) == 0){
				strcpy(rooms[i].room_connections[rooms[i].room_num_connection], names[rand_room]);
				strcpy(rooms[rand_room].room_connections[rooms[rand_room].room_num_connection], rooms[i].room_name);
				rooms[i].room_num_connection++;
				rooms[rand_room].room_num_connection++;
			}
			else
				num_of_connec--;
		}
	}

}
/*
 * Name: init_rooms
 * Descriptions: initialize the struct room names and types
 */
void init_rooms() {
	const char *names[] = {"Keep", "Plaza", "Dungeon", "Cave", "Hallway", "Vault", "Mine"};
	const char *types[] = {"START_ROOM", "END_ROOM", "MID_ROOM"};
	int i, final;
	
	do {
		start = rand() % 7;
		final = rand() % 7;
		if (start != final) {
			strcpy(rooms[start].room_type, types[0]);
			strcpy(rooms[final].room_type, types[1]);
		}
	} while (start == final);

	for(i=0; i < 7; i++) {
		rooms[i].room_num_connection=0;
		strcpy(rooms[i].room_name, names[i]);
		if(rooms[i].room_type[0] == '\0') {
			strcpy(rooms[i].room_type, types[2]);
		}
	}
	init_room_connection(names);	
	strcpy(rooms[start].room_type, types[0]);
}

void main () {
	srand(time(0));
	init_rooms();	

	int i,j;
	for(i =0; i < 7; i++) {
		printf("ROOM NAME: %s\n", rooms[i].room_name);
		for(j=0; j < rooms[i].room_num_connection; j++) {
			printf("CONNECTION: %s\n", rooms[i].room_connections[j]);
		}
		printf("ROOM TYPE: %s\n", rooms[i].room_type);
		printf("---\n");	
	}
	

}
