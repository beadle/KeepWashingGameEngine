#include "KWGeometry.h"
#include "KWEngine.h"

const DWORD KWVertex::FVF = D3DFVF_XYZ | D3DFVF_TEX1;


bool KWRect::ContainPoint( const D3DXVECTOR2& point )
{
	if (point.x >= left && point.x <= right &&
		point.y <= bottom && point.y >= top)
	{
		return true;
	}
	return false;
}

KWBoundingShpere::KWBoundingShpere( void )
{
	m_center = D3DXVECTOR3(0, 0, 0);
	m_radius = 0.0f;
}

bool KWBoundingShpere::ContainPoint( const D3DXVECTOR3& point )
{
	float dist = D3DXVec3Length(&(point-m_center));
	return dist <= m_radius;
}

bool KWBoundingShpere::IntersectWithRay( const KWRay& ray )
{
	D3DXVECTOR3 v = ray.m_origin - m_center;

	float b = 2.0f * D3DXVec3Dot(&ray.m_direction, &v);
	float c = D3DXVec3Dot(&v, &v) - (m_radius * m_radius);

	// find the discriminant
	float discriminant = (b * b) - (4.0f * c);

	// test for imaginary number
	if( discriminant < 0.0f )
		return false;

	discriminant = sqrtf(discriminant);

	float s0 = (-b + discriminant) / 2.0f;
	float s1 = (-b - discriminant) / 2.0f;

	// if a solution is >= 0, then we intersected the sphere
	if( s0 >= 0.0f || s1 >= 0.0f )
		return true;

	return false;
}

void KWBoundingShpere::Scaling( float fac )
{
	m_radius *= fac;
}


//ray struct
KWRay KWRay::CalcuPickingRay( float x, float y )
{
	float px = 0, py = 0;

	LPDIRECT3DDEVICE9 pDevice = KWEngine::sharedInstance()->getDevice();
	D3DVIEWPORT9 vp;
	pDevice->GetViewport(&vp);

	D3DXMATRIX proj;
	pDevice->GetTransform(D3DTS_PROJECTION, &proj);

	px = (((2.0f * x) / vp.Width) - 1.0f) / proj(0, 0);
	py = (((2.0f * y) / vp.Height) - 1.0f) / proj(1, 1);

	KWRay ray;
	ray.m_origin = D3DXVECTOR3(0, 0, 0);
	ray.m_direction = D3DXVECTOR3(px, py, 1);

	return ray;
}

void KWRay::TransformRay( D3DXMATRIX* T )
{
	D3DXVec3TransformCoord(&m_origin, &m_origin, T);

	D3DXVec3TransformNormal(&m_direction, &m_direction, T);

	D3DXVec3Normalize(&m_direction, &m_direction);
}



bool KWBoundingBox::ContainPoint( const D3DXVECTOR3& point ) const
{
	if (point.x >= m_min.x && point.x <= m_max.x
		&& point.y >= m_min.y && point.y <= m_max.y
		&& point.z >= m_min.z && point.z <= m_max.z) 
	{
		return true;
	}
	return false;
}

bool KWBoundingBox::IntersectWithBox( const KWBoundingBox& box ) const
{
	return ContainPoint(box.m_max) ||
		ContainPoint(box.m_min) ||
		box.ContainPoint(m_min) ||
		box.ContainPoint(m_max);
}

bool KWBoundingBox::IntersectWithLine( const KWLine3D& line ) const
{
	D3DXVECTOR3 interpose = line.GetInterpose(this->GetCenter());
	D3DXVECTOR3 ToInterpose = interpose - m_min;
	D3DXVECTOR3 ToMax = GetExtend();
	return ToInterpose.x <= ToMax.x && ToInterpose.x >= 0 &&
		ToInterpose.y <= ToMax.y && ToInterpose.y >= 0 &&
		ToInterpose.z <= ToMax.z && ToInterpose.z >= 0;
}

bool KWBoundingBox::IntersectWithRay( const KWRay& ray ) const
{
	KWLine3D line(ray.m_origin, ray.m_origin+ray.m_direction*2000.0f);
	return IntersectWithLine(line);
}

