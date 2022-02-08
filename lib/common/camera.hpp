#pragma once

#include "utils.hpp"

// lib
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <vector>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum class Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Defines camera action
enum class CamAction {
    ORBIT,
    ROLL,
    PAN,
    DOLLY,
    FOV,
    NONE
};


struct CameraController{
    CamAction action{CamAction::NONE};
    ngn::Feed feed{};
};


// Camera class that processes input and calculates the corresponding view Matrices for use in OpenGL
class Camera
{
private:
    // Default camera values
    // setup from 35mm & FX format (1)
    const int filmHeight = 35;
    const float FOV = 45.0f;
    
    // camera Attributes
    glm::vec3 Position;     // eye point
    glm::vec3 Target;       // view target point
    glm::vec3 WorldUp;      // up (in world coordinates)
    float Fov;

    // camera matrix
    glm::mat4 matrix;
    
public:
    
    /**
     * @brief Construct a new Camera object
     * 
     * @param position
     * @param target 
     * @param up
     * @param Fov - (optional) default = 45
     */
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 5.0f),
            glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f), 
           glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f))
           : Fov(FOV)
    {
    // FIXME: remove hardcoded Front

        Position = position;
        Target = target;
        WorldUp = up;
 
        updateCameraVectors();
    }

    // Not copyable or movable
    Camera(const Camera &) = delete;
    Camera &operator=(const Camera &) = delete;
    Camera(Camera &&) = delete;
    Camera &operator=(Camera &&) = delete;

    /**
     * @brief Get the Focal lenght from 35mm & FX format (1)
     * 
     * @return float 
     *   
     */
    float GetFocal()
    {
        return filmHeight * tan(glm::radians(Fov * 0.5f));
    }

    /**
     * @brief Get the camera field of view in degree
     * 
     * @return float 
     */
    float GetFov() {return Fov;}

    /**
     * @brief Get the Camera Position 
     * 
     * @return glm::vec3 
     */
    glm::vec3 GetPosition() {return Position;}

    /**
     * @brief Get the Target view point
     * 
     * @return glm::vec3 
     */
    glm::vec3 GetTarget() {return Target;}
    
    /**
     * @brief Get the Camera Front Direction **camera points towards the negative z-axis we want to negate the direction vector**
     * 
     * @return glm::vec3 
     */
    glm::vec3 GetFrontVector() {return glm::normalize(- getcameraDirection());}

    /**
     * @brief Get the View Matrix object
     * 
     * @return glm::mat4 
     */
    glm::mat4 GetViewMatrix()
    {
        return matrix;
    }

 
    void cameraOrbit(float xoffset, float yoffset)
    {
        // rot around Y world
        glm::mat4 Ymat = glm::rotate(glm::mat4(1.0f), -xoffset, glm::vec3(0.0f,1.0f,0.0f)); 
        // rot around X cam
        glm::mat4 Xmat = glm::rotate(glm::mat4(1.0f), -yoffset, getcameraRight()); 
        glm::mat4 mat = Ymat * Xmat;

        // update camera position
        Position = glm::vec3(mat * glm::vec4(Position, 1.0f));
        // update camera Front
        Target = glm::vec3(mat * glm::vec4(Target, 1.0f));
        // update up vector
        WorldUp = glm::vec3(mat * glm::vec4(WorldUp, 1.0f));
        updateCameraVectors();
    }

    void cameraRoll(float xoffset, float yoffset)
    {
        // rot around Z cam 
        float stepxy = (yoffset - xoffset) / 2.0f;
        glm::mat4 mat = glm::rotate(glm::mat4(1.0f), stepxy, getcameraDirection()); 

        Position = glm::vec3(mat * glm::vec4(Position, 1.0f));
        WorldUp = glm::vec3(mat * glm::vec4(WorldUp, 1.0f));
        Target = glm::vec3(mat * glm::vec4(Target, 1.0f));
        updateCameraVectors();
    }

    void cameraPan(float xoffset, float yoffset)
    {
        // translate Y cam direction  
        glm::mat4 Ymat = glm::translate(glm::mat4(1.0f), WorldUp * yoffset );
        // translate X cam direction
        glm::mat4 Xmat = glm::translate(glm::mat4(1.0f), getcameraRight() * -xoffset );
        glm::mat4 mat = Xmat * Ymat;

        Position = glm::vec3(mat * glm::vec4(Position, 1.0f));
        Target = glm::vec3(mat * glm::vec4(Target, 1.0f));
        updateCameraVectors();
    }

    void cameraDolly(float xoffset, float yoffset)
    {
        // step xy direction bottomleft to topright
        float stepxy = (yoffset - xoffset) / 2.0f;
        glm::mat4 mat = glm::translate(glm::mat4(1.0f), getcameraDirection() * stepxy);

        Position = glm::vec3(mat * glm::vec4(Position, 1.0f));
        Target = glm::vec3(mat * glm::vec4(Target, 1.0f));
        updateCameraVectors();
    }

    void cameraFov(float yoffset,  bool fixedfocal = false)
    {
        float folcalLenght = glm::length(Position - Target);
        float focalHeight = (float) tan(glm::radians(Fov * 0.5)) * folcalLenght;

        Fov -= (float)yoffset;
        if (Fov < 2.0f)
            Fov = 2.0f;
        if (Fov > 178.0f)
            Fov = 178.0f; 
                
        if (fixedfocal) {
            float newfocalLenght = focalHeight / tan(glm::radians(Fov * 0.5f));
            float deltafocalLenght = newfocalLenght -folcalLenght;
            glm::vec3 cameraDirection = glm::normalize(Position - Target);
            glm::mat4 mat = glm::translate(glm::mat4(1.0f),  cameraDirection *  deltafocalLenght);
            Position = glm::vec3( mat * glm::vec4(Position, 1.0f));

            updateCameraVectors();
        }
    }

    void Update(CameraController controller)
    {
        auto feed = controller.feed;
        switch (controller.action)
        {
        case CamAction::ORBIT :
            cameraOrbit(feed.x, feed.y);
            break;
        case CamAction::PAN :
            cameraPan(feed.x, feed.y);
            break;
        case CamAction::ROLL :
            cameraRoll(feed.x, feed.y);
            break;
       case CamAction::FOV :
            cameraFov(feed.x);
            break;
       default:
            break;
        }
    }

    /**
     * @brief Process input received from mouse movements
     * 
     * @param xoffset 
     * @param yoffset 
     */
