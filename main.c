#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

#define ALIVE  '0'
#define DEAD   ' '
#define WIDTH  32 // Depends on the number of columns in the 'grid.txt' file
#define HEIGHT 32 // Depends on the number of lines in the 'grid.txt' file
#define SIZE   HEIGHT*WIDTH
#define FPS    4

char display[ SIZE ];

void read_grid_from_file(FILE* f) {
	int i = 0;
	char c;
	while ( (c = fgetc(f) ) != EOF ) {
		if ( c != '\n' ) {
			display[i] = (c == '0') ? ALIVE : DEAD; // I want to use any character to represent dead cells in the grid file
			i++;
		}
	}
}

void show() {
	for ( int y = 0; y < HEIGHT; y++ ) {
		for ( int x = 0; x < WIDTH; x++ )
			fputc(display[y * WIDTH + x], stdout);
		fputc('\n', stdout);
	}
}

int get_alive_neighbors(int i) {
	int count = 0;

	// Horizontal:
	int left = i-1;
	if ( left >= 0 && display[left] == ALIVE ) count++;
	int right = i+1;
	if ( right < SIZE && display[right] == ALIVE ) count++;

	// Vertical:
	int up = i-WIDTH;
	if ( up >= 0 && display[up] == ALIVE ) count++;
	int down = i+WIDTH;
	if (down < SIZE && display[down] == ALIVE ) count++;

	// Down diagonal:
	int down_left = down-1;
	if ( down_left < SIZE && display[down_left] == ALIVE ) count++;
	int down_right = down+1;
	if ( down_right < SIZE && display[down_right] == ALIVE ) count++;

	// Up Diagonal:
	int up_left = up-1;
	if ( up_left >= 0 && display[up_left] == ALIVE ) count++;
	int up_right = up+1;
	if ( up_right >= 0 && display[up_right] == ALIVE ) count++;

	return count;
}

// I could change this to not need to copy to display again,
// but for that I would need to not use display inside `get_alive_neighbors(int)`
bool apply_rules() {
	char cells_new_state[SIZE];
	memcpy(cells_new_state, display, sizeof(display));
	for ( int i = 0; i < SIZE; i++ ) {
		if ( display[i] == ALIVE ) {
			// Becomes DEAD only if it has less than 2 or more than 3 alive neighbors
			int n_neighbors = get_alive_neighbors(i);
			if ( n_neighbors < 2 || n_neighbors > 3 ) cells_new_state[i] = DEAD;
		} else if (  display[i] == DEAD ) {
			// Becomes ALIVE only if it has 3 alive neighbors
			if ( get_alive_neighbors(i) == 3 ) cells_new_state[i] = ALIVE;
		}
	}
	if ( memcmp(&cells_new_state, &display, sizeof(display)) == 0 ) {
		return false; // Same state, stop.
	} else {
		memcpy(display, cells_new_state, sizeof(display));
		return true; // Different state, keep iterating
	}
}

// Move terminal cursor to the beginning again
void back() {
	// Move WIDTH to the left
	printf("\x001b[%dD", WIDTH);
	// Move HEIGHT to the top
	printf("\x001b[%dA", HEIGHT);
}

int main(int argc, char* argv[]) {
	FILE* f = fopen("grid.txt", "r");
	read_grid_from_file(f);

	bool keep_iterating = true;
	while (keep_iterating) {
		show();
		keep_iterating = apply_rules();
		back();
		// sleep(1); // Accepts uint32 values
		usleep( (1000*1000) / FPS );
	}
	show();
	fclose(f);
}
