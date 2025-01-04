#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Any general menu first choice
#define FIRST_CHOICE 1
// Main menu exit choice
#define EXIT_CHOICE 4
// Playlist menu exit choice
#define EXIT_PLAYLIST_MENU_CHOICE 6
// Initial values of a song
#define INITIAL_SONG_STREAMS 0
#define INITIAL_SONGS_NUM 0

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

typedef enum Comparator {
    YEAR,
    STREAMS_ASC,
    STREAMS_DESC,
    TITLE
} Comparator;

// User menu choice input
int mainMenuChoice();
int playlistChoice(Playlist** playlists, int playlistsCount);
int playlistMenuChoice();

// Memory management logic
void freeSong(Song* song);
void freePlaylist(Playlist* playlist);
void freeAll(Playlist** playlists, int playlistsCount);
void exitProgram();
char* allocateInputString();

// Sort logic
void swapSongs(Song *first, Song *second);
int isFirstHigher(Song first, Song second, Comparator comp);
void merge(Song* songs, int low, int middle, int high, Comparator comp);
void mergeSortSongs(Song* songs, int low, int high, Comparator comp);
void sortPlaylistSongs(Playlist *playlist);

// Menus logic
void playlistMenu(Playlist* playlist);
void selectPlaylistMenu(Playlist** playlists, int playlistsCount);
void mainMenu(Playlist** playlists, int* playlistsCount);

// Rest of app logic
void addPlaylist(Playlist** playlists, int* playlistsCount);
void removePlaylist(Playlist** playlists, int* playlistsCount);
int chooseSong(Playlist *playlist, char* purpose);
void playSong(Song *song);
void showPlaylist(Playlist* playlist);
void addSongToPlaylist(Playlist* playlist);
void removeSong(Playlist *playlist);
void playAllPlaylistSongs(Playlist *playlist);

int main() {
    // Init playlists
    Playlist* playlists = NULL;
    int playlistsCount = 0;

    // Run main menu and input user choices until exit is chosen
    mainMenu(&playlists, &playlistsCount);

    // Free all playlists before exit
    freeAll(&playlists, playlistsCount);

    // Exit program
    printf("Goodbye!\n");
}

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
 Note: song itself isn't freed because it's assumed to be a part of an array
*/
void freeSong(Song* song) {
    // If song pointer is NULL, return without freeing it
    if (song == NULL) return;

    // Free song properties by their order
    free(song->title);
    free(song->artist);
    free(song->lyrics);
}

