#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>

static const size_t WORD_BITS = 64;
static const size_t MAX_BIT_INDEX = WORD_BITS - 1;


typedef struct moore moore_t;

typedef void (*transition_function_t)(uint64_t *next_state, uint64_t const *input,
                                      uint64_t const *state, size_t n, size_t s);

typedef void (*output_function_t)(uint64_t *output, uint64_t const *state,
                                  size_t m, size_t s);

moore_t * ma_create_full(size_t n, size_t m, size_t s, transition_function_t t,
                         output_function_t y, uint64_t const *q);

moore_t * ma_create_simple(size_t n, size_t m, transition_function_t t);

void ma_delete(moore_t *a);

int ma_connect(moore_t *a_in, size_t in, moore_t *a_out, size_t out, size_t num);

int ma_disconnect(moore_t *a_in, size_t in, size_t num);

int ma_set_input(moore_t *a, uint64_t const *input);

int ma_set_state(moore_t *a, uint64_t const *state);

uint64_t const * ma_get_output(moore_t const *a);

int ma_step(moore_t *at[], size_t num);

typedef struct moore moore_t;


/*
    Structure storing the source automaton and the bit connected to the input.
*/
struct connection_structure
{
    moore_t *source;          
    size_t source_bit_index; 
};
typedef struct connection_structure connection_t;


/*
    Structure storing the automata using the bit as an input.
*/
struct list_of_connections
{
    moore_t *connected_automaton;
    size_t entry;
    struct list_of_connections *next;
};

typedef struct list_of_connections list_of_connections_t;


/*  Structure representing Moore's automaton. Beyond parameters included in the definition,
    it keeps the bufor_input helping perform simultanious calculations on several connected
    automata, the array of connections incoming_connections used to read the input from connected
    atomata, and array of lists outgoing_connections used to control the automata taking input
    from automata's output(relevant for disconetcing and deleting automata).
*/
struct moore
{
    size_t input_size;
    size_t inner_state_size;
    size_t output_size;

    uint64_t *bufor_input;
    uint64_t *input;
    uint64_t *current_state;
    uint64_t *output;

    transition_function_t transition_function;
    output_function_t output_function;

    connection_t *incoming_connections;
    list_of_connections_t *outgoing_connections;
};
typedef struct moore moore_t;

/*
    Auxiliary structure keeping the state of the automaton during execution of ma_step.
*/
struct temp_state
{
    uint64_t *state;
    struct temp_state *next;
};
typedef struct temp_state temp_state_t;

/*
        Auxiliary function clearing the list of temporary states.
*/
void clear_list(temp_state_t *listac) {

    temp_state_t *current = listac;
    while (current != NULL) {
        temp_state_t *next = current->next;
        if (current->state) {
            free(current->state);
        }
        free(current);
        current = next;
    }
}

/*
    Standard functions used to operate on lists of type list_of_connection_t.
*/
static inline void initialize_list(list_of_connections_t *head) {

    head->connected_automaton = NULL;
    head->entry = 0;
    head->next = NULL;
}


static inline int add_to_list(list_of_connections_t *outgoing_list,  moore_t *input_automaton, int entry) {

    list_of_connections_t *new_element = malloc(sizeof(list_of_connections_t));
    if (!new_element) {
        errno = ENOMEM;
        return -1;
    }

    new_element->connected_automaton = input_automaton;
    new_element->entry = entry;
    new_element->next = outgoing_list->next;
    outgoing_list->next = new_element;

    return 0;

}

static inline void remove_from_list(list_of_connections_t *outgoing_lists,
                                     size_t output_bit, size_t removed_connected_bit, moore_t *removed_bit_automaton) {

    if (!outgoing_lists || !removed_bit_automaton) return;
    list_of_connections_t *head = &outgoing_lists[output_bit];
    list_of_connections_t *prev = head;
    list_of_connections_t *current = prev->next;
    while (current) {
        if (current->connected_automaton == removed_bit_automaton && current->entry == removed_connected_bit) {
            prev->next = current->next;
            free(current);
            current = prev->next;
        } else {
            prev = current;
            current = current->next;
        }
    }
}

/*
    Function updating input signals after reading them from the connected automaton.
*/
static inline void update_entries(const connection_t *connections, uint64_t *input, size_t input_size) {

    for(size_t i = 0; i < input_size; i++) {

        if(connections[i].source && connections[i].source_bit_index < connections[i].source->output_size) {

            int bit = connections[i].source_bit_index;
            bool signal = (connections[i].source)->output[bit / WORD_BITS] >>  (bit % WORD_BITS) & 1;
            if (signal) {
                input[i / WORD_BITS] |= (1ULL << (i % WORD_BITS));
            }
            else {
                input[i / WORD_BITS] &= ~(1ULL << (i % WORD_BITS));
            }          
        }
    }
}

