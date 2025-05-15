#ifndef KOALA_WINDOW_H
#define KOALA_WINDOW_H

#include <GLFW/glfw3.h>
#include "../types.h"

typedef struct KoalaUIWindow {
#ifdef _glfw3_h_
    GLFWwindow* window;
#endif
} KoalaUIWindow;

Result koala_create_window() {
    Result res;
#ifdef _glfw3_h_
    if (!glfwInit()) {
        res.is_ok = 0;
        GENERIC_SET(res.err, "Failed to initialize GLFW");
        res.err.type = GENERIC_TYPE_STR;
        return res;
    }
    GLFWwindow* window = glfwCreateWindow("KOALA UI (GLFW).", 800, 600, NULL, NULL);
    if (window == NULL) {
        res.is_ok = 0;
        GENERIC_SET(res.err, "Failed to create glfw window");
        res.err.type = GENERIC_TYPE_STR;
        return res;
    }
#endif
    res.is_ok = 1;
    GENERIC_SET(res.ok, window);
    res.ok.type = GENERIC_TYPE_PTR;
    return res;
}


#endif // KOALA_WINDOW_H