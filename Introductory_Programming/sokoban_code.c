#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct list_of_files listf;
struct list_of_files
{
    char value;
    int number;
    int row;
    int col;
    listf* prev;
    listf* next;
    listf* last;
};

typedef struct board_file file;
struct board_file
{
    int number;
    char sign;
    int row;
    int col;
};

typedef struct position_structure position;
struct position_structure
{
    int prev_char_row;
    int prev_char_col;
    int new_box_row;
    int new_box_col;
    int new_char_row;
    int new_char_col;

    position* prev;
    position* next;
    position* last;
};

void initialize_listf(listf* list1)
{
    list1->value = '0';
    list1->number = 0;
    list1->prev = NULL;
    list1->next = NULL;
    list1->last = list1;
}

/// Adds element to the list of type listf with a given character(sign) and assigns the number following list numeration.
void add_to_listf(listf* list1, char sign) 
{
    listf* list2 = malloc(sizeof(listf));
    list2->value = sign;
    if(sign != '\n'){list2->number = list1->last->number + 1;}
    else{list2->number = list1->last->number;}
    list1->last->next = list2;
    list2->prev = list1->last;
    list1->last = list2;
    list2->last = list2;
    list2->next = NULL;
}

void initialize_position_list(position* starting)
{
    starting->last = starting;
    starting->prev = NULL;
    starting->next = NULL;
}

/// Adds position of the previous character location(p_char_row and p_char_col), new character location(n_char_row and n_char_col)
/// and new box location (n_box_row and n_box_col), and sets 'last' pointer to the added position. This struccture will be used to undo
/// previously played moves.
void add_position(position* list_of_positions, int p_char_row, int p_char_col, int n_box_row, int n_box_col, int n_char_row, int n_char_col)
{
    position* current = malloc(sizeof(position));
    current->prev_char_row = p_char_row;
    current->prev_char_col = p_char_col;
    current->new_box_row = n_box_row;
    current->new_box_col = n_box_col;
    current->new_char_row = n_char_row;
    current->new_char_col = n_char_col;
    current->prev = list_of_positions->last;
    current->prev->next = current;
    list_of_positions->last = current;
    current->last = current;
    current->next = NULL;
}

/// Sets the 'last' pointer to the penultimate element of the list and frees the memory after former 'last'
void remove_position(position* list_of_positions)
{
    if(list_of_positions->last->prev != NULL)
    {
        list_of_positions->last = list_of_positions->last->prev;
        if(list_of_positions->last->next != NULL)
        {
            free(list_of_positions->last->next);
        }
    }
}

bool special_case(char character)
{
    if(character == '*' || character == '@'){return true;}
    else if(character>='a' && character<='z'){return true;}
    else if(character>='A' && character<='Z'){return true;}
    return false;
}

/// Finds the  element with given value(character) on the list, and if found, sets row and col as its row and col.
void set_row_and_col(listf* some_list, char character, int* row, int* col)
{
    listf* temp = some_list;
    while(temp != NULL && temp->value != character)
    {
        temp = temp->next;
    }
    if(temp != NULL)
    {
        *row = temp->row;
        *col = temp->col;
    }
}

///Chenges value, number, row and col of the element with given value(character).
void change_listf_element(listf* element, char changed, int row, int col, char value, int number)
{
    listf* temp = element;
    while(temp != NULL && temp->value != changed)
    {
        temp = temp->next;
    }
    if(temp != NULL)
    {
        temp->col = col;
        temp->row = row;
        temp->value = value;
        temp->number = number;
    }
}

/// Loads the playing board to the list, changes the number of files to the number of loaded elements and returns number of lines.
int upload_board(listf* files_list, int* number_of_files)
{
    char sign;
    char next;
    sign = (char)(getchar());
    int lines_counter = 1;
    int characters_counter = 0;

    while(sign != '.')
    {
        characters_counter++;
        if(sign == '\n')
        {
            lines_counter++;
            next = (char)(getchar());
            if(next == '\n')
            {
                add_to_listf(files_list, sign);
                break;
            }
            else{ungetc(next, stdin);}
        }
        add_to_listf(files_list, sign);
        sign = (char)(getchar());
    }

    *number_of_files = characters_counter;
    return lines_counter;
}

