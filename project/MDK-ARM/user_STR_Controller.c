//
//  user_STR_Controller.c
//  MR_DKNC
//
//  Created by NguyenQ.Thai on 12/26/20.
//  Copyright © 2020 NguyenQ.Thai. All rights reserved.
//

#include "user_STR_Controller.h"

struct STRPara STR = {};
struct PIPara PI = {};

//Update STR.u when finish function PI_Controller()
//Update STR.y and PI.error when read speed from encoder
//Call PI_Controller() before calling LMS_Estimation()
//int main(int argc, const char * argv[]) {
//    initPI_Para();
//    initSTR_Para();
//    //while(1){
//    //  PI_Controller();
//    //  delay_ms(PI.T*1000);
//    //  STR.pre_y = STR.y;
//    //  STR.y = TIM5->CNT...
//    //  PI.pre_error = PI.error;
//    //  PI.error = PI.setpoint - STR.y;
//    //  LMS_Estimation();
//    //}
//    
//    return 0;
//}

void initPI_Para(){
    PI.T            = 0.025;
    PI.pre_error    = 0;
    PI.error        = 0;
    PI.setpoint     = 0;
}

void initSTR_Para(){
    STR.lambda = 0.95; 
    STR.error  = 0;
		STR.pre_y  = 0;
    STR.y      = 0;
		STR.pre_u  = 0;
    STR.u      = 0;
	  float tmp_data[ARRAY_MAX_SIZE]  = {1, 1};
    float tmp_data1[ARRAY_MAX_SIZE] = {1, 0, 0, 1};
		
    matrixDelete(STR.phi, 			ARRAY_MAX_SIZE, ARRAY_MAX_SIZE);
    matrixDelete(STR.phi_T, 		ARRAY_MAX_SIZE, ARRAY_MAX_SIZE);
		matrixCreate(STR.theta, 		2, 1, tmp_data);
		matrixCreate(STR.pre_theta, 2, 1, tmp_data);
    matrixDelete(STR.L, 				ARRAY_MAX_SIZE, ARRAY_MAX_SIZE);
		matrixCreate(STR.P, 		2, 2, tmp_data1);
		matrixCreate(STR.pre_P, 2, 2, tmp_data1);
}

void PI_Controller(){
    STR.u = STR.pre_u + (0.508 - STR.theta[0][0])/STR.theta[1][0]*(PI.error - PI.pre_error) + 1.21/STR.theta[1][0]*PI.T/2*(PI.error + PI.pre_error);
    STR.pre_u = STR.u;
}

void LMS_Estimation(){
    float tmp_matrix1[ARRAY_MAX_SIZE][ARRAY_MAX_SIZE]; matrixDelete(tmp_matrix1, ARRAY_MAX_SIZE, ARRAY_MAX_SIZE);
    float tmp_matrix2[ARRAY_MAX_SIZE][ARRAY_MAX_SIZE]; matrixDelete(tmp_matrix2, ARRAY_MAX_SIZE, ARRAY_MAX_SIZE);
    float tmp_matrix3[ARRAY_MAX_SIZE][ARRAY_MAX_SIZE]; matrixDelete(tmp_matrix3, ARRAY_MAX_SIZE, ARRAY_MAX_SIZE);
    
    float tmp_data[ARRAY_MAX_SIZE] = {-STR.pre_y, STR.pre_u};
    
    matrixCreate(STR.phi, 2, 1, tmp_data);
    matrixTranspose(STR.phi, STR.phi_T, 2, 1);
    
    matrixMultiply(STR.phi_T, STR.pre_theta, tmp_matrix1, 1, 2, 2, 1);
    STR.error = STR.y - tmp_matrix1[0][0];                                  //Calculate error
    
    matrixMultiply(STR.pre_P, 	STR.phi, 		tmp_matrix1, 2, 2, 2, 1);
    matrixMultiply(STR.phi_T, 	STR.pre_P,  tmp_matrix2, 1, 2, 2, 2);
    matrixMultiply(tmp_matrix2, STR.phi, 		tmp_matrix3, 1, 2, 2, 1);
    matrix_numberMultiply(tmp_matrix1, STR.L, 2, 1, 1/tmp_matrix3[0][0]);   //Calculate L
    
    matrixMultiply(STR.pre_P, 	STR.phi, 	 tmp_matrix1, 2, 2, 2, 1);
    matrixMultiply(tmp_matrix1, STR.phi_T, tmp_matrix2, 2, 1, 1, 2);
    matrixMultiply(tmp_matrix2, STR.pre_P, tmp_matrix3, 2, 2, 2, 2);
    matrixMultiply(STR.phi_T, 	STR.pre_P, tmp_matrix1, 1, 2, 2, 2);
    matrixMultiply(tmp_matrix1, STR.phi, 	 tmp_matrix2, 1, 2, 2, 1);
    matrix_numberMultiply(tmp_matrix3, tmp_matrix1, 2, 2, -1/(tmp_matrix2[0][0] + STR.lambda));
    matrixSum(STR.pre_P, tmp_matrix1, tmp_matrix2, 2, 2);
    matrix_numberMultiply(tmp_matrix2, STR.P, 2, 2, 1/STR.lambda);          //Calculate P
    
    matrix_numberMultiply(STR.L, tmp_matrix1, 2, 1, STR.error);
    matrixSum(tmp_matrix1, STR.pre_theta, STR.theta, 2, 1);                 //Calculate theta
		
		matrixDelete(tmp_matrix1, ARRAY_MAX_SIZE, ARRAY_MAX_SIZE);
    matrixSum(tmp_matrix1, STR.P, STR.pre_P, 2, 2);
		matrixDelete(tmp_matrix1, ARRAY_MAX_SIZE, ARRAY_MAX_SIZE);
    matrixSum(tmp_matrix1, STR.theta, STR.pre_theta, 2, 1);
}

