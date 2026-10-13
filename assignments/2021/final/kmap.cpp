#ifndef COURSE_KMAP_CPP
#define COURSE_KMAP_CPP
#include "logic.hpp"
#include <fstream>
#include <sstream>

inline std::uint32_t gray_assignment(unsigned n, std::uint32_t position) {
    const unsigned columns = (n + 1) / 2;
    const std::uint32_t low = (std::uint32_t(1) << columns) - 1;
    const std::uint32_t row = position >> columns, col = position & low;
    return ((row ^ (row >> 1)) << columns) | (col ^ (col >> 1));
}
// Self-written compatible replacement for the documented kmap.cpp interface.
inline void top() {
    std::ifstream in("kmap_in.txt");
    if (!in) throw std::runtime_error("cannot open kmap_in.txt");
    int count_value;
    if (!(in >> count_value) || count_value < 0 || count_value > 10000)
        throw std::runtime_error("invalid K-map function count");
    const unsigned count = static_cast<unsigned>(count_value);
    std::ostringstream result;
    for (unsigned f = 0; f < count; ++f) {
        int n_value;
        if (!(in >> n_value) || n_value < 0 || n_value > 8)
            throw std::runtime_error("invalid K-map variable count");
        const unsigned n = static_cast<unsigned>(n_value);
        std::vector<std::uint32_t> on, dc;
        for (std::uint32_t i = 0; i < (std::uint32_t(1) << n); ++i) {
            int value;
            if (!(in >> value) || value < 0 || value > 2) throw std::runtime_error("invalid K-map value");
            if (value == 1) on.push_back(gray_assignment(n, i));
            if (value == 2) dc.push_back(gray_assignment(n, i));
        }
        const std::vector<logic::Cube> answer = logic::minimize(n, on, dc);
        result << '#' << f + 1 << '\n';
        for (const logic::Cube& c : answer) {
            const std::string p = logic::pattern(c, n);
            for (unsigned j = 0; j < n; ++j) result << (j ? " " : "") << p[j];
            // For zero-variable functions a 1 is a unique constant-one marker.
            if (n == 0) result << '1';
            result << '\n';
        }
    }
    std::string excess;
    if (in >> excess) throw std::runtime_error("extra data in kmap_in.txt");
    std::ofstream out("kmap_out.txt");
    if (!out || !(out << result.str())) throw std::runtime_error("cannot write kmap_out.txt");
}
#endif
