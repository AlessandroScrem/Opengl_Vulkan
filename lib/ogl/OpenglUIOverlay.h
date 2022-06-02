#pragma once

struct GLFWwindow;

class OpenglUIOverlay
{
public:

    void init();

    void newFrame();
    void draw();
    void cleanup();
    
    GLFWwindow *windowPtr;
};