void matrixDelete(float inputMatrix[][ARRAY_MAX_SIZE], int rowInput, int columnInput){
    for(int i = 0; i < rowInput; i++){
        for(int j = 0; j < columnInput; j++){
            inputMatrix[i][j] = 0;
        }
    }
}

void matrix_numberMultiply(float inputMatrix[][ARRAY_MAX_SIZE], float outputMatrix[][ARRAY_MAX_SIZE], int rowInput, int columnInput, float gain){
    for (int row = 0; row < rowInput; row++){
        for (int col = 0; col < columnInput; col++){
            outputMatrix[row][col] = inputMatrix[row][col]*gain;
        }
    }
}

void matrixCreate(float returnMatrix[][ARRAY_MAX_SIZE], int outputRow, int outputColumn, float data[ARRAY_MAX_SIZE]){
    //matrixDelete(returnMatrix, ARRAY_MAX_SIZE, ARRAY_MAX_SIZE);
    for (int row = 0; row < outputRow; row++){
        for (int col = 0; col < outputColumn; col++){
            returnMatrix[row][col] = data[row*outputColumn+col];
        }
    }
}

void matrixMultiply(float firstMatrix[][ARRAY_MAX_SIZE], float secondMatrix[][ARRAY_MAX_SIZE], float returnMatrix[][ARRAY_MAX_SIZE], int rowFirst, int columnFirst, int rowSecond, int columnSecond){
    if (columnFirst == rowSecond){
        matrixDelete(returnMatrix, rowFirst, columnSecond);
        for (int row = 0; row < rowFirst; row++){
            for (int col = 0; col < columnSecond; col++){
                for (int cnt = 0; cnt < columnFirst; cnt++){
                    returnMatrix[row][col] += firstMatrix[row][cnt]*secondMatrix[cnt][col];
                }
            }
        }
    }
}

void matrixSum(float firstMatrix[][ARRAY_MAX_SIZE], float secondMatrix[][ARRAY_MAX_SIZE], float returnMatrix[][ARRAY_MAX_SIZE], int rowInput, int columnInput){
    for (int row = 0; row < rowInput; row++){
        for (int col = 0; col < columnInput; col++){
            returnMatrix[row][col] = firstMatrix[row][col] + secondMatrix[row][col];
        }
    }
}

void matrixTranspose(float inputMatrix[][ARRAY_MAX_SIZE], float returnMatrix[][ARRAY_MAX_SIZE], int rowInput, int columnInput){
    int rowOutput = columnInput;
    int columnOutput = rowInput;
    //matrixDelete(returnMatrix, rowOutput, columnOutput);
    for (int row = 0; row < rowOutput; row++){
        for (int col = 0; col < columnOutput; col++){
            returnMatrix[row][col] = inputMatrix[col][row];
        }
    }
}



