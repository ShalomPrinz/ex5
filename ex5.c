#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FIRST_CHOICE 1
#define EXIT_CHOICE 4
#define EXIT_PLAYLIST_MENU_CHOICE 6
#define DEFAULT_SONG_STREAMS 0
#define DEFAULT_SONGS_NUM 0

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
    Song* songs;
    int songsNum;
} Playlist;

/*
 Inputs a valid main menu option and returns it
*/
int mainMenuChoice() {
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
        if (choice >= FIRST_CHOICE && choice <= playlistsCount + 1)
            break;

        // Invalid choice, input again for a valid playlist
        printf("Invalid option\n");
    } while (1);
    // (choice - 1) because menu starts from 1, but array starts from index 0
    return choice - 1;
}

/*
 Inputs a valid playlist menu choice and returns it
*/
int playlistMenuChoice() {
    // Default menu choice is "exit" to go back to playlist choice
    int choice = EXIT_PLAYLIST_MENU_CHOICE;
    do {
        printf("\t1. Show Playlist\n\t2. Add Song\n\t3. Delete Song\n\t4. Sort\n\t5. Play\n\t6. exit\n");
        scanf("%d", &choice);

        // Valid menu choice, stop the loop and return it
        if (choice >= FIRST_CHOICE && choice <= EXIT_PLAYLIST_MENU_CHOICE)
            break;

        // Invalid choice, input again for a valid menu choice
        printf("Invalid option\n");
    } while (1);
    return choice;
}

/*
 Frees a single song
 Note: song is not freed because it's assumed to be a part of an array
*/
void freeSong(Song* song) {
    // If song pointer is NULL, return without freeing it
    if (song == NULL) return;

    // Free song properties by order
    free(song->title);
    free(song->artist);
    free(song->lyrics);
}

/*
 Frees a single playlist
 Note: playlist is not freed because it's assumed to be a part of an array
*/
void freePlaylist(Playlist* playlist) {
    // If playlist pointer is NULL, return without freeing it
    if (playlist == NULL) return;

    // Free playlist properties by their order
    free(playlist->name);

    // If playlist songs pointer is NULL, return without freeing them
    if (playlist->songs == NULL) return;

    // Free each song in songs and then free the array pointer
    for (int song = 0; song < playlist->songsNum; song++)
        freeSong(&playlist->songs[song]);
    free(playlist->songs);
}

/*
 Frees all playlists
*/
void freeAll(Playlist** playlists, int playlistsCount) {
    // If playlists array pointer is NULL, return without freeing it
    if (playlists == NULL) return;

    for (int pl = 0; pl < playlistsCount; pl++)
        // Free each playlist. &(*playlists)[pl]: address of playlist number "pl"
        freePlaylist(&(*playlists)[pl]);

    // Free main playlists array pointer
    free(*playlists);
}

/*
 Exits program: exits with error code 1 due to allocation failure
*/
void exitProgram() {
    printf("Allocation failure: exit program\n");
    exit(1);
}

