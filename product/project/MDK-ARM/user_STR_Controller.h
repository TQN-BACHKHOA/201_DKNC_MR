//
//  user_STR_Controller.h
//  MR_DKNC
//
//  Created by NguyenQ.Thai on 12/26/20.
//  Copyright © 2020 NguyenQ.Thai. All rights reserved.
//

#ifndef user_STR_Controller_h
#define user_STR_Controller_h

#include <stdio.h>
#include <stdlib.h>

#define ARRAY_MAX_SIZE 2

void matrixDelete(float inputMatrix[][ARRAY_MAX_SIZE], int rowInput, int columnInput);
void matrixMultiply(float firstMatrix[][ARRAY_MAX_SIZE], float secondMatrix[][ARRAY_MAX_SIZE], float returnMatrix[][ARRAY_MAX_SIZE], int rowFirst, int columnFirst, int rowSecond, int columnSecond);
void matrixSum(float firstMatrix[][ARRAY_MAX_SIZE], float secondMatrix[][ARRAY_MAX_SIZE], float returnMatrix[][ARRAY_MAX_SIZE], int rowInput, int columnInput);
void matrixTranspose(float inputMatrix[][ARRAY_MAX_SIZE], float returnMatrix[][ARRAY_MAX_SIZE], int rowInput, int columnInput);
void matrixCreate(float returnMatrix[][ARRAY_MAX_SIZE], int outputRow, int outputColumn, float data[ARRAY_MAX_SIZE]);
void matrix_numberMultiply(float inputMatrix[][ARRAY_MAX_SIZE], float outputMatrix[][ARRAY_MAX_SIZE], int rowInput, int columnInput, float gain);
void initPI_Para(void);
void initSTR_Para(void);
void PI_Controller(void);
void LMS_Estimation(void);
void updatePara(void);

struct STRPara{
    float lambda, error, y, pre_y, u, pre_u;
    float phi[ARRAY_MAX_SIZE][ARRAY_MAX_SIZE], phi_T[ARRAY_MAX_SIZE][ARRAY_MAX_SIZE];
    float theta[ARRAY_MAX_SIZE][ARRAY_MAX_SIZE], pre_theta[ARRAY_MAX_SIZE][ARRAY_MAX_SIZE];
    float L[ARRAY_MAX_SIZE][ARRAY_MAX_SIZE];
    float P[ARRAY_MAX_SIZE][ARRAY_MAX_SIZE], pre_P[ARRAY_MAX_SIZE][ARRAY_MAX_SIZE];
} extern STR;

struct PIPara{
    float T, error, pre_error, setpoint, direction;
} extern PI;

struct FilterPara{
		float a,b,c,d;
} extern Filter;

#endif /* user_STR_Controller_h */
