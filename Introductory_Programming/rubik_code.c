#include <stdio.h>
#include <stdlib.h>

#ifndef N
#define N 5
#endif

void initialize_cube_face(int array[N][N], int k) ///fills array representing cube's face with given integer
{
    for (int i = 0; i < N; i++){
            for (int j = 0; j < N; j++){
                array[i][j] = k;
            }
            }
}

struct cube{

    int up[N][N];
    int left[N][N];
    int front[N][N];
    int right[N][N];
    int down[N][N];
    int back[N][N];

};

void initialize_cube(struct cube *rubik) /// fills cube with starting position
{
    initialize_cube_face(rubik->up, 0);
    initialize_cube_face(rubik->left, 1);
    initialize_cube_face(rubik->front, 2);
    initialize_cube_face(rubik->right, 3);
    initialize_cube_face(rubik->down, 5);
    initialize_cube_face(rubik->back, 4);
}

void print_cube(struct cube rubik)
{
    printf("\n");
    for (int i = 0; i < 3 * N; i++){
        for (int j = 0; j < 4 * N + 3; j++){
           if(i < N)
            {
              if(j < N + 1)
                {
                printf(" ");
                }
              else if(j< 2 * N + 1)
                {
                printf("%d", rubik.up[i][j - N - 1]);
                }
            }
            else if (i < 2 * N)
            {
                if(j < N)
                {
                    printf("%d", rubik.left[i - N][j]);
                }
                else if (j > N && j < 2 * N + 1){
                    printf("%d", rubik.front[i -  N][j - N - 1]);
                }
                else if (j > 2 * N + 1 && j < 3 * N + 2){
                    printf("%d", rubik.right[i -  N][j - 2 * N - 2]);
                }
                else if(j > 3 * N + 2){printf("%d", rubik.back[i -  N][j - 3 * N - 3]);
                }
            }
            else if(i > 2 * N - 1)
            {
              if(j < N + 1)
                {
                printf(" ");
                }
                else if(j < 2 * N + 1)
                {
                printf("%d", rubik.down[i - 2 * N][j - N - 1]);
                }
            }
            if((j == N|| j == 2*N || j == 3*N + 1) && !(i < N || i > 2*N-1))
            {
                printf("|");
            }
        }
        printf("\n");
    }
    
}

void copy_array(int array1[N][N], int array2[N][N], int row_start, int row_end, int col_start, int col_end) ///copies set part of the array
{
    for(int i = col_start; i<=col_end; ++i){
        for(int j = row_start; j<=row_end; ++j){
            array1[i][j] = array2[i][j];
        }
    }
}

void rotate_face(int face[N][N], char direction) ///rotates the front face of the cube
{
    int temp_face[N][N];
    int rep;
    if(direction == '*'){rep = 1;};/// 90 degrees
    if(direction == '"'){rep = 2;};/// 90 degrees
    if(direction == '\''){rep = 3;};/// 90 degrees
    for(int k=0; k<rep; k++)
    {
        copy_array(temp_face, face, 0, N-1, 0, N-1);
        for(int i = 0; i<N; ++i){
            for(int j = 0; j<N; ++j){
                face[i][j] = temp_face[N - j - 1][i];
            }
        }
    }
}

