#ifdef XPROFILE_EXPORTS
#define XPROFILE_API __declspec(dllexport)
#else
#define XPROFILE_API __declspec(dllimport)
#endif



extern XPROFILE_API void _cyg_profile_func_enter(void* pfn_this, void* pCaller);
extern XPROFILE_API void _cyg_profile_func_exit(void* pfn_this);


//extern "C" XPROFILE_API void _stdcall _CAP_Start_Profiling(void* pCaller, void* pCallee);
//extern "C" XPROFILE_API void _stdcall _CAP_End_Profiling(void* pCallee);

extern "C" XPROFILE_API void _stdcall _CAP_Start_Profiling(void* pCaller, void* pCallee);
extern "C" XPROFILE_API void _stdcall _CAP_End_Profiling(void* pCallee);