/*
    Function ma_disconnect disconnects num following a_in automaton input signals,
    starting with the input with number in. If an input has been disconnected, it remains disconnected.
    
Parameters:
    a_in – wskaźnik na strukturę reprezentującą automat;
    in – numer wejścia automatu a_in;
    num – liczba rozłączanych sygnałów.

Output:
    0 – if the operation was sucesful;
    -1 – if the pointer has a NULL value, num parameter is equal 0, or given parameters range is incorrect;
         in such cases the function sets errno as EINVAL
*/
int ma_disconnect(moore_t *a_in, size_t in, size_t num) {

    if (!a_in || num == 0 || in >= a_in->input_size || num > a_in->input_size - in) {
        errno = EINVAL;
        return -1;
    }

    /// The loop removing connections the list of outgoing connections in "out" automaton, 
    /// and from the incoming connections in "in" automaton.
    if (a_in->input_size > 0) {
        for (size_t i = 0; i < num; i++) {
            if(a_in->incoming_connections[in + i].source) {

                moore_t *source = a_in->incoming_connections[in + i].source; 
                size_t source_bit = a_in->incoming_connections[in + i].source_bit_index;
                remove_from_list(source->outgoing_connections, source_bit, in + i, a_in);
                a_in->incoming_connections[in + i].source = NULL;
                a_in->incoming_connections[in + i].source_bit_index = 0;
            }
        }
    }
    
    return 0;
}

/*
Function ma_connect connects num following input signals of a_in automaton, starting with input entry indexed with in,
to num following output signals of a_out automaton starting with output indexed with out

Parameters:

    a_in – pointer to the "in" automaton;
    in – index of the first connected input signal;
    a_out – pointer to the "out" automaton;
    out – index of the first connected output signal;
    num – number of connections.

*/
int ma_connect(moore_t *a_in, size_t in, moore_t *a_out, size_t out, size_t num) {

    if (!a_in || !a_out || num == 0 ||in >= a_in->input_size || 
        out >= a_out->output_size || num > a_in->input_size - in ||
        num > a_out->output_size - out) {
        
        errno = EINVAL;
        return -1;
    }

    for(size_t i = 0; i < num; i++) {

        if (!a_in->incoming_connections[in + i].source) {
            a_in->incoming_connections[in + i].source = a_out;
            a_in->incoming_connections[in + i].source_bit_index = out + i;
            add_to_list(&a_out->outgoing_connections[out + i], a_in, in + i);
        }
        else {
            ma_disconnect(a_in, in + i, 1);
            a_in->incoming_connections[in + i].source = a_out;
            a_in->incoming_connections[in + i].source_bit_index = out + i;
            add_to_list(&a_out->outgoing_connections[out + i], a_in, in + i);
        }
    }

    update_entries(a_in->incoming_connections, a_in->input, a_in->input_size);

    return 0;
}

/*
    Function deleting the given automaton and freeing all memory used by it. If called with NULL pointer does nothing.
    Parameters:
        a - pointer to the deleted automaton
*/
void ma_delete(moore_t *a) {
    
    if (!a) return;
    //Step 1: we detach connections incoming to the automaton.  
    for (size_t i = 0; i < a->input_size; i++) {
        if(a->incoming_connections[i].source) {

            moore_t *source = a->incoming_connections[i].source;
            size_t source_bit = a->incoming_connections[i].source_bit_index;
            remove_from_list(source->outgoing_connections, source_bit, i, a);
        }
    }

    //Step 2: we detach connections outgoing from the automaton. 
        for (size_t out_bit = 0; out_bit < a->output_size; out_bit++) {
            list_of_connections_t *current = a->outgoing_connections[out_bit].next;
            while (current) {

                list_of_connections_t *next = current->next;
                moore_t *connected_automaton = current->connected_automaton;
                size_t in_bit = current->entry;         

                connected_automaton->incoming_connections[in_bit].source = NULL;
                connected_automaton->incoming_connections[in_bit].source_bit_index = 0;                 

                free(current);
                current = next;
            }
            a->outgoing_connections[out_bit].next = NULL;
        }

    // Step 3: We free the remaining automaton's resources.
    if(a->input_size>0) {
        free(a->bufor_input);
        free(a->input);
        free(a->incoming_connections);
    }
    free(a->current_state);
    free(a->output);
    free(a->outgoing_connections);
    free(a);
}

