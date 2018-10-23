#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

struct room {
	char room_name[8];
	char room_type[10];
	char room_connections[6][8];
	int room_num_connection;
};

struct room rooms[7];
char dirRooms[] = "sanchegr.rooms.";
int start;

/*
 * Name: init_room_dir
 * Description: initialize a room directory with the PID number attached to the end (e.g. /sanchegr.rooms.99999)
 */
void init_room_dir() {
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
	int j;
	for(i=0; i < 7; i++) {
		int num_of_connec = rooms[i].room_num_connection;
		/* 
 		 * Loop through the current rooms connections array until a random number from 3 to 6
 		 * If a random room is not already in the connections array, add it to the current room and the random room
 		 * else keep looking for a random room until the random number is reached
 		 */
		for(num_of_connec; num_of_connec < (rand() % (6-3+1) + 3); num_of_connec++) {
			rand_room = rand() % 7;
			if(contains_connection(i, names[rand_room]) == 0){
				memset(rooms[i].room_connections[rooms[i].room_num_connection], '\0', sizeof(rooms[i].room_connections[rooms[i].room_num_connection]));
				memset(rooms[rand_room].room_connections[rooms[rand_room].room_num_connection], '\0', sizeof(rooms[rand_room].room_connections[rooms[rand_room].room_num_connection]));
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
	/* Find a start and ending room and assign their room types to it as long as the rooms are not equal */
	do {
		start = rand() % 7;
		final = rand() % 7;
		if (start != final) {
			memset(rooms[start].room_type, '\0', sizeof(rooms[start].room_type));
			memset(rooms[final].room_type, '\0', sizeof(rooms[final].room_type));

			strcpy(rooms[start].room_type, "START_ROOM");
			strcpy(rooms[final].room_type, "END_ROOM");
		}
	} while (start == final);

	/* Inizialize each room with a name, and number of connections. Also, assign the middle rooms to the other rooms */
	for(i=0; i < 7; i++) {
		rooms[i].room_num_connection=0;
		memset(rooms[i].room_name, '\0', sizeof(rooms[i].room_name));
		strcpy(rooms[i].room_name, names[i]);
		if(rooms[i].room_type[0] == '\0') {
			memset(rooms[i].room_type, '\0', sizeof(rooms[i].room_type));
			strcpy(rooms[i].room_type, types[2]);
		}
	}
	init_room_connection(names);
}

void init_room_files () {
	ssize_t nwritten;
	int file_descriptor, i, j;
	char file[30], text[30], buffer[3];

	/* Open a file and write to a file for every room */
	for(i =0; i < 7; i++) {
		/* Makes a file path for each room with the form "sanchegr.rooms.$$PID$$/Test_room" */
		memset(file, '\0', sizeof(file)); memset(text, '\0', sizeof(text)); memset(buffer, '\0', sizeof(buffer));
		strcpy(file, dirRooms);
		strcat(file, "/");
		strcat(file, rooms[i].room_name);
		strcat(file, "_room");
		file_descriptor = open(file, O_WRONLY | O_CREAT, 0600);

		if(file_descriptor < 0) {
			fprintf(stderr, "Could not open file %s\n", file);
			perror("Error in init_room_files()");
			exit(1);
		}

		/*For every room create each line using the variable text, and write it to the file */
		strcpy(text, "ROOM NAME: ");
		strcat(text, rooms[i].room_name);
		strcat(text, "\n");
		nwritten = write(file_descriptor, text, strlen(text) * sizeof(char));
		/*Loop through each connection and write each one to the file. */
		for(j=0; j < rooms[i].room_num_connection; j++) {
			sprintf(buffer, "%d", j);
			strcpy(text, "CONNECTION "); strcat(text, buffer); strcat(text, ": ");
			strcat(text, rooms[i].room_connections[j]);
			strcat(text, "\n");
			nwritten = write(file_descriptor, text, strlen(text) * sizeof(char));
		}
		strcpy(text, "ROOM TYPE: ");
		if(strstr(rooms[i].room_type, "START_ROOM")) {
			strcat(text, "START_ROOM");
		} else {
			strcat(text, rooms[i].room_type);
		}
		strcat(text, "\n");
		nwritten = write(file_descriptor, text, strlen(text) * sizeof(char));
		close(file_descriptor);
	}

}

void main () {
	srand(time(0));
	init_rooms();	
	init_room_dir();
	init_room_files();
}
