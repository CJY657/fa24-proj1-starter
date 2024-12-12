#include "state.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "snake_utils.h"

/* Helper function definitions */
static void set_board_at(game_state_t *state, unsigned int row, unsigned int col, char ch);
static bool is_tail(char c);
static bool is_head(char c);
static bool is_snake(char c);
static char body_to_tail(char c);
static char head_to_body(char c);
static unsigned int get_next_row(unsigned int cur_row, char c);
static unsigned int get_next_col(unsigned int cur_col, char c);
static void find_head(game_state_t *state, unsigned int snum);
static char next_square(game_state_t *state, unsigned int snum);
static void update_tail(game_state_t *state, unsigned int snum);
static void update_head(game_state_t *state, unsigned int snum);

/* Task 1 */
game_state_t *create_default_state() {
  game_state_t *newgame = (game_state_t *)malloc(sizeof(game_state_t));
  newgame->num_rows = 18;
  newgame->num_snakes = 1;
  newgame->snakes = (snake_t *)malloc(sizeof(snake_t) * newgame->num_snakes);
  snake_t *newsnake = &newgame->snakes[0];
  newsnake ->live = true;
  newsnake->tail_row = 2;
  newsnake->tail_col = 2;
  newsnake->head_row = 2;
  newsnake->head_col = 4;
  newgame->board = (char **)malloc(sizeof(char *) * newgame->num_rows);
  for (int i = 0; i < newgame->num_rows; i++) {
    newgame->board[i] = (char *)malloc(sizeof(char) * 21);
    strcpy(newgame->board[i], "#                  #");
  }
  strcpy(newgame->board[0], "####################");
  strcpy(newgame->board[2], "# d>D    *         #");
  strcpy(newgame->board[newgame->num_rows - 1], "####################");
  return newgame;
}

/* Task 2 */
void free_state(game_state_t *state) {
  for(int i = 0; i < state->num_rows; i++){
    free(state->board[i]);
  }
  free(state->board);
  free(state -> snakes);
  return;
}

/* Task 3 */
void print_board(game_state_t *state, FILE *fp) {
    for(size_t i = 0; i < state->num_rows - 1; i++){
      fprintf(fp, "%s\n", state->board[i]);
    }
    fprintf(fp, "%s\n", state->board[state->num_rows-1]);
    return;
}

/*
  Saves the current state into filename. Does not modify the state object.
  (already implemented for you).
*/
void save_board(game_state_t *state, char *filename) {
  FILE *f = fopen(filename, "wb");
  print_board(state, f);
  fclose(f);
}

/* Task 4.1 */

/*
  Helper function to get a character from the board
  (already implemented for you).
*/
char get_board_at(game_state_t *state, unsigned int row, unsigned int col) { return state->board[row][col]; }

/*
  Helper function to set a character on the board
  (already implemented for you).
*/
static void set_board_at(game_state_t *state, unsigned int row, unsigned int col, char ch) {
  state->board[row][col] = ch;
}

/*
  Returns true if c is part of the snake's tail.
  The snake consists of these characters: "wasd"
  Returns false otherwise.
*/
static bool is_tail(char c) {
  // TODO: Implement this function.
  switch (c)
  {
  case 'w':
  case 's':
  case 'd':
  case 'a':
    return true;
  default:
    return false;
  }
}

/*
  Returns true if c is part of the snake's head.
  The snake consists of these characters: "WASDx"
  Returns false otherwise.
*/
static bool is_head(char c) {
  // TODO: Implement this function.
  switch(c){
    case 'A':
    case 'S':
    case 'D':
    case 'W':
      return true;
    
    default:
      return false;
  }
}

/*
  Returns true if c is part of the snake.
  The snake consists of these characters: "wasd^<v>WASDx"
*/
static bool is_snake(char c) {
  // TODO: Implement this function.
  switch(c){
    case 'w':
    case 'a':
    case 's':
    case 'd':
    case '^':
    case '<':
    case 'v':
    case 'W':
    case 'A':
    case 'S':
    case 'D':
    case '>':
      return true;
    default:
      return false;
  }
}

/*
  Converts a character in the snake's body ("^<v>")
  to the matching character representing the snake's
  tail ("wasd").
*/
static char body_to_tail(char c) {
  // TODO: Implement this function.
  switch(c){
    case '^':
      return 'w';
    case '<':
      return 'a';
    case 'v':
      return 's';
    default:
      return 'd';
  }
}

/*
  Converts a character in the snake's head ("WASD")
  to the matching character representing the snake's
  body ("^<v>").
*/
static char head_to_body(char c) {
  // TODO: Implement this function.
  switch (c){
    case 'W':
      return '^';
    case 'A':
      return '<';
    case 'S':
      return 'v';
    default:
      return '>';
  }
}

/*
  Returns cur_row + 1 if c is 'v' or 's' or 'S'.
  Returns cur_row - 1 if c is '^' or 'w' or 'W'.
  Returns cur_row otherwise.
*/
static unsigned int get_next_row(unsigned int cur_row, char c) {
  // TODO: Implement this function.
  switch(c){
    case 'v':
    case 's':
    case 'S':
      return cur_row + 1;
    case '^':
    case 'w':
    case 'W':
      return cur_row - 1;
    default:
      return cur_row;
  }
}

/*
  Returns cur_col + 1 if c is '>' or 'd' or 'D'.
  Returns cur_col - 1 if c is '<' or 'a' or 'A'.
  Returns cur_col otherwise.
*/
static unsigned int get_next_col(unsigned int cur_col, char c) {
  // TODO: Implement this function.
  switch(c){
    case '>':
    case 'd':
    case 'D':
      return cur_col + 1;
    case '<':
    case 'a':
    case 'A':
      return cur_col - 1;
    default:
      return cur_col;
  }
}

