#include "OpenglUIOverlay.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
//lib
#include <stdexcept>

void OpenglUIOverlay::init()
{
	// Setup Platform/Renderer bindings
	if(!ImGui_ImplGlfw_InitForOpenGL(windowPtr, true)){
        throw std::runtime_error("failed to initialize ImGui_ImplGlfw_InitForOpenGL!");
    }

    // TODO update glsl_version acconrdigly with opengl context creation
    const char *glsl_version = "#version 450 core";
	if(!ImGui_ImplOpenGL3_Init(glsl_version)){
        throw std::runtime_error("failed to initialize ImGui_ImplOpenGL3_Init!");
    }
}

void OpenglUIOverlay::cleanup()
{    
    // Cleanup ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
}

void OpenglUIOverlay::newFrame()
{
    // feed inputs to dear imgui, start new frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
}  

void OpenglUIOverlay::draw()
{
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
