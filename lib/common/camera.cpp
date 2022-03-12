

#include "camera.hpp"
//libs

/**
 * @brief Update camera position from fontroller
 * 
 * @param controller 
 */
void Camera::Update(CameraController controller)
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
 * @brief Rotate camera matrix around Y axis then cameraRight()
 * 
 * @param xoffset (radianns) 
 * @param yoffset (radianns)
 */
void Camera::cameraOrbit(float xoffset, float yoffset)
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

/**
 * @brief Rotate camera matrix around cameraDirection
 * 
 * @param xoffset 
 * @param yoffset 
 */
void Camera::cameraRoll(float xoffset, float yoffset)
{
    // rot around Z cam 
    //float stepxy = (yoffset - xoffset) / 2.0f;
    float step = (abs(xoffset) > abs(yoffset)) ? xoffset : yoffset;
    glm::mat4 mat = glm::rotate(glm::mat4(1.0f), step, getcameraDirection()); 

    Position = glm::vec3(mat * glm::vec4(Position, 1.0f));
    WorldUp = glm::vec3(mat * glm::vec4(WorldUp, 1.0f));
    Target = glm::vec3(mat * glm::vec4(Target, 1.0f));
    updateCameraVectors();
}

/**
 * @brief Move camera matrix X , Y directions
 * 
 * @param xoffset 
 * @param yoffset 
 */
void Camera::cameraPan(float xoffset, float yoffset)
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

/**
 * @brief Move camera matrix +/-Z directions
 * 
 * @param xoffset 
 * @param yoffset 
 */
void Camera::cameraDolly(float xoffset, float yoffset)
{
    // step xy direction bottomleft to topright
    float step = (abs(xoffset) > abs(yoffset)) ? xoffset : yoffset;
    glm::mat4 mat = glm::translate(glm::mat4(1.0f), getcameraDirection() * step);

    Position = glm::vec3(mat * glm::vec4(Position, 1.0f));
    Target = glm::vec3(mat * glm::vec4(Target, 1.0f));
    updateCameraVectors();
}

/**
 * @brief Increment/decrement camera Fov
 * 
 * @param yoffset   fov increment
 * @param fixedfocal if (fixedfocal) change camera position accordingly
 */
void Camera::cameraFov(float yoffset,  bool fixedfocal)
{
    float folcalLenght = glm::length(Position - Target);
    float focalHeight = (float) tan(glm::radians(Fov.get() * 0.5)) * folcalLenght;

    Fov.set(Fov.get() + yoffset);
            
    if (fixedfocal) {
        float newfocalLenght = focalHeight / tan(glm::radians(Fov.get() * 0.5f));
        float deltafocalLenght = newfocalLenght -folcalLenght;
        glm::vec3 cameraDirection = glm::normalize(Position - Target);
        glm::mat4 mat = glm::translate(glm::mat4(1.0f),  cameraDirection *  deltafocalLenght);
        Position = glm::vec3( mat * glm::vec4(Position, 1.0f));

        updateCameraVectors();
    }
}

