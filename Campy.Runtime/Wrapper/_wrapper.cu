#include "_BCL_.h"
#include "MetaData.h"
#include "System.Array.h"
#include "Type.h"

__device__  __host__ void __cdecl InternalInitTheBcl(void * g, size_t size, int count, void * s);

__declspec(dllexport) void InitTheBcl(void * g, size_t size, int count, void * s)
{
	InternalInitTheBcl(g, size, count, s);
}

__device__ __host__ void InternalInitFileSystem();

__declspec(dllexport) void InitFileSystem()
{
	InternalInitFileSystem();
}

__device__ __host__ void InternalGfsAddFile(void * name, void * file, size_t length, void * result);

__declspec(dllexport) void GfsAddFile(void * name, void * file, size_t length, void * result)
{
	InternalGfsAddFile(name, file, length, result);
}

__device__ __host__ void InternalInitializeBCL1();

__declspec(dllexport) void InitializeBCL1()
{
	InternalInitializeBCL1();
}

__device__ __host__ void InternalInitializeBCL2();

__declspec(dllexport) void InitializeBCL2()
{
	InternalInitializeBCL2();
}


__declspec(dllexport) void* BclHeapAlloc(void* type_def)
{
	void * result = (void*)Heap_AllocType((tMD_TypeDef *)type_def);
	return result;
}

__declspec(dllexport) void* BclArrayAlloc(void* element_type_def, int rank, unsigned int* lengths)
{
	tMD_TypeDef* array_type_def = Type_GetArrayTypeDef((tMD_TypeDef*)element_type_def, NULL, NULL);
	return (void*)SystemArray_NewVector(array_type_def, rank, lengths);
}

__declspec(dllexport) void* BclGetMetaOfType(char* assemblyName, char* nameSpace, char* name, void* nested)
{
	return (void*)MetaData_GetTypeDefFromFullNameAndNestedType(assemblyName, nameSpace, name, (tMD_TypeDef*)nested);
}

