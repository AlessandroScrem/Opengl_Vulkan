#pragma once

// common lib
#include <utils.hpp>
#include <camera.hpp>
// libs
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp> 

namespace ngn
{
    class Command
    {
    protected:
        glm::vec2 offset_;
    public:
        Command(glm::vec2 offset) : offset_(offset){}
        virtual ~Command(){}
        virtual void Execute() const = 0;

        glm::vec2 get_offset(){return offset_;}
    };
    
    class CmdOrbit : public Command
    {
        private:
        Camera &cam_;
        
        public:
        CmdOrbit(Camera &cam, glm::vec2 offset) : cam_(cam) , Command(offset){}

        void Execute() const override{
            if(Mouse::isClicked()){ 
                glm::vec2 mouse = Mouse::getDirection();
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

        public:
        CmdFov(Camera &cam, glm::vec2 offset) : cam_(cam) , Command(offset){}

        void Execute() const override{
            // x axis only
            cam_.cameraFov(offset_.x);
        }
    }; 

    class CmdPan : public Command
    {
        private:
        Camera &cam_;

        public:
        CmdPan(Camera &cam, glm::vec2 offset) : cam_(cam) , Command(offset){}

        void Execute() const override{
            if(Mouse::isClicked()){ 
                glm::vec2 mouse = Mouse::getDirection();
                cam_.cameraPan(mouse.x, mouse.y);
            }else{
                cam_.cameraPan(offset_.x, offset_.y);
            }
        }
    };

    class CmdDolly : public Command
    {
        private:
        Camera &cam_;

        public:
        CmdDolly(Camera &cam, glm::vec2 offset) : cam_(cam) , Command(offset){}

        void Execute() const override{
            // x axis only
            cam_.cameraDolly(offset_.x, offset_.y);
        }
    }; 

    class CmdRoll : public Command
    {
        private:
        Camera &cam_;

        public:
        CmdRoll(Camera &cam, glm::vec2 offset) : cam_(cam) , Command(offset){}

        void Execute() const override{
            if(Mouse::isClicked()){ 
                glm::vec2 mouse = Mouse::getDirection();
                cam_.cameraRoll(mouse.x, mouse.y);
            }else{
                cam_.cameraRoll(offset_.x, offset_.y);
            }
        }
    };   
} // namespace ngn

