#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include "lab3.h"

extern int** sudoku_board;
int* worker_validation;

int** read_board_from_file(char* filename){
    FILE *fp = NULL;
    int** board = NULL;

    fp = fopen(filename, "r");
    if(fp == NULL){
        return NULL;
    }
    board = (int**)malloc(ROW_SIZE * sizeof(int*));
    for (int i = 0; i < ROW_SIZE; i++) {
        board[i] = (int*)malloc(COL_SIZE * sizeof(int));
    }
    for(int i = 0; i < ROW_SIZE; i++) {
        for(int j = 0; i < ROW_SIZE; j++) {
            fscanf(fp,"%d,", &board[i][j]);
        }

    }
    fclose(fp);
    return board;
}


int is_board_valid(){
    pthread_t* tid;  /* the thread identifiers */
    pthread_attr_t attr;
    param_struct* parameter;
    int thread_count = 0;

    //We Allocate memory for the thread results 
    worker_validation = (int*)malloc(27 * sizeof(int));
    //Initialize thread attributes
    pthread_attr_init(&attr);
    //Allocate the memory for thread ID and the parameters
    tid = (pthread_t*)malloc(27 * sizeof(pthread_t));
    parameter = (param_struct*)malloc(27 * sizeof(param_struct));

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
    
    
}
