#pragma once

#include "Tool_CC_Define.h"
#include "Camera.h"

class CCameraKeyFrame;

class CCamera_Direction final : public CCamera
{
public:
	CCamera_Direction();
	~CCamera_Direction();

public:
	static shared_ptr<CCamera_Direction> Create();

public:
	HRESULT Initialize();
	virtual void PriorityTick(float _fTimeDelta) override;
	virtual void Tick(float _fTimeDelta) override;
	virtual void LateTick(float _fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void StartDirection(const vector<shared_ptr<CCameraKeyFrame>>& _KeyFrames);
	void EndDirection();
	void DirectionTick(float _fTimeDelta);
	bool isDirection() { return m_isDirectioning; }

private:
	void KeyInput(float _fTimeDelta);
	void MouseMove(float _fTimeDelta);
	void CursorFixCenter();
	void CatmullRomFunc(int _i0, int _i1, int _i2, int _i3, float _fRatio);
	void LinearFunc(float _fRatio);

private:
	float m_fMouseSensitivity = 0.0f;
	bool m_isDirectioning = false;

	UINT m_iCurrentKey{ 0 };
	float m_fTimeAcc{ 0.0f };
	bool m_isFadeComplete{ false };
	bool m_isShakeComplete{ false };
	vector<shared_ptr<CCameraKeyFrame>> m_DirectionKeyFrame;
};

