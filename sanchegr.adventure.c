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

pthread_mutex_t lock;

/*
 * Name: current time
 * Descriptions: Thread that creats and writes the current time and date to a text file
 */
void* cur_time(void* arg) {
	FILE *fp;
	time_t curTime;
	struct tm *timeinfo;
	char buffer[64];

	/*Lock the thread so no two threads can be going at the same time */
	pthread_mutex_lock(&lock);

	/*Create a buffer that holds the contents of the current time and date */
	time(&curTime);
	timeinfo = localtime(&curTime);
	strftime(buffer, 200, "%I:%M%p, %A, %B %d, %Y", timeinfo);
	
	/*Open and  the currentTime.txt file. Then close and unlock the thread*/
	fp = fopen("currentTime.txt", "w");
	fprintf(fp, "%s", buffer);
	fclose(fp);
	pthread_mutex_unlock(&lock);
	return NULL;
}


/*
 * Name: get recent directory
 * Descriptions: Scans through the current directory searcing for all the sanchegr.rooms.*
 * 			directorys and finds the most recently made one.
 */
void get_recent_directory() {
	time_t newestChange=0;
	struct stat buf;		

	DIR* dir;
	struct dirent* file;
	
	/*Open the current directory*/
	dir = opendir(".");
	
	if(dir >0){
		/*Scan through each file (including directories) in the directory */
		while( (file = readdir(dir)) != NULL){
			/*Only look at files(directories) that are sanchegr.rooms.* */
			if(strstr(file->d_name, "sanchegr.rooms.") != NULL) {
				/*Open the file information and check to see if the directory was made more recently*/
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
 * Name: init start and final rooms
 * Descriptions: Scans through the most recent rooms directory to search for 
 * 			the start and end room path locations
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
		/* Scan through each room file in the rooms directory */
		while( (file = readdir(dir)) != NULL ) {
			if(strstr(file->d_name, "_room") != NULL) {

				/*Create a path for the room file (e.g. "sanchegr.rooms.8765/Keep_room") */
				memset(filePath, '\0', sizeof(filePath));
				strcpy(filePath, dirPath);
				strcat(filePath, "/");
				strcat(filePath, file->d_name);

				/*Open the file for read only*/
				fd = open(filePath, O_RDONLY);

				if (fd == -1) {
					printf("Unable to open %s\n", filePath);
					perror("In get_start_final_room()");
					exit(1);
				}
				
				/*Reset the buffer and file pointer to get ready for reading */
				memset(buffer, '\0', sizeof(buffer));
				lseek(fd, 0, SEEK_SET);
				nread = read(fd, buffer, sizeof(buffer));
	
				/*If the buffer contains "START_ROOM" or "END_ROOM" then save the file path 
 				 * 	and make the current (starting location) to this room for starting the game */
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

/*
 * Name: init_struct
 * Descriptions: initialize the struct room names and types
 */
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
 * Name: get room
 * Descriptions: Scans through the current room the user is at and imports its contents into a struct
 * 			for easier use in displaying the room contents
 * Parameters: path to the current room (e.g. "sanchegr.rooms.8765/Keep_room")
 */
void get_room (char *room) {
	FILE *fp;
	char buffer[256];
	
	/*Open the needed room*/
	fp = fopen(room, "r");

	if (fp == NULL) {
		printf("Unable to open %s\n", room);
		perror("In get_start_final_room()");
		exit(1);
	}
	
	/*Initialize the struct so no memory errors occr when trying to assing strings*/
	init_struct();	

	/*Scan through each line in the room file*/
	memset(buffer, '\0', sizeof(buffer));
	while( fgets(buffer, 256, fp) !=NULL) {
		/*Remove any trailing new line charaters*/
		char *p = strchr(buffer, '\n'); *p=0;
		
		/*If the line is ROOM *N*AME, ROOM *T*YPE, or is a connection assign those values
 		 *	to the appropriate location the room struct */
		if(buffer[5] == 'N') {
			strcpy(curRoom.room_name, strchr(buffer, ':')+2);
		} else if ( buffer[5] == 'T') {
			strcpy(curRoom.room_type, strchr(buffer, ':')+2); 
		} else {
			strcpy(curRoom.room_connections[curRoom.room_num_connection], strchr(buffer, ':')+2);
			curRoom.room_num_connection++;
		}
		memset(buffer, '\0', sizeof(buffer));
	}
	fclose(fp);
}

/*
 * Name: valid room selection
 * Description: Scans through the crrent room to check if the user's room choice
 * 			picked a room that is connected to the current room
 */
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
 * Name: play game
 * Descriptions: initilizes and runs the game for the user
 */
void play_game() {
	FILE *fp;
	char buffer[64];

	char finalPath[1024];
	int finalCount=0;
	char userChoice[256];
	int i = 0, print_all=1;

	pthread_t timeThread;
	int resultCode;
	/*Create and initilize the mutex and time thread*/
	if(pthread_mutex_init(&lock, NULL) != 0) {
		printf("Mutex init has failed\n");
	}	
	resultCode = pthread_create(&timeThread, NULL, &cur_time, NULL);
	if(resultCode != 0)
		printf("THREAD can't be created : %s\n", strerror(resultCode));	
	pthread_mutex_lock(&lock);

	/*get the room contents of the start room*/
	get_room(curPath);
	memset(finalPath, '\0', sizeof(finalPath));


	/*===========================ACUTAL GAME=============================*/
	/*Make an infinite loop to play the game until the user finds the end room*/
	while(strcmp(curPath, endPath) != 0) {
		
		/* Give a prompt for the user. Showing current room lcation, what rooms are connected, and where the user wants to go*/
		if(print_all == 1) {
			printf("CURRENT LOCATION: %s\n", curRoom.room_name);
			printf("POSSIBLE CONNECTIONS: ");
			for(i = 0; i < curRoom.room_num_connection; i++) {
				printf("%s", curRoom.room_connections[i]);
				if( (i+1) < curRoom.room_num_connection) { printf(", "); } 
				else { printf("."); }
			}
			printf("\nWHERE TO? >");
		} /*If the user previously selected "time", only ask where to go*/ 
		else if (print_all == 0) {
			print_all=1;
			printf("WHERE TO? >");
		}
		memset(userChoice, '\0', sizeof(userChoice));

		/*Get the user input, and pick what to do with it*/
		if(fgets(userChoice, sizeof(userChoice), stdin) != NULL) {
			
			/*Clear any leading spaces or new lines*/
			char *p = strchr(userChoice, ' '); 
			if(p != NULL){ *p=0; } 
			else {
				char *k = strchr(userChoice, '\n'); 
				*k=0;
			}
	
			/*============================VALID ROOM SELECTION===========================*/
			/*If the user picked a room connected to the current room or wants to see the time*/
			if(valid_room_selection(userChoice) == 1) {

				/*Increment the final path count, append the room name to the final path ,
 				 * make the current path to the newly selected room */
				printf("\n\n");
				memset(curPath, '\0', sizeof(curPath));
				finalCount++;
				strcat(finalPath, userChoice); strcat(finalPath, "\n");
				strcpy(curPath, dirPath); strcat(curPath, "/"); strcat(curPath, userChoice); strcat(curPath, "_room");
				get_room(curPath);	

			} /*===========================TIME SELECTION================================*/ 
			else if (strcmp(userChoice, "time")==0){
				print_all=0;
	
				/*Unlock the mutex thread, run the time thread, lock the mutex thread, and create the new time thread*/
				pthread_mutex_unlock(&lock);
				resultCode = pthread_join(timeThread, NULL);
				pthread_mutex_lock(&lock);
				resultCode = pthread_create(&timeThread, NULL, &cur_time, NULL);
				if(resultCode != 0){
					printf("THREAD can't be created : %s\n", strerror(resultCode));
					exit(1);
				}
				
				/*Open (read only) the curret time text file containg the current locatl time, and print it out to the user*/
				fp = fopen("currentTime.txt", "r");
				if (fp == NULL) {
					printf("Unable to open %s\n", "currentTime.txt");
					perror("In play_game()");
					exit(1);
				}
				if(fgets(buffer, 64, fp) != NULL) {
					printf("\n%s\n\n", buffer);
				}
			} else { /*========================INVALID SELECTION=================*/
				printf("\nHUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n\n");
			
			}
		}
	}

	/*Destory the mutex and display the wining steps and path*/
	pthread_mutex_destroy(&lock);
	printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\nYOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n%s", finalCount, finalPath);
	exit(0);
}

void main () {
	get_recent_directory();
	get_start_final_room();
	play_game();
}