/*
    Function ma_get_output returns pointer to the sequence of bits containg autput of the automaton.
    Parameters:

    a – pointer to the structure representing automaton.
*/
uint64_t const * ma_get_output(moore_t const *a) {

    if (!a) {
        errno = EINVAL;
        return NULL;
    }

    a->output_function(a->output, a->current_state, a->output_size, a->inner_state_size);
    
    return  a->output;
}

/*
Function ma_set_input sest the signals on unpluged bits(entries) of the automaton,
ignoring entries that are already plugged.
Parameters:

    a – pointer to the automaton structure;
    input – pointer to the sequence of n bits containing n inputs(entries) of the automaton
    
Output:

    0 – if the operation was sucessful;
    -1 – if the automaton has no entries, or one of the arguments is a NULL pointer,
         in this case errno is set as EINVAL
*/
int ma_set_input(moore_t *a, uint64_t const *input) {

    if(!a || !input || a->input_size == 0)
    {
        errno = EINVAL;
        return -1;
    }

    for(size_t i = 0 ; i < a->input_size; i++) {
        if(!a->incoming_connections[i].source) {
            size_t bit = i;
            bool signal = input[bit / WORD_BITS] >> (bit % WORD_BITS) & 1;
            if (signal) {
                a->input[bit / WORD_BITS] |= (1ULL << (bit % WORD_BITS));
            }
            else {
                a->input[bit / WORD_BITS] &= ~(1ULL << (bit % WORD_BITS));
            }        
        }
    }

    for(uint64_t i = 0 ; i < a->input_size; i++) {
        if(!a->incoming_connections[i].source) {
            size_t bit = i;
            bool signal = input[bit / WORD_BITS] >> (bit % WORD_BITS) & 1;
            if (signal) {
                a->bufor_input[bit / WORD_BITS] |= (1ULL << (bit % WORD_BITS));
            }
            else {
                a->bufor_input[bit / WORD_BITS] &= ~(1ULL << (bit % WORD_BITS));
            }  
        }
    }

    update_entries(a->incoming_connections, a->input, a->input_size);

    a->output_function(a->output, a->current_state, a->output_size, a->inner_state_size);

    return 0;
}

/*
Function for setting the state of automaton.
Parameters:
    a – pointer to the automaton's structure;
    state – pointer to the sequence of bits - new state to set up.

Output:
    0 – in case of sucess;
    -1 – if one of the arguments is a NULL pointer, in this case errno set as EINVAL.

*/
int ma_set_state(moore_t *a, uint64_t const *state) {
    if (!a || !state) {
        errno = EINVAL;
        return -1;
    }

    memcpy(a->current_state, state, ((a->inner_state_size + MAX_BIT_INDEX) / WORD_BITS) * sizeof(uint64_t));

    a->output_function(a->output, a->current_state, a->output_size, a->inner_state_size);

    return 0;
}

// Output function setting automaton's state as an output.
static inline void identity(uint64_t *output, const uint64_t *state, size_t m, size_t s) {
    (void)m; 
    size_t words = (s + MAX_BIT_INDEX) / WORD_BITS;
    for(size_t i = 0; i < words; i++) {
        output[i] = state[i];
    }  
}

