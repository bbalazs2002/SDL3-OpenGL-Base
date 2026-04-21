#include "CameraManipulator.h"

#include "Camera.h"

#include <SDL3/SDL.h>

CameraManipulator::CameraManipulator()
{
}

CameraManipulator::~CameraManipulator()
{
}

void CameraManipulator::SetCamera( Camera* _pCamera )
{
    m_pCamera = _pCamera;

    if ( !m_pCamera ) return;

    // Set the initial spherical coordinates.
    m_center = m_pCamera->GetAt();
    glm::vec3 ToAim = m_center - m_pCamera->GetEye();

    m_distance = glm::length( ToAim );

    m_u = atan2f( ToAim.z, ToAim.x );
    m_v = acosf( ToAim.y / m_distance );

}

void CameraManipulator::Update(float _deltaTime) {
	if (!m_pCamera) return;

	// Update the camera based on the model parameters.

	// Calculate the new view direction based on spherical coordinates.
	glm::vec3 lookDirection(cosf(m_u) * sinf(m_v),
		cosf(m_v),
		sinf(m_u) * sinf(m_v));

	// Calculate the new camera position based on the view direction and distance.
	glm::vec3 eye = m_center - m_distance * lookDirection;

	// Set the new up vector to be the same as the world's up vector.
	glm::vec3 up = m_pCamera->GetWorldUp();

	// Calculate the new right vector from the cross product of the view direction and the up vector.
	glm::vec3 right = glm::normalize(glm::cross(lookDirection, up));

	// Calculate the new forward vector from the cross product of the up and right vectors.
	glm::vec3 forward = glm::cross(up, right);

	// Calculate the movement offset based on the camera's movement direction and speed.
	glm::vec3 deltaPosition = (m_goForward * forward + m_goRight * right + m_goUp * up) * m_speed * _deltaTime;

	// Update the camera position and the view target position.
	eye += deltaPosition;
	m_center += deltaPosition;

	// Update the camera with the new position and look-at target.
	m_pCamera->SetView(eye, m_center, m_pCamera->GetWorldUp());
}


void CameraManipulator::KeyboardDown(const SDL_KeyboardEvent& key)
{
	switch ( key.key )
	{
	case SDLK_LSHIFT:
	case SDLK_RSHIFT:
		if ( key.repeat == 0 ) m_speed /= 4.0f;
		break;
	case SDLK_W:
		m_goForward = 1;
		break;
	case SDLK_S:
		m_goForward = -1;
		break;
	case SDLK_A:
		m_goRight = -1;
		break;
	case SDLK_D:
		m_goRight = 1;
		break;
	case SDLK_E:
		m_goUp = 1;
		break;
	case SDLK_Q:
		m_goUp = -1;
		break;
	}
}

void CameraManipulator::KeyboardUp(const SDL_KeyboardEvent& key)
{
	
	switch ( key.key )
	{
	case SDLK_LSHIFT:
	case SDLK_RSHIFT:
		m_speed *= 4.0f;
		break;
	case SDLK_W:
	case SDLK_S:
		m_goForward = 0;
		break;
	case SDLK_A:
	case SDLK_D:
		m_goRight = 0;
		break;
	case SDLK_Q:
	case SDLK_E:
		m_goUp = 0;
		break;
	}
}


void CameraManipulator::MouseMove(const SDL_MouseMotionEvent& mouse)
{
	if ( mouse.state & SDL_BUTTON_LMASK )
	{
		float du = mouse.xrel / 100.0f;
		float dv = mouse.yrel / 100.0f;

		m_u += du;
		m_v = glm::clamp<float>( m_v + dv, 0.1f, 3.1f );
	}
	if ( mouse.state & SDL_BUTTON_RMASK )
	{
		float dDistance = mouse.yrel / 100.0f;
		m_distance += dDistance;
	}
}

void CameraManipulator::MouseWheel(const SDL_MouseWheelEvent& wheel)
{
	float dDistance = static_cast<float>( wheel.y ) * m_speed / -100.0f;
	m_distance += dDistance;
}