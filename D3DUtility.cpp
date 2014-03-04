#include "D3DUtility.h"
#include "KWEngine.h"
#include "KWGeometry.h"


DWORD d3d::FtoDw( float f )
{
	return *((DWORD*)&f);
}

D3DXVECTOR2 d3d::ConvertToDXCoord( const D3DXVECTOR2& vec )
{
	return D3DXVECTOR2(vec.x, KWEngine::sharedInstance()->getScreenHeight() - vec.y);
}

float d3d::GetRandomFloat( float lowBound, float highBound )
{
	if( lowBound >= highBound ) // bad input
		return lowBound;

	// get random float in [0, 1] interval
	float f = (rand() % 10000) * 0.0001f; 

	// return float in [lowBound, highBound] interval. 
	return (f * (highBound - lowBound)) + lowBound; 
}

float d3d::GetRandom_0_1()
{
	return (float)rand() / RAND_MAX;
}

float d3d::GetRandom_Minus1_1()
{
	return GetRandomFloat(-1.0f, 1.0f);
}

void d3d::GetRandomVector( D3DXVECTOR3* out, D3DXVECTOR3* min, D3DXVECTOR3* max )
{
	out->x = GetRandomFloat(min->x, max->x);
	out->y = GetRandomFloat(min->y, max->y);
	out->z = GetRandomFloat(min->z, max->z);
}

KWRay d3d::GetPickingRay( const D3DXVECTOR2& point )
{
	KWRay ray = KWRay::CalcuPickingRay(point.x, point.y);

	D3DXMATRIX matView;
	KWEngine::sharedInstance()->getDevice()->GetTransform(D3DTS_VIEW, &matView);

	D3DXMatrixInverse(&matView, 0, &matView);
	ray.TransformRay(&matView);

	return ray;
}


D3DMATERIAL9 d3d::GetMaterial( D3DXCOLOR a, D3DXCOLOR d, D3DXCOLOR s, D3DXCOLOR e, float p )
{
	D3DMATERIAL9 mtrl;
	mtrl.Ambient  = a;
	mtrl.Diffuse  = d;
	mtrl.Specular = s;
	mtrl.Emissive = e;
	mtrl.Power    = p;
	return mtrl;
}



D3DLIGHT9 d3d::InitDirectionalLight(const D3DXVECTOR3& direction, const D3DXCOLOR& color)
{
	D3DLIGHT9 light;
	::ZeroMemory(&light, sizeof(light));

	light.Type      = D3DLIGHT_DIRECTIONAL;
	light.Ambient   = color * 0.6f;
	light.Diffuse   = color;
	light.Specular  = color * 0.6f;
	light.Direction = direction;

	return light;
}

D3DLIGHT9 d3d::InitPointLight(const D3DXVECTOR3& position, const D3DXCOLOR& color)
{
	D3DLIGHT9 light;
	::ZeroMemory(&light, sizeof(light));

	light.Type      = D3DLIGHT_POINT;
	light.Ambient   = color * 0.6f;
	light.Diffuse   = color;
	light.Specular  = color * 0.6f;
	light.Position  = position;
	light.Range        = 1000.0f;
	light.Falloff      = 1.0f;
	light.Attenuation0 = 1.0f;
	light.Attenuation1 = 0.0f;
	light.Attenuation2 = 0.0f;

	return light;
}

D3DLIGHT9 d3d::InitSpotLight(const D3DXVECTOR3& position, const D3DXVECTOR3& direction, const D3DXCOLOR& color)
{
	D3DLIGHT9 light;
	::ZeroMemory(&light, sizeof(light));

	light.Type      = D3DLIGHT_SPOT;
	light.Ambient   = color * 0.0f;
	light.Diffuse   = color;
	light.Specular  = color * 0.6f;
	light.Position  = position;
	light.Direction = direction;
	light.Range        = 1000.0f;
	light.Falloff      = 1.0f;
	light.Attenuation0 = 1.0f;
	light.Attenuation1 = 0.0f;
	light.Attenuation2 = 0.0f;
	light.Theta        = 0.4f;
	light.Phi          = 0.9f;

	return light;
}

KWQuadPyramid d3d::GetViewPyramid()
{
	KWEngine* engine = KWEngine::sharedInstance();
	KWRay pickingRay = d3d::GetPickingRay(engine->GetWindowSize() / 2);
	KWQuadPyramid pyramid;
	pyramid.Set(pickingRay.m_origin, 
		pickingRay.m_direction,
		D3DX_PI/4.0f,
		1.0f,
		1024.0f);
	return pyramid;
}

