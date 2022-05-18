#pragma once

//lib
#include <glm/glm.hpp>
//std
#include <string>
#include <chrono>

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
private:
    // Time between current frame and last frame
    inline static float     frameTime    = 1.0f;
    inline static float     fpsTimer     = 0.f; 
    inline static uint32_t  lastFPS      = 0;
    inline static uint32_t  frameCounter = 0;;
    inline static std::chrono::time_point<std::chrono::high_resolution_clock>  tStart;
    inline static std::chrono::time_point<std::chrono::high_resolution_clock>  lastTimestamp;

public:
    /**
     * @brief Get the Frame Time object
     * 
     * @return float 
     */
    static float getFrameTime(){ return frameTime; }

    static bool time(){ return (fpsTimer >=1000.0); }

    static uint32_t geFps(){ return lastFPS; }

    static void start() { tStart = std::chrono::high_resolution_clock::now(); }

    static void end() 
    {   
        frameCounter++;
        auto tEnd = std::chrono::high_resolution_clock::now();
	    fpsTimer = std::chrono::duration<float, std::milli>(tEnd - lastTimestamp).count();
        if (fpsTimer >= 1000.0 ) // If more than 1 sec ago
        { 
	        frameTime = std::chrono::duration<float, std::milli>(tEnd - tStart).count();
            lastFPS = static_cast<uint32_t>((float)frameCounter * (1000.0f / fpsTimer));

            frameCounter = 0;
		    lastTimestamp = tEnd;
        }
    }



   /**
     * @brief Update timeframe calculation
     * 
     */
    [[deprecated("use start end instead")]]
    static void update() {
        static uint32_t nbFrames = 0;
        static std::chrono::time_point<std::chrono::high_resolution_clock>  lastTime;
        auto newTime = std::chrono::high_resolution_clock::now();
        auto deltaTime = (float)(std::chrono::duration<double, std::milli>(newTime - lastTime).count());

        if (deltaTime >= 1000.0 ){ // If more than 1 sec ago
            frameTime = 1.0f / (float) nbFrames;
            nbFrames = 0;
            lastTime = newTime;
        }
        nbFrames++;
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
    inline static const float mouseSensitivity = {0.01f};

    // mouse position first point
    inline static glm::vec2 clickpoint{0.f}; 
    inline static glm::vec2 prevpoint{0.f};
    // mouse movement
    inline static glm::vec2 offset_{0.0f};

    // is checking for direction
    inline static bool checkdir{false};
    // movement is just started
    inline static bool is_clicked{false};
    inline static Direction direction = Direction::Free;

    /**
     * @brief Check for mouse movement is free or straight
     * 
     */
    static void ckeckdirection(float xpos, float ypos){
        float deltax = (float) abs(xpos - clickpoint.x);
        float deltay = (float) abs(ypos - clickpoint.y);
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
        clickpoint = prevpoint; 
        checkdir = true;
        is_clicked = true;
        offset_ = {0.0f, 0.0f};
        direction = Direction::Free;
   }

    /**
     * @brief Stop after mouse button up 
     * 
     */
    static void Stop(){
        is_clicked = false;
        checkdir = false;
        offset_ = {0.0f, 0.0f};
    }

    /**
     * @brief Mouse is moving
     * 
     * @param xpos 
     * @param ypos 
     */
    static void Move(float xpos, float ypos){
        if(checkdir){
            ckeckdirection(xpos, ypos);
        }
        offset_.x = xpos - prevpoint.x;
        offset_.y = ypos - prevpoint.y;
        prevpoint.x = xpos;
        prevpoint.y = ypos;
    }

    /**
     * @brief Get the mouse Direction offset x,y
     * 
     * @return glm::vec2 mouse offset [x] left to right, [y] top  to bottom
     *                         
     */
    static glm::vec2 getDirection(){
        glm::vec2  offset{0.0f};

        if(!is_clicked) {
            return offset;
        }

        offset = offset_ * mouseSensitivity;

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
        return offset;
    }
};    
} // namespace utils