/*
  Task 4.2

  Helper function for update_state. Return the character in the cell the snake is moving into.

  This function should not modify anything.
*/
static char next_square(game_state_t *state, unsigned int snum) {
  // TODO: Implement this function.
  char direction = state->board[state->snakes[snum].head_row][state->snakes[snum].head_col];
  unsigned int row = get_next_row(state->snakes[snum].head_row, direction);
  unsigned int  col = get_next_col(state->snakes[snum].head_col, direction);
  return state->board[row][col];
}

/*
  Task 4.3

  Helper function for update_state. Update the head...

  ...on the board: add a character where the snake is moving

  ...in the snake struct: update the row and col of the head

  Note that this function ignores food, walls, and snake bodies when moving the head.
*/
static void update_head(game_state_t *state, unsigned int snum) {
  // TODO: Implement this function.
    unsigned int cur_row = state->snakes[snum].head_row;
    unsigned int cur_col = state->snakes[snum].head_col;
    char temp = state->board[cur_row][cur_col];
    unsigned int next_row = get_next_row(cur_row, state->board[cur_row][cur_col]);
    unsigned int next_col = get_next_col(cur_col, state->board[cur_row][cur_col]);
    state->snakes[snum].head_row = next_row;
    state->snakes[snum].head_col = next_col;
    char charac = head_to_body(state->board[cur_row][cur_col]);
    state->board[cur_row][cur_col] = charac;
    state->board[state->snakes[snum].head_row][state->snakes[snum].head_col] = temp;
    return;
}

/*
  Task 4.4

  Helper function for update_state. Update the tail...

  ...on the board: blank out the current tail, and change the new
  tail from a body character (^<v>) into a tail character (wasd)

  ...in the snake struct: update the row and col of the tail
*/
static void update_tail(game_state_t *state, unsigned int snum) {
  // TODO: Implement this function.
  unsigned int cur_row = state->snakes[snum].tail_row;
  unsigned int  cur_col = state->snakes[snum].tail_col;
  unsigned int next_row_pos = get_next_row(cur_row, state->board[cur_row][cur_col]);
  unsigned int next_col_pos = get_next_col(cur_col, state->board[cur_row][cur_col]);
  char new_charac = body_to_tail(state->board[next_row_pos][next_col_pos]);
  state->board[next_row_pos][next_col_pos] = new_charac;
  state->board[cur_row][cur_col] = ' ';
  state->snakes[snum].tail_row = next_row_pos;
  state->snakes[snum].tail_col = next_col_pos;
  return;
}

/* Task 4.5 */
void update_state(game_state_t *state, int (*add_food)(game_state_t *state)) {
  // TODO: Implement this function.
  for(unsigned int  i = 0; i < state->num_snakes; i++){
    char next_char = next_square(state, i);
    if(next_char == '*'){
      update_head(state, i);
      add_food(state);
    }else if(is_snake(next_char)){
      state->board[state->snakes[i].head_row][state->snakes[i].head_col] = 'x';
      state->snakes[i].live = false;
    }else if(next_char == '#'){
      state->board[state->snakes[i].head_row][state->snakes[i].head_col] = 'x';
      state->snakes[i].live = false;
    }else{
      update_head(state, i);
      update_tail(state, i);
    }
  }


  return;
}

/* Task 5.1 */
char *read_line(FILE *fp) {
  // TODO: Implement this function.
  char *temp_str = (char *)malloc(sizeof(char) * 30);
  if(temp_str == NULL){
    return NULL;
  }
  if(fgets(temp_str, 100, fp) != NULL){
    return temp_str;
  }
  free(temp_str);
  return NULL;
}

/* Task 5.2 */
game_state_t *load_board(FILE *fp) {
  // TODO: Implement this function.
  if(fp == NULL){
    return NULL;
  }
  unsigned int lines = 0;
  game_state_t *newstate = (game_state_t *)malloc(sizeof(game_state_t));
  if(newstate == NULL){
    return NULL;
  }
  newstate -> snakes = NULL;
  newstate -> num_snakes = 0;
  newstate->board = NULL;
  char *temp = NULL;
  while((temp = read_line(fp)) != NULL){
    lines++;
    newstate->board = realloc(newstate->board, lines * sizeof(char *));
    newstate->board[lines - 1] = (char *)malloc((strlen(temp)+1) * sizeof(char));
    temp[strlen(temp) - 1] = '\0';
    strcpy(newstate->board[lines - 1], temp);
    free(temp);
  }
  newstate->num_rows = lines;
  return newstate;
}


/*
  Task 6.1

  Helper function for initialize_snakes.
  Given a snake struct with the tail row and col filled in,
  trace through the board to find the head row and col, and
  fill in the head row and col in the struct.
*/
static void find_head(game_state_t *state, unsigned int snum) {
  // TODO: Implement this function.

    unsigned int row = state->snakes[snum].tail_row;
    unsigned int col = state->snakes[snum].tail_col;
    while(!is_head(state->board[row][col])){
      char cur_char = state->board[row][col];
      row = get_next_row(row, cur_char);
      col = get_next_col(col, cur_char);
    }
    state->snakes[snum].head_row = row;
    state->snakes[snum].head_col = col;
  
  return;
}

/* Task 6.2 */
game_state_t *initialize_snakes(game_state_t *state) {
  // TODO: Implement this function.
  state->num_snakes = 1;
  state->snakes = (snake_t *)malloc(sizeof(snake_t) * state->num_snakes);
  state->snakes[0].tail_row = 2;
  state->snakes[0].tail_col = 2;
  state->snakes[0].live = true;
  find_head(state, 0);
  return state;
}
