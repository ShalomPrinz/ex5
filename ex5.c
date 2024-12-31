#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FIRST_CHOICE 1
#define EXIT_CHOICE 4
#define EXIT_PLAYLIST_MENU_CHOICE 6
#define DEFAULT_SONG_STREAMS 0
#define DEFAULT_SONGS_NUM 0

// TODO validate all string.h had been seen in TIRGUL
// TODO before commit fix add playlist, remove getchar because 2shalom

typedef enum Action {
    NO_ACTION = 0,
    SAFE_EXIT = 1
} Action;

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
        if (choice >= FIRST_CHOICE || choice <= playlistsCount)
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
        if (choice >= FIRST_CHOICE || choice <= EXIT_PLAYLIST_MENU_CHOICE)
            break;

        // Invalid choice, input again for a valid menu choice
        printf("Invalid option\n");
    } while (1);
    return choice;
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
            free(playlist->songs[song].title);
            free(playlist->songs[song].artist);
            free(playlist->songs[song].lyrics);
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

// TODO test free logic here + maybe shoudln't exit? check instructions
/*
 Safely exits program: frees all playlists and exits with error code 1
*/
void safeExit(Playlist** playlists, int playlistsCount) {
    printf("Allocation failure: free all and exit safely\n");
    freeAll(playlists, playlistsCount);
    free(*playlists);
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
        safeExit(playlists, *playlistsCount);
    *playlists = newPlaylists;

    printf("Enter playlist's name:\n");
    // Get new playlist name
    char* name = allocateInputString();
    if (name == NULL)
        safeExit(playlists, *playlistsCount);

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

void showPlaylist(Playlist* playlist) {
    for (int song = 0; song < playlist->songsNum; song++) {
        Song currentSong = playlist->songs[song];
        printf("%d. Title: %s\n   Artist: %s\n   Released: %d\n   Streams: %d\n\n",
            song + 1, currentSong.title, currentSong.artist, currentSong.year, currentSong.streams);
    }
    printf("choose a song to play, or 0 to quit:\n");

    int choice;
    scanf("%d", &choice);
    while (choice > 0 && choice <= playlist->songsNum) {
        Song chosenSong = playlist->songs[choice - 1];
        printf("Now playing %s:\n$ %s $\nchoose a song to play, or 0 to quit:\n",
            chosenSong.title, chosenSong.lyrics);
        scanf("%d", &choice);
    }
}

Action addSongToPlaylist(Playlist* playlist) {
    Song* newSongs = realloc(playlist->songs, (playlist->songsNum + 1) * sizeof(Song));
    if (newSongs == NULL)
        return SAFE_EXIT;
    playlist->songs = newSongs;

    printf("Enter song's details\nTitle:\n");
    char* title = allocateInputString();
    if (title == NULL)
        return SAFE_EXIT;

    printf("Artist:\n");
    char* artist = allocateInputString();
    if (artist == NULL)
        return SAFE_EXIT;

    printf("Year of release:\n");
    int year;
    scanf("%d", &year);

    printf("Lyrics:\n");
    char* lyrics = allocateInputString();
    if (lyrics == NULL)
        return SAFE_EXIT;

    Song* song = &playlist->songs[playlist->songsNum];
    song->title = title;
    song->artist = artist;
    song->year = year;
    song->lyrics = lyrics;
    song->streams = DEFAULT_SONG_STREAMS;

    // Increase playlist songs count by one
    playlist->songsNum++;
    return NO_ACTION;
}

/*
 Playlist actions menu, inputs actions and acts accordingly until exit is chosen
 Returns an Action which describes what action should be taken when this function returns
*/
Action playlistMenu(Playlist* playlist) {
    // Prints playlist name once
    printf("playlist %s:\n", playlist->name);

    Action action = NO_ACTION;
    int plMenuChoice = playlistMenuChoice();
    while (plMenuChoice != EXIT_PLAYLIST_MENU_CHOICE) {
        switch (plMenuChoice) {
            // Show Playlist
            case 1:
                showPlaylist(playlist);
                break;

            // Add Song
            case 2:
                action = addSongToPlaylist(playlist);
                break;

            // Delete Song
            case 3:
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

        if (action == SAFE_EXIT)
            return SAFE_EXIT;
        plMenuChoice = playlistMenuChoice();
    }

    return NO_ACTION;
}

/*
 Playlist selection menu, inputs playlist numbers and acts accordingly until exit is chosen
*/
void selectPlaylistMenu(Playlist** playlists, int playlistsCount) {
    int selected = playlistChoice(playlists, playlistsCount);
    while (selected < playlistsCount) {
        Action action = playlistMenu(&(*playlists)[selected]);
        if (action == SAFE_EXIT)
            safeExit(playlists, playlistsCount);

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
    free(playlists);

    // Exit program
    printf("Goodbye!\n");
}
