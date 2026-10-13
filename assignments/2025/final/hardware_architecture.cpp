#include <iostream>
#include <fstream>
#include "hardware_architecture.h"
using namespace std;

static float input_matrix[3][3];
static float psum_matrix[3][3];
static float weight_matrix[3][3];
static float output_matrix[3][3];
static float line_buffer_array_1[28];
static float line_buffer_array_2[28];
static float energy_counter = 0;
static int step_counter = 0;



// This function might be called randomly to verify that you are using this header file.
// Function you can't call in your main.cpp
void output(int number) {
    cout << "number = " << number << endl;
}

static bool energy_called = false;
static bool step_called = false;

// Returns energy_counter
// Revised: Function you might call in your main.cpp
__attribute__((noinline))
float return_energy() {
    energy_called = true;
    return energy_counter;
}

// Returns step counter
// Function you might call in your main.cpp
__attribute__((noinline))
int return_step() {
    step_called = true;
    return step_counter;
}

// Resets line buffer and matrix
// Function you might call in your main.cpp
void reset_buffer_and_matrix(){
    for(int item=0; item<28 ; item++){
        line_buffer_array_1[item] = 0;
        line_buffer_array_2[item] = 0;
    }
    for(int i = 0; i<3; i++){
        for(int j=0;j<3;j++){
            input_matrix[i][j]=0;
            psum_matrix[i][j]=0;
            weight_matrix[i][j]=0;
            output_matrix[i][j]=0;
        }
    }
}

// Updates the line buffers after pe_array computation
void update_line_buffer(float buffer_1_new, float buffer_2_new) {
    for(int item=27; item>0 ; item--){
        line_buffer_array_1[item] = line_buffer_array_1[item-1];
        line_buffer_array_2[item] = line_buffer_array_2[item-1];
    }
    line_buffer_array_1[0] = buffer_1_new;
    line_buffer_array_2[0] = buffer_2_new;
}

// Gets item from line buffer
// Function you might call in your main.cpp
float get_item_from_line_buffer(int item, int buffer_number){
    if (buffer_number == 1) {
        return line_buffer_array_1[item];
    }
    else {
        return line_buffer_array_2[item];
    }
}

// Updates input matrix(PE1, PE4, PE7)
// Function you might call in your main.cpp
float* input_selection(bool pe1_use_buffer, int lb_1_index, bool pe2_use_buffer, int lb_2_index, float new_data[3]){
    // for PE1
    if(pe1_use_buffer){
        input_matrix[0][0] = get_item_from_line_buffer(lb_1_index, 1);
        energy_counter = energy_counter + 2.5;
    }
    else if(!pe1_use_buffer){
        input_matrix[0][0] = new_data[0];
        energy_counter = energy_counter + 10;
    }
    // for PE4
    if(pe2_use_buffer){
        input_matrix[1][0] = get_item_from_line_buffer(lb_2_index, 2);
        energy_counter = energy_counter + 2.5;
    }
    else if(!pe2_use_buffer){
        input_matrix[1][0] = new_data[1];
        energy_counter = energy_counter + 10;
    }
    // for PE7
    input_matrix[2][0] = new_data[2];
    energy_counter = energy_counter + 10;
    float* pe_output = pe_array_computation();
    return pe_output;
}


// Updates psum matrix and input matrix after pe_array computation
void update_matrixs(float* pe_output_matrix){
    update_line_buffer(input_matrix[1][2],input_matrix[2][2]);
    for(int i = 0; i < 3; ++i) {
        for(int j = 0; j < 3; ++j) {
            
            if(i + 1 < 3){
                psum_matrix[i+1][j] = pe_output_matrix[i*3 + j];
            }

            if(j!=2){
                input_matrix[i][2-j] = input_matrix[i][1-j];
            }
            else{
                input_matrix[i][0] = 0;
            }
        }
    }
}

// Sets weight matrix
// Function you might call in your main.cpp
void set_weight(float (*weight_input)[3]) {
    energy_counter = energy_counter + 5;
    for(int i = 0; i < 3; ++i) {
        for(int j = 0; j < 3; ++j) {
            weight_matrix[i][j] = weight_input[i][j];
        }
    }
}

// 3x3 PE array performing convolution computation
float* pe_array_computation(){
    int kernel_size = 3;
    static float output_matrix[3][3] = {0};
    for(int i = 0; i < kernel_size; i++){
        for(int j = 0; j < kernel_size; j++){
            output_matrix[i][j] = (input_matrix[i][j] * weight_matrix[i][j]) + psum_matrix[i][j];
        }
    }
    update_matrixs(&output_matrix[0][0]);
    step_counter ++;
    return &output_matrix[0][0];
}

struct CallLogger {
    ~CallLogger() {
        std::ofstream fout("/app/temp/.call_log");
        fout << "energy=" << energy_called << "\n";
        fout << "step=" << step_called << "\n";
    }
} _call_logger_instance;