bool KWBoundingBox::IsFullInside( const KWBoundingBox& box ) const
{
	return m_min.x >= box.m_min.x &&
		m_min.y >= box.m_min.y &&
		m_min.z >= box.m_min.z &&
		m_max.x <= box.m_max.x &&
		m_max.y <= box.m_max.y &&
		m_max.z <= box.m_max.z;
}

void KWBoundingBox::AddInternalPoint( const D3DXVECTOR3& point )
{
	if (point.x>m_max.x) m_max.x = point.x;
	if (point.y>m_max.y) m_max.y = point.y;
	if (point.z>m_max.z) m_max.z = point.z;

	if (point.x<m_min.x) m_min.x = point.x;
	if (point.y<m_min.y) m_min.y = point.y;
	if (point.z<m_min.z) m_min.z = point.z;
}

void KWBoundingBox::AddInternalBox( const KWBoundingBox& box )
{
	AddInternalPoint(box.m_min);
	AddInternalPoint(box.m_max);
}








D3DXVECTOR3 KWLine3D::GetInterpose( const D3DXVECTOR3& p ) const
{
	D3DXVECTOR3 middle = GetCenter();
	D3DXVECTOR3 lineVect = GetVector();
	D3DXVec3Normalize(&lineVect, &lineVect);
	D3DXVECTOR3 mToO = p - middle;
	float len = D3DXVec3Dot(&mToO, &lineVect);
	return middle + lineVect * len;
}


//quad pyramid
void KWQuadPyramid::Set( const D3DXVECTOR3& pos, const D3DXVECTOR3& lookAt, float fovY, float aspect, float height )
{
	Origin = pos;
	Direction = lookAt;
	D3DXVec3Normalize(&Direction, &Direction);
	FovY = fovY;
	FovX = fovY * aspect;
	Fov = max(FovX, FovY);
	CosFov = cosf(Fov);
	Height = height;
}

bool KWQuadPyramid::ContainPoint( const D3DXVECTOR3& point ) const
{
	D3DXVECTOR3 toPoint = point - Origin;
	float toLength = D3DXVec3Length(&toPoint);
	D3DXVec3Normalize(&toPoint, &toPoint);
	float dot = D3DXVec3Dot(&toPoint, &Direction);
	if (dot <= CosFov) return false;
	if (toLength * dot >= Height) return false;
	return true;
}

bool KWQuadPyramid::ContainBox( const KWBoundingBox& box ) const
{
	D3DXVECTOR3 ep = box.m_min;
	if (!ContainPoint(ep)) return false;
	ep.x = box.m_max.x;
	if (!ContainPoint(ep)) return false;
	ep.z = box.m_max.y;
	if (!ContainPoint(ep)) return false;
	ep.x = box.m_min.x;
	if (!ContainPoint(ep)) return false;
	ep = box.m_min;
	ep.y = box.m_max.y;
	if (!ContainPoint(ep)) return false;
	ep.x = box.m_max.x;
	if (!ContainPoint(ep)) return false;
	ep.z = box.m_max.z;
	if (!ContainPoint(ep)) return false;
	ep.x = box.m_min.x;
	if (!ContainPoint(ep)) return false;
	return true;
}

bool KWQuadPyramid::IntersectWithBox( const KWBoundingBox& box ) const
{
	D3DXVECTOR3 ep = box.m_min;
	if (ContainPoint(ep)) return true;
	ep.x = box.m_max.x;
	if (ContainPoint(ep)) return true;
	ep.z = box.m_max.y;
	if (ContainPoint(ep)) return true;
	ep.x = box.m_min.x;
	if (ContainPoint(ep)) return true;
	ep = box.m_min;
	ep.y = box.m_max.y;
	if (ContainPoint(ep)) return true;
	ep.x = box.m_max.x;
	if (ContainPoint(ep)) return true;
	ep.z = box.m_max.z;
	if (ContainPoint(ep)) return true;
	ep.x = box.m_min.x;
	if (ContainPoint(ep)) return true;
	return false;
}