///Loads elements from the list to the dynamic array.
file** import_to_table(int lsize, listf* files, int* rows_sizes, listf* starting)
{
    listf* temp = files->next;
    int element_counter = 0;
    int row_number = 0;
    while(temp != NULL)
    {
        if(temp->value == '\n')
        {
            *(rows_sizes+row_number) = element_counter;
            element_counter = 0;
            row_number++;
        }
        else{element_counter++;}
        temp = temp->next;
    }

    file** board = malloc((long unsigned int)(lsize) * (long unsigned int)(sizeof(file*)));

    listf* temp2 = files->next;
    int i=0;
    while(temp2 != NULL)
    {
        *(board+i) = malloc((long unsigned int)(rows_sizes[i]) * (long unsigned int)(sizeof(file)));
        int j=0;
        while(temp2->value != '\n')
        {
            if(special_case(temp2->value))
            {
                if(temp2->value>='A' && temp2->value<='Z'){add_to_listf(starting, temp2->value+32);}
                else{add_to_listf(starting, temp2->value);}
                starting->number = temp2->number;
                starting->last->row = i;
                starting->last->col = j;
            }
            board[i][j].number = temp2->number;
            board[i][j].sign = temp2->value;
            temp2 = temp2->next;
            j++;
        }
        temp2 = temp2->next;
        i++;
    }

    return board;
}

void print_board(file** board, int number_of_rows, int* rows_sizes)
{
   for(int i = 0; i<number_of_rows-1; i++)
    {
        for(int j = 0 ; j<rows_sizes[i]; j++)
        {
            printf("%c", board[i][j].sign);
        }
        printf("\n");
    }
}

///Moves the board to the state before last legal move and upadates the list of relevant characters.
void reverse_play(file** board, position* positions_played, listf* player_and_boxes)
{
    if(positions_played->last->prev == NULL){return;}
    int prev_char_row = positions_played->last->prev_char_row;
    int prev_char_col = positions_played->last->prev_char_col;
    int curr_char_row = positions_played->last->new_char_row;
    int curr_char_col = positions_played->last->new_char_col;
    int curr_box_row = positions_played->last->new_box_row;
    int curr_box_col = positions_played->last->new_box_col;
    bool flag_1 = true;
    bool flag_2 = true;
    if(board[curr_char_row][curr_char_col].sign == '*')
    {
        flag_1 = false;
        if((int)(board[curr_box_row][curr_box_col].sign)>='A' && (int)(board[curr_box_row][curr_box_col].sign)<='Z')
        {
            board[curr_char_row][curr_char_col] = board[curr_box_row][curr_box_col];
        }
        else
        {
            board[curr_char_row][curr_char_col].sign = board[curr_box_row][curr_box_col].sign - 32;
        }
    }
    else
    {
        if((int)(board[curr_box_row][curr_box_col].sign)>='A' && (int)(board[curr_box_row][curr_box_col].sign)<='Z')
        {
            board[curr_char_row][curr_char_col].sign = board[curr_box_row][curr_box_col].sign +32;
        }
        else
        {
            board[curr_char_row][curr_char_col].sign = board[curr_box_row][curr_box_col].sign;
        }
    }
    if((int)(board[curr_box_row][curr_box_col].sign)>='A' && (int)(board[curr_box_row][curr_box_col].sign)<='Z')
    {
        board[curr_box_row][curr_box_col].sign = '+';
    }
    else
    {
        board[curr_box_row][curr_box_col].sign = '-';
    }
    if(board[prev_char_row][prev_char_col].sign == '-')
    {
        board[prev_char_row][prev_char_col].sign = '@';
    }
    else
    {
        flag_2 = false;
        board[prev_char_row][prev_char_col].sign = '*';
    }

    if(flag_1)
    {
        if(!flag_2)
        {change_listf_element(player_and_boxes, '@', prev_char_row, prev_char_col, '*', board[prev_char_row][prev_char_col].number);}
        else
        {change_listf_element(player_and_boxes, '@', prev_char_row, prev_char_col, '@', board[prev_char_row][prev_char_col].number);}
    }
    else
    {
        if(flag_2)
        {change_listf_element(player_and_boxes, '*', prev_char_row, prev_char_col, '@', board[prev_char_row][prev_char_col].number);}
        else
        {change_listf_element(player_and_boxes, '*', prev_char_row, prev_char_col, '*', board[prev_char_row][prev_char_col].number);}
    }
    if(board[curr_char_row][curr_char_col].sign>='a' && board[curr_char_row][curr_char_col].sign<='z')
    {
        change_listf_element(player_and_boxes, board[curr_char_row][curr_char_col].sign, curr_char_row, curr_char_col, board[curr_char_row][curr_char_col].sign, board[curr_char_row][curr_char_col].number);
    }
    else
    {
        change_listf_element(player_and_boxes, board[curr_char_row][curr_char_col].sign+32, curr_char_row, curr_char_col, board[curr_char_row][curr_char_col].sign+32, board[curr_char_row][curr_char_col].number);
    }
}

