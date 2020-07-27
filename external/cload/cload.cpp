#include "cload.h"
clGetPlatformIDsPTR cload_clGetPlatformIDs;
clGetPlatformInfoPTR cload_clGetPlatformInfo;
clGetDeviceIDsPTR cload_clGetDeviceIDs;
clGetDeviceInfoPTR cload_clGetDeviceInfo;
clCreateSubDevicesPTR cload_clCreateSubDevices;
clRetainDevicePTR cload_clRetainDevice;
clReleaseDevicePTR cload_clReleaseDevice;
clSetDefaultDeviceCommandQueuePTR cload_clSetDefaultDeviceCommandQueue;
clGetDeviceAndHostTimerPTR cload_clGetDeviceAndHostTimer;
clGetHostTimerPTR cload_clGetHostTimer;
clCreateContextPTR cload_clCreateContext;
clCreateContextFromTypePTR cload_clCreateContextFromType;
clRetainContextPTR cload_clRetainContext;
clReleaseContextPTR cload_clReleaseContext;
clGetContextInfoPTR cload_clGetContextInfo;
clCreateCommandQueueWithPropertiesPTR cload_clCreateCommandQueueWithProperties;
clRetainCommandQueuePTR cload_clRetainCommandQueue;
clReleaseCommandQueuePTR cload_clReleaseCommandQueue;
clGetCommandQueueInfoPTR cload_clGetCommandQueueInfo;
clCreateBufferPTR cload_clCreateBuffer;
clCreateSubBufferPTR cload_clCreateSubBuffer;
clCreateImagePTR cload_clCreateImage;
clCreatePipePTR cload_clCreatePipe;
clRetainMemObjectPTR cload_clRetainMemObject;
clReleaseMemObjectPTR cload_clReleaseMemObject;
clGetSupportedImageFormatsPTR cload_clGetSupportedImageFormats;
clGetMemObjectInfoPTR cload_clGetMemObjectInfo;
clGetImageInfoPTR cload_clGetImageInfo;
clGetPipeInfoPTR cload_clGetPipeInfo;
clSetMemObjectDestructorCallbackPTR cload_clSetMemObjectDestructorCallback;
clSVMAllocPTR cload_clSVMAlloc;
clSVMFreePTR cload_clSVMFree;
clCreateSamplerWithPropertiesPTR cload_clCreateSamplerWithProperties;
clRetainSamplerPTR cload_clRetainSampler;
clReleaseSamplerPTR cload_clReleaseSampler;
clGetSamplerInfoPTR cload_clGetSamplerInfo;
clCreateProgramWithSourcePTR cload_clCreateProgramWithSource;
clCreateProgramWithBinaryPTR cload_clCreateProgramWithBinary;
clCreateProgramWithBuiltInKernelsPTR cload_clCreateProgramWithBuiltInKernels;
clCreateProgramWithILPTR cload_clCreateProgramWithIL;
clRetainProgramPTR cload_clRetainProgram;
clReleaseProgramPTR cload_clReleaseProgram;
clBuildProgramPTR cload_clBuildProgram;
clCompileProgramPTR cload_clCompileProgram;
clLinkProgramPTR cload_clLinkProgram;
clSetProgramReleaseCallbackPTR cload_clSetProgramReleaseCallback;
clSetProgramSpecializationConstantPTR cload_clSetProgramSpecializationConstant;
clUnloadPlatformCompilerPTR cload_clUnloadPlatformCompiler;
clGetProgramInfoPTR cload_clGetProgramInfo;
clGetProgramBuildInfoPTR cload_clGetProgramBuildInfo;
clCreateKernelPTR cload_clCreateKernel;
clCreateKernelsInProgramPTR cload_clCreateKernelsInProgram;
clCloneKernelPTR cload_clCloneKernel;
clRetainKernelPTR cload_clRetainKernel;
clReleaseKernelPTR cload_clReleaseKernel;
clSetKernelArgPTR cload_clSetKernelArg;
clSetKernelArgSVMPointerPTR cload_clSetKernelArgSVMPointer;
clSetKernelExecInfoPTR cload_clSetKernelExecInfo;
clGetKernelInfoPTR cload_clGetKernelInfo;
clGetKernelArgInfoPTR cload_clGetKernelArgInfo;
clGetKernelWorkGroupInfoPTR cload_clGetKernelWorkGroupInfo;
clGetKernelSubGroupInfoPTR cload_clGetKernelSubGroupInfo;
clWaitForEventsPTR cload_clWaitForEvents;
clGetEventInfoPTR cload_clGetEventInfo;
clCreateUserEventPTR cload_clCreateUserEvent;
clRetainEventPTR cload_clRetainEvent;
clReleaseEventPTR cload_clReleaseEvent;
clSetUserEventStatusPTR cload_clSetUserEventStatus;
clSetEventCallbackPTR cload_clSetEventCallback;
clGetEventProfilingInfoPTR cload_clGetEventProfilingInfo;
clFlushPTR cload_clFlush;
clFinishPTR cload_clFinish;
clEnqueueReadBufferPTR cload_clEnqueueReadBuffer;
clEnqueueReadBufferRectPTR cload_clEnqueueReadBufferRect;
clEnqueueWriteBufferPTR cload_clEnqueueWriteBuffer;
clEnqueueWriteBufferRectPTR cload_clEnqueueWriteBufferRect;
clEnqueueFillBufferPTR cload_clEnqueueFillBuffer;
clEnqueueCopyBufferPTR cload_clEnqueueCopyBuffer;
clEnqueueCopyBufferRectPTR cload_clEnqueueCopyBufferRect;
clEnqueueReadImagePTR cload_clEnqueueReadImage;
clEnqueueWriteImagePTR cload_clEnqueueWriteImage;
clEnqueueFillImagePTR cload_clEnqueueFillImage;
clEnqueueCopyImagePTR cload_clEnqueueCopyImage;
clEnqueueCopyImageToBufferPTR cload_clEnqueueCopyImageToBuffer;
clEnqueueCopyBufferToImagePTR cload_clEnqueueCopyBufferToImage;
clEnqueueMapBufferPTR cload_clEnqueueMapBuffer;
clEnqueueMapImagePTR cload_clEnqueueMapImage;
clEnqueueUnmapMemObjectPTR cload_clEnqueueUnmapMemObject;
clEnqueueMigrateMemObjectsPTR cload_clEnqueueMigrateMemObjects;
clEnqueueNDRangeKernelPTR cload_clEnqueueNDRangeKernel;
clEnqueueNativeKernelPTR cload_clEnqueueNativeKernel;
clEnqueueMarkerWithWaitListPTR cload_clEnqueueMarkerWithWaitList;
clEnqueueBarrierWithWaitListPTR cload_clEnqueueBarrierWithWaitList;
clEnqueueSVMFreePTR cload_clEnqueueSVMFree;
clEnqueueSVMMemcpyPTR cload_clEnqueueSVMMemcpy;
clEnqueueSVMMemFillPTR cload_clEnqueueSVMMemFill;
clEnqueueSVMMapPTR cload_clEnqueueSVMMap;
clEnqueueSVMUnmapPTR cload_clEnqueueSVMUnmap;
clEnqueueSVMMigrateMemPTR cload_clEnqueueSVMMigrateMem;
clGetExtensionFunctionAddressForPlatformPTR cload_clGetExtensionFunctionAddressForPlatform;
clCreateImage2DPTR cload_clCreateImage2D;
clCreateImage3DPTR cload_clCreateImage3D;
clEnqueueMarkerPTR cload_clEnqueueMarker;
clEnqueueWaitForEventsPTR cload_clEnqueueWaitForEvents;
clEnqueueBarrierPTR cload_clEnqueueBarrier;
clUnloadCompilerPTR cload_clUnloadCompiler;
clGetExtensionFunctionAddressPTR cload_clGetExtensionFunctionAddress;
clCreateCommandQueuePTR cload_clCreateCommandQueue;
clCreateSamplerPTR cload_clCreateSampler;
clEnqueueTaskPTR cload_clEnqueueTask;
clCreateFromGLBufferPTR cload_clCreateFromGLBuffer;
clCreateFromGLTexturePTR cload_clCreateFromGLTexture;
clCreateFromGLRenderbufferPTR cload_clCreateFromGLRenderbuffer;
clGetGLObjectInfoPTR cload_clGetGLObjectInfo;
clGetGLTextureInfoPTR cload_clGetGLTextureInfo;
clEnqueueAcquireGLObjectsPTR cload_clEnqueueAcquireGLObjects;
clEnqueueReleaseGLObjectsPTR cload_clEnqueueReleaseGLObjects;
clCreateFromGLTexture2DPTR cload_clCreateFromGLTexture2D;
clCreateFromGLTexture3DPTR cload_clCreateFromGLTexture3D;
clGetGLContextInfoKHRPTR cload_clGetGLContextInfoKHR;
#ifdef __linux__ 
#include <dlfcn.h>
using libHandle_t = void*;
#define OPEN_CLOAD_LIB dlopen ("libOpenCL.so", RTLD_LAZY)
#define CLOAD_LOAD_SYMBOL dlsym
#elif _WIN32
#include <Windows.h>
using libHandle_t = HMODULE;
#define OPEN_CLOAD_LIB LoadLibrary("opencl.dll")
#define CLOAD_LOAD_SYMBOL GetProcAddress
#else