/*
Function performs one step of computation for every automaton.
Parameters:
    at – an array of automata;
    num – size of an array.

Output:
    0 – if the operation was sucess;
    -1 – if one of the pointers is a NULL pointer, or memory couldn't be allocated.
*/
int ma_step(moore_t *at[], size_t num) {
    
    if (at == NULL || num == 0) {
        errno = EINVAL;
        return -1;
    }
    for(size_t i = 0; i < num; i++) {
        if (!at[i]) {
            errno = EINVAL;
            return -1;
        }
    }
 
    temp_state_t *head = NULL;
    temp_state_t **iterator = &head;

    // Step 1: We create temporary list of new states
    for (size_t i = 0; i < num; i++) {
        temp_state_t *node = malloc(sizeof(temp_state_t));
        if (!node) {
            clear_list(head);
            errno = ENOMEM;
            return -1;
        }

        size_t state_size = (at[i]->inner_state_size + MAX_BIT_INDEX) / WORD_BITS;
        node->state = malloc(state_size * sizeof(uint64_t));
             
        if (!node->state) {
            free(node);
            clear_list(head);
            errno = ENOMEM;
            return -1;
        }

        node->next = NULL;
        *iterator = node;
        iterator = &node->next;
    }

    /// Step 2: Use the transition function that puts new states on the temp_states liste,
    temp_state_t *curr = head;
    for (size_t i = 0; i < num; i++) {

        at[i]->transition_function(curr->state, at[i]->input,
                                   at[i]->current_state, at[i]->input_size, at[i]->inner_state_size);

        curr = curr->next;
    }

    /// Step 3: We copy the new states to the current_state storage, and calculate the new output.
    curr = head;
    for (size_t i = 0; i < num; i++) {

        memcpy(at[i]->current_state, curr->state, 
                ((at[i]->inner_state_size + MAX_BIT_INDEX) / WORD_BITS) * sizeof(uint64_t));
        
        at[i]->output_function(at[i]->output, curr->state,
                               at[i]->output_size,at[i]->inner_state_size);
        curr = curr->next;
    }

    /// Step 4: We update entries, inputs and bufors ensuring communication between atutomata.
    for (size_t i = 0; i < num; i++) {

        if(at[i]->input_size > 0) {
            update_entries(at[i]->incoming_connections, at[i]->bufor_input, at[i]->input_size);
        }
        
        size_t num_words = (at[i]->input_size + MAX_BIT_INDEX) / WORD_BITS;

        if (at[i]->input_size>0) {
            memcpy(at[i]->input, at[i]->bufor_input, num_words * sizeof(uint64_t));
        }
        
    }

    ///Step 5: We clear the auxiliary list.
    clear_list(head);
    
    return 0;
}

/*
    The function initializing simple Moore's automaton, where output size is equal state size (measured in bits),
    and the output function is an identity function. If the function couldn't create the automaton due to 
    lack of memory or improper values of parameters, it returns NULL pointer and sets errno as ENOMEM or 
    EINVAL accordingly.
    Paramaters:

    n – number of input signals;
    s – number of inner state's bits and output's bits;
    t – transition function.
*/
moore_t * ma_create_simple(size_t n, size_t s, transition_function_t t)
{
    if (s == 0 || !t) {
        errno = EINVAL;
        return NULL;
    }

    if (n > SIZE_MAX - MAX_BIT_INDEX || s > SIZE_MAX - MAX_BIT_INDEX) {
        errno = ENOMEM;
        return NULL;
    }

    moore_t *new_moore = malloc(sizeof(moore_t));
    if (!new_moore) {
        return NULL;
    }

    new_moore->input_size = n;
    new_moore->inner_state_size = s;
    new_moore->output_size = s;

    size_t input_words = ((n + MAX_BIT_INDEX) / WORD_BITS) * sizeof(uint64_t);
    size_t state_words = ((s + MAX_BIT_INDEX) / WORD_BITS) * sizeof(uint64_t);

    if (n > 0) {
        new_moore->input = calloc(input_words, sizeof(uint64_t));
        new_moore->bufor_input = calloc(input_words, sizeof(uint64_t));
        if(!new_moore->input || !new_moore->bufor_input) {
            free(new_moore->input);
            free(new_moore->bufor_input);
            free(new_moore);
            errno = ENOMEM;
            return NULL;
        }
    }
    else {
        new_moore->bufor_input = NULL;
        new_moore->input = NULL;
    }
    new_moore->current_state = malloc(state_words);
    new_moore->output = calloc(state_words, sizeof(uint64_t));

    if (!new_moore->current_state || !new_moore->output) {

        free(new_moore->input);
        free(new_moore->bufor_input);
        free(new_moore->current_state);
        free(new_moore->output);
        free(new_moore);
        errno = ENOMEM;
        return NULL;
    }

    new_moore->output_function = identity;
    new_moore->transition_function = t;

    if (n > 0) {
        new_moore->incoming_connections = malloc(n * sizeof(connection_t));
        if (!new_moore->incoming_connections) {
            free(new_moore->input);
            free(new_moore->bufor_input);
            free(new_moore->current_state);
            free(new_moore->output);
            free(new_moore->incoming_connections);
            free(new_moore);
            errno = ENOMEM;
            return NULL;
        }
    }
    else {
        new_moore->incoming_connections = NULL;
    }
    new_moore->outgoing_connections = malloc(s * sizeof(list_of_connections_t));


    if (!new_moore->outgoing_connections) {
        free(new_moore->input);
        free(new_moore->bufor_input);
        free(new_moore->current_state);
        free(new_moore->output);
        free(new_moore->incoming_connections);
        free(new_moore->outgoing_connections);
        free(new_moore);
        errno = ENOMEM;
        return NULL;
    }

    for (size_t i = 0; i < ((s + MAX_BIT_INDEX) / WORD_BITS); i++) {
        new_moore->current_state[i] = 0;
    }

    for (size_t i = 0; i < n; i++) {
        new_moore->incoming_connections[i].source = NULL;
        new_moore->incoming_connections[i].source_bit_index = 0;
    }


    for (size_t i = 0; i < s; i++) {
        initialize_list(&new_moore->outgoing_connections[i]);
    }

    new_moore->output_function(new_moore->output, new_moore->current_state,
                                new_moore->output_size,new_moore->inner_state_size);

    return new_moore;

}