bool is_free(file** board, int row, int col)
{
    if((int)(board[row][col].sign) == '@' || (int)(board[row][col].sign) == '-' || (int)(board[row][col].sign) == '+' || (int)(board[row][col].sign) =='*')
        {return true;}
    else
        {return false;}
}

///Checks if the file with given row and column exists on the board.
bool exists(int row, int col, int rows_number, int* rows_sizes)
{
    if(row<rows_number-1)
    {
        if(row>=0 && col>=0 && col<rows_sizes[row]){return true;}
        else{return false;}
    }
    else{return false;}
}

bool find_path(file** board, int start_row, int start_col, int goal_row, int goal_col, int* visited, int rows_number, int* rows_sizes)
{
    visited[board[start_row][start_col].number]=1;
    if(exists(start_row-1, start_col, rows_number, rows_sizes))
    {if(!visited[board[start_row-1][start_col].number] && is_free(board, start_row-1, start_col))
        {find_path(board, start_row-1, start_col, goal_row, goal_col, visited,rows_number, rows_sizes);}
    }
    if(exists(start_row, start_col+1, rows_number, rows_sizes))
    {if(!visited[board[start_row][start_col+1].number] && is_free(board, start_row, start_col+1))
        {find_path(board, start_row, start_col+1, goal_row, goal_col, visited, rows_number, rows_sizes);}
    }
    if(exists(start_row+1, start_col, rows_number, rows_sizes))
    {if(!visited[board[start_row+1][start_col].number] && is_free(board, start_row+1, start_col))
        {find_path(board, start_row+1, start_col, goal_row, goal_col, visited, rows_number, rows_sizes);}
    }
    if(exists(start_row, start_col-1, rows_number, rows_sizes))
    {if(!visited[board[start_row][start_col-1].number] && is_free(board, start_row, start_col-1))
        {find_path(board, start_row, start_col-1, goal_row, goal_col, visited, rows_number, rows_sizes);}
    }

    return 1;
}

///Checks if the path between two given files((start_row, start_col) and (goal_row, goal_col)) exits.
bool path_exists(file** board, int start_row, int start_col, int goal_row, int goal_col, int number_of_files, int rows_number, int* rows_sizes)
{
    int* visited = malloc((long unsigned int)(number_of_files) * (long unsigned int)(sizeof(int)));
    for(int i=0; i<number_of_files; i++){visited[i]=0;}
    bool file_exists = exists(goal_row, goal_col, rows_number, rows_sizes);
    if(file_exists)
    {
        find_path(board, start_row, start_col, goal_row, goal_col, visited, rows_number, rows_sizes);
    }
    bool path_found = (file_exists && visited[board[goal_row][goal_col].number]);
    free(visited);
    if(path_found){return true;}
    else {return false;}
}

