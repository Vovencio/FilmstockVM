#include <iostream>
#include <cmath>
#include <iomanip>
#include <fstream>
#include <vector>
#include <cstring>
#include <chrono>

#define CHARACTER_COUNT 256

double* film;
int* instructions;
bool finished = false;

int currentInstruction = 0;
int instructionCount = 0;
int exitCode = 0;

double modulo(double num, double base) {
    return std::fmod(num, base);
}

char doubleToChar(double number) {
    int index = static_cast<int>(modulo(number, CHARACTER_COUNT));
    return static_cast<char>(index);
}

char* getStringFrom(int start, int end) {
    char* result = new char[end - start + 1]();
    for (int i = start; i < end; i++) {
        result[i - start] = doubleToChar(film[i]);
    }
    result[end - start] = '\0';


    return result;
}

void print(int start, int end) {
    char* toPrint = getStringFrom(start, end);
    std::cout << toPrint;
    delete[] toPrint;
}

void printNumbers(int start, int end) {
    for (int i = start; i < end; i++) {
        std::cout << std::fixed << std::setprecision(2) << film[i];
        if (i < end - 1) std::cout << " ";
    }
}

void set(int pos, double value) {
    film[pos] = value;
}

//region Mathematical operations

void copy(int pos, int saveTo) {
    film[saveTo] = film[pos];
}

void add(int pos1, int pos2, int saveTo) {
    film[saveTo] = film[pos1] + film[pos2];
}

void subtract(int pos1, int pos2, int saveTo) {
    film[saveTo] = film[pos1] - film[pos2];
}

void multiply(int pos1, int pos2, int saveTo) {
    film[saveTo] = film[pos1] * film[pos2];
}

void divide(int pos1, int pos2, int saveTo) {
    film[saveTo] = film[pos1] / film[pos2];
}

void squareRoot(int pos, int saveTo) {
    film[saveTo] = sqrt(film[pos]);
}

void sin(int pos, int saveTo) {
    film[saveTo] = sin(film[pos]);
}

void cos(int pos, int saveTo) {
    film[saveTo] = cos(film[pos]);
}

void tan (int pos, int saveTo) {
    film[saveTo] = tan(film[pos]);
}

void asin (int pos, int saveTo) {
    film[saveTo] = asin(film[pos]);
}

void acos (int pos, int saveTo) {
    film[saveTo] = acos(film[pos]);
}

void atan2 (int yPos, int xPos, int saveTo) {
    film[saveTo] = atan2(film[yPos], film[xPos]);
}

void pow (int basePos, int exponentPos, int saveTo) {
    film[saveTo] = pow(film[basePos], film[exponentPos]);
}

void mod (int basePos, int exponentPos, int saveTo) {
    film[saveTo] = modulo(film[basePos], film[exponentPos]);
}

void equal (int pos1, int pos2, int saveTo) {
    film[saveTo] = film[pos1] == film[pos2];
}

void compare (int pos1, int pos2, int saveTo) {
    film[saveTo] = film[pos1] > film[pos2];
}

void notOp (int pos1, int saveTo) {
    film[saveTo] = (film[pos1] == 0) ? 1 : 0;
}

void andOp (int pos1, int pos2, int saveTo) {
    film[saveTo] = (film[pos1] == 1.0 && film[pos2] == 1.0) ? 1.0 : 0.0;
}

void orOp (int pos1, int pos2, int saveTo) {
    film[saveTo] = (film[pos1] == 1.0 || film[pos2] == 1.0) ? 1.0 : 0.0;
}

void jumpIf (int pos1, int jumpTo) {
    currentInstruction = (film[pos1] != 0) ? static_cast<int>(film[jumpTo]) : currentInstruction + 1;
}

void iterate (int pos1) {
    film[pos1] += 1;
}

void copyFrom (int pos1, int copyTo) {
    copy(pos1, static_cast<int>(film[copyTo]));
}

void pointer (int pos1, int copyTo) {
    film[copyTo] = pos1;
}

void position (int pos1) {
    film[pos1] = currentInstruction;
}

void jump (int jumpTo) {
    currentInstruction = static_cast<int>(film[jumpTo]);
}

void end (int getCodeFrom) {
    exitCode = static_cast<int>(film[getCodeFrom]);
    finished = true;
}

void floor (int pos1, int saveTo) {
    film[saveTo] = std::floor(film[pos1]);
}

void round (int pos1, int saveTo) {
    film[saveTo] = std::round(film[pos1]);
}

void ceil (int pos1, int saveTo) {
    film[saveTo] = std::ceil(film[pos1]);
}

