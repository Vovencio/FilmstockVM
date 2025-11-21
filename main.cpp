#include <sstream>
#include <iostream>
#include <cmath>
#include <iomanip>
#include <fstream>
#include <vector>
#include <cstring>
#include <chrono>
#include <random>
#include <thread>
#include <algorithm>

#define CHARACTER_COUNT 256
#define STEP 4

#include <stdio.h>

#include "renderer.h"

double* film;
int* instructions;
std::vector<char> printBuffer;

std::vector<std::vector<double>> matrix; // Escape the Matrix

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

std::string doubleVectorToString(const std::vector<double>& input) {
    std::string result;
    result.reserve(input.size());

    for (double d : input) {
        result += doubleToChar(d);  // your custom function
    }

    return result;  // safe: returned by value, no leaks
}

char* getStringFrom(int start, int end) {
    char* result = new char[end - start + 1]();
    for (int i = start; i < end; i++) {
        result[i - start] = doubleToChar(film[i]);
    }
    result[end - start] = '\0';


    return result;
}

void addToBuffer(char c) {
    printBuffer.push_back(c);
}

void addToBuffer(const std::string& s) {
    printBuffer.insert(printBuffer.end(), s.begin(), s.end());
}

void print(int start, int length, int trash) {
    char* toPrint = getStringFrom(start, start + length);
    addToBuffer(toPrint);
    delete[] toPrint;
}

void updateConsole(int trash, int trash2, int trash3){
    if (!printBuffer.empty()) {
        std::cout.write(printBuffer.data(), static_cast<long>(printBuffer.size()));
        std::cout.flush();
        printBuffer.clear();
    }
}

void addDoubleToBuffer(double value, int precision = 2) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(precision) << value;
    addToBuffer(oss.str());
}

void printNumbers(int start, int length, int trash) {
    for (int i = start; i < (start+length); i++) {
        addDoubleToBuffer(film[i]);
        if (i < (start+length) - 1) addToBuffer(" ");
    }
}

void set(int pos, double value) {
    film[pos] = value;
}

void oppSetNewColor(int pos, int trash, int trash2) {
    setNewColor(film[pos]);
}

void oppSetPosition(int xPos, int yPos, int trash) {
    setCursorPosition(film[xPos], film[yPos]);
}

void oppSetTextSize(int pos, int trash, int trash2) {
    setTextSize(film[pos]);
}

void oppDrawTriangles(int pos, int trash, int trash2) {
    int id = static_cast<int>(film[pos]);

    drawTriangles(matrix[id]);
}

void oppDrawText(int pos, int trash, int trash2) {
    int id = static_cast<int>(film[pos]);

    drawText(doubleVectorToString(matrix[id]));
}

void oppBeginDrawing(int trash, int trash2, int trash3) {
    beginDrawing();
}

void oppEndDrawing(int trash, int trash2, int trash3) {
    endDrawing();
}

void oppIsPressed(int c, int saveTo, int trash) {
    film[saveTo] = isPressed(static_cast<int>(film[c]));
}

void mousePressed(int type, int saveTo, int trash) {
    int t = static_cast<int> (film[type]);

    switch (t) {
        case 0 :
            film[saveTo] = IsMouseButtonDown(MOUSE_LEFT_BUTTON); break;
        case 1 :
            film[saveTo] = IsMouseButtonDown(MOUSE_RIGHT_BUTTON); break;
        case 2 :
            film[saveTo] = IsMouseButtonDown(MOUSE_MIDDLE_BUTTON); break;
        default:
            cerr << "Unknown mouse button: " << t;
    }
}

void getMouseX(int saveTo, int trash, int trash2) {
    film[saveTo] = GetMousePosition().x;
}

void getMouseY(int saveTo, int trash, int trash2) {
    film[saveTo] = GetMousePosition().y;
}

void getScroll(int saveTo, int trash, int trash2) {
    film[saveTo] = GetMouseWheelMove();
}

void getWidth(int saveTo, int trash, int trash2) {
    film[saveTo] = GetScreenWidth();
}