///Sets row and col as row and col of the next file in the direction given by dir.
void next_file(char dir, int* row, int* col)
{
    if(dir == '2')
    {
        *row = *row+1;
    }
    else if(dir == '4')
    {
        *col = *col-1;
    }
    else if(dir == '8')
    {
        *row = *row-1;
    }
    else if(dir == '6')
    {
        *col = *col + 1;
    }
}

/// The function reads the input characters and proceeds according to their values: it prints in case of '\n', takes back the move in case of '0',
/// sets the box to '.', which ends the game, or assigns the values of 'box' and 'dir' based on the loaded input."
bool process_move(file** board, position* positions_list, listf* player_and_boxes, char* box, char* dir, int rows_number, int* rows_sizes)
{
    char new_box = '1';
    char new_dir = '1';
    new_box = (char)(getchar());
    if(new_box == '\n')
    {
        print_board(board, rows_number, rows_sizes);
        return false;
    }
    else if(new_box == '0')
    {
        reverse_play(board, positions_list, player_and_boxes);
        remove_position(positions_list);
        return false;
    }
    else if(new_box == '.')
    {
        *box = new_box;
        return false;
    }
    else
    {
        new_dir = (char)(getchar());
    }
    *box = new_box;
    *dir = new_dir;
    return true;
}

bool next_file_is_free(file** board, int* row_sizes, int row, int col, char dir, int rows_number)
{
    if(dir == '8')
    {
        if(exists(row-1, col, rows_number, row_sizes) && is_free(board, row-1, col))
        {return 1;}
    }
    else if(dir == '6')
    {
        if(exists(row, col+1, rows_number, row_sizes) && is_free(board, row, col+1))
        {return 1;}
    }
    else if(dir == '2')
    {
        if(exists(row+1, col, rows_number, row_sizes) && is_free(board, row+1, col))
        {return 1;}
    }
    else if(dir == '4')
    {
        if(exists(row, col-1, rows_number, row_sizes) && is_free(board, row, col-1))
        {return 1;}
    }
    return 0;
}

/// Updates values of given files(what-the box that is pushed, where-the file where it is pushed, old_player-previous position
/// of the player) if the move is posiible. 
void update_board(file* what, file* where, file* old_player)
{
    if((int)(what->sign)<='z' && (int)(what->sign)>='a' && ((int)(where->sign) == '+' || (int)(where->sign) == '*'))
    {
        where->sign = what->sign - 32;
    }
    else if(((int)(what->sign)<='Z' && (int)(what->sign)>='A') && ((int)(where->sign) == '-' || (int)(where->sign) == '@'))
    {
        where->sign = what->sign + 32;
    }
    else
    {
        where->sign = what->sign;
    }

    if(old_player->sign == '*' && where != old_player)
    {
        old_player->sign = '+';
    }
    else if(where != old_player)
    {
        old_player->sign = '-';
    }
    if((int)(what->sign)<='Z' && (int)(what->sign)>='A')
    {
        what->sign = '*';
    }
    else
    {
        what->sign = '@';
    }
} 

