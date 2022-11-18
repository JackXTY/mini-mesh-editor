#include "camera.h"

#include <la.h>
#include <iostream>


Camera::Camera():
    Camera(400, 400)
{}

Camera::Camera(unsigned int w, unsigned int h):
    Camera(w, h, glm::vec3(0,0,0), glm::vec3(0,0,0), glm::vec3(0,1,0))
{}

Camera::Camera(unsigned int w, unsigned int h, const glm::vec3 &e, const glm::vec3 &r, const glm::vec3 &worldUp):
    fovy(45),
    width(w),
    height(h),
    near_clip(0.1f),
    far_clip(1000),
    eye(e),
    ref(r),
    world_up(worldUp),
    theta(0),
    phi(0),
    zoom(10),
    eyeBasis(e)
{
    RecomputeAttributes();
}

Camera::Camera(const Camera &c):
    fovy(c.fovy),
    width(c.width),
    height(c.height),
    near_clip(c.near_clip),
    far_clip(c.far_clip),
    aspect(c.aspect),
    eye(c.eye),
    ref(c.ref),
    look(c.look),
    up(c.up),
    right(c.right),
    world_up(c.world_up),
    V(c.V),
    H(c.H),
    theta(c.theta),
    phi(c.phi),
    zoom(c.zoom),
    eyeBasis(c.eyeBasis)
{}


void Camera::RecomputeAttributes()
{
    //rotate(theta, y_axis) * rotate(phi, x_axis) * translate(zoom, z_axis)
    glm::mat4 translation = glm::translate(glm::mat4(), glm::vec3(0, 0, zoom));
    glm::mat4 rotation = glm::rotate(glm::mat4(), phi, glm::vec3(1, 0, 0));
    rotation = glm::rotate(rotation, theta, glm::vec3(0, 1, 0));

    glm::mat4 t = rotation * translation;

    eye = glm::vec3(t * glm::vec4(0, 0, 0, 1)) + eyeBasis;
    look = -glm::vec3(t * glm::vec4(0, 0, 1, 0));
    up = glm::vec3(t * glm::vec4(0, 1, 0, 0));
    right = glm::vec3(t * glm::vec4(1, 0, 0, 0));

    float tan_fovy = tan(glm::radians(fovy/2));
    float len = glm::length(ref - eye);
    aspect = width / static_cast<float>(height);
    V = up*len*tan_fovy;
    H = right*len*aspect*tan_fovy;
}

glm::mat4 Camera::getViewProj()
{
    return glm::perspective(glm::radians(fovy), width / (float)height, near_clip, far_clip) * glm::lookAt(eye, ref, up);
}

void Camera::RotateAboutUp(float deg)
{
    phi += glm::radians(deg);
    RecomputeAttributes();
}
void Camera::RotateAboutRight(float deg)
{
    theta += glm::radians(deg);
    RecomputeAttributes();
}

void Camera::TranslateAlongLook(float amt)
{
    glm::vec3 translation = look * amt;
    eyeBasis += translation;
    ref += translation;
}

void Camera::TranslateAlongRight(float amt)
{
    glm::vec3 translation = right * amt;
    eyeBasis += translation;
    ref += translation;
}
void Camera::TranslateAlongUp(float amt)
{
    glm::vec3 translation = up * amt;
    eyeBasis += translation;
    ref += translation;
}