void getHeight(int saveTo, int trash, int trash2) {
    film[saveTo] = GetScreenHeight();
}

//region Mathematical operations

void copy(int pos, int saveTo, int trash) {
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

void squareRoot(int pos, int saveTo, int trash) {
    film[saveTo] = sqrt(film[pos]);
}

void sin(int pos, int saveTo, int trash) {
    film[saveTo] = sin(film[pos]);
}

void cos(int pos, int saveTo, int trash) {
    film[saveTo] = cos(film[pos]);
}

void tan(int pos, int saveTo, int trash) {
    film[saveTo] = tan(film[pos]);
}

void asin(int pos, int saveTo, int trash) {
    film[saveTo] = asin(film[pos]);
}

void acos(int pos, int saveTo, int trash) {
    film[saveTo] = acos(film[pos]);
}

void atan2(int yPos, int xPos, int saveTo) {
    film[saveTo] = atan2(film[yPos], film[xPos]);
}

void pow(int basePos, int exponentPos, int saveTo) {
    film[saveTo] = pow(film[basePos], film[exponentPos]);
}

void mod(int basePos, int exponentPos, int saveTo) {
    film[saveTo] = modulo(film[basePos], film[exponentPos]);
}

void equal(int pos1, int pos2, int saveTo) {
    film[saveTo] = film[pos1] == film[pos2];
}

void compare(int pos1, int pos2, int saveTo) {
    film[saveTo] = film[pos1] > film[pos2];
}

void notOp(int pos1, int saveTo, int trash) {
    film[saveTo] = (film[pos1] == 0) ? 1 : 0;
}

void andOp(int pos1, int pos2, int saveTo) {
    film[saveTo] = (film[pos1] != 0 && film[pos2] != 0.0) ? 1.0 : 0.0;
}

void orOp(int pos1, int pos2, int saveTo) {
    film[saveTo] = (film[pos1] != 0.0 || film[pos2] != 0.0) ? 1.0 : 0.0;
}

void jumpIf(int pos1, int jumpTo, int trash) {
    currentInstruction = (film[pos1] != 0) ? static_cast<int>(film[jumpTo])*4 - STEP :
    currentInstruction;
}

void iterate(int pos1, int trash, int trash2) {
    film[pos1] += 1;
}

void copyFrom(int pos1, int copyTo, int trash) {
    film[pos1] = film[static_cast<int>(film[copyTo])];
}

void pointer(int pos1, int copyTo, int trash) {
    film[copyTo] = pos1;
}

void position(int pos1, int trash, int trash2) {
    film[pos1] = currentInstruction;
}

void jump(int jumpTo, int trash, int trash2) {
    currentInstruction = static_cast<int>(film[jumpTo])*4 - STEP;
}

void end(int getCodeFrom, int trash, int trash2) {
    exitCode = static_cast<int>(film[getCodeFrom]);
    currentInstruction = INT_MAX;
}

void floor(int pos1, int saveTo, int trash) {
    film[saveTo] = std::floor(film[pos1]);
}

void round(int pos1, int saveTo, int trash) {
    film[saveTo] = std::round(film[pos1]);
}

void ceil(int pos1, int saveTo, int trash) {
    film[saveTo] = std::ceil(film[pos1]);
}

void time(int pos1, int trash, int trash2) {
    auto now = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

    film[pos1] = ms;
}

void newList(int saveTo, int trash, int trash2) {
    matrix.push_back(std::vector<double>());
    film[saveTo] = static_cast<double>(matrix.size() - 1);
}

void copyFromList(int listId, int pos1, int copyTo) {
    film[copyTo] = matrix[static_cast<int>(film[listId])][static_cast<int>(film[pos1])];
}

void lengthOfList(int listId, int copyTo, int trash) {
    film[copyTo] = static_cast<double>(matrix[static_cast<int>(film[listId])].size());
}

void listAmount(int copyTo, int trash, int trash2) {
    film[copyTo] = static_cast<double>(matrix.size());
}

void addList(int listId, int pos1, int trash) {
    matrix[static_cast<int>(film[listId])].push_back(film[pos1]);
}

void removeAt(int listId, int pos1, int trash) {
    int lId = static_cast<int>(film[listId]);

    matrix[lId].erase(matrix[lId].begin() + static_cast<int>(film[pos1]));
}

void emptyList(int listId, int trash, int trash2) {
    matrix[static_cast<int>(film[listId])].clear();
}

void removeAll(int trash, int trash2, int trash3) {
    matrix.clear();
}

void addAt(int listId, int pos2, int pos1) {
    int lId = static_cast<int>(film[listId]);
    int index = static_cast<int>(film[pos1]);
    int value = static_cast<int>(film[pos2]);

    if (lId < 0 || lId >= matrix.size()) return;
    if (index < 0) index = 0;
    if (index > matrix[lId].size()) index = matrix[lId].size();

    matrix[lId].insert(matrix[lId].begin() + index, value);
}


void reverseList(int listId, int trash, int trash2) {
    std::reverse(matrix[static_cast<int>(film[listId])].begin(), matrix[static_cast<int>(film[listId])].end());
}

void shuffleList(int listId, int trash, int trash2) {
    int lId = static_cast<int>(film[listId]);

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(matrix[lId].begin(), matrix[lId].end(), g);
}


void opp_sort(int listId, int type, int trash) {
    int lId = static_cast<int>(film[listId]);

    if (film[type] == 0) {
        std::sort(matrix[lId].begin(), matrix[lId].end()); // Ascending if 0
    }
    else {
        std::sort(matrix[lId].begin(), matrix[lId].end(), std::greater<double>()); // Descending if not 0
    }
}


void sleep(int msPos, int trash, int trash2) {
    std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(film[msPos])));
}