/*
 Frees a single playlist
 Note: playlist itself isn't freed because it's assumed to be a part of an array
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
    exit(EXIT_FAILURE);
}

/*
 Inputs unlimited string (until enter is pressed) and returns the string pointer
 Returns NULL pointer if reallocation failed during the process
*/
char* allocateInputString() {
    int size = 0;
    char* str = NULL;

    char c;
    scanf(" %c", &c);
    // Scan chars until enter is pressed (or /r is inserted)
    while (c != '\n' && c != '\r') {
        // Allocate space for the new char
        size++;
        char* temp = realloc(str, (size + 1) * sizeof(char));

        // Allocation failed - free str to prevent possible leak and return NULL pointer
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
    playlist->songsNum = INITIAL_SONGS_NUM;

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
 Plays given song by printing its lyrics to the console
*/
void playSong(Song *song) {
    printf("Now playing %s:\n$ %s $\n", song->title, song->lyrics);
    // Each time a song is played, increase its streams by one
    song->streams++;
}

/*
 Asks the user to choose a song to play until user chooses to quit play mode
*/
void showPlaylist(Playlist* playlist) {
    int choice = chooseSong(playlist, "play");
    // Validates choice is in valid choice range (song index = choice - 1)
    while (choice > 0 && choice <= playlist->songsNum) {
        playSong(&playlist->songs[choice - 1]);
        printf("choose a song to play, or 0 to quit:\n");
        scanf("%d", &choice);
    }
}

/*
 Inputs all required song properties from user
 Appends a new song with those properties to given playlist's song array
*/
void addSongToPlaylist(Playlist* playlist) {
    // Allocates memory for new array
    Song* newSongs = realloc(playlist->songs, (playlist->songsNum + 1) * sizeof(Song));
    if (newSongs == NULL)
        exitProgram();
    playlist->songs = newSongs;

    // Inputs song properties by required order
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

    // Set song properties to given inputs
    Song* song = &playlist->songs[playlist->songsNum];
    song->title = title;
    song->artist = artist;
    song->year = year;
    song->lyrics = lyrics;
    song->streams = INITIAL_SONG_STREAMS;

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
 Swaps song pointers content
*/
void swapSongs(Song *first, Song *second) {
    Song temp = *first;
    *first = *second;
    *second = temp;
}

/*
 Returns whether first is higher than first, determines how to compare by given "comp"
 Maintains previous order when comparison results in equality
*/
int isFirstHigher(Song first, Song second, Comparator comp) {
    switch (comp) {
        // Sort by year in ascending order
        case YEAR:
            return first.year > second.year;

        // Sort by streams in ascending order
        case STREAMS_ASC:
            return first.streams > second.streams;

        // Sort by streams in descending order
        case STREAMS_DESC:
            return first.streams < second.streams;

        // Sort by title in ascending order
        case TITLE:
            return strcmp(first.title, second.title) > 0;

        // Default: sort by title in ascending order
        default:
            return strcmp(first.title, second.title) > 0;
    }
}

/*
 Merges two subarrays of given songs array
 Left subarray is: (low - middle), right subarray is: (middle+1 - high)
*/
void merge(Song* songs, int low, int middle, int high, Comparator comp) {
    // Calculate each subarray length separately
    int leftLength = middle - low + 1;
    int rightLength = high - middle;

    // Allocate memory for each subarray to sort it outside of main songs array
    Song* leftSubarray = malloc(leftLength * sizeof(Song));
    Song* rightSubarray = malloc(rightLength * sizeof(Song));

    // Validate allocation is successful
    if (leftSubarray == NULL || rightSubarray == NULL)
        exitProgram();

    // Copy left subarray from songs into its dedicated array
    for (int i = 0; i < leftLength; i++)
        leftSubarray[i] = songs[low + i];

    // Copy right subarray from songs into its dedicated array
    for (int i = 0; i < rightLength; i++)
        rightSubarray[i] = songs[middle + 1 + i];

    // Separate index to track each array separatly
    int leftIndex = 0, rightIndex = 0, mainIndex = low;
    /*
     Merge both subarrays into main songs array according to merge-sort algorithm
     Loop until one of the indices reaches its array end, which means all its songs was copied to main array
    */
    while (leftIndex < leftLength && rightIndex < rightLength) {
        /*
         Determine which song should be "higher" in main array
         Copy the "lower" song to main array
         If both songs are equal by given comparator comparison, copy left song
        */
        if (isFirstHigher(leftSubarray[leftIndex], rightSubarray[rightIndex], comp))
            songs[mainIndex++] = rightSubarray[rightIndex++];
        else
            songs[mainIndex++] = leftSubarray[leftIndex++];
    }

    // Copy all left subarray songs to main songs array, if there are any
    while (leftIndex < leftLength)
        songs[mainIndex++] = leftSubarray[leftIndex++];

    // Copy all right subarray songs to main songs array, if there are any
    while (rightIndex < rightLength)
        songs[mainIndex++] = rightSubarray[rightIndex++];

    // Free both subarrays, as all their songs had been sorted into main songs array
    free(leftSubarray);
    free(rightSubarray);
}

/*
 Sorts given songs by given comparator using merge sort algorithm
 This function is responsible for given array division into many subarrays  
*/
void mergeSortSongs(Song* songs, int low, int high, Comparator comp) {
    // Base case: Chosen subarray has 0 / 1 items
    if (low >= high) return;

    // Calculates array middle, using this term to avoid potential overflow with big arrays
    int middle = low + (high - low) / 2;

    // Recursively sort first half subarray, and then second half subarray
    mergeSortSongs(songs, low, middle, comp);
    mergeSortSongs(songs, middle + 1, high, comp);

    // Perform merge sort algorithm
    merge(songs, low, middle, high, comp);
}

/*
 Sorts playlist songs by a chosen criterion of allowed criteria list
 Defaults to sort by title
*/
void sortPlaylistSongs(Playlist *playlist) {
    printf("choose:\n"
        "1. sort by year\n"
        "2. sort by streams - ascending order\n"
        "3. sort by streams - descending order\n"
        "4. sort alphabetically\n");

    // Sort according to user's choice
    int choice;
    scanf("%d", &choice);
    mergeSortSongs(playlist->songs, 0, playlist->songsNum - 1, (Comparator) (choice - 1));

    printf("sorted\n");
}

/*
 Plays all playlist songs by their insertion order
*/
void playAllPlaylistSongs(Playlist *playlist) {
    for (int song = 0; song < playlist->songsNum; song++) {
        playSong(&playlist->songs[song]);
        printf("\n");
    }
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
                sortPlaylistSongs(playlist);
                break;

            // Play
            case 5:
                playAllPlaylistSongs(playlist);
                break;

            // exit - implemented in while terminating condition
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
    // Loop until user selects to exit this menu
    while (selected < playlistsCount) {
        // An inner menu for handling selected playlist
        playlistMenu(&(*playlists)[selected]);
        // Input next user selection
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

            // Exit - implemented in while terminating condition
            case 4:
                break;

            default:
                printf("Invalid option\n");
                break;
        }
        choice = mainMenuChoice();
    }
}
