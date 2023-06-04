#pragma once

#include <d3dx10math.h>

class CollisionDetection
{
public:
	void getRayDirection(int width, int height, int mouseX, int mouseY, D3DXVECTOR3 &rayPosition, D3DXVECTOR3 &rayDirection)
	{
		float pointX, pointY;
		D3DXMATRIX inverseViewMatrix, translateMatrix, inverseWorldMatrix;
		D3DXVECTOR3 direction;

		// Move the mouse cursor coordinates into the -1 to +1 range.
		pointX = ((2.0f * (float)mouseX) / (float)width) - 1.0f;
		pointY = (((2.0f * (float)mouseY) / (float)height) - 1.0f) * -1.0f;

		// Adjust the points using the projection matrix to account for the aspect ratio of the viewport.
		pointX = pointX / m_projectionMatrix._11;
		pointY = pointY / m_projectionMatrix._22;

		// Get the inverse of the view matrix.
		D3DXMatrixInverse(&inverseViewMatrix, NULL, &m_viewMatrix);

		// Calculate the direction of the picking ray in view space.
		direction.x = (pointX * inverseViewMatrix._11) + (pointY * inverseViewMatrix._21) + inverseViewMatrix._31;
		direction.y = (pointX * inverseViewMatrix._12) + (pointY * inverseViewMatrix._22) + inverseViewMatrix._32;
		direction.z = (pointX * inverseViewMatrix._13) + (pointY * inverseViewMatrix._23) + inverseViewMatrix._33;

		// Now get the inverse of the translated world matrix.
		D3DXMatrixInverse(&inverseWorldMatrix, NULL, &m_worldMatrix);

		// Now transform the ray origin and the ray direction from view space to world space.
		D3DXVec3TransformCoord(&rayPosition, &m_position, &inverseWorldMatrix);
		D3DXVec3TransformNormal(&rayDirection, &direction, &inverseWorldMatrix);

		// Normalize the ray direction.
		D3DXVec3Normalize(&rayDirection, &rayDirection);
	}

	bool RaySphereIntersect(D3DXVECTOR3 rayOrigin, D3DXVECTOR3 rayDirection, float radius)
	{
		float a, b, c, discriminant;

		// Calculate the a, b, and c coefficients.
		a = (rayDirection.x * rayDirection.x) + (rayDirection.y * rayDirection.y) + (rayDirection.z * rayDirection.z);
		b = ((rayDirection.x * rayOrigin.x) + (rayDirection.y * rayOrigin.y) + (rayDirection.z * rayOrigin.z)) * 2.0f;
		c = ((rayOrigin.x * rayOrigin.x) + (rayOrigin.y * rayOrigin.y) + (rayOrigin.z * rayOrigin.z)) - (radius * radius);

		// Find the discriminant.
		discriminant = (b * b) - (4 * a * c);

		// if discriminant is negative the picking ray missed the sphere, otherwise it intersected the sphere.
		return discriminant >= 0.0f;
	}

	bool RayBoxIntersect(D3DXVECTOR3 rayOrigin, D3DXVECTOR3 rayDirection, D3DXVECTOR3 minPlane, D3DXVECTOR3 maxPlane)
	{
		D3DXVECTOR3 dirfrac;
		// r.dir is unit direction vector of ray
		dirfrac.x = 1.0f / rayDirection.x;
		dirfrac.y = 1.0f / rayDirection.y;
		dirfrac.z = 1.0f / rayDirection.z;
		// lb is the corner of AABB with minimal coordinates - left bottom, rt is maximal corner
		// r.org is origin of ray
		float t1 = (minPlane.x - rayOrigin.x) * dirfrac.x;
		float t2 = (maxPlane.x - rayOrigin.x) * dirfrac.x;
		float t3 = (minPlane.y - rayOrigin.y) * dirfrac.y;
		float t4 = (maxPlane.y - rayOrigin.y) * dirfrac.y;
		float t5 = (minPlane.z - rayOrigin.z) * dirfrac.z;
		float t6 = (maxPlane.z - rayOrigin.z) * dirfrac.z;

		float tmin = max(max(min(t1, t2), min(t3, t4)), min(t5, t6));
		float tmax = min(min(max(t1, t2), max(t3, t4)), max(t5, t6));

		// if tmax < 0, ray (line) is intersecting AABB, but the whole AABB is behind us
		if (tmax < 0) {
			return false;
		}

		// if tmin > tmax, ray doesn't intersect AABB
		if (tmin > tmax) {
			return false;
		}

		return true;
	}

	float getDistanceByPoints(D3DXVECTOR3 point1, D3DXVECTOR3 point2)
	{
		return sqrt(pow(point2.x - point1.x, 2) + pow(point2.y - point1.y, 2) + pow(point2.z - point1.z, 2));
	}

public:
	D3DXMATRIX m_projectionMatrix;
	D3DXMATRIX m_viewMatrix;
	D3DXMATRIX m_worldMatrix;
	D3DXVECTOR3 m_position;
};