void random(int saveTo, int trash, int trash2) {
    std::random_device rd;               // hardware/random entropy source
    std::uniform_real_distribution<> dis(0.0, 1.0);
    film[saveTo] = dis(rd);             // generate a value in [0,1)
}

void listSet(int listId, int index, int setTo) {
    matrix[static_cast<int>(film[listId])][static_cast<int>(film[index])] = film[setTo];
}

void printVector(int p1, int trash, int trash2) {
    for (double d : matrix[static_cast<int>(film[p1])]) {
        addToBuffer(doubleToChar(d));
    }
}

void printVectorNumbers(int p1, int trash, int trash2) {
    addToBuffer("[");
    std::vector<double> vec = matrix[static_cast<int>(film[p1])];
    for (size_t i = 0; i < vec.size(); ++i) {
        addDoubleToBuffer(vec[i], 2);
        if (i + 1 != vec.size()) {
            addToBuffer(", ");
        }
    }

    addToBuffer("]\n");
}

void printVectorSeparated(int p1, int p2, int p3) {
    const std::vector<double>& vec = matrix[static_cast<int>(film[p1])];
    char separator = doubleToChar(static_cast<int>(film[p2]));
    int separateEach = static_cast<int>(film[p3]);

    for (size_t i = 0; i < vec.size(); ++i) {
        addToBuffer(doubleToChar(vec[i]));
        if ((i + 1) % separateEach == 0 && i + 1 != vec.size()) {
            addToBuffer(separator);
        }
    }
}

//endregion

typedef void (*Operation)(int, int, int) ;

