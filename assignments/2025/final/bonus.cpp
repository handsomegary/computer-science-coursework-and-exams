#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <iomanip>
#include "quantization.h"

void token(std::ifstream& input, char text[64]) {
    char c;
    while (input.get(c)) {
        if (c == '#') { while (input.get(c) && c != '\n') {} continue; }
        if (c == ' ' || c == '\t' || c == '\r' || c == '\n') continue;
        int size = 0;
        do {
            if (size >= 63) throw "PGM token too long";
            text[size++] = c;
        } while (input.get(c) && c != ' ' && c != '\t' && c != '\r' && c != '\n');
        text[size] = '\0'; return;
    }
    throw "Truncated PGM input";
}
int number(std::ifstream& input) {
    char text[64]; token(input, text);
    long long value = 0;
    for (int i = 0; text[i]; ++i) {
        if (text[i] < '0' || text[i] > '9') throw "Invalid PGM number";
        value = value * 10 + text[i] - '0';
        if (value > 2147483647) throw "PGM number too large";
    }
    return static_cast<int>(value);
}
int main(int argc, char** argv) {
    if (argc < 2 || argc > 3) { std::cerr << "Usage: preprocess image.pgm [img_label_bonus.txt]\n"; return 1; }
    try {
        std::ifstream input(argv[1]); if (!input) throw "Cannot open PGM image";
        char magic[64]; token(input, magic);
        if (magic[0] != 'P' || magic[1] != '2' || magic[2]) throw "Only ASCII P2 PGM is supported";
        int width = number(input), height = number(input), maximum = number(input);
        if (width <= 0 || height <= 0 || width > 4096 || height > 4096 || maximum <= 0 || maximum > 65535) throw "Invalid PGM dimensions or range";
        std::vector<int> pixels(width * height);
        for (int& pixel : pixels) { pixel = number(input); if (pixel < 0 || pixel > maximum) throw "Invalid pixel"; }
        std::ofstream output(argc == 3 ? argv[2] : "img_label_bonus.txt"); if (!output) throw "Cannot create output";
        output << std::fixed << std::setprecision(10);
        for (int r = 0; r < 28; ++r) {
            for (int c = 0; c < 28; ++c) {
                // Bilinear interpolation at pixel centers, clamped at the edges.
                double y = (r + 0.5) * height / 28.0 - 0.5, x = (c + 0.5) * width / 28.0 - 0.5;
                if (y < 0) y = 0;
                if (y > height - 1) y = height - 1;
                if (x < 0) x = 0;
                if (x > width - 1) x = width - 1;
                int y0 = static_cast<int>(y), x0 = static_cast<int>(x), y1 = y0 + (y0 + 1 < height), x1 = x0 + (x0 + 1 < width);
                double fy = y - y0, fx = x - x0;
                double gray = (1 - fy) * ((1 - fx) * pixels[y0 * width + x0] + fx * pixels[y0 * width + x1]) + fy * ((1 - fx) * pixels[y1 * width + x0] + fx * pixels[y1 * width + x1]);
                float normalized = static_cast<float>(1 - gray / maximum);
                if (c) output << ',';
                output << quantize(normalized);
            }
            output << '\n';
        }
    } catch (const char* error) { std::cerr << error << '\n'; return 1; }
    catch (...) { std::cerr << "Invalid PGM input\n"; return 1; }
}
