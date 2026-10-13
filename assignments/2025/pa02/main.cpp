#include <iostream>
#include <iomanip>
#include <string>
#include <algorithm>
#include <unordered_set>
#include <cstdlib>

// Fixed arrays keep the search state explicit; no user-defined class or struct.
int n, limit_steps, initial_energy, bonus_value, energy_value, penalty;
char board[400];
int neighbors[400][8], degree_count[400], blocked_from[400];
bool visited[400];
int path[101], best_path[101], best_score = 0, best_steps = 0;
bool bonus_mode = false, summary_only = false;
std::unordered_set<std::string> seen;
const int dr[8] = {-2, -2, -1, -1, 1, 1, 2, 2};
const int dc[8] = {-1, 1, -2, 2, -2, 2, -1, 1};

bool safe(int p, int step) { return board[p] != '#' && step < blocked_from[p]; }
int score_gain(int p) { return 1 + (board[p] == 'B' ? bonus_value : 0) - (board[p] == 'X' ? penalty : 0); }
int min2(int a, int b) { return a < b ? a : b; }

// The connected component and optimistic item selection form an admissible bound.
int upper_bound(int current, int step, int energy, int score) {
    bool marked[400] = {};
    int distance[400] = {};
    int queue[400], front = 0, back = 0;
    queue[back++] = current; marked[current] = true;
    int counts[101][2] = {}, bonuses[101][2] = {}, energies[101] = {}, bombs[101][2] = {};
    int current_color = (current / n + current % n) % 2;
    while (front < back) {
        int p = queue[front++];
        for (int k = 0; k < degree_count[p]; ++k) {
            int q = neighbors[p][k];
            int d = distance[p] + 1;
            if (marked[q] || visited[q] || d > limit_steps - step || !safe(q, step + d)) continue;
            marked[q] = true; queue[back++] = q;
            distance[q] = d;
            int color = ((q / n + q % n) % 2) != current_color;
            ++counts[d][color];
            bonuses[d][color] += board[q] == 'B'; energies[d] += board[q] == 'E'; bombs[d][color] += board[q] == 'X';
        }
    }
    int maximum = score;
    for (int m = 1; m <= limit_steps - step; ++m) {
        for (int color = 0; color < 2; ++color) {
            counts[m][color] += counts[m-1][color];
            bonuses[m][color] += bonuses[m-1][color];
            bombs[m][color] += bombs[m-1][color];
        }
        energies[m] += energies[m-1];
        int need[2] = {m / 2, (m + 1) / 2};
        if (need[0] > counts[m][0] || need[1] > counts[m][1] || m > energy + energies[m] * energy_value) continue;
        int collect = 0, forced_bombs = 0;
        for (int color = 0; color < 2; ++color) {
            collect += min2(need[color], bonuses[m][color]);
            int clean = counts[m][color] - bombs[m][color];
            if (need[color] > clean) forced_bombs += need[color] - clean;
        }
        int bound = score + m + bonus_value * collect - penalty * forced_bombs;
        if (bound > maximum) maximum = bound;
    }
    return maximum;
}
void search(int current, int step, int energy, int score) {
    if (score > best_score) {
        best_score = score; best_steps = step;
        for (int i = 0; i <= step; ++i) best_path[i] = path[i];
    }
    if (step == limit_steps || energy == 0 || upper_bound(current, step, energy, score) <= best_score) return;
    if (n <= 8) {
        // Visited set determines steps, score and energy, so these need no key fields.
        std::string key(10, '\0');
        for (int p = 0; p < n * n; ++p) if (visited[p]) key[p / 8] = static_cast<char>(static_cast<unsigned char>(key[p / 8]) | (1u << (p % 8)));
        key[8] = static_cast<char>(current); key[9] = static_cast<char>(current >> 8);
        if (!seen.insert(key).second) return;
        // Evicting memo entries affects runtime only, never correctness.
        if (seen.size() > 1000000) seen.clear();
    }
    int candidates[8], priorities[8], count = 0;
    for (int k = 0; k < degree_count[current]; ++k) {
        int q = neighbors[current][k];
        if (visited[q] || !safe(q, step + 1)) continue;
        int onward = 0;
        for (int j = 0; j < degree_count[q]; ++j) {
            int p = neighbors[q][j];
            if (!visited[p] && safe(p, step + 2)) ++onward;
        }
        int priority = score_gain(q) * 32 + (board[q] == 'E' ? energy_value * 16 : 0) - onward;
        int index = count++;
        while (index > 0 && priorities[index - 1] < priority) {
            candidates[index] = candidates[index - 1]; priorities[index] = priorities[index - 1]; --index;
        }
        candidates[index] = q; priorities[index] = priority;
    }
    for (int k = 0; k < count; ++k) {
        int q = candidates[k];
        visited[q] = true; path[step + 1] = q;
        search(q, step + 1, energy - 1 + (board[q] == 'E' ? energy_value : 0), score + score_gain(q));
        visited[q] = false;
    }
}
void notice(int step) {
    if (!bonus_mode) return;
    if (step == 0) std::cout << "<!> OH NO <!> ANDY HAS AWAKENED!!! The FINAL COUNTDOWN begins...\n";
    if (step == 5) std::cout << "<!> WARNING <!> ANDY IS ENRAGED!!! Run if you can...\n";
    if (step == 10) std::cout << "<!> DANGER <!> ANDY GROWS STRONGER!!! You feel the end is near...\n";
    if (step == 15) std::cout << "<!> CRITICAL <!> ANDY UNLEASHES CHAOS!!! Farewell, hero. This will be the end of your journey...\n";
}
int main(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string option = argv[i];
        if (option == "--bonus") bonus_mode = true;
        else if (option == "--summary") summary_only = true;
        else { std::cerr << "Unknown option\n"; return 1; }
    }
    if (!summary_only) std::cout << "Welcome to Knight's Treasure Hunt!!\nEnter board size N, step limit, and starting energy (e.g., 7 25 5): ";
    if (!(std::cin >> n >> limit_steps >> initial_energy) || n < 1 || n > 20 || limit_steps < 1 || limit_steps > 100 || initial_energy < 1 || initial_energy > 100) { std::cerr << "Invalid input\n"; return 1; }
    if (!summary_only) std::cout << "Enter values for Bonus (B), Energy (E), and Bomb penalty (X) (e.g., 5 4 10): ";
    if (!(std::cin >> bonus_value >> energy_value >> penalty) || bonus_value < 1 || bonus_value > 10 || energy_value < 1 || energy_value > 10 || penalty < 1 || penalty > 10) { std::cerr << "Invalid input\n"; return 1; }
    if (!summary_only) std::cout << "Enter the board with " << n << " lines, each with " << n << " tokens separated by spaces:\n";
    int start = -1;
    for (int p = 0; p < n * n; ++p) {
        if (!(std::cin >> board[p]) || (std::string("SBEX#O").find(board[p]) == std::string::npos && !(bonus_mode && board[p] == 'A'))) { std::cerr << "Invalid board\n"; return 1; }
        if (board[p] == 'S') { if (start != -1) { std::cerr << "Multiple starts\n"; return 1; } start = p; }
        blocked_from[p] = 101;
    }
    if (start < 0) { std::cerr << "Missing start\n"; return 1; }
    for (int p = 0; p < n * n; ++p) {
        int r = p / n, c = p % n;
        for (int k = 0; k < 8; ++k) {
            int rr = r + dr[k], cc = c + dc[k];
            if (rr >= 0 && rr < n && cc >= 0 && cc < n) neighbors[p][degree_count[p]++] = rr * n + cc;
        }
        if (bonus_mode) for (int q = 0; q < n * n; ++q) if (board[q] == 'A') {
            int distance = std::max(std::abs(r - q / n), std::abs(c - q % n));
            if (distance <= 3) blocked_from[p] = min2(blocked_from[p], distance * 5);
        }
    }
    visited[start] = true; path[0] = best_path[0] = start;
    search(start, 0, initial_energy, 0);
    if (summary_only) { std::cout << best_score << ' ' << best_steps << '\n'; return 0; }
    std::cout << "\n----The Result Of Knight's Treasure Hunt----\nBest score: " << best_score << "\nSteps: " << best_steps << "\n\n----The Replay Of Knight's Treasure Hunt----\n";
    std::cout << std::left << std::setw(6) << "Step" << std::setw(10) << "(x,y)" << std::setw(12) << "Type" << std::setw(10) << "Score" << "Energy\n------------------------------------------------\n";
    int score = 0, energy = initial_energy, index[400];
    std::fill(index, index + n * n, -1);
    for (int s = 0; s <= best_steps; ++s) {
        int p = best_path[s]; index[p] = s;
        std::string type = "start";
        if (s) {
            score += score_gain(p); energy -= 1;
            if (board[p] == 'E') { energy += energy_value; type = "energy"; }
            else if (board[p] == 'B') type = "bonus";
            else if (board[p] == 'X') type = "bomb";
            else type = "normal";
        }
        std::string coordinates = "(" + std::to_string(p % n) + "," + std::to_string(n - 1 - p / n) + ")";
        std::cout << std::setw(6) << s << std::setw(10) << coordinates << std::setw(12) << type << std::setw(10) << score << energy << '\n';
        notice(s);
    }
    int width = 2 + static_cast<int>(std::to_string(best_steps).size());
    std::cout << "\n----The Best Path Grid Of Knight's Treasure Hunt----\n";
    for (int r = 0; r < n; ++r) {
        for (int c = 0; c < n; ++c) {
            int p = r * n + c;
            std::string token = index[p] == 0 ? "S" : (index[p] > 0 ? std::to_string(index[p]) : (best_steps >= blocked_from[p] ? "A" : std::string(1, board[p])));
            std::cout << std::setw(width) << token;
        }
        std::cout << '\n';
    }
}