/*
    The function initializing Moore's automaton with given set of parameters. 
    If the function couldn't create the automaton due to lack of memory or improper values of parameters,
    it returns NULL pointer and sets errno as ENOMEM or EINVAL accordingly.
Parameters:

    n – number of input bits;
    m – number of output bits;
    s – number of current state bits;
    t – transition function;
    y – output function;
    q – pointer to the sruence of bits containing current state of the automaton.
*/
moore_t * ma_create_full(size_t n, size_t m, size_t s, transition_function_t t, output_function_t y, uint64_t const *q)
{
    
    if (s<=0 || m <= 0 || !t || !y || !q) {
        errno = EINVAL;
        return NULL;
    }

    if(n > SIZE_MAX - MAX_BIT_INDEX || s > SIZE_MAX - MAX_BIT_INDEX || m > SIZE_MAX - MAX_BIT_INDEX) {
        errno = ENOMEM;
        return NULL;
    }

    moore_t *new_moore = malloc(sizeof(moore_t));
    if (!new_moore) {
        free(new_moore);
        return NULL;
    }

    new_moore->input_size = n;
    new_moore->output_size = m;
    new_moore->inner_state_size = s;

    size_t input_words = ((n + MAX_BIT_INDEX) / WORD_BITS) * sizeof(uint64_t);
    size_t state_words = ((s + MAX_BIT_INDEX) / WORD_BITS) * sizeof(uint64_t);
    size_t output_words = ((m + MAX_BIT_INDEX) / WORD_BITS) * sizeof(uint64_t);

    if(n > 0) {
        new_moore->input = calloc(input_words, sizeof(uint64_t));
        new_moore->bufor_input = calloc(input_words, sizeof(uint64_t));
        if (!new_moore->input || !new_moore->bufor_input) {
            free(new_moore->input);
            free(new_moore->bufor_input);
            free(new_moore);
            errno = ENOMEM;
            return NULL;
        }
    }
    else {
        new_moore->bufor_input = NULL;
        new_moore->input = NULL;
    }

    new_moore->current_state = malloc(state_words);
    new_moore->output = malloc(output_words);

    if (!new_moore->current_state || !new_moore->output) {
        free(new_moore->input);
        free(new_moore->bufor_input);
        free(new_moore->current_state);
        free(new_moore->output);
        free(new_moore);
        errno = ENOMEM;
        return NULL;
    }

    memcpy(new_moore->current_state, q, state_words);

    new_moore->output_function = y;
    new_moore->transition_function = t;

    if (n > 0) {
        new_moore->incoming_connections = malloc(n * sizeof(connection_t));
        if (!new_moore->incoming_connections) {
            free(new_moore->input);
            free(new_moore->bufor_input);
            free(new_moore->current_state);
            free(new_moore->output);
            free(new_moore->incoming_connections);
            free(new_moore);
            errno = ENOMEM;
            return NULL;
        }
    }
    else {
        new_moore->incoming_connections = NULL;
    }

    new_moore->outgoing_connections = malloc(m * sizeof(list_of_connections_t));

    if (!new_moore->outgoing_connections) {
        free(new_moore->input);
        free(new_moore->bufor_input);
        free(new_moore->current_state);
        free(new_moore->output);
        free(new_moore->incoming_connections);
        free(new_moore);
        errno = ENOMEM;
        return NULL;
    }

    for (size_t i = 0; i < n; i++) {
        new_moore->incoming_connections[i].source = NULL;
        new_moore->incoming_connections[i].source_bit_index = 0;
    }

    for (size_t i = 0; i < m; i++) {
        initialize_list(&new_moore->outgoing_connections[i]);
    }
    new_moore->output_function(new_moore->output, new_moore->current_state,
                               new_moore->output_size,new_moore->inner_state_size);

    return new_moore;
}