void time (int pos1) {
    auto now = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

    film[pos1] = ms;
}

//endregion

void execute() {
    int type = instructions[currentInstruction * 4];
    int p1   = instructions[currentInstruction * 4 + 1];
    int p2   = instructions[currentInstruction * 4 + 2];
    int p3   = instructions[currentInstruction * 4 + 3];

    switch(type) {
        case 0: copy(p1, p2); break;
        case 1: add(p1, p2, p3); break;
        case 2: subtract(p1, p2, p3); break;
        case 3: multiply(p1, p2, p3); break;
        case 4: divide(p1, p2, p3); break;
        case 5: squareRoot(p1, p2); break;
        case 6: sin(p1, p2); break;
        case 7: cos(p1, p2); break;
        case 8: tan(p1, p2); break;
        case 9: asin(p1, p2); break;
        case 10: acos(p1, p2); break;
        case 11: atan2(p1, p2, p3); break;
        case 12: pow(p1, p2, p3); break;
        case 13: mod(p1, p2, p3); break;
        case 14: equal(p1, p2, p3); break;
        case 15: compare(p1, p2, p3); break;
        case 16: notOp(p1, p2); break;
        case 17: andOp(p1, p2, p3); break;
        case 18: orOp(p1, p2, p3); break;
        case 19: jumpIf(p1, p2); break;
        case 20 : print(p1, p2+p1); break;
        case 21 : printNumbers(p1, p2+p1); break;
        case 22 : iterate(p1); break;
        case 23 : copyFrom(p1, p2); break;
        case 24 : pointer(p1, p2); break;
        case 25 : position(p1); break;
        case 26 : jump(p1); break;
        case 27 : end(p1); break;
        case 28 : floor(p1, p2); break;
        case 29 : round(p1, p2); break;
        case 30 : ceil(p1, p2); break;
        case 31 : time(p1); break;
        default:
            printf("Unknown instruction type: %d\n", type);
            break;
    }

    if (type != 19)
        currentInstruction++; // move to the next instruction
}

int getIntFrom(unsigned char* buff, int start) {
    int value = *(int*)&buff[start];

    return value;
}

double getDoubleFrom(unsigned char* buff, int start) {
    double value = *(double*)&buff[start];

    return value;
}

int main() {
    const char* path = "/home/vladiboi/Documents/GitHub/FilmStock/FilmStockCompilerAssembler/test.roll";

    // Load into vector first
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file) {
        std::cerr << "Failed to open file\n";
        return 1;
    }

    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<unsigned char> vec(fileSize);
    if (!file.read(reinterpret_cast<char*>(vec.data()), fileSize)) {
        std::cerr << "Failed to read file\n";
        return 1;
    }
    file.close();

    // Now copy vector data into a raw array
    unsigned char* buffer = new unsigned char[vec.size()];
    std::memcpy(buffer, vec.data(), vec.size());

    unsigned int bufferSize = vec.size();

    // Find the margin
    int mChars = 0;
    int startMargin = 0;

    for (int i = 4; i < bufferSize; i++) {
        if (buffer[i] == 255) {
            mChars++;
        }
        else {
            mChars = 0;
        }

        if (mChars == 64) {
            startMargin = i - 63;
            break;
        }
    }

    if (startMargin == 0) {
        fprintf(stderr, "Error reading file! No margin found!\n");
        return -1;
    }

    int length = getIntFrom(buffer, 0);
    film = new double[length]();

    for (int i = 4; i < startMargin; i+=12) {
        int id = getIntFrom(buffer, i);
        double value = getDoubleFrom(buffer, i+4);

        set(id, value);
    }

    const int instructionAmount = bufferSize - startMargin - 64;
    if (instructionAmount % 16 != 0) {
        fprintf(stderr, "Error reading file!\n");
        return -1;
    }

    instructions = new int[instructionAmount / 4] {};

    int c = 0;
    for (int i = startMargin + 64; i < bufferSize; i+=16) {
        for (int b = 0; b < 4; b++) {
            int id = getIntFrom(buffer, i+b*4);
            instructions[c] = getIntFrom(buffer, i+b*4);
            c++;
        }
    }

    instructionCount = instructionAmount / 4;

    // Start time
    auto start = std::chrono::high_resolution_clock::now();

    while (true) {
        if (currentInstruction * 4 >= instructionCount || finished){
            break;
        }
        execute();
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << std::endl << "[Program finished in " << duration.count() << "ms with exit code: " << exitCode << "]" << std::endl;

    return 0;
}