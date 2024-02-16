#pragma once

#include<GL/glew.h>

//macros:
//this is an intresic to the compiler so its different for each compiler
// this is using msvc compiler (identified by the "__")

#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))

void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);