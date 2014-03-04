#pragma once

#include <d3d9.h>
#include <d3dx9.h>

struct KWRay;
struct KWQuadPyramid;

namespace d3d
{
	//
	//	Conversion
	//
	DWORD FtoDw(float f);

	D3DXVECTOR2		ConvertToDXCoord(const D3DXVECTOR2& vec);
	KWRay			GetPickingRay(const D3DXVECTOR2& point);
	KWQuadPyramid	GetViewPyramid();

	//
	// Randomness
	//

	// Desc: Return random float in [lowBound, highBound] interval.
	float GetRandomFloat(float lowBound, float highBound);
	float GetRandom_0_1();
	float GetRandom_Minus1_1();


	// Desc: Returns a random vector in the bounds specified by min and max.
	void GetRandomVector(
		D3DXVECTOR3* out,
		D3DXVECTOR3* min,
		D3DXVECTOR3* max);

	//
	// Colors
	//
	const D3DXCOLOR      WHITE( D3DCOLOR_XRGB(255, 255, 255) );
	const D3DXCOLOR      BLACK( D3DCOLOR_XRGB(  0,   0,   0) );
	const D3DXCOLOR        RED( D3DCOLOR_XRGB(255,   0,   0) );
	const D3DXCOLOR      GREEN( D3DCOLOR_XRGB(  0, 255,   0) );
	const D3DXCOLOR       BLUE( D3DCOLOR_XRGB(  0,   0, 255) );
	const D3DXCOLOR     YELLOW( D3DCOLOR_XRGB(255, 255,   0) );
	const D3DXCOLOR       CYAN( D3DCOLOR_XRGB(  0, 255, 255) );
	const D3DXCOLOR    MAGENTA( D3DCOLOR_XRGB(255,   0, 255) );

	//
	// Materials
	//
	D3DMATERIAL9 GetMaterial(D3DXCOLOR a, D3DXCOLOR d, D3DXCOLOR s, D3DXCOLOR e, float p);

	const D3DMATERIAL9 WHITE_MTRL  = GetMaterial(WHITE, WHITE, WHITE, BLACK, 2.0f);
	const D3DMATERIAL9 RED_MTRL    = GetMaterial(RED, RED, RED, BLACK, 2.0f);
	const D3DMATERIAL9 GREEN_MTRL  = GetMaterial(GREEN, GREEN, GREEN, BLACK, 2.0f);
	const D3DMATERIAL9 BLUE_MTRL   = GetMaterial(BLUE, BLUE, BLUE, BLACK, 2.0f);
	const D3DMATERIAL9 YELLOW_MTRL = GetMaterial(YELLOW, YELLOW, YELLOW, BLACK, 2.0f);

	//
	// Lights
	//

	D3DLIGHT9 InitDirectionalLight(const D3DXVECTOR3& direction, const D3DXCOLOR& color);
	D3DLIGHT9 InitPointLight(const D3DXVECTOR3& position, const D3DXCOLOR& color);
	D3DLIGHT9 InitSpotLight(const D3DXVECTOR3& position, const D3DXVECTOR3& direction, const D3DXCOLOR& color);

	//
	// Vectors
	//
	const D3DXVECTOR3 VECTOR3DZERO = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	const D3DXVECTOR3 VECTOR2DZERO = D3DXVECTOR2(0.0f, 0.0f);

}

