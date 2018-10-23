#include <fcntl.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>

struct room {
	char room_name[9];
	char room_type[11];
	char room_connections[6][8];
	int room_num_connection;
};

struct room curRoom;
char dirPath[64];
char curPath[64];
char startPath[64];
char endPath[64];


void* cur_Time(void* arg) {
	FILE* fp;
	time_t curTime;
	struct tm *timeinfo;
	char buffer[64];

	time(&curTime);
	timeinfo = localtime(&curTime);
	strftime(buffer, 200, "%I:%M%p, %A, %B %d, %Y", timeinfo);
	
	fp = fopen("currentTime.txt", "w");
	fprintf(fp, "%s", outstr);
	fclose(fp);
	return NULL;
}


/*
 * Name: init_rooms
 * Descriptions: initialize the struct room names and types
 */
void get_recent_directory() {
	time_t newestChange=0;
	struct stat buf;		

	DIR* dir;
	struct dirent* file;

	dir = opendir(".");
	
	if(dir >0){
		while( (file = readdir(dir)) != NULL){
			if(strstr(file->d_name, "sanchegr.rooms.") != NULL) {
				stat(file->d_name, &buf);
				if(buf.st_mtime > newestChange) {
					newestChange = buf.st_mtime;
					memset(dirPath, '\0', sizeof(dirPath));
					strcpy(dirPath, file->d_name);
				}
			}
		}
	}
}

/*
 * Name: init_rooms
 * Descriptions: initialize the struct room names and types
 */
void get_start_final_room() {
	int fd;
	char filePath[64];
	ssize_t nread;
	char buffer[256];

	DIR* dir;
	struct dirent* file;

	dir = opendir(dirPath);

	if(dir > 0) {
		while( (file = readdir(dir)) != NULL ) {
			if(strstr(file->d_name, "_room") != NULL) {
				memset(filePath, '\0', sizeof(filePath));
				strcpy(filePath, dirPath);
				strcat(filePath, "/");
				strcat(filePath, file->d_name);

				fd = open(filePath, O_RDONLY);

				if (fd == -1) {
					printf("Unable to open %s\n", filePath);
					perror("In get_start_final_room()");
					exit(1);
				}

				memset(buffer, '\0', sizeof(buffer));
				lseek(fd, 0, SEEK_SET);
				nread = read(fd, buffer, sizeof(buffer));
	
				if(strstr(buffer, "START_ROOM") != NULL) {
					memset(startPath, '\0', sizeof(startPath));
					memset(curPath, '\0', sizeof(curPath));
					strcpy(startPath, filePath);
					strcpy(curPath, filePath);
				}
				else if (strstr(buffer, "END_ROOM") != NULL) {
					memset(endPath, '\0', sizeof(endPath));
					strcpy(endPath, filePath);
				}
				close(fd);
			}
		}
	}
}

void init_struct() {
	int i;
	memset(curRoom.room_name, '\0', sizeof(curRoom.room_name));
	memset(curRoom.room_type, '\0', sizeof(curRoom.room_type));
	for(i = 0; i < 7; i++) {
		memset(curRoom.room_connections[i], '\0', sizeof(curRoom.room_connections[i]));
	}
	curRoom.room_num_connection=0;
}


/*
 * Name: init_rooms
 * Descriptions: initialize the struct room names and types
 */
void get_room (char *room) {
	FILE *fp;
	char buffer[256];
		
	fp = fopen(room, "r");

	if (fp == NULL) {
		printf("Unable to open %s\n", room);
		perror("In get_start_final_room()");
		exit(1);
	}
	
	init_struct();	

	memset(buffer, '\0', sizeof(buffer));
	while( fgets(buffer, 256, fp) !=NULL) {
		char *p = strchr(buffer, '\n'); *p=0;
		if(buffer[5] == 'N') {
			strcpy(curRoom.room_name, strchr(buffer, ':')+2);
		} else if ( buffer[5] == 'T') {
			strcpy(curRoom.room_type, strchr(buffer, ':')+2); 
		} else {
			strcpy(curRoom.room_connections[curRoom.room_num_connection], strchr(buffer, ':')+2);
			curRoom.room_num_connection++;
		}
		memset(buffer, '\0', sizeof(buffer));
	}/*
	int i;
	printf("Room name: %s\n", curRoom.room_name);
	for(i = 0; i < curRoom.room_num_connection;i++) {
		printf("Connection %d: %s\n", i, curRoom.room_connections[i]);
	}
	printf("Connection num: %d\n", curRoom.room_num_connection);
	printf("Room Type: %s\n", curRoom.room_type); */
	fclose(fp);
}

int valid_room_selection(char *userChoice) {
	int i;
	for(i=0; i < curRoom.room_num_connection; i++) {
		if(strcmp(curRoom.room_connections[i], userChoice) == 0) {
			return 1;
		}
	}
	return 0;
}
/*
 * Name: init_rooms
 * Descriptions: initialize the struct room names and types
 */
void play_game() {
	char finalPath[1024];
	int finalCount=0;
	char userChoice[256];
	int i = 0;
	get_room(curPath);
	memset(finalPath, '\0', sizeof(finalPath));
	while(strcmp(curPath, endPath) != 0) {
		printf("CURRENT LOCATION: %s\n", curRoom.room_name);
		printf("POSSIBLE CONNECTIONS: ");
		for(i = 0; i < curRoom.room_num_connection; i++) {
			printf("%s", curRoom.room_connections[i]);
			if( (i+1) < curRoom.room_num_connection) {
				printf(", ");
			} else {
				printf(".");
			}
		}
		printf("\nWHERE TO? >");
		memset(userChoice, '\0', sizeof(userChoice));
		if(fgets(userChoice, sizeof(userChoice), stdin) != NULL) {
			char *p = strchr(userChoice, ' '); 
			if(p != NULL){ 
				*p=0; 
			} else {
				char *k = strchr(userChoice, '\n'); 
				*k=0;
			}
			if(valid_room_selection(userChoice) == 1) {
				printf("\n\n");
				memset(curPath, '\0', sizeof(curPath));
				finalCount++;
				strcat(finalPath, userChoice); strcat(finalPath, "\n");
				strcpy(curPath, dirPath); strcat(curPath, "/"); strcat(curPath, userChoice); strcat(curPath, "_room");
				get_room(curPath);	
			} else {
				printf("\nHUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n\n");
			}
		}
	}
	printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\nYOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n%s", finalCount, finalPath);
	exit(0);
}

void main () {
	get_recent_directory();
	get_start_final_room();
	/*play_game();*/
}
