#include <stdio.h>
#include <stdlib.h>

#define N  81 /// maximal possible number of cards

///uploading deck
int upload_deck(int deck[])
{
    int character = getchar();
    int i = 0;
    int data_size = 0;

    while(character != EOF){
        if(character > '0' && character < '4' ){
            while(character > '0' && character < '4'){
                deck[i] = 10 * deck [i] + (character - '0');
                character = getchar();
            }
            i++;
            data_size++;
        }
        else{
            character = getchar();
        }
    }

    return data_size;
}


///adding cards:
int draw_a_card(int D[])
{
    int c = 0;
    int i = 0;
    while(c == 0 && i < 81){
        if(D[i] != 0){
            c = D[i];
            D[i] = 0;
            i = 0;
        }
        else i++;
    }
    return c;
}

void add_a_card(int T[], int card)
{
    int i = N-1;
    while(card != 0 && i > 0){
        if(T[i-1] != 0){
            T[i] = card;
            card = 0;
        }
        else i--;
    }
    if(i == 0){
        T[i] = card;
    }

}

int add_triple(int T[], int D[], int not_found)
{
    int stop = 0;
    int card1 = draw_a_card(D);
    int card2 = draw_a_card(D);
    int card3 = draw_a_card(D);
    add_a_card(T, card1);
    add_a_card(T, card2);
    add_a_card(T, card3);
    if(card1 == 0){
        stop = 1;
    }
    else{
        if(not_found > 0){
            printf("+\n");
        }
    }
    return stop;
}


///discarding cards
void discard_set(int T[], int x, int y, int z)
{
    printf("- %d %d %d\n", T[x], T[y], T[z]);
    T[x] = 0;
    T[y] = 0;
    T[z] = 0;

}


///printing the table
void print_table(int T[], int n)
{
    printf("=");
   for (int i = 0; i< n; i++){
    if(T[i] != 0) printf(" %d", T[i]);
   }
   printf("\n");
}

///fiding set
int find_set(int T[], int n) /// function finds a set, discards it, and returns 1 if a set was found and 0 otherwise
{
    int found = 0;
    int x = 0, y = 0, z = 0;
    for (int i = 0; i < n && !found; i++){
        if(T[i] != 0){
        for(int j = i + 1; j < n && !found; j++){
            if(T[j] != 0){
            for (int k = j + 1; k < n && !found; k++){
                if(T[k] != 0){
                    int s_colour = (T[i] + T[j] + T[k])/1000 % 3;
                    int s_shape = (T[i] + T[j] + T[k])/100 % 3;
                    int s_number = (T[i] + T[j] + T[k])/10 % 3;
                    int s_shading = (T[i] + T[j] + T[k]) % 3;
                if(s_colour == 0 && s_shape == 0 && s_number == 0 && s_shading == 0){
                    found = 1;
                    x = i;
                    y = j;
                    z = k;
                }
                }
            }
            }
        }
        }
    }

    if(found == 1){
        discard_set(T, x, y, z);
    }
    return found;
}

void play_set(int D[], int T[], int n, int cards_on_table)
{
    int end_of_deck = 0;
    int not_found = 0;
    while( not_found == 0 || end_of_deck != 1){
            print_table(T, n);
            if(!find_set(T, n)){
                not_found++;
                end_of_deck = add_triple(T, D, not_found);
                if(!end_of_deck){
                    cards_on_table+=3;
                }
            }
            else{
                    if(!end_of_deck){
                }
                cards_on_table-=3;
                not_found = 0;
                if(cards_on_table<12){
                end_of_deck = add_triple(T, D, not_found);
                if(!end_of_deck){
                    cards_on_table+=3;
                }
                }
            }

        }
}

int load_table(int T[], int D[], int deck_size)
{
    int cards_on_table=0;
    for(int i = 0; i < N; i++){
         if(i < 12 && i < deck_size){
             T[i] = D[i];
             D[i] = 0;
             cards_on_table++;
         }
         else T[i] = 0;
     }
     return cards_on_table;

}


int main()
{
    int deck[N];
    for(int i = 0; i<N; i++){
        deck[i] = 0;
    }

    int deck_size = upload_deck(deck);
    int table[N];

    int cards_on_table = load_table(table, deck, deck_size);

    play_set(deck, table, N, cards_on_table);

    return 0;
}
