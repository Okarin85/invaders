#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <GLFW/glfw3.h>

#include "types.h"
#include "invaders.h"
#include "gpu.h"

const int EMU_WIDTH = 224;
const int EMU_HEIGHT = 256;
const int FPS = 60;

static invaders si;

static void error_callback(int error, const char* description) {
    fprintf(stderr, "error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode,
                         int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        if (key == GLFW_KEY_C) { // coin
            si.port1 = (1 << 0) | si.port1;
        }
        else if (key == GLFW_KEY_2) { // P2 start button
            si.port1 = (1 << 1) | si.port1;
        }
        else if (key == GLFW_KEY_ENTER) { // P1 start button
            si.port1 = (1 << 2) | si.port1;
        }
        // else if (key == x) { // ?
        //     si.port1 = (1 << 3) | si.port1;
        // }
        else if (key == GLFW_KEY_SPACE) {
            si.port1 = (1 << 4) | si.port1; // P1 shoot button
            si.port2 = (1 << 4) | si.port2; // P2 shoot button
        }
        else if (key == GLFW_KEY_LEFT) {
            si.port1 = (1 << 5) | si.port1; // P1 joystick left
            si.port2 = (1 << 5) | si.port2; // P2 joystick left
        }
        else if (key == GLFW_KEY_RIGHT) {
            si.port1 = (1 << 6) | si.port1; // P1 joystick right
            si.port2 = (1 << 6) | si.port2; // P2 joystick right
        }
        // else if (key == x) { // ?
        //     si.port1 = (1 << 7) | si.port1;
        // }
    }
    else if (action == GLFW_RELEASE) {
        if (key == GLFW_KEY_C) { // coin
            si.port1 = 0b11111110 & si.port1;
        }
        else if (key == GLFW_KEY_2) { // P2 start button
            si.port1 = 0b11111101 & si.port1;
        }
        else if (key == GLFW_KEY_ENTER) { // P1 start button
            si.port1 = 0b11111011 & si.port1;
        }
        // else if (key == x) { // ?
        // }
        else if (key == GLFW_KEY_SPACE) {
            si.port1 = 0b11101111 & si.port1; // P1 shoot button
            si.port2 = 0b11101111 & si.port2; // P2 shoot button
        }
        else if (key == GLFW_KEY_LEFT) {
            si.port1 = 0b11011111 & si.port1; // P1 joystick left
            si.port2 = 0b11011111 & si.port2; // P2 joystick left
        }
        else if (key == GLFW_KEY_RIGHT) {
            si.port1 = 0b10111111 & si.port1; // P1 joystick right
            si.port2 = 0b10111111 & si.port2; // P2 joystick right
        }
        // else if (key == x) { // ?
        // }
    }
}

int main(int argc, char **argv) {
    // run tests
    if (argc == 2 && !strcmp(argv[1], "--test")) {
        i8080 cpu;
        cpu_init(&cpu);
        cpu_run_tests(&cpu, "roms/tests/TEST.COM");
        cpu_init(&cpu);
        cpu_run_tests(&cpu, "roms/tests/8080PRE.COM");
        cpu_init(&cpu);
        cpu_run_tests(&cpu, "roms/tests/CPUTEST.COM");
        cpu_init(&cpu);
        cpu_run_tests(&cpu, "roms/tests/8080EX1.COM");
        return 0;
    }

    invaders_init(&si);
    if (cpu_load_file(&si.cpu, "roms/invaders.h", 0x0000) != 0) return -1;
    if (cpu_load_file(&si.cpu, "roms/invaders.g", 0x0800) != 0) return -1;
    if (cpu_load_file(&si.cpu, "roms/invaders.f", 0x1000) != 0) return -1;
    if (cpu_load_file(&si.cpu, "roms/invaders.e", 0x1800) != 0) return -1;

    // GLFW setup
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow* window;
    glfwSetErrorCallback(error_callback);

    if (!glfwInit()) {
        return -1;
    }

    // create a window and its context
    window = glfwCreateWindow(EMU_WIDTH * 2, EMU_HEIGHT * 2, "Space Invaders",
                              NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwSetKeyCallback(window, key_callback);

    glfwSetWindowSizeLimits(window, EMU_WIDTH, EMU_HEIGHT,
                            GLFW_DONT_CARE, GLFW_DONT_CARE);
    glfwMakeContextCurrent(window);

    // retrieve frame size for viewport
    int frame_width, frame_height;
    glfwGetFramebufferSize(window, &frame_width, &frame_height);

    // setting up viewport and orthographic projection
    glViewport(0, 0, frame_width, frame_height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, EMU_WIDTH, EMU_HEIGHT, 0, -1, 1);

    glfwSwapInterval(1);

    float step_timer = glfwGetTime();
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);
        gpu_draw(&si.cpu);
        glfwSwapBuffers(window);

        glfwPollEvents();

        if (glfwGetTime() - step_timer > 1.f / (FPS + 10)) {
            // (+10 to boost up the game's speed a little)
            step_timer = glfwGetTime();
            invaders_update(&si);
        }
    }

    glfwTerminate();
    return 0;
}
