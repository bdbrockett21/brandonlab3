#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include "lab3.h"

extern int** sudoku_board;
int* worker_validation;

// Add the missing validate_region function
void* validate_region(void* param) {
    param_struct* params = (param_struct*)param;
    int used[9] = {0}; // Track numbers 1-9
    
    // For rows
    if (params->starting_col == 0 && params->ending_col == 8) {
        for (int col = 0; col < 9; col++) {
            int num = sudoku_board[params->starting_row][col];
            if (num < 1 || num > 9 || used[num-1]) {
                worker_validation[params->id] = 0;
                return NULL;
            }
            used[num-1] = 1;
        }
    }
    // For columns
    else if (params->starting_row == 0 && params->ending_row == 8) {
        for (int row = 0; row < 9; row++) {
            int num = sudoku_board[row][params->starting_col];
            if (num < 1 || num > 9 || used[num-1]) {
                worker_validation[params->id] = 0;
                return NULL;
            }
            used[num-1] = 1;
        }
    }
    // For 3x3 subgrids
    else {
        for (int row = params->starting_row; row < params->ending_row; row++) {
            for (int col = params->starting_col; col < params->ending_col; col++) {
                int num = sudoku_board[row][col];
                if (num < 1 || num > 9 || used[num-1]) {
                    worker_validation[params->id] = 0;
                    return NULL;
                }
                used[num-1] = 1;
            }
        }
    }
    
    worker_validation[params->id] = 1;
    return NULL;
}

int** read_board_from_file(char* filename) {
    FILE *fp = NULL;
    int** board = NULL;

    fp = fopen(filename, "r");
    if(fp == NULL) {
        return NULL;
    }
    board = (int**)malloc(ROW_SIZE * sizeof(int*));
    for (int i = 0; i < ROW_SIZE; i++) {
        board[i] = (int*)malloc(COL_SIZE * sizeof(int));
    }
    // Fix the inner loop condition: j < ROW_SIZE instead of i < ROW_SIZE
    for(int i = 0; i < ROW_SIZE; i++) {
        for(int j = 0; j < ROW_SIZE; j++) {
            fscanf(fp, "%d,", &board[i][j]);
        }
    }
    fclose(fp);
    return board;
}

int is_board_valid() {
    pthread_t* tid;  /* the thread identifiers */
    pthread_attr_t attr;
    param_struct* parameter;
    int thread_count = 0;

    // We Allocate memory for the thread results 
    worker_validation = (int*)malloc(27 * sizeof(int));
    // Initialize thread attributes
    pthread_attr_init(&attr);
    // Allocate the memory for thread ID and the parameters
    tid = (pthread_t*)malloc(27 * sizeof(pthread_t));
    parameter = (param_struct*)malloc(27 * sizeof(param_struct));

    // Create 9 threads for rows
    for (int i = 0; i < 9; i++) {
        parameter[thread_count].id = thread_count;
        parameter[thread_count].starting_row = i;
        parameter[thread_count].ending_row = i;
        parameter[thread_count].starting_col = 0;
        parameter[thread_count].ending_col = 8;
        pthread_create(&tid[thread_count], &attr, validate_region, &parameter[thread_count]);
        thread_count++;
    }
    
    // Create 9 threads for columns
    for (int i = 0; i < 9; i++) {
        parameter[thread_count].id = thread_count;
        parameter[thread_count].starting_row = 0;
        parameter[thread_count].ending_row = 8;
        parameter[thread_count].starting_col = i;
        parameter[thread_count].ending_col = i;
        pthread_create(&tid[thread_count], &attr, validate_region, &parameter[thread_count]);
        thread_count++;
    }
    
    // Create 9 threads for 3x3 subgrids
    for (int i = 0; i < 9; i += 3) {
        for (int j = 0; j < 9; j += 3) {
            parameter[thread_count].id = thread_count;
            parameter[thread_count].starting_row = i;
            parameter[thread_count].ending_row = i + 3;
            parameter[thread_count].starting_col = j;
            parameter[thread_count].ending_col = j + 3;
            pthread_create(&tid[thread_count], &attr, validate_region, &parameter[thread_count]);
            thread_count++;
        }
    }
    
    // We wait for all threads to complete
    for(int i = 0; i < thread_count; i++) {
        pthread_join(tid[i], NULL);
    }

    // Fix variable name: is_valid instead of is_vaild
    int is_valid = 1;
    for(int i = 0; i < thread_count; i++) {
        if(!worker_validation[i]) {
            is_valid = 0;
            break;
        }
    }
    
    // Clean up (fix spelling of worker_validation)
    free(tid);
    free(parameter);
    free(worker_validation);

    return is_valid;
}