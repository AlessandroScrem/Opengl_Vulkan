#pragma once

#include "mytypes.hpp"

//std
#include <memory>
#include <iostream>



class Engine
{    
public:
    Engine(){ std::cout << "Engine  constructor\n";}
    virtual ~Engine(){std::cout << "Engine  destructor\n";}
       
    virtual void run() = 0;

   static std::unique_ptr<Engine> create(EngineType type);
private:
    static std::unique_ptr<Engine> makeVulkan();
    static std::unique_ptr<Engine> makeOpengl();
};

