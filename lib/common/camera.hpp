#pragma once

#include "utils.hpp"

// lib
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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

    class Fov_{
        const float max = 178.0f; 
        const float min = 2.0f; 
        float fov_;
    public:
        /**
        * @brief Fov should be from 2 to 178
        * 
        */
        void set(float fov){
            fov_ = fov;
            if (fov_ < min)
                fov_ = min;
            if (fov_ > max)
                fov_ = max; 
        }
        float get() {return fov_; }
    }Fov;

    // Default camera values
    // setup from 35mm & FX format (1)
    const int filmHeight = 35;
    
    // camera Attributes
    glm::vec3 Position;     // eye point
    glm::vec3 Target;       // view target point
    glm::vec3 WorldUp;      // up (in world coordinates)

    // camera matrix
    glm::mat4 matrix;
    
public:
    
    /**
     * @brief Construct a new Camera object default Yup
     * 
     * @param position
     * @param target 
     * @param up
     * @param Fov - (optional) default = 45
     */
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 5.0f),
            glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f), 
            glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
            float fov = 45.0f);


    float GetFov() {return Fov.get();}
    void  SetFov(float value) {Fov.set(value); }

    /**
     * @brief Get the Focal lenght from 35mm & FX format (1)
     * 
     * @return float 
     *   
     */
    float GetFocal()
    {
        return filmHeight * tan(glm::radians(Fov.get() * 0.5f));
    }
    
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
     * @brief vector pointing in the reverse direction of what it is targeting,
     *        that represents the positive z-axis of the camera space.
     * 
     * @return glm::vec3 
     */
    glm::vec3 getcameraDirection() { return glm::normalize(Position - Target); }
 
     /**
     * @brief right vector that represents the positive x-axis of the camera space
     * 
     * @return glm::vec3 
     */
    glm::vec3 getcameraRight() { return glm::normalize(glm::cross(WorldUp, getcameraDirection() )); }

    /**
     * @brief Get the View Matrix object
     * 
     * @return glm::mat4 
     */
    glm::mat4 GetViewMatrix()
    {
        return matrix;
    }

    void Update(CameraController controller);

    void cameraOrbit(float xoffset, float yoffset);
    void cameraRoll(float xoffset, float yoffset);
    void cameraPan(float xoffset, float yoffset);
    void cameraDolly(float xoffset, float yoffset);
    void cameraFov(float yoffset,  bool fixedfocal = false);
  
private:

    void normalizeFov();

    /**
     * @brief update matrix from lookAt function
     * 
     */
    void updateCameraVectors()
    {
        matrix = glm::lookAt(Position, Target, WorldUp);
     }
};
