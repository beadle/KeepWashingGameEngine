#include "KWCamera.h"

/************************************************************************/
//free camera
/************************************************************************/
KWFreeCamera::KWFreeCamera( D3DXVECTOR3 pos, D3DXVECTOR3 target, D3DXVECTOR3 up )
{
	m_pos = pos;
	//注视点target减去pos，就是前向量的方向
	m_look = target - pos;
	D3DXVec3Normalize(&m_look, &m_look);

	//上向量up叉乘前向量m_look，就是右向量的方向
	D3DXVec3Cross(&m_right, &up, &m_look);
	D3DXVec3Normalize(&m_right, &m_right);

	//矫正up向量，使之垂直于look向量与right向量构成的平面
	D3DXVec3Cross(&m_up, &m_look, &m_right);
	D3DXVec3Normalize(&m_up, &m_up);

}

KWFreeCamera* KWFreeCamera::Create( D3DXVECTOR3 pos, D3DXVECTOR3 target, D3DXVECTOR3 up )
{
	KWFreeCamera* camera = new KWFreeCamera(pos, target, up);
	camera->AutoRelease();
	return camera;
}

D3DXMATRIX KWFreeCamera::GetViewMatrix()
{
	D3DXMATRIX m;
	D3DXVec3Normalize(&m_look, &m_look);
	D3DXVec3Cross(&m_up, &m_look, &m_right);
	D3DXVec3Normalize(&m_up, &m_up);
	D3DXVec3Cross(&m_right, &m_up, &m_look);
	D3DXVec3Normalize(&m_right, &m_right);

	m._11 = m_right.x;
	m._12 = m_up.x;
	m._13 = m_look.x;
	m._14 = 0;

	m._21 = m_right.y;
	m._22 = m_up.y;
	m._23 = m_look.y;
	m._24 = 0;

	m._31 = m_right.z;
	m._32 = m_up.z;
	m._33 = m_look.z;
	m._34 = 0;

	m._41 = -D3DXVec3Dot(&m_pos, &m_right);
	m._42 = -D3DXVec3Dot(&m_pos, &m_up);
	m._43 = -D3DXVec3Dot(&m_pos, &m_look);
	m._44 = 1.0f;

	return m;
}

void KWFreeCamera::Pitch( float angle )
{
	D3DXMATRIX mat;
	D3DXMatrixRotationAxis(&mat, &m_right, angle);

	D3DXVec3TransformCoord(&m_up, &m_up, &mat);
	D3DXVec3TransformCoord(&m_look, &m_look, &mat);
}

void KWFreeCamera::Yaw( float angle )
{
	//构造绕up旋转angle弧度的旋转矩阵
	D3DXMATRIX rotMat;
	D3DXMatrixRotationAxis(&rotMat, &m_up, angle);

	//分别对right和look做旋转操作
	D3DXVec3TransformCoord(&m_right, &m_right, &rotMat);
	D3DXVec3TransformCoord(&m_look, &m_look, &rotMat);
}

void KWFreeCamera::Roll( float angle )
{
	//构造绕look旋转angle弧度的旋转矩阵
	D3DXMATRIX rotMat;
	D3DXMatrixRotationAxis(&rotMat, &m_look, angle);

	//分别对up和right做旋转操作
	D3DXVec3TransformCoord(&m_up, &m_up, &rotMat);
	D3DXVec3TransformCoord(&m_right, &m_right, &rotMat);
}

void KWFreeCamera::Strafe( float units )
{
	m_pos += m_right * units;
}

void KWFreeCamera::Fly( float units )
{
	m_pos += m_up * units;
}

void KWFreeCamera::Walk( float units )
{
	m_pos += m_look * units;
}


/************************************************************************/
//fps camera
/************************************************************************/
KWFpsCamera::KWFpsCamera(D3DXVECTOR3 pos, D3DXVECTOR3 target, float 	maxPitchAngle, float minPitchAngle)
	:m_fMaxPitchAngle(maxPitchAngle), 	m_fMinPitchAngle(minPitchAngle)
{
	m_pos = pos;
	m_angle = 0;

	//计算并单位化前进方向向量
	m_dir = target - pos;
	m_dir.y = 0;
	D3DXVec3Normalize(&m_dir, &m_dir);
}

KWFpsCamera* KWFpsCamera::Create( D3DXVECTOR3 pos, D3DXVECTOR3 target, float maxPitchAngle, float minPitchAngle )
{
	KWFpsCamera* camera = new KWFpsCamera(pos, target, maxPitchAngle, minPitchAngle);
	camera->AutoRelease();
	return camera;
}

D3DXMATRIX KWFpsCamera::GetViewMatrix()
{
	D3DXMATRIX m;
	m_dir.y = 0;
	D3DXVec3Normalize(&m_dir, &m_dir);

	D3DXVECTOR3 right, up, look; //摄像机的右、上、前向量
	D3DXVECTOR3 worldUp(0, 1.0f, 0); //地面的法向量
	D3DXVec3Cross(&right, &worldUp, &m_dir);
	D3DXVec3Normalize(&right, &right);

	D3DXMATRIX matRot;
	D3DXMatrixRotationAxis(&matRot, &right, m_angle);
	D3DXVec3TransformCoord(&look, &m_dir, &matRot);

	D3DXVec3Cross(&up, &look, &right);
	D3DXVec3Normalize(&up, &up);

	m._11 = right.x;
	m._12 = up.x;
	m._13 = look.x;
	m._14 = 0;

	m._21 = right.y;
	m._22 = up.y;
	m._23 = look.y;
	m._24 = 0;

	m._31 = right.z;
	m._32 = up.z;
	m._33 = look.z;
	m._34 = 0;

	m._41 = -D3DXVec3Dot(&m_pos, &right);
	m._42 = -D3DXVec3Dot(&m_pos, &up);
	m._43 = -D3DXVec3Dot(&m_pos, &look);
	m._44 = 1.0f;

	return m;

}

void KWFpsCamera::Pitch( float angle )
{
	m_angle += angle;

	//如果仰角超过了最大仰角限制
	if(m_angle > m_fMaxPitchAngle)
		m_angle = m_fMaxPitchAngle;

	//如果仰角小于了最小仰角限制
	if(m_angle < m_fMinPitchAngle)
		m_angle = m_fMinPitchAngle;
}

void KWFpsCamera::Yaw( float angle )
{
	D3DXMATRIX rotMat;
	D3DXMatrixRotationY(&rotMat, angle);
	D3DXVec3TransformCoord(&m_dir, &m_dir, &rotMat);
}

void KWFpsCamera::Strafe( float units )
{
	D3DXVECTOR3 right;
	D3DXVECTOR3 worldUp(0.0f, 1.0f, 0.0f);

	D3DXVec3Cross(&right, &worldUp, &m_dir);
	D3DXVec3Normalize(&right, &right);

	m_pos += right * units;

}

void KWFpsCamera::Walk( float units )
{
	m_pos += m_dir * units;
}

