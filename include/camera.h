#ifndef __cplusplus
  #error "C++ is required"
#endif

#ifndef CAMERA_H
#define CAMERA_H

#define GLM_FORCE_RADIANS 
#define GLM_FORCE_DEPTH_ZERO_TO_ONE 
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <algorithm>

namespace tr {

using float2   = glm::vec2;
using float3   = glm::vec3;
using float4   = glm::vec4;
using float2x2 = glm::mat2x2;
using float2x3 = glm::mat2x3;
using float2x4 = glm::mat2x4;
using float3x2 = glm::mat3x2;
using float3x3 = glm::mat3x3;
using float3x4 = glm::mat3x4;
using float4x2 = glm::mat4x2;
using float4x3 = glm::mat4x3;
using float4x4 = glm::mat4x4;

class Camera {
public:
  Camera() {}
  
  Camera(const float3 eye, const float3& look_at, const float3& up, float fov_degrees, float aspect, float near_clip = 0.1f, float far_clip = 10000.0f) {
    LookAt(eye, look_at, up);
    Perspective(fov_degrees, aspect, near_clip, far_clip);
    MouseMove(0, 0);
  }

  ~Camera() {}

  void LookAt(const float3& eye, const float3& look_at, const float3& up = float3(0, 1, 0)) {
    m_eye_position = eye;
    m_look_at = look_at;
    m_up = up;
    m_view_direction = glm::normalize(look_at - m_eye_position);
    m_view_dirty = true;
    m_view_projection_dirty = true;
  }

  void Perspective(float fov_degrees, float aspect, float near_clip = 0.1f, float far_clip = 10000.0f) {
    m_fov_degrees = fov_degrees;
    m_aspect = aspect;
    m_near_clip = near_clip;
    m_far_clip = far_clip;
    m_view_dirty = true;
    m_view_projection_dirty = true;
  }

  const float3& GetEyePosition() const {
    return m_eye_position;
  }

  const float3& GetViewDirection() const { 
    return m_view_direction; 
  }

  const float4x4& GetViewMatrix() const {
    if (m_view_dirty) {
      float3 axis = glm::cross(m_view_direction, m_up);
      glm::quat pitch_quat = glm::angleAxis(m_pitch, axis);
      glm::quat heading_quat = glm::angleAxis(m_heading, m_up);
      glm::quat q = glm::cross(pitch_quat, heading_quat);
      q = glm::normalize(q);
      m_view_direction = glm::rotate(q, m_view_direction);
      m_look_at = m_eye_position + m_view_direction;
      m_view_matrix = glm::lookAt(m_eye_position, m_look_at, m_up);
      m_view_dirty = false;


      //m_view_matrix = glm::lookAt(m_eye_position, m_look_at, m_up);
      //m_view_dirty = false;
    }
    return m_view_matrix;
  }

  const float4x4& GetProjectionMatrix() const {
    if (m_projection_dirty) {
      m_projection_matrix = glm::perspective(glm::radians(m_fov_degrees), m_aspect, m_near_clip, m_far_clip);
      m_projection_dirty = false;
    }
    return m_projection_matrix;
  }

  const float4x4& GetViewProjectionMatrix() const {
    if (m_view_projection_dirty) {
      const float4x4& view = GetViewMatrix();
      const float4x4& proj = GetProjectionMatrix();
      m_view_projection_matrix = proj * view;
      m_view_projection_dirty = false;
    }
    return m_view_projection_matrix;
  }

  void MouseMove(float dx, float dy) {
    const float k_mouse_sensitivity_x = 0.002f;
    const float k_mouse_sensitivity_y = 0.002f;
    m_heading += -k_mouse_sensitivity_x * dx;
    m_pitch   += -k_mouse_sensitivity_y * dy;

  const float kPi = 3.14159265359f;
  const float kHalfPi = kPi /2.0f;
  m_pitch = std::max(-kHalfPi, std::min(kHalfPi, m_pitch));

    m_view_dirty = true;
  }

private:
  bool              m_pixel_aligned         = false;
  float3            m_eye_position          = float3(0, 0, 1);
  mutable float3    m_look_at               = float3(0, 0, 0);
  float3            m_up                    = float3(0, 1, 0);
  float             m_fov_degrees           = 60.0f;
  float             m_aspect                = 1.0f;
  float             m_near_clip             = 0.1f;
  float             m_far_clip              = 10000.0f;
  mutable float3    m_view_direction        = float3(0, 0, -1);
  mutable bool      m_view_dirty            = true;
  mutable bool      m_projection_dirty      = true;
  mutable bool      m_view_projection_dirty = true;
  mutable float4x4  m_view_matrix;
  mutable float4x4  m_projection_matrix;
  mutable float4x4  m_view_projection_matrix;

  mutable float     m_pitch = 0;
  mutable float     m_heading = 0;
};

} // namespace tr

#endif // CAMERA_H