/*     void ProcessMouseMovement(float xoffset, float yoffset)
    {
    // TODO: add pivot point
  
        if (cam_action == CamAction::NONE){
            return;
        }  
        if (cam_action == CamAction::ORBIT) {
            cameraOrbit(xoffset, yoffset);
        } 
       if (cam_action == CamAction::ROLL) {
           cameraRoll(xoffset, yoffset);       
        } 
        if (cam_action == CamAction::PAN) { 
            cameraPan(xoffset, yoffset);
        } 
         if (cam_action == CamAction::DOLLY) {
             cameraDolly(xoffset, yoffset);
        }       
    } 
*/
 
    /**
     * @brief Change Field of view (FOV)
     * 
     * @param yoffset  increment/decrement (FOV)
     * @param fixedfocal fix focal lenght
     */
 /*    void ProcessMouseScroll(float yoffset,  bool fixedfocal = false)
    {
        cameraFov(yoffset, fixedfocal);
    }
 */
private:
    /**
     * @brief update matrix from lookAt function
     * 
     */
    void updateCameraVectors()
    {
        matrix = glm::lookAt(Position, Target, WorldUp);
     }
   
    glm::vec3 getcameraDirection() { return glm::normalize(Position - Target); }
    glm::vec3 getcameraRight() { return glm::normalize(glm::cross(WorldUp, getcameraDirection() )); }

};