void set_orientation(struct cube *rubik, char face) /// sets orientation so that requested face is a front face
{
    if(face=='l')
    {
        rotate_face(rubik->up,'\'');
        rotate_face(rubik->down,'*');
        int temp[N][N];
        copy_array(temp,rubik->left,0,N-1,0,N-1);
        copy_array(rubik->left,rubik->back,0,N-1,0,N-1);
        copy_array(rubik->back,rubik->right,0,N-1,0,N-1);
        copy_array(rubik->right,rubik->front,0,N-1,0,N-1);
        copy_array(rubik->front,temp,0,N-1,0,N-1);
    }

    if(face=='r')
    {
        rotate_face(rubik->up,'*');
        rotate_face(rubik->down,'\'');
        int temp[N][N];
        copy_array(temp,rubik->front,0,N-1,0,N-1);
        copy_array(rubik->front,rubik->right,0,N-1,0,N-1);
        copy_array(rubik->right,rubik->back,0,N-1,0,N-1);
        copy_array(rubik->back,rubik->left,0,N-1,0,N-1);
        copy_array(rubik->left,temp,0,N-1,0,N-1);
    }

    if(face=='b')
    {
        rotate_face(rubik->up,'"');
        rotate_face(rubik->down,'"');
        int temp[N][N];
        copy_array(temp,rubik->front,0,N-1,0,N-1);
        copy_array(rubik->front,rubik->back,0,N-1,0,N-1);
        copy_array(rubik->back,temp,0,N-1,0,N-1);
        copy_array(temp,rubik->left,0,N-1,0,N-1);
        copy_array(rubik->left,rubik->right,0,N-1,0,N-1);
        copy_array(rubik->right,temp,0,N-1,0,N-1);
    }

    if(face=='u')
    {
        rotate_face(rubik->left,'*');
        rotate_face(rubik->right,'\'');
        int temp[N][N];
        copy_array(temp,rubik->front,0,N-1,0,N-1);
        copy_array(rubik->front,rubik->up,0,N-1,0,N-1);
        copy_array(rubik->up,rubik->back,0,N-1,0,N-1);
        rotate_face(rubik->up,'"');///
        copy_array(rubik->back,rubik->down,0,N-1,0,N-1);
        rotate_face(rubik->back,'"');///
        copy_array(rubik->down,temp,0,N-1,0,N-1);
        ///rotate_face(rubik->up,'"');
    }

    if(face=='d')
    {
        rotate_face(rubik->left,'\'');
        rotate_face(rubik->right,'*');
        int temp[N][N];
        copy_array(temp,rubik->front,0,N-1,0,N-1);
        copy_array(rubik->front,rubik->down,0,N-1,0,N-1);
        copy_array(rubik->down,rubik->back,0,N-1,0,N-1);
        copy_array(rubik->back,rubik->up,0,N-1,0,N-1);
        copy_array(rubik->up,temp,0,N-1,0,N-1);
        rotate_face(rubik->down,'"');
    }
}

void reset_orientation(struct cube *rubik, char face) ///reverses orientation to the one given before applying set_orientation(rubik, face)
{
    if(face=='l')
    {
        rotate_face(rubik->up,'*');
        rotate_face(rubik->down,'\'');
        int temp[N][N];
        copy_array(temp,rubik->front,0,N-1,0,N-1);
        copy_array(rubik->front,rubik->right,0,N-1,0,N-1);
        copy_array(rubik->right,rubik->back,0,N-1,0,N-1);
        copy_array(rubik->back,rubik->left,0,N-1,0,N-1);
        copy_array(rubik->left,temp,0,N-1,0,N-1);
    }

    if(face=='r')
    {
        rotate_face(rubik->up,'\'');
        rotate_face(rubik->down,'*');
        int temp[N][N];
        copy_array(temp,rubik->left,0,N-1,0,N-1);
        copy_array(rubik->left,rubik->back,0,N-1,0,N-1);
        copy_array(rubik->back,rubik->right,0,N-1,0,N-1);
        copy_array(rubik->right,rubik->front,0,N-1,0,N-1);
        copy_array(rubik->front,temp,0,N-1,0,N-1);
    }

    if(face=='b')
    {
        rotate_face(rubik->up,'"');
        rotate_face(rubik->down,'"');
        int temp[N][N];
        copy_array(temp,rubik->front,0,N-1,0,N-1);
        copy_array(rubik->front,rubik->back,0,N-1,0,N-1);
        copy_array(rubik->back,temp,0,N-1,0,N-1);
        copy_array(temp,rubik->left,0,N-1,0,N-1);
        copy_array(rubik->left,rubik->right,0,N-1,0,N-1);
        copy_array(rubik->right,temp,0,N-1,0,N-1);
    }

    if(face=='u')
    {
        rotate_face(rubik->left,'\'');
        rotate_face(rubik->right,'*');
        int temp[N][N];
        copy_array(temp,rubik->front,0,N-1,0,N-1);
        copy_array(rubik->front,rubik->down,0,N-1,0,N-1);
        copy_array(rubik->down,rubik->back,0,N-1,0,N-1);
        rotate_face(rubik->down,'"');
        copy_array(rubik->back,rubik->up,0,N-1,0,N-1);
        rotate_face(rubik->back,'"');
        copy_array(rubik->up,temp,0,N-1,0,N-1);
    }

    if(face=='d')
    {
        rotate_face(rubik->left,'*');
        rotate_face(rubik->right,'\'');
        int temp[N][N];
        copy_array(temp,rubik->front,0,N-1,0,N-1);
        copy_array(rubik->front,rubik->up,0,N-1,0,N-1);
        copy_array(rubik->up,rubik->back,0,N-1,0,N-1);
        copy_array(rubik->back,rubik->down,0,N-1,0,N-1);
        copy_array(rubik->down,temp,0,N-1,0,N-1);
        rotate_face(rubik->back,'"');
    }
}

