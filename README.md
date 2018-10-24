# CS344-program2
This assignment asks you to write a simple game akin to old text adventure games like Adventure:

[http://en.wikipedia.org/wiki/Colossal_Cave_Adventure](http://en.wikipedia.org/wiki/Colossal_Cave_Adventure)

You'll write two programs that will introduce you to programming in C on UNIX based systems, and will get you familiar with reading and writing files. 

# Overview

This assignment is split up into two C programs (no other languages is allowed). The first program (hereafter called the "rooms program") will be contained in a file named "<*STUDENT ONID USERNAME*>.buildrooms.c", which when compiled with the same name (minus the extension) and run creates a series of files that hold descriptions of the in-game rooms and how the rooms are connected.

The second program (hereafter called the "game") will be called "<*STUDENT ONID USERNAME*>.adventure.c" and when compiled with the same name (minus the extension) and run provides an interface for playing the game using the most recently generated rooms.

In the game, the player will begin in the "starting room" and will win the game automatically upon entering the "ending room", which causes the game to exit, displaying the path taken by the player.

During the game, the player can also enter a command that returns the current time - this functionality utilizes mutexes and multithreading.

For this assignment, do not use the C99 standard: this should be done using raw C (which is C89). In the complete example and grading instructions below, note the absence of the -c99 compilation flag.

# Specifications

## Rooms Program

The first thing your rooms program must do is create a directory called "<YOUR STUDENT ONID USERNAME>.rooms.<PROCESS ID OF ROOMS PROGRAM>". Next, it must generate 7 different room files, which will contain one room per file, in the directory just created. You may use any filenames you want for these 7 room files, and these names should be hard-coded into your program. For example, if John Smith was writing the program, he might see this directory and filenames. Note that 19903 was the PID of the rooms program at the time it was executed, and was not hard-coded:
  
```sh
$ ls smithj.rooms.19903
Crowther_room Dungeon_room PLUGH_room PLOVER_room twisty_room XYZZY_room Zork_room
```
The elements that make up an actual room defined inside a room file are listed below, along with some additional specifications:

* A Room Name
     * A room name cannot be assigned to more than one room.
     * Each name can be at max 8 characters long, with only uppercase and lowercase letters allowed (thus, no numbers, special characters, or spaces). This restriction is not extended to the room file's filename.
      * You must hard code a list of ten different Room Names into your rooms program and have your rooms program randomly assign one of these to each room generated. Thus, for a given run of your rooms program, 7 of the 10 hard-coded room names will be used.
* A Room Type
     * The possible room type entries are: START_ROOM, END_ROOM, and MID_ROOM.
     * The assignment of which room gets which type should be randomly generated each time the rooms program is run.
     * Naturally, only one room should be assigned the START_ROOM type, and only one room should be assigned the END_ROOM type. The rest of the rooms will receive the MID_ROOM type.
* Outbound connections to other rooms
     * There must be at least 3 outbound connections and at most 6 outbound connections from this room to other rooms.
     * The oubound connections from one room to other rooms should be assigned randomly each time the rooms program is run.
     * Outbound connections must have matching connections coming back: if room A connects to room B, then room B must have a connection back to room A. Because of all of these specs, there will always be at least one path through.
     * A room cannot have an outbound connection that points to itself.
     * A room cannot have more than one outbound connection to the same room.
