//
// Created by vladiboi on 12.09.25.
//

#ifndef UNTITLED_RENDERER_H
#define UNTITLED_RENDERER_H

#endif //UNTITLED_RENDERER_H

#include <string>
#include <vector>

#include "raylib.h"

using namespace std;

inline float cursorX, cursorY = 0;
inline float textSize;
inline Color currentColor;

inline void InitGraphics() {
    SetTraceLogLevel(LOG_NONE);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 600, "Filmstock");

    BeginDrawing();
    ClearBackground(RAYWHITE);

    DrawText("Empty Window!", 190, 200, 20, LIGHTGRAY);

    EndDrawing();
}

inline void Cleanup() {
    CloseWindow();
}

inline void setNewColor(double input) {
    // whole number double can hold color -> turn into long long
    const auto i = static_cast<long long>(input);

    // Last bite -> red, second last green, etc.
    const int r = static_cast<int>(i >> 0) & 0xFF;
    const int g = static_cast<int>(i >> 8) & 0xFF;
    const int b = static_cast<int>(i >> 16) & 0xFF;
    const int a = static_cast<int>(i >> 24) & 0xFF;

    currentColor = (Color){
        (unsigned char) r,
        (unsigned char) g,
        (unsigned char) b,
        (unsigned char) a};
}

inline void setCursorPosition(double x, double y) {
    cursorX = static_cast<float>(x);
    cursorY = static_cast<float>(y);
}

inline void setTextSize(float size) {
    textSize = size;
}

inline void drawTriangles(vector<double> points) {
    const int pointAmount = static_cast<int> (points.size()) / 2;

    const int triangleAmount = pointAmount / 3;

    int t = 0;
    for (int i = 0; i < triangleAmount; i++) {
        t = i*6;
        const Vector2 v1 = {static_cast<float>(points[t])   + cursorX, static_cast<float>(points[t+1]) + cursorY};
        const Vector2 v2 = {static_cast<float>(points[t+2]) + cursorX, static_cast<float>(points[t+3]) + cursorY};
        const Vector2 v3 = {static_cast<float>(points[t+4]) + cursorX, static_cast<float>(points[t+5]) + cursorY};

        // cout << "[" << "(" << v1.x << "," << v1.y << ")," << "(" << v2.x << "," << v2.y << ")," << "(" << v3.x << "," << v3.y << ")]" << endl;

        DrawTriangle(v1, v2, v3, currentColor);
    }
    // cout << cursorX << ", " << cursorY;
    // cout << endl;
}

inline void drawText(const std::string& message) {
    DrawText(message.c_str(),
             static_cast<int>(cursorX),
             static_cast<int>(cursorY),
             static_cast<int>(textSize),
             currentColor);
}


inline double isPressed(int code) {
    return IsKeyDown(code);
}

inline void endDrawing() {
    EndDrawing();
}

inline void beginDrawing() {
    BeginDrawing();
}