/*
 Inputs unlimited string (until enter is pressed) and returns the string pointer
*/
char* allocateInputString() {
    int size = 0;
    char* str = NULL;

    char c;
    scanf(" %c", &c);
    // Scan chars until enter is pressed
    while (c != '\n') {
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

        // Scan next char
        scanf("%c", &c);
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
    // Allocate space for the new playlist
    Playlist* newPlaylists = realloc(*playlists, (*playlistsCount + 1) * sizeof(Playlist));
    if (newPlaylists == NULL)
        exitProgram();
    *playlists = newPlaylists;

    printf("Enter playlist's name:\n");
    // Get new playlist name
    char* name = allocateInputString();
    if (name == NULL)
        exitProgram();

    // Set playlist properties
    Playlist* playlist = &(*playlists)[*playlistsCount];
    playlist->name = name;
    playlist->songs = NULL;
    playlist->songsNum = DEFAULT_SONGS_NUM;

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

/*
 Displays all songs and asks the user to choose a song index for the given "purpose"
 Returns user's choice
*/
int chooseSong(Playlist *playlist, char* purpose) {
    // Display all songs by given format
    for (int song = 0; song < playlist->songsNum; song++) {
        Song currentSong = playlist->songs[song];
        printf("%d. Title: %s\n   Artist: %s\n   Released: %d\n   Streams: %d\n\n",
            song + 1, currentSong.title, currentSong.artist, currentSong.year, currentSong.streams);
    }
    printf("choose a song to %s, or 0 to quit:\n", purpose);

    // Returns user's choice
    int choice;
    scanf("%d", &choice);
    return choice;
}

/*
 Asks the user to choose a song to play until user chooses to quit play mode
*/
void showPlaylist(Playlist* playlist) {
    int choice = chooseSong(playlist, "play");
    // Validates choice is in valid choice range (song index = choice - 1)
    while (choice > 0 && choice <= playlist->songsNum) {
        Song chosenSong = playlist->songs[choice - 1];
        printf("Now playing %s:\n$ %s $\nchoose a song to play, or 0 to quit:\n",
            chosenSong.title, chosenSong.lyrics);
        scanf("%d", &choice);
    }
}

void addSongToPlaylist(Playlist* playlist) {
    Song* newSongs = realloc(playlist->songs, (playlist->songsNum + 1) * sizeof(Song));
    if (newSongs == NULL)
        exitProgram();
    playlist->songs = newSongs;

    printf("Enter song's details\nTitle:\n");
    char* title = allocateInputString();
    if (title == NULL)
        exitProgram();

    printf("Artist:\n");
    char* artist = allocateInputString();
    if (artist == NULL)
        exitProgram();

    printf("Year of release:\n");
    int year;
    scanf("%d", &year);

    printf("Lyrics:\n");
    char* lyrics = allocateInputString();
    if (lyrics == NULL)
        exitProgram();

    Song* song = &playlist->songs[playlist->songsNum];
    song->title = title;
    song->artist = artist;
    song->year = year;
    song->lyrics = lyrics;
    song->streams = DEFAULT_SONG_STREAMS;

    // Increase playlist songs count by one
    playlist->songsNum++;
}

/*
 Inputs a song index and removes it from songs array of given playlist
*/
void removeSong(Playlist *playlist) {
    // Gets song index
    int choice = chooseSong(playlist, "delete");
    // Invalid song index / chosen one is 0 = exit
    if (choice <= 0 || choice > playlist->songsNum) return;

    // Free chosen song pointer
    freeSong(&playlist->songs[choice - 1]);
    for (int currentSong = choice - 1; currentSong < playlist->songsNum - 1; currentSong++)
        playlist->songs[currentSong] = playlist->songs[currentSong + 1];

    // Decrease playlist songs count by one
    playlist->songsNum--;
    printf("Song deleted successfully.\n");
}

/*
 Playlist actions menu, asks for user action choice and acts accordingly until exit menu is chosen
*/
void playlistMenu(Playlist* playlist) {
    // Prints playlist name once
    printf("playlist %s:\n", playlist->name);

    int plMenuChoice = playlistMenuChoice();
    while (plMenuChoice != EXIT_PLAYLIST_MENU_CHOICE) {
        switch (plMenuChoice) {
            // Show Playlist
            case 1:
                showPlaylist(playlist);
                break;

            // Add Song
            case 2:
                addSongToPlaylist(playlist);
                break;

            // Delete Song
            case 3:
                removeSong(playlist);
                break;

            // Sort
            case 4:
                break;

            // Play
            case 5:
                break;

            // exit
            case 6:
                break;

            default:
                break;
        }

        plMenuChoice = playlistMenuChoice();
    }
}

/*
 Playlist selection menu, inputs playlist numbers and acts accordingly until exit is chosen
*/
void selectPlaylistMenu(Playlist** playlists, int playlistsCount) {
    int selected = playlistChoice(playlists, playlistsCount);
    while (selected < playlistsCount) {
        playlistMenu(&(*playlists)[selected]);
        selected = playlistChoice(playlists, playlistsCount);
    }
}

/*
 Application main menu, inputs user choices and acts accordingly until exit is chosen
*/
void mainMenu(Playlist** playlists, int* playlistsCount) {
    int choice = mainMenuChoice();
    while (choice != EXIT_CHOICE) {
        switch (choice) {
            // Watch playlists
            case 1:
                selectPlaylistMenu(playlists, *playlistsCount);
                break;

            // Add playlist
            case 2:
                addPlaylist(playlists, playlistsCount);
                break;

            // Remove playlist
            case 3:
                removePlaylist(playlists, playlistsCount);
                break;

            // Exit
            case 4:
                break;

            default:
                printf("Invalid option\n");
                break;
        }
        choice = mainMenuChoice();
    }
}

int main() {
    // Init playlists
    Playlist* playlists = NULL;
    int playlistsCount = 0;

    // Run main menu
    mainMenu(&playlists, &playlistsCount);

    // Free all playlists before exit
    freeAll(&playlists, playlistsCount);

    // Exit program
    printf("Goodbye!\n");
}
