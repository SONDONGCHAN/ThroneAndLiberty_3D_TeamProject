#ifndef Engine_Function_h__
#define Engine_Function_h__

namespace Engine
{	
	template<typename T>
	void Safe_Delete(T& pPointer)
	{
		if (nullptr != pPointer)
		{
			delete pPointer;
			pPointer = nullptr;
		}
	}

	template<typename T>
	void Safe_Delete_Array(T& pPointer)
	{
		if (nullptr != pPointer)
		{
			delete [] pPointer;
			pPointer = nullptr;
		}
	}

	template<typename T>
	struct SharedEnabler : public T 
	{
		SharedEnabler() : T() {}
	};


	inline short ScaleFloatToShort(float _Value, float _Min, float _Max)
	{
		return short((_Value - _Min) / (_Max - _Min) * SHRT_MAX);
	}

	inline float ScaleShortToFloat(short _Value, float _Min, float _Max)
	{
		return float(_Value) / SHRT_MAX * (_Max - _Min) + _Min;
	}
	
	inline float BtwA2B(float _Value, float _A, float _B)
	{
		if (_A <= _B)
			return _Value >= _A ? (_Value <= _B ? _Value : _B) : _A;
		else
			return _Value >= _B ? (_Value <= _A ? _Value : _A) : _B;
	}

	// 선형보간
	template<typename T>
	inline T LinearIp(T _Start, T _End, float _fRatio)
	{
		return _fRatio <= 0.0f ? _Start : (_fRatio >= 1.0f ? _End : _Start + (_End - _Start) * _fRatio);
	}

	// 코사인보간
	template<typename T>
	inline T CosIp(T _Start, T _End, float _fRatio)
	{
		float Theta = _fRatio <= 0.0f ? 0.0f : (_fRatio >= 1.0f ? 3.141592654f : _fRatio * 3.141592654f);
		float ComputeRatio = (1 - cos(Theta)) * 0.5f;
		return _Start + (_End - _Start) * ComputeRatio;
	}

	/* Input float between 0.0f to 1.0f or more */
	inline bool GetRandomChance(float _fPercent)
	{
		return (_fPercent - ((float)rand() / (float)RAND_MAX)) >= 0.0f;
	}

}

#endif // Engine_Function_h__
