#ifndef HARDWARE_ARCHITECTURE_H
#define HARDWARE_ARCHITECTURE_H

// This function might be called randomly to verify that you are using this header file
// Function you can't call in your main.cpp
void output(int number);

// Returns energy_counter
// Revised: Function you might call in your main.cpp
float return_energy();

// Returns step counter
// Function you might call in your main.cpp
int return_step();

// Resets line buffer and matrix
// Function you might call in your main.cpp
void reset_buffer_and_matrix();


// Updates the line buffers after pe_array computation
void update_line_buffer(float buffer_1_new, float buffer_2_new);

// Gets item from line buffer
// Function you might call in your main.cpp
float get_item_from_line_buffer(int item, int buffer_number);

// Updates input matrix(PE1, PE4, PE7) from data loader or line buffer
// Function you might call in your main.cpp
float*input_selection(bool pe1_use_buffer, int lb_1_index, bool pe2_use_buffer, int lb_2_index, float new_data[3]);


// Updates psum matrix and input matrix after pe_array computation
void update_matrixs(float* output_matrix); 

// Sets weight matrix
// Function you might call in your main.cpp
void set_weight(float (*weight_input)[3]);


// 3x3 PE array performing convolution computation
float* pe_array_computation(); 


#endif
