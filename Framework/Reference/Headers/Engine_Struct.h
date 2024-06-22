#ifndef Engine_Struct_h__
#define Engine_Struct_h__

#include "Engine_Macro.h"
#include "Engine_Typedef.h"

namespace Engine
{
	typedef struct 
	{	
		HWND			hWnd;
		unsigned int	iWinSizeX, iWinSizeY;
		bool			isWindowed;
	} GRAPHIC_DESC;


	typedef struct
	{
		enum TYPE { TYPE_DIRECTIONAL, TYPE_POINT, TYPE_END };

		TYPE			eType;
		_float4		vDirection;
		_float4		vPosition;
		float			fRange;
		_float4		vDiffuse;
		_float4		vAmbient;
		_float4		vSpecular;
		_bool		bUsingNoise;
		_float		fCullDistance;
	
	}LIGHT_DESC;

	typedef struct ENGINE_DLL VTXPOS
	{
		_float3		vPosition;

		static const unsigned int	iNumElements = { 1 };
		static const D3D11_INPUT_ELEMENT_DESC		Elements[1];
	}VTXPOS;

	/* hlsl File Tag : RC */
	typedef struct ENGINE_DLL VTXPOSTEX
	{
		_float3 vPositon;
		_float2 vTexcoord;

		static const unsigned int iNumElements = { 2 };
		static const D3D11_INPUT_ELEMENT_DESC Elements[2];

	}VTXPOSTEX;

