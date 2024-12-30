#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FIRST_CHOICE 1
#define EXIT_CHOICE 4

// TODO validate all string.h had been seen in TIRGUL

typedef struct Song {
    char* title;
    char* artist;
    int year;
    char* lyrics;
    int streams;
} Song;

typedef struct Playlist {
    char* name;
    Song** songs;
    int songsNum;
} Playlist;

/*
 Inputs a valid main menu option and returns it
*/
int playlistManagerChoice() {
    // Default choice is "exit" to exit program
    int choice = EXIT_CHOICE;
    do {
        printf("Please Choose:\n");
        printf("\t1. Watch playlists\n\t2. Add playlist\n\t3. Remove playlist\n\t4. exit\n");
        scanf("%d", &choice);

        // Valid choice, stop the loop and return it
        if (choice >= FIRST_CHOICE && choice <= EXIT_CHOICE)
            break;

        // Invalid choice, input again for a valid choice
        printf("Invalid option\n");
    } while (1);
    return choice;
}

/*
 Inputs a valid playlist number and returns it
*/
int playlistChoice(Playlist** playlists, int playlistsCount) {
    // Default choice is "back to main menu"
    int choice = playlistsCount;
    do {
        printf("Choose a playlist:\n");
        int plNum = 1;
        // TODO is this for allowed? check coding style how
        // Print playlists menu, including "back to main menu" option
        for (; plNum - 1 < playlistsCount; plNum++)
            printf("\t%d. %s\n", plNum, (*playlists)[plNum - 1].name);
        printf("\t%d. Back to main menu\n", plNum);
        scanf("%d", &choice);

        // Valid choice, stop the loop and return it
        if (choice >= FIRST_CHOICE || choice <= playlistsCount)
            break;

        // Invalid choice, input again for a valid playlist
        printf("Invalid option\n");
    } while (1);
    // (choice - 1) because menu starts from 1, but array starts from index 0
    return choice - 1;
}

/*
 Frees a single playlist
*/
void freePlaylist(Playlist* playlist) {
    if (playlist == NULL) return;

    // Free playlist properties by their order
    free(playlist->name);
    if (playlist->songs != NULL) {
        for (int song = 0; song < playlist->songsNum; song++) {
            // TODO remove song dedicated function
            free(playlist->songs[song]->title);
            free(playlist->songs[song]->artist);
            free(playlist->songs[song]->lyrics);
        }
        free(playlist->songs);
    }
}

/*
 Frees all playlists
*/
void freeAll(Playlist** playlists, int playlistsCount) {
    if (playlists == NULL) return;

    for (int pl = 0; pl < playlistsCount; pl++)
        // Free each playlist. &(*playlists)[pl]: address of playlist number "pl"
        freePlaylist(&(*playlists)[pl]);
}

// TODO test free logic here
/*
 Safely exits program: prints given message and frees all playlists
*/
void safeExit(char* message, Playlist** playlists, int playlistsCount) {
    printf("%s\n", message);
    freeAll(playlists, playlistsCount);
    exit(1);
}

/*
 Inputs unlimited string (until enter is pressed) and returns the string pointer
*/
char* allocateInputString() {
    int size = 0;
    char* str = NULL;

    // Clean the buffer before string input
    getchar();
    char c;
    // Scan chars until enter is pressed
    while (scanf("%c", &c) == 1 && c != '\n') {
        // Allocate space for the new char
        size++;
        char* temp = realloc(str, (size + 1) * sizeof(char));
        // Allocation failed - free built string before function exit
        if (temp == NULL) {
            free(str);
            return NULL;
        }

        // Successful new char allocation, add it to the built string
        str = temp;
        str[size - 1] = c;
    }

    // Return NULL if no chars were scanned, otherwise return string with null terminator
    if (str != NULL)
        str[size] = '\0';
    return str;
}

/*
 Inputs a playlist name and adds it to playlists array
*/
void addPlaylist(Playlist** playlists, int* playlistsCount) {
    printf("Enter playlist's name:\n");
    // Get new playlist name
    char* name = allocateInputString();
    if (name == NULL)
        safeExit("Playlist name input failed (realloc)", playlists, *playlistsCount);

    // Allocate space for the new playlist
    Playlist* newPlaylists = realloc(*playlists, (*playlistsCount + 1) * sizeof(Playlist));
    if (newPlaylists == NULL) {
        free(name);
        safeExit("Add playlist failed (realloc)", playlists, *playlistsCount);
    }
    *playlists = newPlaylists;

    // Set playlist properties
    (*playlists)[*playlistsCount].name = name;
    (*playlists)[*playlistsCount].songs = NULL;
    (*playlists)[*playlistsCount].songsNum = 0;

    // Increase playlists count by one
    (*playlistsCount)++;
}

/*
 Inputs a playlist number and removes it from playlists array
*/
void removePlaylist(Playlist** playlists, int* playlistsCount) {
    // Gets playlist number
    int plChoice = playlistChoice(playlists, *playlistsCount);
    // Invalid playlist number / chosen one is "back to main menu"
    if (plChoice < 0 || plChoice >= *playlistsCount) return;

    // Free chosen playlist pointer
    freePlaylist(&(*playlists)[plChoice]);
    // Move all playlists (after chosen playlist) one step back in playlists array
    for (int currentPl = plChoice; currentPl < *playlistsCount - 1; currentPl++)
        (*playlists)[currentPl] = (*playlists)[currentPl + 1];

    // Decrease playlists count by one
    (*playlistsCount)--;
    printf("Playlist deleted.\n");
}

int main() {
    // Init playlists
    Playlist* playlists = NULL;
    int playlistsCount = 0;

    int choice = playlistManagerChoice();
    while (choice != EXIT_CHOICE) {
        switch (choice) {
            // Watch playlists
            case 1:
                int plChoice = playlistChoice(&playlists, playlistsCount);
                // User chose "back to main menu"
                if (plChoice == playlistsCount) break;
                // TODO Enter specific playlist menu

            // Add playlist
            case 2:
                addPlaylist(&playlists, &playlistsCount);
                break;

            // Remove playlist
            case 3:
                removePlaylist(&playlists, &playlistsCount);
                break;

            // Exit
            case 4:
                break;

            default:
                printf("Invalid option\n");
                break;
        }
        choice = playlistManagerChoice();
    }

    // Free all playlists before exit
    freeAll(&playlists, playlistsCount);
    free(playlists);

    // Exit program
    printf("Goodbye!\n");
}
