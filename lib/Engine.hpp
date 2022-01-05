#pragma once

#include "mytypes.hpp"

//std
#include <memory>
#include <iostream>




class Engine
{    
public:
    Engine(){ SPDLOG_TRACE("constructor");     
    }
    virtual ~Engine(){SPDLOG_TRACE("destructor");}
       
    virtual void run() = 0;

   static std::unique_ptr<Engine> create(EngineType type);
private:
    static std::unique_ptr<Engine> makeVulkan();
    static std::unique_ptr<Engine> makeOpengl();
};

