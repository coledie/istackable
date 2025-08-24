#pragma once

// Debug functionality controlled by DEBUG_MODE preprocessor flag
// Usage: build.bat debug   (enables debug mode)
//        build.bat        (normal mode, no debug output)

#ifdef DEBUG_MODE
    #include <cstdio>
    #define DEBUG_PRINT(fmt, ...) printf(fmt, ##__VA_ARGS__)
    #define DEBUG_ENABLED true
#else
    #define DEBUG_PRINT(fmt, ...)
    #define DEBUG_ENABLED false
#endif

// Debug output macros for different categories
#ifdef DEBUG_MODE
    #define DEBUG_CLICK(fmt, ...) printf("[CLICK] " fmt, ##__VA_ARGS__)
    #define DEBUG_DRAG(fmt, ...) printf("[DRAG] " fmt, ##__VA_ARGS__)
    #define DEBUG_CARD(fmt, ...) printf("[CARD] " fmt, ##__VA_ARGS__)
    #define DEBUG_ANIMATION(fmt, ...) printf("[ANIM] " fmt, ##__VA_ARGS__)
#else
    #define DEBUG_CLICK(fmt, ...)
    #define DEBUG_DRAG(fmt, ...)
    #define DEBUG_CARD(fmt, ...)
    #define DEBUG_ANIMATION(fmt, ...)
#endif