///function rotate sets the orientation of the cube so that 'face' is front face, performs given rotation,
/// and reverses orientation to the original one
void rotate(struct cube *rubik, char face, char angle, int layers)
{
    set_orientation(rubik, face);
    int repetition;
    if(angle == '*'){repetition = 1;}
    if(angle == '"'){repetition = 2;}
    if(angle == '\''){repetition = 3;}

    for(int k = 0; k<repetition; k++)
    {
        int temp_rotate[N][N];

        for(int i = 0; i < N; i++){ ///temp up
            for (int j = 0; j < layers; j++){
                temp_rotate[i][j] = rubik->up[N-1-j][i];
            }
        }

        for(int i = N-layers; i < N; i++){ ///update up
            for (int j = 0; j < N; j++){
                rubik->up[i][j] = rubik->left[N-j-1][i];
            }
        }

        for(int i = 0; i < N; i++){ ///update left
            for (int j = N-layers; j < N; j++){
                rubik->left[i][j] = rubik->down[N-1-j][i];
            }
        }

        for(int i = 0; i < layers; i++){ ///update down
            for (int j = 0; j < N; j++){
                rubik->down[i][j] = rubik->right[N-1-j][i];
            }
        }

        for(int i = 0; i < N; i++){ ///update right
            for (int j = 0; j < layers; j++){
                rubik->right[i][j] = temp_rotate[i][j];
            }
        }

        rotate_face(rubik->front, '*');
    }
        if(layers == N)  /// If the number of layers = cube size, we rotate also a back face with respect to a given angle
        {
            if(angle == '*')
            {
                rotate_face(rubik->back, '\'');
            }
            if(angle == '"')
            {
                rotate_face(rubik->back, '"');
            }
        if(angle == '\'')
            {
                rotate_face(rubik->back, '*');
            }
        }
    

    reset_orientation(rubik, face);
}

void play(struct cube *rubik) ///reads instructions, performs given rotations and prints results
{
    char sign = 'x'; ///initializing sign with random value != '.'
    int layers;

    while(sign != '.'){
        sign = (char)getchar();
        if(sign != '.'){
        if(sign == '\n')
        {
            print_cube(*rubik);
        }
        else
        {
            if((sign <= '0' + 9) && sign > '0')
            {
                layers = sign - '0';
                sign = (char)getchar();
                   while(sign >= '0' && sign <= '1' + 9)
            {
                layers*=10;
                layers+=(sign-'0');
                sign = (char)getchar();
            }
            }
            else
            {
                layers = 1;
            }
            char next = (char)getchar();

            if(next == '\n' || ((next <= '0' + 9) && next > '0') || next == 'u' || next == 'b' || next == 'f' || next == 'l' || next == 'r' || next == 'd')
            {
                rotate(rubik, sign, '*', layers);
                ungetc(next, stdin);
            }
            else
            {
                rotate(rubik, sign, next, layers);
            }
        }
        }
    }

}


int main()
{
    struct cube rubik;
    
    initialize_cube(&rubik);
    play(&rubik);

    return 0;
}
