#ifndef VOUCHERS_HPP
#define VOUCHERS_HPP

#include <algorithm>
#include <array>
#include <iomanip>
#include <iostream>
#include <string>

// Validate a whole token before accumulating: even a very long invalid token
// cannot overflow an intermediate integer or corrupt the next input.
inline bool parsePrice(const std::string& token, long long& value) {
    if (token == "-1") {
        value = -1;
        return true;
    }
    std::size_t i = 0;
    if (!token.empty() && token[0] == '+') ++i;
    if (i == token.size()) return false;
    value = 0;
    for (; i < token.size(); ++i) {
        if (token[i] < '0' || token[i] > '9') return false;
        const int digit = token[i] - '0';
        if (value > (2147483647LL - digit) / 10) return false;
        value = value * 10 + digit;
    }
    return true;
}

inline int walletValue(const std::array<int, 5>& wallet,
                       const std::array<int, 5>& value) {
    int total = 0;
    for (int i = 0; i < 5; ++i) total += wallet[i] * value[i];
    return total;
}

inline bool exactFrom(int first, int need, const std::array<int, 5>& wallet,
                      const std::array<int, 5>& value) {
    // Every denomination is a multiple of the next one, so a maximal greedy
    // selection can decide whether the smaller available vouchers pay exactly.
    for (int i = first; i < 5; ++i) {
        const int count = std::min(wallet[i], need / value[i]);
        need -= count * value[i];
    }
    return need == 0;
}

inline std::array<int, 5> originalPayment(
    int need, const std::array<int, 5>& wallet,
    const std::array<int, 5>& value) {
    std::array<int, 5> used = {{0, 0, 0, 0, 0}};
    for (int i = 0; i < 5 && need > 0; ++i) {
        used[i] = std::min(wallet[i], need / value[i]);
        const int rest = need - used[i] * value[i];
        // If exact smaller payment is impossible, the current larger voucher
        // must be preferred even when smaller vouchers would lose less money.
        if (used[i] < wallet[i] && !exactFrom(i + 1, rest, wallet, value)) {
            ++used[i];
        }
        need -= used[i] * value[i];
    }
    return used;
}

inline void searchPayment(int index, int paid, int count, int need,
                          const std::array<int, 5>& wallet,
                          const std::array<int, 5>& value,
                          std::array<int, 5>& candidate,
                          std::array<int, 5>& best,
                          int& bestPaid, int& bestCount) {
    if (index == 5) {
        if (paid < need) return;
        if (paid < bestPaid || (paid == bestPaid && count < bestCount) ||
            (paid == bestPaid && count == bestCount && candidate > best)) {
            bestPaid = paid;
            bestCount = count;
            best = candidate;
        }
        return;
    }
    for (int n = 0; n <= wallet[index]; ++n) {
        candidate[index] = n;
        searchPayment(index + 1, paid + n * value[index], count + n, need,
                      wallet, value, candidate, best, bestPaid, bestCount);
    }
}

inline std::array<int, 5> minimumLossPayment(
    int need, const std::array<int, 5>& wallet,
    const std::array<int, 5>& value) {
    std::array<int, 5> candidate = {{0, 0, 0, 0, 0}};
    std::array<int, 5> best = candidate;
    int bestPaid = 831;
    int bestCount = 26;
    searchPayment(0, 0, 0, need, wallet, value, candidate, best,
                  bestPaid, bestCount);
    return best;
}

inline void showVouchers(const std::array<int, 5>& counts,
                         const std::array<int, 5>& value) {
    for (int i = 0; i < 5; ++i) {
        if (counts[i] != 0) {
            std::cout << "    " << counts[i] << " $" << std::setw(3)
                      << value[i] << " voucher\n";
        }
    }
}

inline int runVouchers(bool minimizeLoss) {
    const std::array<int, 5> value = {{100, 50, 10, 5, 1}};
    std::array<int, 5> wallet = {{5, 5, 5, 5, 5}};
    std::cout << "*** Welcome to Quintuple Stimulus Vouchers System ***\n";
    while (walletValue(wallet, value) != 0) {
        std::cout << "Please enter the good price: ";
        std::string token;
        if (!(std::cin >> token)) return 0;
        long long price;
        if (!parsePrice(token, price)) {
            std::cout << "Invalid good price. Please try again.\n";
            continue;
        }
        if (price == -1) {
            std::cout << "Bye~\n";
            return 0;
        }
        std::string type;
        do {
            std::cout << "Please enter the type of vouchers (d/p): ";
            if (!(std::cin >> type)) return 0;
            if (type != "d" && type != "p")
                std::cout << "Invalid voucher type. Please try again.\n";
        } while (type != "d" && type != "p");
        int rate = 90;
        if (type == "d") {
            std::string member;
            do {
                std::cout << "Are you a member of NCU Bank (y/n): ";
                if (!(std::cin >> member)) return 0;
                if (member != "y" && member != "n")
                    std::cout << "Invalid member status. Please try again.\n";
            } while (member != "y" && member != "n");
            rate = member == "y" ? 72 : 80;
        }
        const long long discounted = price * rate / 100;
        std::cout << "The price you need to pay: " << discounted << '\n';
        if (discounted > walletValue(wallet, value)) {
            std::cout << "You don't have enough voucher to buy the good\n\n";
            continue;
        }
        if (discounted != 0) {
            const std::array<int, 5> used = minimizeLoss
                ? minimumLossPayment(static_cast<int>(discounted), wallet, value)
                : originalPayment(static_cast<int>(discounted), wallet, value);
            std::cout << "The payment is processed as follows:\n";
            showVouchers(used, value);
            for (int i = 0; i < 5; ++i) wallet[i] -= used[i];
        }
        if (walletValue(wallet, value) == 0) {
            std::cout << "You do not remain any voucher\nBye~\n";
            return 0;
        }
        std::cout << "The remaining vouchers are as follows:\n";
        showVouchers(wallet, value);
        std::cout << '\n';
    }
    return 0;
}

#endif
