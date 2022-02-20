#pragma once

#include "common/input/utils.hpp"
#include "common/camera.hpp"

// libs
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp> 

namespace ngn
{
    class Command
    {
    public:
        virtual ~Command(){}

        virtual void Execute() const = 0;
    };
    
    class CmdOrbit : public Command
    {
        private:
        Camera &cam_;
        glm::vec2 offset_;

        public:
        CmdOrbit(Camera &cam, glm::vec2 offset) : cam_(cam) , offset_(offset){}

        void Execute() const override{
            if(Mouse::isClicked()){ 
                glm::vec2 mouse = Mouse::getDirection() * Time::getFrameTime();
                cam_.cameraOrbit(mouse.x, mouse.y);
            }else{
                cam_.cameraOrbit(offset_.x, offset_.y);
            }
        }
    }; 
     
    class CmdFov : public Command
    {
        private:
        Camera &cam_;
        glm::vec2 offset_;

        public:
        CmdFov(Camera &cam, glm::vec2 offset) : cam_(cam) , offset_(offset){}

        void Execute() const override{
            // x axis only
            cam_.cameraFov(offset_.x);
        }
    }; 

    class CmdPan : public Command
    {
        private:
        Camera &cam_;
        glm::vec2 offset_;

        public:
        CmdPan(Camera &cam, glm::vec2 offset) : cam_(cam) , offset_(offset){}

        void Execute() const override{
            if(Mouse::isClicked()){ 
                glm::vec2 mouse = Mouse::getDirection() * Time::getFrameTime();
                cam_.cameraPan(mouse.x, mouse.y);
            }else{
                cam_.cameraPan(offset_.x, offset_.y);
            }
        }
    }; 

    class CmdRoll : public Command
    {
        private:
        Camera &cam_;
        glm::vec2 offset_;

        public:
        CmdRoll(Camera &cam, glm::vec2 offset) : cam_(cam) , offset_(offset){}

        void Execute() const override{
            if(Mouse::isClicked()){ 
                glm::vec2 mouse = Mouse::getDirection() * Time::getFrameTime();
                cam_.cameraRoll(mouse.x, mouse.y);
            }else{
                cam_.cameraRoll(offset_.x, offset_.y);
            }
        }
    };   
} // namespace ngn

