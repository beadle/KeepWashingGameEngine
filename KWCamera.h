#pragma once

#include "KWObject.h"

#include <d3d9.h>
#include <d3dx9.h>

class KWCamera : public KWObject
{
protected:
	D3DXVECTOR3 m_pos;		//位置

public:
	virtual D3DXMATRIX GetViewMatrix() = 0;

	virtual void Pitch(float angle) {}	//俯仰
	virtual void Yaw(float angle) {}	//偏航
	virtual void Roll(float angle) {}	//滚动
	virtual void Strafe(float units) {}	//扫视
	virtual void Fly(float units) {}	//升降
	virtual void Walk(float units) {}	//行走

	virtual D3DXVECTOR3 GetPosition() const { return m_pos; }
	virtual void SetPosition(const D3DXVECTOR3& pos) { m_pos = pos; }
};


class KWFreeCamera : public KWCamera
{
protected:
	D3DXVECTOR3 m_right;	//右向量
	D3DXVECTOR3 m_up;		//上向量
	D3DXVECTOR3 m_look;		//前向量

public:
	KWFreeCamera(D3DXVECTOR3 pos, D3DXVECTOR3 target, D3DXVECTOR3 up);
	static KWFreeCamera* Create(D3DXVECTOR3 pos, D3DXVECTOR3 target, D3DXVECTOR3 up);

	D3DXMATRIX GetViewMatrix();

	void Pitch(float angle);
	void Yaw(float angle);
	void Roll(float angle);	
	void Strafe(float units);
	void Fly(float units);
	void Walk(float units);	

};


class KWFpsCamera : public KWCamera
{
protected:
	D3DXVECTOR3 m_dir; //行走方向
	float m_angle; //仰角

	float m_fMaxPitchAngle;  //最大仰角
	float m_fMinPitchAngle;  //最小仰角


public:
	KWFpsCamera(D3DXVECTOR3 pos, D3DXVECTOR3 target, float maxPitchAngle, float minPitchAngle);
	static KWFpsCamera* Create(D3DXVECTOR3 pos, D3DXVECTOR3 target, float maxPitchAngle, float minPitchAngle);

	D3DXMATRIX GetViewMatrix();

	void Pitch(float angle);
	void Yaw(float angle);
	void Strafe(float units);
	void Walk(float units);


};