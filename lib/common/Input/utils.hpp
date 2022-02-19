#pragma once

//std
#include <sstream>
#include <string>
#include <cassert>
#include <chrono>

//lib
#include <glm/glm.hpp>

 namespace ngn
{

// action 
struct Feed{

    Feed(float x_ = 0.f , float y_ = 0.f) : x{x_}, y{y_}{}
    Feed(glm::vec2 v) : x{v.x}, y{v.y}{}

    Feed& operator+=(const Feed& rhs){
        this->x += rhs.x;
        this->y += rhs.y;
        return *this;}
    Feed& operator+=(const glm::vec2& rhs){
        this->x += rhs.x;
        this->y += rhs.y;
        return *this;}

    float x{0.f};
    float y{0.f};
};

struct Time{

    // Time between current frame and last frame
    inline static float frameTime = 1.0f;
    static float getFrameTime(){ return frameTime; }

   /**
     * @brief Get the Fps calculation
     * 
     * @return float (ms/frame)
     */
    static float getFps() {
        static int nbFrames = 0;
        static std::chrono::time_point<std::chrono::high_resolution_clock>  lastTime;
        auto newTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - lastTime).count();

        if (deltaTime >= 1.0 ){ // If more than 1 sec ago
            frameTime = 0.001f * (float) nbFrames;
            nbFrames = 0;
            lastTime = newTime;
        }
        nbFrames++;
        return frameTime;
    }
};


// mouse direction utility
class Mouse{
private:
    	   
    enum class  Direction
    { 
        straightX, 
        straightY, 
        Free
    };

    // Constants
    inline static const float mouseSensitivity = {0.1f};

    // mouse position first point
    inline static glm::vec2 clickpoint{glm::vec2(0.f)}; 
    inline static glm::vec2 prevpoint{glm::vec2(0.f)};
    inline static glm::vec2 pos{glm::vec2(0.f)};
    // is checking for direction
    inline static bool checkdir{false};
    // movement is just started
    inline static bool is_clicked{false};
    inline static Direction direction = Direction::Free;

    /**
     * @brief Check for mouse movement is free or straight
     * 
     */
    static void ckeckdirection(){
        float deltax = (float) abs(pos.x - clickpoint.x);
        float deltay = (float) abs(pos.y - clickpoint.y);
        // 
        if(deltax > 30|| deltay > 30){
            if (deltay <  3 ){
                // if the angle is almost horizontal -> mouse forced to X direction
                direction = Direction::straightX;
            } else if (deltax < 3 ){
                // if the angle is almost verical -> mouse forced to Y direction
                direction = Direction::straightY;
            }
            else {
                // mouse set to Free movements
                direction = Direction::Free;  
            }
            checkdir = false;
        }
     }

public:

    static float getMouseSensitivity(){return mouseSensitivity;}

    static std::string getDirection_str(){
        std::string dir{};
        switch (direction)
        {
        case Direction::straightX:
            dir = "straightX";
            break;
        case Direction::straightY:
            dir = "straightY";
            break;
        case Direction::Free:
            dir = "Free";
            break;   
        default:
            dir = "";
            break;
        }
        return dir;
    }

    static bool isClicked(){return is_clicked;}

   /**
    * @brief Start after click
    * 
    */
    static void Start(){
        clickpoint = pos; 
        prevpoint = clickpoint;
        checkdir = true;
        is_clicked = true;
   }

    /**
     * @brief Call to stop mouse movement calculation
     * 
     */
    static void Stop(){
        is_clicked = false;
        checkdir = false;
        direction = Direction::Free;
    }

    // dummy function
    static void Move(float xpos, float ypos){
        pos.x = xpos;
        pos.y = ypos;
    }

    /**
     * @brief Get the mouse Direction offset x,y
     * 
     * @return glm::vec2 mouse offset [x] left to right, [y] top  to bottom
     *                         
     */
    static glm::vec2 getDirection(){
        glm::vec2  offset = glm::vec2(0.0f);

        if(!is_clicked) {
            return offset;
        }

        if(checkdir){
            ckeckdirection();
        }
        // x direction left to right 
        offset.x = (pos.x - prevpoint.x) * mouseSensitivity * Time::getFrameTime();
        // y direction up to down
        offset.y = (pos.y - prevpoint.y) * mouseSensitivity * Time::getFrameTime();

        switch (direction)
        {
        case Direction::Free:
            break;
        case Direction::straightX:
            offset.y = 0.0f;
            break;
        case Direction::straightY:
            offset.x = 0.0f;
            break;      
        default:
            break;
        }
        // set mouse lastpoint to current position
        prevpoint.x = pos.x;
        prevpoint.y = pos.y;
        
        return offset;
    }
};    
} // namespace utils
