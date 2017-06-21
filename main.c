#include <stdio.h>
#ifdef __APPLE__
#	define GLFW_INCLUDE_GLCOREARB
#endif

#include "glad/glad.h"

#define GLFW_INCLUDE_GLEXT
#include <GLFW/glfw3.h>
#include "nanovg/nanovg.h"
#define NANOVG_GL3_IMPLEMENTATION
#include "nanovg/nanovg_gl.h"
#include "demo.h"


void errorcb(int error, const char* desc)
{
    printf("GLFW error %d: %s\n", error, desc);
}

int blowup = 0;
int screenshot = 0;
int premult = 0;

static void key(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    NVG_NOTUSED(scancode);
    NVG_NOTUSED(mods);
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
        blowup = !blowup;
    if (key == GLFW_KEY_S && action == GLFW_PRESS)
        screenshot = 1;
    if (key == GLFW_KEY_P && action == GLFW_PRESS)
        premult = !premult;
}

int main()
{
    GLFWwindow* window;
    DemoData data;
    NVGcontext* vg = NULL;
    double prevt = 0, cpuTime = 0;

    if (!glfwInit()) {
        printf("Failed to init GLFW.");
        return -1;
    }

    glfwSetErrorCallback(errorcb);
#ifndef _WIN32 // don't require this on win32, and works with more cards
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);

#ifdef DEMO_MSAA
    glfwWindowHint(GLFW_SAMPLES, 4);
#endif
    window = glfwCreateWindow(1000, 600, "NanoVG", NULL, NULL);
//	window = glfwCreateWindow(1000, 600, "NanoVG", glfwGetPrimaryMonitor(), NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwSetKeyCallback(window, key);

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
    {
        printf("Failed to initialize OpenGL context");
        return -1;
    }
    printf("OpenGL Version %d.%d loaded", GLVersion.major, GLVersion.minor);

#ifdef NANOVG_GLEW
    glewExperimental = GL_TRUE;
    if(glewInit() != GLEW_OK) {
        printf("Could not init glew.\n");
        return -1;
    }
    // GLEW generates GL error because it calls glGetString(GL_EXTENSIONS), we'll consume it here.
    glGetError();
#endif

#ifdef DEMO_MSAA
    vg = nvgCreateGL3(NVG_STENCIL_STROKES);
#else
    vg = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES);
#endif
    if (vg == NULL) {
        printf("Could not init nanovg.\n");
        return -1;
    }

    if (loadDemoData(vg, &data) == -1)
        return -1;

    glfwSwapInterval(0);

    glfwSetTime(0);
    prevt = glfwGetTime();

    while (!glfwWindowShouldClose(window))
    {
        double mx, my, t, dt;
        int winWidth, winHeight;
        int fbWidth, fbHeight;
        float pxRatio;

        t = glfwGetTime();
        dt = t - prevt;
        prevt = t;

        glfwGetCursorPos(window, &mx, &my);
        glfwGetWindowSize(window, &winWidth, &winHeight);
        glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
        // Calculate pixel ration for hi-dpi devices.
        pxRatio = (float)fbWidth / (float)winWidth;

        // Update and render
        glViewport(0, 0, fbWidth, fbHeight);
        if (premult)
            glClearColor(0,0,0,0);
        else
            glClearColor(0.3f, 0.3f, 0.32f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);

        nvgBeginFrame(vg, winWidth, winHeight, pxRatio);

        renderDemo(vg, mx,my, winWidth,winHeight, t, blowup, &data);

        nvgEndFrame(vg);

        // Measure the CPU time taken excluding swap buffers (as the swap may wait for GPU)
        cpuTime = glfwGetTime() - t;

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    freeDemoData(vg, &data);

    nvgDeleteGL3(vg);

    glfwTerminate();
    return 0;
}
