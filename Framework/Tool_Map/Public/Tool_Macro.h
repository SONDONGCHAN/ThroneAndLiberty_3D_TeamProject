#pragma once

#define DEVICE CGameInstance::GetInstance()->GetDeviceInfo()
#define CONTEXT CGameInstance::GetInstance()->GetDeviceContextInfo()

#define MANAGER CToolMgr::GetInstance()

#define WRAPPING(CLASSNAME, ...)																\
		[](auto&&... args) -> shared_ptr<CLASSNAME>												\
		{																						\
			struct WRAPPER_STRUCT final : public CLASSNAME										\
			{																					\
				WRAPPER_STRUCT(decltype(args)&&... ctorArgs)									\
					: CLASSNAME(forward<decltype(ctorArgs)>(ctorArgs)...) { }					\
				~WRAPPER_STRUCT() { }															\
			};																					\
			return make_shared<WRAPPER_STRUCT>(forward<decltype(args)>(args)...);				\
		}

#define NO_COPY(CLASSNAME)																		\
		private:																				\
		CLASSNAME(const CLASSNAME&);															\
		CLASSNAME& operator = (const CLASSNAME&);						

#define DECLARE_SINGLETON_MAPTOOL(CLASSNAME)															\
		NO_COPY(CLASSNAME)																		\
		private:																				\
		static shared_ptr<CLASSNAME> m_pInstance;												\
		public:																					\
		static shared_ptr<CLASSNAME> GetInstance( void );										\
		static HRESULT DestroyInstance( void );

#define IMPLEMENT_SINGLETON_MAPTOOL(CLASSNAME)															\
		shared_ptr<CLASSNAME>	CLASSNAME::m_pInstance = nullptr;								\
		shared_ptr<CLASSNAME>	CLASSNAME::GetInstance( void )									\
		{																						\
			if(nullptr == m_pInstance)															\
			{																					\
				m_pInstance = WRAPPING(CLASSNAME)();											\
			}																					\
			return m_pInstance;																	\
		}																						\
		HRESULT CLASSNAME::DestroyInstance(void)												\
		{																						\
			if (m_pInstance.use_count() != 1)													\
				return E_FAIL;																	\
			m_pInstance.reset();																\
			return S_OK;																		\
		}

#define CHECK_REFERENCE_COUNT

#ifndef CHECK_REFERENCE_COUNT
#define DESTRUCTOR(CLASSNAME) virtual ~CLASSNAME() = default; // virtual ~CLASSNAME() = default
#else
#define DESTRUCTOR(CLASSNAME) virtual ~CLASSNAME(); // virtual ~CLASSNAME();
#endif // CHECK_REFERENCE_COUNT