#ifndef COURSE_LOGIC_HPP
#define COURSE_LOGIC_HPP
#include <algorithm>
#include <cstdint>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

// A fixed bit in mask is a literal; a zero bit is a don't-care.
namespace logic {
struct Cube {
    std::uint32_t bits, mask;
    Cube(std::uint32_t b = 0, std::uint32_t m = 0) : bits(b & m), mask(m) {}
    bool operator<(const Cube& c) const {
        return mask < c.mask || (mask == c.mask && bits < c.bits);
    }
    bool operator==(const Cube& c) const { return bits == c.bits && mask == c.mask; }
    bool covers(std::uint32_t m) const { return (m & mask) == bits; }
};
inline unsigned popcount(std::uint32_t n) {
    unsigned c = 0;
    while (n) { n &= n - 1; ++c; }
    return c;
}
inline std::string pattern(const Cube& c, unsigned n) {
    std::string s;
    for (unsigned i = n; i > 0; --i) {
        const std::uint32_t b = std::uint32_t(1) << (i - 1);
        s += !(c.mask & b) ? '2' : (c.bits & b ? '1' : '0');
    }
    return s;
}
inline bool subset(const std::vector<unsigned>& a, const std::vector<unsigned>& b) {
    return std::includes(b.begin(), b.end(), a.begin(), a.end());
}
inline std::vector<Cube> minimize(unsigned n, std::vector<std::uint32_t> on,
                                  std::vector<std::uint32_t> dc = std::vector<std::uint32_t>()) {
    if (n > 26) throw std::runtime_error("at most 26 Boolean variables are supported");
    const std::uint32_t full = (std::uint32_t(1) << n) - 1;
    std::sort(on.begin(), on.end());
    on.erase(std::unique(on.begin(), on.end()), on.end());
    if (on.empty()) return std::vector<Cube>();
    std::set<Cube> current, primes;
    for (std::uint32_t m : on) {
        if (m > full) throw std::runtime_error("minterm outside variable range");
        current.insert(Cube(m, full));
    }
    for (std::uint32_t m : dc) {
        if (m > full) throw std::runtime_error("don't-care outside variable range");
        current.insert(Cube(m, full));
    }
    // Quine-McCluskey: combine cubes with exactly one differing fixed bit.
    while (!current.empty()) {
        std::set<Cube> combined, used;
        for (const Cube& c : current) {
            for (unsigned i = 0; i < n; ++i) {
                const std::uint32_t bit = std::uint32_t(1) << i;
                if (!(c.mask & bit) || (c.bits & bit)) continue;
                const Cube other(c.bits | bit, c.mask);
                if (current.count(other)) {
                    used.insert(c); used.insert(other);
                    combined.insert(Cube(c.bits, c.mask & ~bit));
                }
            }
        }
        for (const Cube& c : current) if (!used.count(c)) primes.insert(c);
        current.swap(combined);
    }
    std::vector<Cube> p;
    for (const Cube& c : primes) {
        for (std::uint32_t m : on) if (c.covers(m)) { p.push_back(c); break; }
    }
    std::sort(p.begin(), p.end(), [n](const Cube& a, const Cube& b) {
        return pattern(a, n) < pattern(b, n);
    });
    std::vector<std::vector<unsigned> > chart;
    std::set<unsigned> essential;
    for (std::uint32_t m : on) {
        std::vector<unsigned> choices;
        for (unsigned i = 0; i < p.size(); ++i) if (p[i].covers(m)) choices.push_back(i);
        if (choices.empty()) throw std::runtime_error("uncovered minterm");
        if (choices.size() == 1) essential.insert(choices[0]);
        chart.push_back(choices);
    }
    std::vector<std::vector<unsigned> > clauses;
    for (const std::vector<unsigned>& row : chart) {
        bool done = false;
        for (unsigned i : row) if (essential.count(i)) done = true;
        if (!done) clauses.push_back(row);
    }
    std::sort(clauses.begin(), clauses.end(), [](const std::vector<unsigned>& a,
                                                const std::vector<unsigned>& b) {
        return a.size() < b.size() || (a.size() == b.size() && a < b);
    });
    clauses.erase(std::unique(clauses.begin(), clauses.end()), clauses.end());
    // Petrick's method: multiply sums of prime indices. X + XY = X
    // absorbs strict supersets after each multiplication.
    std::vector<std::vector<unsigned> > products(1);
    for (const std::vector<unsigned>& clause : clauses) {
        std::set<std::vector<unsigned> > next;
        for (const std::vector<unsigned>& product : products) {
            for (unsigned choice : clause) {
                std::vector<unsigned> term = product;
                const auto pos = std::lower_bound(term.begin(), term.end(), choice);
                if (pos == term.end() || *pos != choice) term.insert(pos, choice);
                next.insert(term);
            }
        }
        std::vector<std::vector<unsigned> > ordered(next.begin(), next.end());
        std::sort(ordered.begin(), ordered.end(), [](const std::vector<unsigned>& a,
                                                    const std::vector<unsigned>& b) {
            return a.size() < b.size() || (a.size() == b.size() && a < b);
        });
        products.clear();
        for (const std::vector<unsigned>& term : ordered) {
            bool absorbed = false;
            for (const std::vector<unsigned>& kept : products) {
                if (subset(kept, term)) { absorbed = true; break; }
            }
            if (!absorbed) products.push_back(term);
        }
    }
    std::vector<unsigned> best;
    unsigned best_literals = 0;
    bool have_best = false;
    for (std::vector<unsigned> term : products) {
        term.insert(term.end(), essential.begin(), essential.end());
        std::sort(term.begin(), term.end());
        unsigned literals = 0;
        for (unsigned i : term) literals += popcount(p[i].mask);
        if (!have_best || term.size() < best.size() ||
            (term.size() == best.size() && (literals < best_literals ||
             (literals == best_literals && term < best)))) {
            best = term; best_literals = literals; have_best = true;
        }
    }
    std::vector<Cube> result;
    for (unsigned i : best) result.push_back(p[i]);
    return result;
}
inline std::string sop(const std::vector<Cube>& cubes, const std::vector<std::string>& names) {
    if (cubes.empty()) return "0";
    std::string expression;
    for (const Cube& c : cubes) {
        if (!expression.empty()) expression += "+";
        if (!c.mask) { expression += "1"; continue; }
        expression += "(";
        bool first = true;
        for (unsigned i = 0; i < names.size(); ++i) {
            const std::uint32_t bit = std::uint32_t(1) << (names.size() - 1 - i);
            if (!(c.mask & bit)) continue;
            if (!first) expression += "*";
            if (!(c.bits & bit)) expression += "!";
            expression += names[i]; first = false;
        }
        expression += ")";
    }
    return expression;
}
} // namespace logic
#endif