	typedef struct ENGINE_DLL VTXNORTEX
	{
		_float3		vPosition;
		_float3		vNormal;
		_float2		vTexcoord;

		static const unsigned int	iNumElements = { 3 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[3];
	}VTXNORTEX;

	typedef struct ENGINE_DLL VTXMESH
	{
		_float3		vPosition;
		_float3		vNormal;
		_float2		vTexcoord;
		_float3		vTangent;

		static const unsigned int	iNumElements = { 4 };
		static const D3D11_INPUT_ELEMENT_DESC		Elements[4];
	}VTXMESH;

	typedef struct ENGINE_DLL VTXANIMMESH
	{
		_float3		vPosition;
		_float3		vNormal;
		_float2		vTexcoord;
		_float3		vTangent;

		XMUINT4		vBlendIndices;
		_float4		vBlendWeights;

		static const unsigned int	iNumElements = { 6 };
		static const D3D11_INPUT_ELEMENT_DESC		Elements[6];
	}VTXANIMMESH;

	/* hlsl File Tag : Point */
	typedef struct ENGINE_DLL VTXPOINT
	{
		_float3		vPosition;
		_float2		vPSize;

		static const unsigned int	iNumElements = { 2 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[2];
	}VTXPOINT;

	typedef struct ENGINE_DLL VTXRECT_INSTANCE
	{
		static const unsigned int	iNumElements = { 7 };
		static const D3D11_INPUT_ELEMENT_DESC		Elements[7];
	}VTX_RECT_INSTANCE;

	typedef struct ENGINE_DLL VTXPOINT_INSTANCE
	{
		static const unsigned int	iNumElements = { 7 };
		static const D3D11_INPUT_ELEMENT_DESC		Elements[7];
	}VTX_POINT_INSTANCE;

	typedef struct ENGINE_DLL VTXINSTANCE
	{
		_float4	vRight, vUp, vLook, vTranslation;
		_float4	vColor;

	}VTXINSTANCE;

	typedef struct ENGINE_DLL VTXPARTICLE
	{
		_float4	vRight, vUp, vLook, vTranslation;
		_float4	vColor;
		_float4	vPivot;

	}VTXPARTICLE;

	typedef struct ENGINE_DLL VTXPARTICLE_INSTANCE
	{
		static const unsigned int	iNumElements = { 8 };
		static const D3D11_INPUT_ELEMENT_DESC		Elements[8];
	}VTXPARTICLE_INSTANCE;

	typedef struct ENGINE_DLL VTXCUBE
	{
		_float3		vPosition;
		_float3		vTexcoord;

		static const unsigned int	iNumElements = { 2 };
		static const D3D11_INPUT_ELEMENT_DESC		Elements[2];
	}VTXCUBE;

	/*메쉬 인스턴싱*/
	typedef struct ENGINE_DLL VTXVERTEXINSTANCE
	{
		_float4	vRight, vUp, vLook, vTranslation;

	}VTXVERTEX_INSTANCE;

	typedef struct ENGINE_DLL VTX_MESH_INSTANCE
	{
		static const unsigned int	iNumElements = { 8 };
		static const D3D11_INPUT_ELEMENT_DESC		Elements[8];
	}VTXMESH_INSTANCE;
	/***********/

	/* UI 인스턴싱 */
	typedef struct ENGINE_DLL VTXUI
	{
		_float4	vRight, vUp, vLook, vTranslation;
		_float4	vColor;

		_uint iSlotUV{}; // 기존의 slotUV
		_float2 vUVRatio; // 두번째 수가 인벤토리 slot관련 Ratio
		_uint iTexIndex{};

	}VTXUI;

	typedef struct ENGINE_DLL VTXUI_INSTANCE
	{
		static const unsigned int iNumElements = { 10 };
		static const D3D11_INPUT_ELEMENT_DESC		Elements[10];
	}VTXUIINSTANCE;

	struct PARTICLE
	{
		_bool       bActive = false;

		_float      fLifeTime = 0.f, fStartLifeTime = 0.f;

		_float3     vPosition = { 0.f, 0.f, 0.f };
		_float3     vDirection = { 0.f, 0.f, 0.f };

		_float4     vColor = { 1.f, 1.f, 1.f, 1.f }, vCurColor = { 1.f, 1.f, 1.f, 1.f }, vNextColor = { 1.f, 1.f, 1.f, 1.f };

		_int        iColorIdx = 0, iSpeedIdx = 0, iScaleIdx = 0;

		_float3     vSpeed = { 0.f, 0.f, 0.f }, vCurSpeed = { 0.f, 0.f, 0.f }, vNextSpeed = { 0.f, 0.f, 0.f };
		_float3     vSpeedAcc = { 0.f, 0.f, 0.f };

		_float3     vScale = { 1.f, 1.f, 1.f }, vCurScale = { 1.f, 1.f, 1.f }, vNextScale = { 1.f, 1.f, 1.f };

		_float3		vRotation = { 0.f, 0.f, 0.f };
		_float3     vTurnVel = { 0.f, 0.f, 0.f }, vTurnAcc = { 0.f, 0.f, 0.f };

		_float		fHazeTime = 0.f;
		_float3		vHazeSpeed = { 0.f, 0.f, 0.f };
		
		_bool		vUsePivot = false;
		_float4		vPivot = { 0.f, 0.f, 0.f, 0.f };

		_float4x4	OwnerMat = SimpleMath::Matrix::Identity;
	};

	struct ENGINE_DLL InstanceUI
	{
		_float3			vCenter{};
		_float2			vSize{};
		_float2			vSpeed{};
		_float2			vLifeTime{};
		_bool			isLoop{ false };
		_float4			vColor{};
		_float			fDuration{ 0.f };

		//char			strTexTag[256] = "";
	};

	struct ENGINE_DLL ROT_DESC
	{
		_float3	vRotAmplitude;
		_float3 vRotFrequency;
		_float2	vBlendInOut = { 0.0f,0.0f };
		_float2 vInOutWeight = { 1.0f,1.0f };
		ESHAKEFUNC eShakeFunc = SHAKE_WAVE;
		bool	isOffSetRand = true;
	};
	struct ENGINE_DLL LOC_DESC
	{
		_float3	vLocAmplitude;
		_float3	vLocFrequency;
		_float2	vBlendInOut = { 0.0f,0.0f };
		_float2 vInOutWeight = { 1.0f,1.0f };
		ESHAKEFUNC eShakeFunc = SHAKE_WAVE;
		bool	isOffSetRand = true;
	};
	struct ENGINE_DLL FOV_DESC
	{
		float	fFovyAmplitude;
		float	fFovyFrequency;
		_float2 vBlendInOut = { 0.0f,0.0f };
		_float2 vInOutWeight = { 1.0f,1.0f };
		ESHAKEFUNC eShakeFunc = SHAKE_WAVE;
	};
}

#endif // Engine_Struct_h__
