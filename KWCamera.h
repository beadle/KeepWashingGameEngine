#pragma once

#include "KWObject.h"

#include <d3d9.h>
#include <d3dx9.h>

class KWCamera : public KWObject
{
protected:
	D3DXVECTOR3 m_pos;		//λ��

public:
	virtual D3DXMATRIX GetViewMatrix() = 0;

	virtual void Pitch(float angle) {}	//����
	virtual void Yaw(float angle) {}	//ƫ��
	virtual void Roll(float angle) {}	//����
	virtual void Strafe(float units) {}	//ɨ��
	virtual void Fly(float units) {}	//����
	virtual void Walk(float units) {}	//����

	virtual D3DXVECTOR3 GetPosition() const { return m_pos; }
	virtual void SetPosition(const D3DXVECTOR3& pos) { m_pos = pos; }
};


class KWFreeCamera : public KWCamera
{
protected:
	D3DXVECTOR3 m_right;	//������
	D3DXVECTOR3 m_up;		//������
	D3DXVECTOR3 m_look;		//ǰ����

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
	D3DXVECTOR3 m_dir; //���߷���
	float m_angle; //����

	float m_fMaxPitchAngle;  //�������
	float m_fMinPitchAngle;  //��С����


public:
	KWFpsCamera(D3DXVECTOR3 pos, D3DXVECTOR3 target, float maxPitchAngle, float minPitchAngle);
	static KWFpsCamera* Create(D3DXVECTOR3 pos, D3DXVECTOR3 target, float maxPitchAngle, float minPitchAngle);

	D3DXMATRIX GetViewMatrix();

	void Pitch(float angle);
	void Yaw(float angle);
	void Strafe(float units);
	void Walk(float units);


};