/// The function checks if the path between starting file((s_row, s_col)-position of the player) and player goal file((char_g_row,char_g_col)-
/// the file next to the box) exists. Then verifies if the file, where the box is to be pushed, is free. If so, it does the move,
///  updates the list of relevant files(player_and_boxes) and adds the position to the list of played positions(moves_played).
void do_the_move(file** board, int number_of_files, int* rows_sizes, int number_of_rows, char dir, char box, listf* player_and_boxes, position* moves_played)
{
    int s_row = 0;
    int s_col = 0;
    set_row_and_col(player_and_boxes, '@', &s_row, &s_col);
    set_row_and_col(player_and_boxes, '*', &s_row, &s_col);
    int g_row = 0;
    int g_col = 0;
    set_row_and_col(player_and_boxes, box, &g_row, &g_col);

    int n_row = g_row;
    int n_col = g_col;

    int char_g_row = g_row;
    int char_g_col = g_col;

    if(dir == '8'){char_g_row = g_row+1;}
    else if(dir == '6'){char_g_col = g_col-1;}
    else if(dir == '2'){char_g_row = g_row-1;}
    else if(dir == '4'){char_g_col = g_col+1;}

    if(path_exists(board, s_row, s_col, char_g_row, char_g_col, number_of_files, number_of_rows, rows_sizes))
    {
        next_file(dir, &n_row, &n_col);
        if(next_file_is_free(board, rows_sizes, g_row, g_col, dir, number_of_rows))
        {
            update_board(&board[g_row][g_col],&board[n_row][n_col],&board[s_row][s_col]);
            if((int)(board[n_row][n_col].sign)<='Z' && (int)(board[n_row][n_col].sign)>='A')
            {
                change_listf_element(player_and_boxes, box, n_row, n_col, board[n_row][n_col].sign+32, board[n_row][n_col].number);
            }
            else
            {
                change_listf_element(player_and_boxes, box, n_row, n_col, board[n_row][n_col].sign, board[n_row][n_col].number);
            }
            change_listf_element(player_and_boxes, '@', g_row, g_col, board[g_row][g_col].sign, board[g_row][g_col].number);
            change_listf_element(player_and_boxes, '*', g_row, g_col, board[g_row][g_col].sign, board[g_row][g_col].number);
            add_position(moves_played, s_row, s_col, n_row, n_col, g_row, g_col);
        }
    }
}

void play(file** board, int number_of_files, int* rows_sizes, int rows_number, listf* player_and_boxes, position* moves_played)
{
    print_board(board, rows_number, rows_sizes);
    char box = '1';
    char dir = '1';
    while(box != '.')
    {
        if(process_move(board, moves_played, player_and_boxes, &box, &dir, rows_number, rows_sizes))
        {
            do_the_move(board, number_of_files, rows_sizes, rows_number, dir, box, player_and_boxes, moves_played);
        }
    }
}

int main()
{
    int number_of_rows = 0;
    int number_of_files = 0;
    listf* files_list = malloc(sizeof(listf));
    listf* player_and_boxes = malloc(sizeof(listf));
    position* positions_played = malloc(sizeof(position));

    initialize_listf(files_list);
    initialize_listf(player_and_boxes);
    initialize_position_list(positions_played);

    number_of_rows = upload_board(files_list, &number_of_files);

    int* rows_sizes = NULL;
    rows_sizes = malloc((long unsigned int)(number_of_rows) * (long unsigned int)(sizeof(int)));

    file** board = import_to_table(number_of_rows, files_list, rows_sizes, player_and_boxes);

    play(board,number_of_files,rows_sizes,number_of_rows,player_and_boxes,positions_played);

    for(int i = 0; i<number_of_rows-1; i++)
    {
        free(*(board+i));
    }
    free(board);

    listf* temp = player_and_boxes;
    while(temp->next != NULL)
    {
        temp = temp->next;
        free(temp->prev);
    }
    free(temp);

    temp = files_list;
    while(temp->next != NULL)
    {
        temp = temp->next;
        free(temp->prev);
    }
    free(temp);


    position* temp2 = positions_played->last;
    while(temp2->prev != NULL)
    {
        temp2 = temp2->prev;
        if(temp2->next != NULL)
        {
            free(temp2->next);
        }
    }
    free(temp2);

    if(rows_sizes != NULL)
    {
        free(rows_sizes);
    }
    return 0;
}