Operation get_operation_from_id(const int id) {
    switch (id) {
        case 0:  return copy;
        case 1:  return add;
        case 2:  return subtract;
        case 3:  return multiply;
        case 4:  return divide;
        case 5:  return squareRoot;
        case 6:  return sin;
        case 7:  return cos;
        case 8:  return tan;
        case 9:  return asin;
        case 10: return acos;
        case 11: return atan2;
        case 12: return pow;
        case 13: return mod;
        case 14: return equal;
        case 15: return compare;
        case 16: return notOp;
        case 17: return andOp;
        case 18: return orOp;
        case 19: return jumpIf;
        case 20: return print;
        case 21: return printNumbers;
        case 22: return iterate;
        case 23: return copyFrom;
        case 24: return pointer;
        case 25: return position;
        case 26: return jump;
        case 27: return end;
        case 28: return floor;
        case 29: return round;
        case 30: return ceil;
        case 31: return time;
        case 32: return newList;
        case 33: return copyFromList;
        case 34: return lengthOfList;
        case 35: return listAmount;
        case 36: return addList;
        case 37: return removeAt;
        case 38: return emptyList;
        case 39: return removeAll;
        case 40: return addAt;
        case 41: return reverseList;
        case 42: return shuffleList;
        case 43: return opp_sort;
        case 44: return sleep;
        case 45: return random;
            // TODO: USE CODE 46
        case 46: return copy;
            // TODO: USE CODE 46
        case 47: return listSet;
        case 48: return updateConsole;
        case 49: return printVector;
        case 50: return printVectorNumbers;
        case 51: return printVectorSeparated;
        case 52: return oppSetNewColor;
        case 53: return oppSetPosition;
        case 54: return oppSetTextSize;
        case 55: return oppDrawTriangles;
        case 56: return oppDrawText;
        case 57: return oppBeginDrawing;
        case 58: return oppEndDrawing;
        case 59: return oppIsPressed;
        case 60: return mousePressed;
        case 61: return getMouseX;
        case 62: return getMouseY;
        case 63: return getScroll;
        case 64: return getWidth;
        case 65: return getHeight;

        default:
            printf("Unknown instruction type: %d\n", id);
            return nullptr;
    }
}

const int opp_amount = 66;

Operation opp_arr[opp_amount];

void setup_op_arr() {
    for (int i = 0; i < opp_amount; ++i) {
        opp_arr[i] = get_operation_from_id(i);
    }
}

void execute() {
    int type = instructions[currentInstruction];
    int p1 = instructions[currentInstruction + 1];
    int p2 = instructions[currentInstruction + 2];
    int p3 = instructions[currentInstruction + 3];

    opp_arr[type](p1, p2, p3);

    currentInstruction += STEP;
}

int getIntFrom(unsigned char* buff, int start) {
    int value = *(int*)&buff[start];

    return value;
}

double getDoubleFrom(unsigned char* buff, int start) {
    double value = *(double*)&buff[start];

    return value;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: runner PATH_TO_LOAD_FROM\n";
        return 1;
    }

    setup_op_arr();

    const char* path = argv[1];

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

    for (int i = 4; i < startMargin; i += 12) {
        int id = getIntFrom(buffer, i);
        double value = getDoubleFrom(buffer, i + 4);

        set(id, value);
    }

    const int instructionAmount = bufferSize - startMargin - 64;
    if (instructionAmount % 16 != 0) {
        fprintf(stderr, "Error reading file!\n");
        return -1;
    }

    instructions = new int[instructionAmount / 4] {};

    int c = 0;
    for (int i = startMargin + 64; i < bufferSize; i += 16) {
        for (int b = 0; b < 4; b++) {
            int id = getIntFrom(buffer, i + b * 4);
            instructions[c] = getIntFrom(buffer, i + b * 4);
            c++;
        }
    }

    instructionCount = instructionAmount / 4;

    bool graphicalMode = false;

    for (int i = 0; i < instructionCount; i++) {
        int opp = instructions[i*4];
        if (opp > 51 && opp < 66) {
            graphicalMode = true;
            break;
        }
    }

    if (graphicalMode) {
        InitGraphics();
    }

    // Start time
    auto start = std::chrono::high_resolution_clock::now();

    const int instructionEnd = instructionCount;
    if (!graphicalMode) {
        while (currentInstruction < instructionEnd) {
            execute();
        }
    }
    else {
        while (!WindowShouldClose() && currentInstruction < instructionEnd) {
            execute();
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    if (graphicalMode) {
        Cleanup();
    }

    std::cout << std::endl << "[Program finished in " << duration.count() << "ms with exit code: " << exitCode << "]" << std::endl;

    return 0;
}