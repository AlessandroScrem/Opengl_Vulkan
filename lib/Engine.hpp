#pragma once

#include "mytypes.hpp"

//std
#include <memory>
#include <iostream>

struct Color
{
    float red = 0.f;;
    float green = 0.f;;
    float blue = 0.f;
    float alpha = 1.0f;
};



class Engine
{    
public:
    Engine(){ SPDLOG_TRACE("constructor");     
    }
    virtual ~Engine(){SPDLOG_TRACE("destructor");}
       
    virtual void run() = 0;

   static std::unique_ptr<Engine> create(EngineType type);

protected:
    Color background{};

private:
    static std::unique_ptr<Engine> makeVulkan();
    static std::unique_ptr<Engine> makeOpengl();
};

