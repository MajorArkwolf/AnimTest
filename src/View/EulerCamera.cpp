#include "EulerCamera.hpp"


#include <glm/gtc/matrix_transform.hpp>

// Constructor with vectors
View::Camera::Camera(glm::dvec3 position,
               glm::dvec3 up, double yaw,
               double pitch)
        : Front(glm::dvec3(0.0f, 0.0f, -1.0f)), MovementSpeed(static_cast<float>(SPEED)),
          MouseSensitivity(SENSITIVITY), Zoom(ZOOM) {
    Position = position;
    WorldUp  = up;
    Yaw      = yaw;
    Pitch    = pitch;
    updateCameraVectors();
}

// Constructor with scalar values
View::Camera::Camera(double posX, double posY, double posZ, double upX, double upY,
               double upZ, double yaw, double pitch)
        : Front(glm::dvec3(0.0f, 0.0f, -1.0f)), MovementSpeed(static_cast<float>(SPEED)),
          MouseSensitivity(SENSITIVITY), Zoom(ZOOM) {
    Position = glm::dvec3(posX, posY, posZ);
    WorldUp  = glm::dvec3(upX, upY, upZ);
    Yaw      = yaw;
    Pitch    = pitch;
    updateCameraVectors();
}

glm::mat4 View::Camera::GetViewMatrix() {
    return glm::lookAt(Position, Position + Front, Up);
}

void View::Camera::ProcessKeyboard(Camera_Movement direction, double deltaTime) {
    double velocity = static_cast<float>(MovementSpeed) * (deltaTime * 1000);
    if (direction == Camera_Movement::FORWARD)
        Position += Front * velocity;
    if (direction == Camera_Movement::BACKWARD)
        Position -= Front * velocity;
    if (direction == Camera_Movement::LEFT)
        Position -= Right * velocity;
    if (direction == Camera_Movement::RIGHT)
        Position += Right * velocity;
}

void View::Camera::ProcessMouseMovement(double xoffset, double yoffset,
                                  GLboolean constrainPitch) {
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw += xoffset;
    Pitch += yoffset;

    // Make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch) {
        if (Pitch > 89.0)
            Pitch = 89.0;
        if (Pitch < -89.0)
            Pitch = -89.0;
    }

    // Update Front, Right and Up Vectors using the updated Euler angles
    updateCameraVectors();
}

void View::Camera::ProcessMouseScroll(double yoffset) {
    if (Zoom >= 1.0 && Zoom <= 45.0)
        Zoom -= yoffset;
    if (Zoom <= 1.0)
        Zoom = 1.0;
    if (Zoom >= 45.0)
        Zoom = 45.0;
}

void View::Camera::updateCameraVectors() {
    // Calculate the new Front vector
    glm::dvec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front   = glm::normalize(front);
    // Also re-calculate the Right and Up vector
    Right = glm::normalize(glm::cross(
            Front, WorldUp)); // Normalize the vectors, because their length gets
    // closer to 0 the more you look up or down which results in slower movement.
    Up = glm::normalize(glm::cross(Right, Front));
}

glm::ivec2 View::Camera::getLocation() const {
    auto key = glm::ivec2(static_cast<int>(Position.x), static_cast<int>(Position.z));
    return key;
}

glm::dvec3 View::Camera::GetRightVector() {
    return glm::normalize(glm::cross(Front, WorldUp)); // Normalize the vectors, because their length gets
}
