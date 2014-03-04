#pragma once

#include <d3d9.h>
#include <d3dx9.h>

const float INFINITY = FLT_MAX;
const float EPSILON = 0.001f;

struct KWVertex
{
	float x, y, z;
	float u, v;
	KWVertex() {}
	KWVertex(float x, float y, float z, float u, float v) 
		:x(x), y(y), z(z), u(u), v(v){}
	static const DWORD FVF;
};

struct KWLine3D
{
	D3DXVECTOR3 start;
	D3DXVECTOR3 end;

	KWLine3D(const D3DXVECTOR3& s, const D3DXVECTOR3& e) :start(s), end(e) {}

	KWLine3D	operator+(const D3DXVECTOR3& point) const	{ return KWLine3D(start + point, end + point); }
	KWLine3D&	operator+=(const D3DXVECTOR3& point)		{ start += point; end += point; return *this; }
	KWLine3D	operator-(const D3DXVECTOR3& point) const	{ return KWLine3D(start - point, end - point); }
	KWLine3D&	operator-=(const D3DXVECTOR3& point)		{ start -= point; end -= point; return *this; }
	bool		operator==(const KWLine3D& other) const
	{ return (start==other.start && end==other.end) || (end==other.start && start==other.end);}
	bool		operator!=(const KWLine3D& other) const
	{ return !(start==other.start && end==other.end) || (end==other.start && start==other.end);}


	D3DXVECTOR3	GetCenter() const	{ return (start + end) / 2; }
	D3DXVECTOR3 GetVector() const	{ return end - start; }
	float		GetLength() const	{ D3DXVec3Length(&(end-start)); }
	D3DXVECTOR3 GetInterpose(const D3DXVECTOR3& p) const;
	
};

struct KWRay
{
	D3DXVECTOR3 m_origin;
	D3DXVECTOR3	m_direction;

	//calculate the picking ray from the mouse client coord
	static KWRay CalcuPickingRay(float x, float y);

	//transform the ray with matrix T
	void TransformRay(D3DXMATRIX* T);

};

struct KWRect
{
	float left, top, right, bottom;

	bool ContainPoint(const D3DXVECTOR2& point);
};

struct KWBoundingShpere
{
	D3DXVECTOR3 m_center;
	float m_radius;

	KWBoundingShpere(void);
	bool ContainPoint(const D3DXVECTOR3& point);

	//calculate wheather the ray intersect with bounding sphere
	bool IntersectWithRay(const KWRay& ray);
	
	//scaling
	void Scaling(float fac);
};

struct KWBoundingBox
{
	D3DXVECTOR3 m_min;
	D3DXVECTOR3 m_max;

	KWBoundingBox(void) 
		: m_max(D3DXVECTOR3(0, 0, 0)), m_min(D3DXVECTOR3(0, 0, 0)) {}
	KWBoundingBox(const D3DXVECTOR3& _min, const D3DXVECTOR3& _max)
		: m_max(_max), m_min(_min) {}


	void	Reset(const D3DXVECTOR3& point) { m_min = m_max = point; }

	bool	ContainPoint(const D3DXVECTOR3& point) const;
	bool	IntersectWithLine(const KWLine3D& line) const;
	bool	IntersectWithRay(const KWRay& ray) const;
	bool	IntersectWithBox(const KWBoundingBox& box) const;
	bool	IsFullInside(const KWBoundingBox& box) const;
	void	AddInternalPoint(const D3DXVECTOR3& point);
	void	AddInternalBox(const KWBoundingBox& box);
	

	float		GetWidth()	const { return m_max.x - m_min.x; }
	float		GetHeight() const { return m_max.y - m_min.y; }
	float		GetDepth()	const { return m_max.z - m_min.z; }
	D3DXVECTOR3 GetExtend() const { return m_max - m_min; }
	D3DXVECTOR3 GetCenter() const { return (m_max+m_min) / 2; } 
};

//ËÄÀâ×¶
struct KWQuadPyramid
{
	D3DXVECTOR3 Origin;
	D3DXVECTOR3 Direction;
	float		FovY;
	float		FovX;
	float		Height;

	float		Fov;
	float		CosFov;

	KWQuadPyramid(void){}

	void Set(const D3DXVECTOR3& pos, const D3DXVECTOR3& lookAt, 
		float fovY, float aspect, float height);
	bool ContainPoint(const D3DXVECTOR3& point) const;
	bool ContainBox(const KWBoundingBox& box) const;
	bool IntersectWithBox(const KWBoundingBox& box) const;
	
};