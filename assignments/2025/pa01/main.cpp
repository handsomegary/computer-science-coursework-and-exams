#include <iostream>

// All three modes use the same transition function and collision detector.
const int LIMIT = 10000;
static int maps[LIMIT + 2][3][9];
static int raw[LIMIT + 2][3];
static signed char parent_state[LIMIT + 2][56];
static signed char parent_command[LIMIT + 2][56];
static int commands[LIMIT + 2];
bool echo_input = true;

bool equal(const char* a, const char* b) {
    while (*a && *a == *b) { ++a; ++b; }
    return *a == *b;
}
void number(int n, int width) {
    if (n < 0) { std::cout << n; return; }
    int divisor = 1;
    for (int i = 1; i < width; ++i) divisor *= 10;
    while (divisor) { std::cout << n / divisor; n %= divisor; divisor /= 10; }
}
void decode(int value, int output[9], int size) {
    for (int x = size - 1; x >= 0; --x) { output[x] = value % 10; value /= 10; }
}
int encode(int y, int mode, bool crouch) { return y * 8 + mode * 2 + crouch; }

// mode 1: first ascent/apex; mode 2: ascent/apex on the upper floor;
// mode 0: standing or descending. The map has not shifted yet.
int transition(int state, int command, const int ceiling[9], bool layers) {
    int y = state / 8, mode = (state % 8) / 2;
    if (command == 1) {
        if (y == 0) return encode(1, 1, false);
        if (layers && y == 2 && mode == 1 && ceiling[1] == 0)
            return encode(3, 2, false);
        if (layers && y == 4 && mode == 0 && ceiling[0] && ceiling[1])
            return encode(5, 2, false);
        return -1;
    }
    if (command == 2) return encode(layers && y >= 4 && ceiling[1] ? 4 : 0, 0, true);
    if (mode == 1 && y == 1) return encode(2, 1, false);
    if (mode == 2 && y < 6) return encode(y + 1, 2, false);
    if (y == 0 || (layers && y == 4 && ceiling[1])) return encode(y, 0, false);
    return encode(y - 1, 0, false);
}
char obstacle(int code, int y) {
    if ((code == 1 && y == 0) || (code == 2 && y <= 1)) return 'C';
    if ((code == 3 && y == 1) || (code == 4 && y == 2)) return 'P';
    return ' ';
}
bool collision(int state, const int map[3][9], bool layers) {
    int y = state / 8, height = state % 2 ? 1 : 2;
    for (int k = 0; k < height; ++k) {
        int row = y + k, floor = row / 4;
        if (floor < (layers ? 2 : 1) && obstacle(map[floor][0], row % 4) != ' ') return true;
    }
    return false;
}
void draw(int state, const int map[3][9], bool layers) {
    int y = state / 8, height = state % 2 ? 1 : 2;
    for (int row = layers ? 7 : 3; row >= 0; --row) {
        for (int x = 0; x < 9; ++x) {
            char c = x == 0 && row >= y && row < y + height ? 'D' : obstacle(map[row / 4][x], row % 4);
            std::cout << "| " << c;
        }
        std::cout << "|\n";
        if (layers && row == 4) {
            for (int x = 0; x < 9; ++x) std::cout << (map[2][x] ? "===" : "=  ");
            std::cout << "=\n";
        }
    }
}
void farewell(const char* message) {
    std::cout << message << "\n======Thanks for playing Dinosaur Game, see you next time======\n";
}
const char* command_prompt = "Please enter a command to control the dinosaur, 0 for no action, 1=jump, 2=crouch: ";
void setup_prompt(bool initial, int layer, bool layers) {
    if (!layers) std::cout << (initial ? "Please enter the initial obstacle setup of the Dinosaur Game. Enter -1 to quit: " : "Please enter the next round's obstacle setup of the Dinosaur Game. Enter -1 to quit: ");
    else if (layer == 0) std::cout << (initial ? "Please enter the first layer initial obstacle setup of the Dinosaur Game. Enter -1 to quit: " : "Please enter the next round's first layer obstacle setup of the Dinosaur Game. Enter -1 to quit: ");
    else if (layer == 1) std::cout << (initial ? "Please enter the second layer initial obstacle setup of the Dinosaur Game: " : "Please enter the next round's second layer obstacle setup of the Dinosaur Game: ");
    else std::cout << (initial ? "Please enter the initial distribution of gap in the ceiling, 0 for gap, 1 for complete ceiling: " : "Please enter the next round's ceiling or gap status: ");
}
void echo(int value, bool initial, int layer) {
    if (echo_input) { number(value, initial ? (layer == 2 ? 9 : 8) : 1); std::cout << '\n'; }
}
bool read_map(int tick, bool initial, bool layers, bool autoplay) {
    for (int layer = 0; layer < (layers ? 3 : 1); ++layer) {
        if (!autoplay) setup_prompt(initial, layer, layers);
        if (!(std::cin >> raw[tick][layer])) return false;
        if (!autoplay) echo(raw[tick][layer], initial, layer);
        if (layer == 0 && raw[tick][0] == -1) return true;
        if (initial) {
            if (layer == 2) decode(raw[tick][layer], maps[tick][layer], 9);
            else decode(raw[tick][layer], maps[tick][layer] + 1, 8);
        } else {
            for (int x = 0; x < 8; ++x) maps[tick][layer][x] = maps[tick - 1][layer][x + 1];
            maps[tick][layer][8] = raw[tick][layer];
        }
    }
    return true;
}
int main(int argc, char** argv) {
    bool layers = false, autoplay = false;
    for (int i = 1; i < argc; ++i) {
        if (equal(argv[i], "--double")) layers = true;
        else if (equal(argv[i], "--auto")) layers = autoplay = true;
        else if (equal(argv[i], "--no-echo")) echo_input = false;
        else { std::cerr << "Unknown option\n"; return 1; }
    }
    std::cout << "======Welcome to the Dinosaur Game======\n";
    if (!read_map(0, true, layers, autoplay)) return 1;
    if (autoplay) { setup_prompt(true, 0, true); echo(raw[0][0], true, 0); }
    if (raw[0][0] == -1) { farewell("The game ended when it just started."); return 0; }
    if (!autoplay) {
        int state = 0;
        draw(state, maps[0], layers);
        for (int tick = 1; tick <= LIMIT; ++tick) {
            std::cout << command_prompt;
            int next = -1;
            while (next < 0) {
                double command;
                if (!(std::cin >> command)) return 1;
                if (echo_input) std::cout << command << '\n';
                if (command != 0 && command != 1 && command != 2)
                    std::cout << "The dinosaur doesn't understand this command, try another one: ";
                else {
                    next = transition(state, static_cast<int>(command), maps[tick - 1][2], layers);
                    if (next < 0) std::cout << "The dinosaur cannot jump again, please enter another command: ";
                }
            }
            if (!read_map(tick, false, layers, false)) return 1;
            if (raw[tick][0] == -1) { farewell("Congratulations, you WIN the game!!!"); return 0; }
            state = next;
            if (collision(state, maps[tick], layers)) { farewell("What a pity, it was close :("); return 0; }
            draw(state, maps[tick], layers);
        }
        std::cerr << "Too many ticks\n"; return 1;
    }
    int count = 0;
    while (count <= LIMIT && read_map(count + 1, false, true, true)) {
        if (raw[count + 1][0] == -1) break;
        if (count == LIMIT) { std::cerr << "Too many ticks\n"; return 1; }
        ++count;
    }
    if (raw[count + 1][0] != -1) { std::cerr << "Missing terminator or too many ticks\n"; return 1; }
    for (int t = 0; t <= count; ++t) for (int s = 0; s < 56; ++s) parent_state[t][s] = -1;
    parent_state[0][0] = 0;
    for (int t = 1; t <= count; ++t)
        for (int s = 0; s < 56; ++s) if (parent_state[t - 1][s] >= 0)
            for (int c = 0; c < 3; ++c) {
                int next = transition(s, c, maps[t - 1][2], true);
                if (next >= 0 && !collision(next, maps[t], true) && parent_state[t][next] < 0) {
                    parent_state[t][next] = static_cast<signed char>(s);
                    parent_command[t][next] = static_cast<signed char>(c);
                }
            }
    int state = -1;
    for (int s = 0; s < 56; ++s) if (parent_state[count][s] >= 0) { state = s; break; }
    if (state < 0) { std::cerr << "No winning path exists\n"; return 2; }
    for (int t = count; t > 0; --t) { commands[t] = parent_command[t][state]; state = parent_state[t][state]; }
    for (int layer = 1; layer < 3; ++layer) { setup_prompt(true, layer, true); echo(raw[0][layer], true, layer); }
    draw(0, maps[0], true);
    for (int t = 1; t <= count + 1; ++t) {
        int c = t <= count ? commands[t] : 0;
        std::cout << command_prompt << c << '\n';
        for (int layer = 0; layer < (t <= count ? 3 : 1); ++layer) { setup_prompt(false, layer, true); echo(raw[t][layer], false, layer); }
        if (t <= count) { state = transition(state, c, maps[t - 1][2], true); draw(state, maps[t], true); }
    }
    farewell("Congratulations, you WIN the game!!!");
}
