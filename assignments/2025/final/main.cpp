#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <iomanip>
#include "hardware_architecture.h"
#include "quantization.h"

typedef std::vector<std::vector<std::vector<float>>> Tensor;
std::vector<float> read_values(const std::string& path) {
    std::ifstream input(path.c_str());
    if (!input) throw std::string("Cannot open input file: ") + path;
    std::vector<float> values;
    std::string token;
    char c;
    while (input.get(c)) {
        if (c == ',' || c == ' ' || c == '\n' || c == '\r' || c == '\t') {
            if (!token.empty()) {
                std::size_t consumed = 0; float value = std::stof(token, &consumed);
                if (consumed != token.size() || !std::isfinite(value)) throw std::string("Invalid numeric input");
                values.push_back(value); token.clear();
            }
        } else token += c;
    }
    if (!token.empty()) {
        std::size_t consumed = 0; float value = std::stof(token, &consumed);
        if (consumed != token.size() || !std::isfinite(value)) throw std::string("Invalid numeric input");
        values.push_back(value);
    }
    return values;
}
void require_size(const std::vector<float>& values, int size) {
    if (static_cast<int>(values.size()) != size) throw std::string("Wrong data shape");
}
int kernel_size(const std::vector<float>& weights, int sets) {
    for (int k = 3; k <= 7; k += 2) if (static_cast<int>(weights.size()) == sets * k * k) return k;
    throw std::string("Kernel must be 3, 5 or 7");
}
int zero_in_buffer(int buffer) {
    for (int i = 0; i < 28; ++i) if (get_item_from_line_buffer(i, buffer) == 0) return i;
    return -1;
}
Tensor convolution(const Tensor& input, const std::vector<float>& weights, const std::vector<float>& biases, int outputs, int k) {
    int channels = static_cast<int>(input.size()), size = static_cast<int>(input[0].size());
    int out_size = (size - k) / 2 + 1;
    Tensor result(outputs, std::vector<std::vector<float>>(out_size, std::vector<float>(out_size, 0)));
    require_size(biases, outputs); require_size(weights, outputs * channels * k * k);
    for (int oc = 0; oc < outputs; ++oc) {
        for (int r = 0; r < out_size; ++r) for (int c = 0; c < out_size; ++c) result[oc][r][c] = biases[oc];
        for (int ic = 0; ic < channels; ++ic)
            for (int br = 0; br < k; br += 3) for (int bc = 0; bc < k; bc += 3) {
                float block[3][3] = {};
                for (int r = 0; r < 3; ++r) for (int c = 0; c < 3; ++c)
                    if (br + r < k && bc + c < k) block[r][c] = weights[((oc * channels + ic) * k + br + r) * k + bc + c];
                reset_buffer_and_matrix(); set_weight(block);
                for (int r = 0; r < out_size; ++r) for (int c = 0; c < out_size; ++c) {
                    float* output = 0;
                    // Reverse columns and skew rows, aligning all nine products
                    // at the bottom three PEs on cycle 4. No CPU convolution.
                    for (int tick = 0; tick < 5; ++tick) {
                        float values[3] = {};
                        bool unused[3] = {};
                        for (int row = 0; row < 3; ++row) {
                            int column = 2 - tick + row;
                            unused[row] = column < 0 || column >= 3 || br + row >= k || bc + column >= k;
                            if (!unused[row]) values[row] = input[ic][2 * r + br + row][2 * c + bc + column];
                        }
                        int z1 = unused[0] ? zero_in_buffer(1) : -1, z2 = unused[1] ? zero_in_buffer(2) : -1;
                        output = input_selection(z1 >= 0, z1 >= 0 ? z1 : 0, z2 >= 0, z2 >= 0 ? z2 : 0, values);
                    }
                    result[oc][r][c] += output[6] + output[7] + output[8];
                }
            }
    }
    return result;
}
double truncate3(float value) { return std::trunc(static_cast<double>(value) * 1000) / 1000; }
void write_tensor(const std::string& path, const Tensor& tensor) {
    std::ofstream output(path.c_str()); if (!output) throw std::string("Cannot create output file: ") + path;
    output << std::fixed << std::setprecision(3);
    for (std::size_t channel = 0; channel < tensor.size(); ++channel)
        for (std::size_t r = 0; r < tensor[channel].size(); ++r) {
            for (std::size_t c = 0; c < tensor[channel][r].size(); ++c) { if (c) output << ','; output << truncate3(tensor[channel][r][c]); }
            output << '\n';
        }
}
std::string directory(std::string path) {
    if (!path.empty() && path.back() != '/' && path.back() != '\\') path += '/';
    return path;
}
int main(int argc, char** argv) {
    if (argc > 3) { std::cerr << "Usage: cnn [input_directory] [output_directory]\n"; return 1; }
    std::string input_dir = argc >= 2 ? directory(argv[1]) : "", output_dir = argc >= 3 ? directory(argv[2]) : "";
    try {
        std::vector<float> image = read_values(input_dir + "img_label.txt"); require_size(image, 784);
        Tensor input(1, std::vector<std::vector<float>>(28, std::vector<float>(28)));
        double error = 0;
        for (int p = 0; p < 784; ++p) {
            float value = quantize(image[p]); input[0][p / 28][p % 28] = value;
            if (image[p] != 0) error += std::fabs(static_cast<double>(image[p]) - value) / std::fabs(image[p]);
        }
        std::vector<float> w1 = read_values(input_dir + "conv1_weight.txt"), w2 = read_values(input_dir + "conv2_weight.txt");
        int k = kernel_size(w1, 2); if (kernel_size(w2, 8) != k) throw std::string("Kernel sizes must match");
        Tensor first = convolution(input, w1, read_values(input_dir + "conv1_bias.txt"), 2, k);
        Tensor second = convolution(first, w2, read_values(input_dir + "conv2_bias.txt"), 4, k);
        write_tensor(output_dir + "result_of_conv1.txt", first); write_tensor(output_dir + "result_of_conv2.txt", second);
        std::vector<float> flat;
        for (const auto& channel : second) for (const auto& row : channel) for (float value : row) flat.push_back(value);
        std::vector<float> fw = read_values(input_dir + "fc_weight.txt"), fb = read_values(input_dir + "fc_bias.txt");
        require_size(fw, static_cast<int>(flat.size()) * 10); require_size(fb, 10);
        float classes[10]; int prediction = 0;
        std::ofstream fc((output_dir + "result_of_fc.txt").c_str()); if (!fc) throw std::string("Cannot create FC output");
        fc << std::fixed << std::setprecision(3);
        for (int c = 0; c < 10; ++c) {
            classes[c] = fb[c];
            for (std::size_t p = 0; p < flat.size(); ++p) classes[c] += flat[p] * fw[c * flat.size() + p];
            if (classes[c] > classes[prediction]) prediction = c;
            if (c) fc << ',';
            fc << truncate3(classes[c]);
        }
        fc << '\n';
        std::ofstream metrics((output_dir + "execution_result.txt").c_str()); if (!metrics) throw std::string("Cannot create metrics output");
        metrics << std::fixed << std::setprecision(2) << "Relative Error after Quantization: " << error * 100 / 784 << "%\nResult of the Prediction: " << prediction << '\n';
        metrics << std::setprecision(1) << "Energy Consumption: " << return_energy() << "\nStep Count: " << return_step() << '\n';
    } catch (const std::string& error) { std::cerr << error << '\n'; return 1; }
    catch (...) { std::cerr << "Invalid numeric input\n"; return 1; }
}
