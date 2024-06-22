#ifndef Engine_Typedef_h__
#define Engine_Typedef_h__

#include <DirectXTK/SimpleMath.h>

namespace Engine
{
	typedef		bool						_bool;

	typedef		signed char					_byte;
	typedef		unsigned char				_ubyte;
	typedef		char						_char;

	typedef		wchar_t						_tchar;
	
	typedef		signed short				_short;
	typedef		unsigned short				_ushort;	

	typedef		signed int					_int;
	typedef		unsigned int				_uint;

	typedef		signed long					_long;
	typedef		unsigned long				_ulong;

	typedef		float						_float;
	typedef		double						_double;
	
	/*SimpleMath Wrapping*/
	typedef		SimpleMath::Vector2			_float2;
	typedef		SimpleMath::Vector3			_float3;
	typedef		SimpleMath::Vector4			_float4;

	typedef		SimpleMath::Matrix			_float4x4;

	typedef		SimpleMath::Quaternion		_quaternion;
	typedef		SimpleMath::Color			_color;
	typedef		SimpleMath::Plane			_plane;
	
	typedef		SimpleMath::Rectangle		_rect;
	typedef		SimpleMath::Ray				_ray;
	
	typedef		SimpleMath::Viewport		_viewPort;
	
}

#endif // Engine_Typedef_h__