#endif
namespace
{
	libHandle_t handle;
}
bool cloadInit()
{
	handle = OPEN_CLOAD_LIB;
	if (!handle)
		return false;
	cload_clGetPlatformIDs = (clGetPlatformIDsPTR)CLOAD_LOAD_SYMBOL(handle, "clGetPlatformIDs");
	if (!cload_clGetPlatformIDs)
		return false;
	cload_clGetPlatformInfo = (clGetPlatformInfoPTR)CLOAD_LOAD_SYMBOL(handle, "clGetPlatformInfo");
	cload_clGetDeviceIDs = (clGetDeviceIDsPTR)CLOAD_LOAD_SYMBOL(handle, "clGetDeviceIDs");
	cload_clGetDeviceInfo = (clGetDeviceInfoPTR)CLOAD_LOAD_SYMBOL(handle, "clGetDeviceInfo");
	cload_clCreateSubDevices = (clCreateSubDevicesPTR)CLOAD_LOAD_SYMBOL(handle, "clCreateSubDevices");
	cload_clRetainDevice = (clRetainDevicePTR)CLOAD_LOAD_SYMBOL(handle, "clRetainDevice");
	cload_clReleaseDevice = (clReleaseDevicePTR)CLOAD_LOAD_SYMBOL(handle, "clReleaseDevice");
	cload_clSetDefaultDeviceCommandQueue = (clSetDefaultDeviceCommandQueuePTR)CLOAD_LOAD_SYMBOL(handle, "clSetDefaultDeviceCommandQueue");
	cload_clGetDeviceAndHostTimer = (clGetDeviceAndHostTimerPTR)CLOAD_LOAD_SYMBOL(handle, "clGetDeviceAndHostTimer");
	cload_clGetHostTimer = (clGetHostTimerPTR)CLOAD_LOAD_SYMBOL(handle, "clGetHostTimer");
	cload_clCreateContext = (clCreateContextPTR)CLOAD_LOAD_SYMBOL(handle, "clCreateContext");
	cload_clCreateContextFromType = (clCreateContextFromTypePTR)CLOAD_LOAD_SYMBOL(handle, "clCreateContextFromType");
	cload_clRetainContext = (clRetainContextPTR)CLOAD_LOAD_SYMBOL(handle, "clRetainContext");
	cload_clReleaseContext = (clReleaseContextPTR)CLOAD_LOAD_SYMBOL(handle, "clReleaseContext");
	cload_clGetContextInfo = (clGetContextInfoPTR)CLOAD_LOAD_SYMBOL(handle, "clGetContextInfo");
	cload_clCreateCommandQueueWithProperties = (clCreateCommandQueueWithPropertiesPTR)CLOAD_LOAD_SYMBOL(handle, "clCreateCommandQueueWithProperties");
	cload_clRetainCommandQueue = (clRetainCommandQueuePTR)CLOAD_LOAD_SYMBOL(handle, "clRetainCommandQueue");
	cload_clReleaseCommandQueue = (clReleaseCommandQueuePTR)CLOAD_LOAD_SYMBOL(handle, "clReleaseCommandQueue");
	cload_clGetCommandQueueInfo = (clGetCommandQueueInfoPTR)CLOAD_LOAD_SYMBOL(handle, "clGetCommandQueueInfo");
	cload_clCreateBuffer = (clCreateBufferPTR)CLOAD_LOAD_SYMBOL(handle, "clCreateBuffer");
	cload_clCreateSubBuffer = (clCreateSubBufferPTR)CLOAD_LOAD_SYMBOL(handle, "clCreateSubBuffer");
	cload_clCreateImage = (clCreateImagePTR)CLOAD_LOAD_SYMBOL(handle, "clCreateImage");
	cload_clCreatePipe = (clCreatePipePTR)CLOAD_LOAD_SYMBOL(handle, "clCreatePipe");
	cload_clRetainMemObject = (clRetainMemObjectPTR)CLOAD_LOAD_SYMBOL(handle, "clRetainMemObject");
	cload_clReleaseMemObject = (clReleaseMemObjectPTR)CLOAD_LOAD_SYMBOL(handle, "clReleaseMemObject");
	cload_clGetSupportedImageFormats = (clGetSupportedImageFormatsPTR)CLOAD_LOAD_SYMBOL(handle, "clGetSupportedImageFormats");
	cload_clGetMemObjectInfo = (clGetMemObjectInfoPTR)CLOAD_LOAD_SYMBOL(handle, "clGetMemObjectInfo");
	cload_clGetImageInfo = (clGetImageInfoPTR)CLOAD_LOAD_SYMBOL(handle, "clGetImageInfo");
	cload_clGetPipeInfo = (clGetPipeInfoPTR)CLOAD_LOAD_SYMBOL(handle, "clGetPipeInfo");
	cload_clSetMemObjectDestructorCallback = (clSetMemObjectDestructorCallbackPTR)CLOAD_LOAD_SYMBOL(handle, "clSetMemObjectDestructorCallback");
	cload_clSVMAlloc = (clSVMAllocPTR)CLOAD_LOAD_SYMBOL(handle, "clSVMAlloc");
	cload_clSVMFree = (clSVMFreePTR)CLOAD_LOAD_SYMBOL(handle, "clSVMFree");
	cload_clCreateSamplerWithProperties = (clCreateSamplerWithPropertiesPTR)CLOAD_LOAD_SYMBOL(handle, "clCreateSamplerWithProperties");
	cload_clRetainSampler = (clRetainSamplerPTR)CLOAD_LOAD_SYMBOL(handle, "clRetainSampler");
	cload_clReleaseSampler = (clReleaseSamplerPTR)CLOAD_LOAD_SYMBOL(handle, "clReleaseSampler");
	cload_clGetSamplerInfo = (clGetSamplerInfoPTR)CLOAD_LOAD_SYMBOL(handle, "clGetSamplerInfo");
	cload_clCreateProgramWithSource = (clCreateProgramWithSourcePTR)CLOAD_LOAD_SYMBOL(handle, "clCreateProgramWithSource");
	cload_clCreateProgramWithBinary = (clCreateProgramWithBinaryPTR)CLOAD_LOAD_SYMBOL(handle, "clCreateProgramWithBinary");
	cload_clCreateProgramWithBuiltInKernels = (clCreateProgramWithBuiltInKernelsPTR)CLOAD_LOAD_SYMBOL(handle, "clCreateProgramWithBuiltInKernels");
	cload_clCreateProgramWithIL = (clCreateProgramWithILPTR)CLOAD_LOAD_SYMBOL(handle, "clCreateProgramWithIL");
	cload_clRetainProgram = (clRetainProgramPTR)CLOAD_LOAD_SYMBOL(handle, "clRetainProgram");
	cload_clReleaseProgram = (clReleaseProgramPTR)CLOAD_LOAD_SYMBOL(handle, "clReleaseProgram");
	cload_clBuildProgram = (clBuildProgramPTR)CLOAD_LOAD_SYMBOL(handle, "clBuildProgram");
	cload_clCompileProgram = (clCompileProgramPTR)CLOAD_LOAD_SYMBOL(handle, "clCompileProgram");
	cload_clLinkProgram = (clLinkProgramPTR)CLOAD_LOAD_SYMBOL(handle, "clLinkProgram");
	cload_clSetProgramReleaseCallback = (clSetProgramReleaseCallbackPTR)CLOAD_LOAD_SYMBOL(handle, "clSetProgramReleaseCallback");
	cload_clSetProgramSpecializationConstant = (clSetProgramSpecializationConstantPTR)CLOAD_LOAD_SYMBOL(handle, "clSetProgramSpecializationConstant");
	cload_clUnloadPlatformCompiler = (clUnloadPlatformCompilerPTR)CLOAD_LOAD_SYMBOL(handle, "clUnloadPlatformCompiler");
	cload_clGetProgramInfo = (clGetProgramInfoPTR)CLOAD_LOAD_SYMBOL(handle, "clGetProgramInfo");
	cload_clGetProgramBuildInfo = (clGetProgramBuildInfoPTR)CLOAD_LOAD_SYMBOL(handle, "clGetProgramBuildInfo");
	cload_clCreateKernel = (clCreateKernelPTR)CLOAD_LOAD_SYMBOL(handle, "clCreateKernel");
	cload_clCreateKernelsInProgram = (clCreateKernelsInProgramPTR)CLOAD_LOAD_SYMBOL(handle, "clCreateKernelsInProgram");
	cload_clCloneKernel = (clCloneKernelPTR)CLOAD_LOAD_SYMBOL(handle, "clCloneKernel");
	cload_clRetainKernel = (clRetainKernelPTR)CLOAD_LOAD_SYMBOL(handle, "clRetainKernel");
	cload_clReleaseKernel = (clReleaseKernelPTR)CLOAD_LOAD_SYMBOL(handle, "clReleaseKernel");
	cload_clSetKernelArg = (clSetKernelArgPTR)CLOAD_LOAD_SYMBOL(handle, "clSetKernelArg");
	cload_clSetKernelArgSVMPointer = (clSetKernelArgSVMPointerPTR)CLOAD_LOAD_SYMBOL(handle, "clSetKernelArgSVMPointer");
	cload_clSetKernelExecInfo = (clSetKernelExecInfoPTR)CLOAD_LOAD_SYMBOL(handle, "clSetKernelExecInfo");
	cload_clGetKernelInfo = (clGetKernelInfoPTR)CLOAD_LOAD_SYMBOL(handle, "clGetKernelInfo");
	cload_clGetKernelArgInfo = (clGetKernelArgInfoPTR)CLOAD_LOAD_SYMBOL(handle, "clGetKernelArgInfo");
	cload_clGetKernelWorkGroupInfo = (clGetKernelWorkGroupInfoPTR)CLOAD_LOAD_SYMBOL(handle, "clGetKernelWorkGroupInfo");
	cload_clGetKernelSubGroupInfo = (clGetKernelSubGroupInfoPTR)CLOAD_LOAD_SYMBOL(handle, "clGetKernelSubGroupInfo");
	cload_clWaitForEvents = (clWaitForEventsPTR)CLOAD_LOAD_SYMBOL(handle, "clWaitForEvents");
	cload_clGetEventInfo = (clGetEventInfoPTR)CLOAD_LOAD_SYMBOL(handle, "clGetEventInfo");
	cload_clCreateUserEvent = (clCreateUserEventPTR)CLOAD_LOAD_SYMBOL(handle, "clCreateUserEvent");
	cload_clRetainEvent = (clRetainEventPTR)CLOAD_LOAD_SYMBOL(handle, "clRetainEvent");
	cload_clReleaseEvent = (clReleaseEventPTR)CLOAD_LOAD_SYMBOL(handle, "clReleaseEvent");
	cload_clSetUserEventStatus = (clSetUserEventStatusPTR)CLOAD_LOAD_SYMBOL(handle, "clSetUserEventStatus");
	cload_clSetEventCallback = (clSetEventCallbackPTR)CLOAD_LOAD_SYMBOL(handle, "clSetEventCallback");
	cload_clGetEventProfilingInfo = (clGetEventProfilingInfoPTR)CLOAD_LOAD_SYMBOL(handle, "clGetEventProfilingInfo");
	cload_clFlush = (clFlushPTR)CLOAD_LOAD_SYMBOL(handle, "clFlush");
	cload_clFinish = (clFinishPTR)CLOAD_LOAD_SYMBOL(handle, "clFinish");
	cload_clEnqueueReadBuffer = (clEnqueueReadBufferPTR)CLOAD_LOAD_SYMBOL(handle, "clEnqueueReadBuffer");
	cload_clEnqueueReadBufferRect = (clEnqueueReadBufferRectPTR)CLOAD_LOAD_SYMBOL(handle, "clEnqueueReadBufferRect");
	cload_clEnqueueWriteBuffer = (clEnqueueWriteBufferPTR)CLOAD_LOAD_SYMBOL(handle, "clEnqueueWriteBuffer");
	cload_clEnqueueWriteBufferRect = (clEnqueueWriteBufferRectPTR)CLOAD_LOAD_SYMBOL(handle, "clEnqueueWriteBufferRect");
	cload_clEnqueueFillBuffer = (clEnqueueFillBufferPTR)CLOAD_LOAD_SYMBOL(handle, "clEnqueueFillBuffer");
	cload_clEnqueueCopyBuffer = (clEnqueueCopyBufferPTR)CLOAD_LOAD_SYMBOL(handle, "clEnqueueCopyBuffer");
	cload_clEnqueueCopyBufferRect = (clEnqueueCopyBufferRectPTR)CLOAD_LOAD_SYMBOL(handle, "clEnqueueCopyBufferRect");
	cload_clEnqueueReadImage = (clEnqueueReadImagePTR)CLOAD_LOAD_SYMBOL(handle, "clEnqueueReadImage");
	cload_clEnqueueWriteImage = (clEnqueueWriteImagePTR)CLOAD_LOAD_SYMBOL(handle, "clEnqueueWriteImage");
	cload_clEnqueueFillImage = (clEnqueueFillImagePTR)CLOAD_LOAD_SYMBOL(handle, "clEnqueueFillImage");
	cload_clEnqueueCopyImage = (clEnqueueCopyImagePTR)CLOAD_LOAD_SYMBOL(handle, "clEnqueueCopyImage");
	cload_clEnqueueCopyImageToBuffer = (clEnqueueCopyImageToBufferPTR)CLOAD_LOAD_SYMBOL(handle, "clEnqueueCopyImageToBuffer");
	cload_clEnqueueCopyBufferToImage = (clEnqueueCopyBufferToImagePTR)CLOAD_LOAD_SYMBOL(handle, "clEnqueueCopyBufferToImage");
	cload_clEnqueueMapBuffer = (clEnqueueMapBufferPTR)CLOAD_LOAD_SYMBOL(handle, "clEnqueueMapBuffer");
	cload_clEnqueueMapImage = (clEnqueueMapImagePTR)CLOAD_LOAD_SYMBOL(handle, "clEnqueueMapImage");
	cload_clEnqueueUnmapMemObject = (clEnqueueUnmapMemObjectPTR)CLOAD_LOAD_SYMBOL(handle, "clEnqueueUnmapMemObject");
	cload_clEnqueueMigrateMemObjects = (clEnqueueMigrateMemObjectsPTR)CLOAD_LOAD_SYMBOL(handle, "clEnqueueMigrateMemObjects");
	cload_clEnqueueNDRangeKernel = (clEnqueueNDRangeKernelPTR)CLOAD_LOAD_SYMBOL(handle, "clEnqueueNDRangeKernel");
	cload_clEnqueueNativeKernel = (clEnqueueNativeKernelPTR)CLOAD_LOAD_SYMBOL(handle, "clEnqueueNativeKernel");
	cload_clEnqueueMarkerWithWaitList = (clEnqueueMarkerWithWaitListPTR)CLOAD_LOAD_SYMBOL(handle, "clEnqueueMarkerWithWaitList");
	cload_clEnqueueBarrierWithWaitList = (clEnqueueBarrierWithWaitListPTR)CLOAD_LOAD_SYMBOL(handle, "clEnqueueBarrierWithWaitList");
	cload_clEnqueueSVMFree = (clEnqueueSVMFreePTR)CLOAD_LOAD_SYMBOL(handle, "clEnqueueSVMFree");
	cload_clEnqueueSVMMemcpy = (clEnqueueSVMMemcpyPTR)CLOAD_LOAD_SYMBOL(handle, "clEnqueueSVMMemcpy");
	cload_clEnqueueSVMMemFill = (clEnqueueSVMMemFillPTR)CLOAD_LOAD_SYMBOL(handle, "clEnqueueSVMMemFill");
	cload_clEnqueueSVMMap = (clEnqueueSVMMapPTR)CLOAD_LOAD_SYMBOL(handle, "clEnqueueSVMMap");
	cload_clEnqueueSVMUnmap = (clEnqueueSVMUnmapPTR)CLOAD_LOAD_SYMBOL(handle, "clEnqueueSVMUnmap");
	cload_clEnqueueSVMMigrateMem = (clEnqueueSVMMigrateMemPTR)CLOAD_LOAD_SYMBOL(handle, "clEnqueueSVMMigrateMem");
	cload_clGetExtensionFunctionAddressForPlatform = (clGetExtensionFunctionAddressForPlatformPTR)CLOAD_LOAD_SYMBOL(handle, "clGetExtensionFunctionAddressForPlatform");
	cload_clCreateImage2D = (clCreateImage2DPTR)CLOAD_LOAD_SYMBOL(handle, "clCreateImage2D");
	cload_clCreateImage3D = (clCreateImage3DPTR)CLOAD_LOAD_SYMBOL(handle, "clCreateImage3D");
	cload_clEnqueueMarker = (clEnqueueMarkerPTR)CLOAD_LOAD_SYMBOL(handle, "clEnqueueMarker");
	cload_clEnqueueWaitForEvents = (clEnqueueWaitForEventsPTR)CLOAD_LOAD_SYMBOL(handle, "clEnqueueWaitForEvents");
	cload_clEnqueueBarrier = (clEnqueueBarrierPTR)CLOAD_LOAD_SYMBOL(handle, "clEnqueueBarrier");
	cload_clUnloadCompiler = (clUnloadCompilerPTR)CLOAD_LOAD_SYMBOL(handle, "clUnloadCompiler");
	cload_clGetExtensionFunctionAddress = (clGetExtensionFunctionAddressPTR)CLOAD_LOAD_SYMBOL(handle, "clGetExtensionFunctionAddress");
	cload_clCreateCommandQueue = (clCreateCommandQueuePTR)CLOAD_LOAD_SYMBOL(handle, "clCreateCommandQueue");
	cload_clCreateSampler = (clCreateSamplerPTR)CLOAD_LOAD_SYMBOL(handle, "clCreateSampler");
	cload_clEnqueueTask = (clEnqueueTaskPTR)CLOAD_LOAD_SYMBOL(handle, "clEnqueueTask");
	cload_clCreateFromGLBuffer = (clCreateFromGLBufferPTR)CLOAD_LOAD_SYMBOL(handle, "clCreateFromGLBuffer");
	cload_clCreateFromGLTexture = (clCreateFromGLTexturePTR)CLOAD_LOAD_SYMBOL(handle, "clCreateFromGLTexture");
	cload_clCreateFromGLRenderbuffer = (clCreateFromGLRenderbufferPTR)CLOAD_LOAD_SYMBOL(handle, "clCreateFromGLRenderbuffer");
	cload_clGetGLObjectInfo = (clGetGLObjectInfoPTR)CLOAD_LOAD_SYMBOL(handle, "clGetGLObjectInfo");
	cload_clGetGLTextureInfo = (clGetGLTextureInfoPTR)CLOAD_LOAD_SYMBOL(handle, "clGetGLTextureInfo");
	cload_clEnqueueAcquireGLObjects = (clEnqueueAcquireGLObjectsPTR)CLOAD_LOAD_SYMBOL(handle, "clEnqueueAcquireGLObjects");
	cload_clEnqueueReleaseGLObjects = (clEnqueueReleaseGLObjectsPTR)CLOAD_LOAD_SYMBOL(handle, "clEnqueueReleaseGLObjects");
	cload_clCreateFromGLTexture2D = (clCreateFromGLTexture2DPTR)CLOAD_LOAD_SYMBOL(handle, "clCreateFromGLTexture2D");
	cload_clCreateFromGLTexture3D = (clCreateFromGLTexture3DPTR)CLOAD_LOAD_SYMBOL(handle, "clCreateFromGLTexture3D");
	//Is not exported
	//RESOLVe other extension function might not be exported too, so switch to clGetExtensionFunctionAddress()
	cload_clGetGLContextInfoKHR = (clGetGLContextInfoKHRPTR)clGetExtensionFunctionAddress("clGetGLContextInfoKHR");
	return true;
}
void cloadTerminate()
{
	FreeLibrary(handle);
}