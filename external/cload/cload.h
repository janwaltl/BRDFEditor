/*
This file contains modified versions of cl.h and cl_gl.h from https://www.khronos.org/registry/
These modifications allow to load the OpenCL library at runtime directly from driver similary as glad does.

*/

/*******************************************************************************
* Copyright (c) 2008-2015 The Khronos Group Inc.
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and/or associated documentation files (the
* "Materials"), to deal in the Materials without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Materials, and to
* permit persons to whom the Materials are furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Materials.
*
* MODIFICATIONS TO THIS FILE MAY MEAN IT NO LONGER ACCURATELY REFLECTS
* KHRONOS STANDARDS. THE UNMODIFIED, NORMATIVE VERSIONS OF KHRONOS
* SPECIFICATIONS AND HEADER INFORMATION ARE LOCATED AT
*    https://www.khronos.org/registry/
*
* THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.
******************************************************************************/

#ifndef __OPENCL_CL_H
#define __OPENCL_CL_H

#ifdef __APPLE__
#include <OpenCL/cl_platform.h>
#else
#include "cl_platform.h"
#endif	

#ifdef __cplusplus
extern "C" {
#endif

	/******************************************************************************/

	typedef struct _cl_platform_id *    cl_platform_id;
	typedef struct _cl_device_id *      cl_device_id;
	typedef struct _cl_context *        cl_context;
	typedef struct _cl_command_queue *  cl_command_queue;
	typedef struct _cl_mem *            cl_mem;
	typedef struct _cl_program *        cl_program;
	typedef struct _cl_kernel *         cl_kernel;
	typedef struct _cl_event *          cl_event;
	typedef struct _cl_sampler *        cl_sampler;

	typedef cl_uint             cl_bool;                     /* WARNING!  Unlike cl_ types in cl_platform.h, cl_bool is not guaranteed to be the same size as the bool in kernels. */
	typedef cl_ulong            cl_bitfield;
	typedef cl_bitfield         cl_device_type;
	typedef cl_uint             cl_platform_info;
	typedef cl_uint             cl_device_info;
	typedef cl_bitfield         cl_device_fp_config;
	typedef cl_uint             cl_device_mem_cache_type;
	typedef cl_uint             cl_device_local_mem_type;
	typedef cl_bitfield         cl_device_exec_capabilities;
	typedef cl_bitfield         cl_device_svm_capabilities;
	typedef cl_bitfield         cl_command_queue_properties;
	typedef intptr_t            cl_device_partition_property;
	typedef cl_bitfield         cl_device_affinity_domain;

	typedef intptr_t            cl_context_properties;
	typedef cl_uint             cl_context_info;
	typedef cl_bitfield         cl_queue_properties;
	typedef cl_uint             cl_command_queue_info;
	typedef cl_uint             cl_channel_order;
	typedef cl_uint             cl_channel_type;
	typedef cl_bitfield         cl_mem_flags;
	typedef cl_bitfield         cl_svm_mem_flags;
	typedef cl_uint             cl_mem_object_type;
	typedef cl_uint             cl_mem_info;
	typedef cl_bitfield         cl_mem_migration_flags;
	typedef cl_uint             cl_image_info;
	typedef cl_uint             cl_buffer_create_type;
	typedef cl_uint             cl_addressing_mode;
	typedef cl_uint             cl_filter_mode;
	typedef cl_uint             cl_sampler_info;
	typedef cl_bitfield         cl_map_flags;
	typedef intptr_t            cl_pipe_properties;
	typedef cl_uint             cl_pipe_info;
	typedef cl_uint             cl_program_info;
	typedef cl_uint             cl_program_build_info;
	typedef cl_uint             cl_program_binary_type;
	typedef cl_int              cl_build_status;
	typedef cl_uint             cl_kernel_info;
	typedef cl_uint             cl_kernel_arg_info;
	typedef cl_uint             cl_kernel_arg_address_qualifier;
	typedef cl_uint             cl_kernel_arg_access_qualifier;
	typedef cl_bitfield         cl_kernel_arg_type_qualifier;
	typedef cl_uint             cl_kernel_work_group_info;
	typedef cl_uint             cl_kernel_sub_group_info;
	typedef cl_uint             cl_event_info;
	typedef cl_uint             cl_command_type;
	typedef cl_uint             cl_profiling_info;
	typedef cl_bitfield         cl_sampler_properties;
	typedef cl_uint             cl_kernel_exec_info;

	typedef struct _cl_image_format {
		cl_channel_order        image_channel_order;
		cl_channel_type         image_channel_data_type;
	} cl_image_format;

	typedef struct _cl_image_desc {
		cl_mem_object_type      image_type;
		size_t                  image_width;
		size_t                  image_height;
		size_t                  image_depth;
		size_t                  image_array_size;
		size_t                  image_row_pitch;
		size_t                  image_slice_pitch;
		cl_uint                 num_mip_levels;
		cl_uint                 num_samples;
#ifdef __GNUC__
		__extension__   /* Prevents warnings about anonymous union in -pedantic builds */
#endif
			union {
			cl_mem                  buffer;
			cl_mem                  mem_object;
		};
	} cl_image_desc;

	typedef struct _cl_buffer_region {
		size_t                  origin;
		size_t                  size;
	} cl_buffer_region;


	/******************************************************************************/

	/* Error Codes */
#define CL_SUCCESS                                  0
#define CL_DEVICE_NOT_FOUND                         -1
#define CL_DEVICE_NOT_AVAILABLE                     -2
#define CL_COMPILER_NOT_AVAILABLE                   -3
#define CL_MEM_OBJECT_ALLOCATION_FAILURE            -4
#define CL_OUT_OF_RESOURCES                         -5
#define CL_OUT_OF_HOST_MEMORY                       -6
#define CL_PROFILING_INFO_NOT_AVAILABLE             -7
#define CL_MEM_COPY_OVERLAP                         -8
#define CL_IMAGE_FORMAT_MISMATCH                    -9
#define CL_IMAGE_FORMAT_NOT_SUPPORTED               -10
#define CL_BUILD_PROGRAM_FAILURE                    -11
#define CL_MAP_FAILURE                              -12
#define CL_MISALIGNED_SUB_BUFFER_OFFSET             -13
#define CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST -14
#define CL_COMPILE_PROGRAM_FAILURE                  -15
#define CL_LINKER_NOT_AVAILABLE                     -16
#define CL_LINK_PROGRAM_FAILURE                     -17
#define CL_DEVICE_PARTITION_FAILED                  -18
#define CL_KERNEL_ARG_INFO_NOT_AVAILABLE            -19

#define CL_INVALID_VALUE                            -30
#define CL_INVALID_DEVICE_TYPE                      -31
#define CL_INVALID_PLATFORM                         -32
#define CL_INVALID_DEVICE                           -33
#define CL_INVALID_CONTEXT                          -34
#define CL_INVALID_QUEUE_PROPERTIES                 -35
#define CL_INVALID_COMMAND_QUEUE                    -36
#define CL_INVALID_HOST_PTR                         -37
#define CL_INVALID_MEM_OBJECT                       -38
#define CL_INVALID_IMAGE_FORMAT_DESCRIPTOR          -39
#define CL_INVALID_IMAGE_SIZE                       -40
#define CL_INVALID_SAMPLER                          -41
#define CL_INVALID_BINARY                           -42
#define CL_INVALID_BUILD_OPTIONS                    -43
#define CL_INVALID_PROGRAM                          -44
#define CL_INVALID_PROGRAM_EXECUTABLE               -45
#define CL_INVALID_KERNEL_NAME                      -46
#define CL_INVALID_KERNEL_DEFINITION                -47
#define CL_INVALID_KERNEL                           -48
#define CL_INVALID_ARG_INDEX                        -49
#define CL_INVALID_ARG_VALUE                        -50
#define CL_INVALID_ARG_SIZE                         -51
#define CL_INVALID_KERNEL_ARGS                      -52
#define CL_INVALID_WORK_DIMENSION                   -53
#define CL_INVALID_WORK_GROUP_SIZE                  -54
#define CL_INVALID_WORK_ITEM_SIZE                   -55
#define CL_INVALID_GLOBAL_OFFSET                    -56
#define CL_INVALID_EVENT_WAIT_LIST                  -57
#define CL_INVALID_EVENT                            -58
#define CL_INVALID_OPERATION                        -59
#define CL_INVALID_GL_OBJECT                        -60
#define CL_INVALID_BUFFER_SIZE                      -61
#define CL_INVALID_MIP_LEVEL                        -62
#define CL_INVALID_GLOBAL_WORK_SIZE                 -63
#define CL_INVALID_PROPERTY                         -64
#define CL_INVALID_IMAGE_DESCRIPTOR                 -65
#define CL_INVALID_COMPILER_OPTIONS                 -66
#define CL_INVALID_LINKER_OPTIONS                   -67
#define CL_INVALID_DEVICE_PARTITION_COUNT           -68
#define CL_INVALID_PIPE_SIZE                        -69
#define CL_INVALID_DEVICE_QUEUE                     -70
#define CL_INVALID_SPEC_ID                          -71
#define CL_MAX_SIZE_RESTRICTION_EXCEEDED            -72

	/* OpenCL Version */
#define CL_VERSION_1_0                              1
#define CL_VERSION_1_1                              1
#define CL_VERSION_1_2                              1
#define CL_VERSION_2_0                              1
#define CL_VERSION_2_1                              1
#define CL_VERSION_2_2                              1

	/* cl_bool */
#define CL_FALSE                                    0
#define CL_TRUE                                     1
#define CL_BLOCKING                                 CL_TRUE
#define CL_NON_BLOCKING                             CL_FALSE

	/* cl_platform_info */
#define CL_PLATFORM_PROFILE                         0x0900
#define CL_PLATFORM_VERSION                         0x0901
#define CL_PLATFORM_NAME                            0x0902
#define CL_PLATFORM_VENDOR                          0x0903
#define CL_PLATFORM_EXTENSIONS                      0x0904
#define CL_PLATFORM_HOST_TIMER_RESOLUTION           0x0905

	/* cl_device_type - bitfield */
#define CL_DEVICE_TYPE_DEFAULT                      (1 << 0)
#define CL_DEVICE_TYPE_CPU                          (1 << 1)
#define CL_DEVICE_TYPE_GPU                          (1 << 2)
#define CL_DEVICE_TYPE_ACCELERATOR                  (1 << 3)
#define CL_DEVICE_TYPE_CUSTOM                       (1 << 4)
#define CL_DEVICE_TYPE_ALL                          0xFFFFFFFF

	/* cl_device_info */
#define CL_DEVICE_TYPE                                   0x1000
#define CL_DEVICE_VENDOR_ID                              0x1001
#define CL_DEVICE_MAX_COMPUTE_UNITS                      0x1002
#define CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS               0x1003
#define CL_DEVICE_MAX_WORK_GROUP_SIZE                    0x1004
#define CL_DEVICE_MAX_WORK_ITEM_SIZES                    0x1005
#define CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR            0x1006
#define CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT           0x1007
#define CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT             0x1008
#define CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG            0x1009
#define CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT           0x100A
#define CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE          0x100B
#define CL_DEVICE_MAX_CLOCK_FREQUENCY                    0x100C
#define CL_DEVICE_ADDRESS_BITS                           0x100D
#define CL_DEVICE_MAX_READ_IMAGE_ARGS                    0x100E
#define CL_DEVICE_MAX_WRITE_IMAGE_ARGS                   0x100F
#define CL_DEVICE_MAX_MEM_ALLOC_SIZE                     0x1010
#define CL_DEVICE_IMAGE2D_MAX_WIDTH                      0x1011
#define CL_DEVICE_IMAGE2D_MAX_HEIGHT                     0x1012
#define CL_DEVICE_IMAGE3D_MAX_WIDTH                      0x1013
#define CL_DEVICE_IMAGE3D_MAX_HEIGHT                     0x1014
#define CL_DEVICE_IMAGE3D_MAX_DEPTH                      0x1015
#define CL_DEVICE_IMAGE_SUPPORT                          0x1016
#define CL_DEVICE_MAX_PARAMETER_SIZE                     0x1017
#define CL_DEVICE_MAX_SAMPLERS                           0x1018
#define CL_DEVICE_MEM_BASE_ADDR_ALIGN                    0x1019
#define CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE               0x101A
#define CL_DEVICE_SINGLE_FP_CONFIG                       0x101B
#define CL_DEVICE_GLOBAL_MEM_CACHE_TYPE                  0x101C
#define CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE              0x101D
#define CL_DEVICE_GLOBAL_MEM_CACHE_SIZE                  0x101E
#define CL_DEVICE_GLOBAL_MEM_SIZE                        0x101F
#define CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE               0x1020
#define CL_DEVICE_MAX_CONSTANT_ARGS                      0x1021
#define CL_DEVICE_LOCAL_MEM_TYPE                         0x1022
#define CL_DEVICE_LOCAL_MEM_SIZE                         0x1023
#define CL_DEVICE_ERROR_CORRECTION_SUPPORT               0x1024
#define CL_DEVICE_PROFILING_TIMER_RESOLUTION             0x1025
#define CL_DEVICE_ENDIAN_LITTLE                          0x1026
#define CL_DEVICE_AVAILABLE                              0x1027
#define CL_DEVICE_COMPILER_AVAILABLE                     0x1028
#define CL_DEVICE_EXECUTION_CAPABILITIES                 0x1029
#define CL_DEVICE_QUEUE_PROPERTIES                       0x102A    /* deprecated */
#define CL_DEVICE_QUEUE_ON_HOST_PROPERTIES               0x102A
#define CL_DEVICE_NAME                                   0x102B
#define CL_DEVICE_VENDOR                                 0x102C
#define CL_DRIVER_VERSION                                0x102D
#define CL_DEVICE_PROFILE                                0x102E
#define CL_DEVICE_VERSION                                0x102F
#define CL_DEVICE_EXTENSIONS                             0x1030
#define CL_DEVICE_PLATFORM                               0x1031
#define CL_DEVICE_DOUBLE_FP_CONFIG                       0x1032
#define CL_DEVICE_HALF_FP_CONFIG                         0x1033
#define CL_DEVICE_PREFERRED_VECTOR_WIDTH_HALF            0x1034
#define CL_DEVICE_HOST_UNIFIED_MEMORY                    0x1035   /* deprecated */
#define CL_DEVICE_NATIVE_VECTOR_WIDTH_CHAR               0x1036
#define CL_DEVICE_NATIVE_VECTOR_WIDTH_SHORT              0x1037
#define CL_DEVICE_NATIVE_VECTOR_WIDTH_INT                0x1038
#define CL_DEVICE_NATIVE_VECTOR_WIDTH_LONG               0x1039
#define CL_DEVICE_NATIVE_VECTOR_WIDTH_FLOAT              0x103A
#define CL_DEVICE_NATIVE_VECTOR_WIDTH_DOUBLE             0x103B
#define CL_DEVICE_NATIVE_VECTOR_WIDTH_HALF               0x103C
#define CL_DEVICE_OPENCL_C_VERSION                       0x103D
#define CL_DEVICE_LINKER_AVAILABLE                       0x103E
#define CL_DEVICE_BUILT_IN_KERNELS                       0x103F
#define CL_DEVICE_IMAGE_MAX_BUFFER_SIZE                  0x1040
#define CL_DEVICE_IMAGE_MAX_ARRAY_SIZE                   0x1041
#define CL_DEVICE_PARENT_DEVICE                          0x1042
#define CL_DEVICE_PARTITION_MAX_SUB_DEVICES              0x1043
#define CL_DEVICE_PARTITION_PROPERTIES                   0x1044
#define CL_DEVICE_PARTITION_AFFINITY_DOMAIN              0x1045
#define CL_DEVICE_PARTITION_TYPE                         0x1046
#define CL_DEVICE_REFERENCE_COUNT                        0x1047
#define CL_DEVICE_PREFERRED_INTEROP_USER_SYNC            0x1048
#define CL_DEVICE_PRINTF_BUFFER_SIZE                     0x1049
#define CL_DEVICE_IMAGE_PITCH_ALIGNMENT                  0x104A
#define CL_DEVICE_IMAGE_BASE_ADDRESS_ALIGNMENT           0x104B
#define CL_DEVICE_MAX_READ_WRITE_IMAGE_ARGS              0x104C
#define CL_DEVICE_MAX_GLOBAL_VARIABLE_SIZE               0x104D
#define CL_DEVICE_QUEUE_ON_DEVICE_PROPERTIES             0x104E
#define CL_DEVICE_QUEUE_ON_DEVICE_PREFERRED_SIZE         0x104F
#define CL_DEVICE_QUEUE_ON_DEVICE_MAX_SIZE               0x1050
#define CL_DEVICE_MAX_ON_DEVICE_QUEUES                   0x1051
#define CL_DEVICE_MAX_ON_DEVICE_EVENTS                   0x1052
#define CL_DEVICE_SVM_CAPABILITIES                       0x1053
#define CL_DEVICE_GLOBAL_VARIABLE_PREFERRED_TOTAL_SIZE   0x1054
#define CL_DEVICE_MAX_PIPE_ARGS                          0x1055
#define CL_DEVICE_PIPE_MAX_ACTIVE_RESERVATIONS           0x1056
#define CL_DEVICE_PIPE_MAX_PACKET_SIZE                   0x1057
#define CL_DEVICE_PREFERRED_PLATFORM_ATOMIC_ALIGNMENT    0x1058
#define CL_DEVICE_PREFERRED_GLOBAL_ATOMIC_ALIGNMENT      0x1059
#define CL_DEVICE_PREFERRED_LOCAL_ATOMIC_ALIGNMENT       0x105A
#define CL_DEVICE_IL_VERSION                             0x105B
#define CL_DEVICE_MAX_NUM_SUB_GROUPS                     0x105C
#define CL_DEVICE_SUB_GROUP_INDEPENDENT_FORWARD_PROGRESS 0x105D

	/* cl_device_fp_config - bitfield */
#define CL_FP_DENORM                                (1 << 0)
#define CL_FP_INF_NAN                               (1 << 1)
#define CL_FP_ROUND_TO_NEAREST                      (1 << 2)
#define CL_FP_ROUND_TO_ZERO                         (1 << 3)
#define CL_FP_ROUND_TO_INF                          (1 << 4)
#define CL_FP_FMA                                   (1 << 5)
#define CL_FP_SOFT_FLOAT                            (1 << 6)
#define CL_FP_CORRECTLY_ROUNDED_DIVIDE_SQRT         (1 << 7)

	/* cl_device_mem_cache_type */
#define CL_NONE                                     0x0
#define CL_READ_ONLY_CACHE                          0x1
#define CL_READ_WRITE_CACHE                         0x2

	/* cl_device_local_mem_type */
#define CL_LOCAL                                    0x1
#define CL_GLOBAL                                   0x2

	/* cl_device_exec_capabilities - bitfield */
#define CL_EXEC_KERNEL                              (1 << 0)
#define CL_EXEC_NATIVE_KERNEL                       (1 << 1)

	/* cl_command_queue_properties - bitfield */
#define CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE      (1 << 0)
#define CL_QUEUE_PROFILING_ENABLE                   (1 << 1)
#define CL_QUEUE_ON_DEVICE                          (1 << 2)
#define CL_QUEUE_ON_DEVICE_DEFAULT                  (1 << 3)

	/* cl_context_info  */
#define CL_CONTEXT_REFERENCE_COUNT                  0x1080
#define CL_CONTEXT_DEVICES                          0x1081
#define CL_CONTEXT_PROPERTIES                       0x1082
#define CL_CONTEXT_NUM_DEVICES                      0x1083

	/* cl_context_properties */
#define CL_CONTEXT_PLATFORM                         0x1084
#define CL_CONTEXT_INTEROP_USER_SYNC                0x1085

	/* cl_device_partition_property */
#define CL_DEVICE_PARTITION_EQUALLY                 0x1086
#define CL_DEVICE_PARTITION_BY_COUNTS               0x1087
#define CL_DEVICE_PARTITION_BY_COUNTS_LIST_END      0x0
#define CL_DEVICE_PARTITION_BY_AFFINITY_DOMAIN      0x1088

	/* cl_device_affinity_domain */
#define CL_DEVICE_AFFINITY_DOMAIN_NUMA               (1 << 0)
#define CL_DEVICE_AFFINITY_DOMAIN_L4_CACHE           (1 << 1)
#define CL_DEVICE_AFFINITY_DOMAIN_L3_CACHE           (1 << 2)
#define CL_DEVICE_AFFINITY_DOMAIN_L2_CACHE           (1 << 3)
#define CL_DEVICE_AFFINITY_DOMAIN_L1_CACHE           (1 << 4)
#define CL_DEVICE_AFFINITY_DOMAIN_NEXT_PARTITIONABLE (1 << 5)

	/* cl_device_svm_capabilities */
#define CL_DEVICE_SVM_COARSE_GRAIN_BUFFER           (1 << 0)
#define CL_DEVICE_SVM_FINE_GRAIN_BUFFER             (1 << 1)
#define CL_DEVICE_SVM_FINE_GRAIN_SYSTEM             (1 << 2)
#define CL_DEVICE_SVM_ATOMICS                       (1 << 3)

	/* cl_command_queue_info */
#define CL_QUEUE_CONTEXT                            0x1090
#define CL_QUEUE_DEVICE                             0x1091
#define CL_QUEUE_REFERENCE_COUNT                    0x1092
#define CL_QUEUE_PROPERTIES                         0x1093
#define CL_QUEUE_SIZE                               0x1094
#define CL_QUEUE_DEVICE_DEFAULT                     0x1095

	/* cl_mem_flags and cl_svm_mem_flags - bitfield */
#define CL_MEM_READ_WRITE                           (1 << 0)
#define CL_MEM_WRITE_ONLY                           (1 << 1)
#define CL_MEM_READ_ONLY                            (1 << 2)
#define CL_MEM_USE_HOST_PTR                         (1 << 3)
#define CL_MEM_ALLOC_HOST_PTR                       (1 << 4)
#define CL_MEM_COPY_HOST_PTR                        (1 << 5)
	/* reserved                                         (1 << 6)    */
#define CL_MEM_HOST_WRITE_ONLY                      (1 << 7)
#define CL_MEM_HOST_READ_ONLY                       (1 << 8)
#define CL_MEM_HOST_NO_ACCESS                       (1 << 9)
#define CL_MEM_SVM_FINE_GRAIN_BUFFER                (1 << 10)   /* used by cl_svm_mem_flags only */
#define CL_MEM_SVM_ATOMICS                          (1 << 11)   /* used by cl_svm_mem_flags only */
#define CL_MEM_KERNEL_READ_AND_WRITE                (1 << 12)

	/* cl_mem_migration_flags - bitfield */
#define CL_MIGRATE_MEM_OBJECT_HOST                  (1 << 0)
#define CL_MIGRATE_MEM_OBJECT_CONTENT_UNDEFINED     (1 << 1)

	/* cl_channel_order */
#define CL_R                                        0x10B0
#define CL_A                                        0x10B1
#define CL_RG                                       0x10B2
#define CL_RA                                       0x10B3
#define CL_RGB                                      0x10B4
#define CL_RGBA                                     0x10B5
#define CL_BGRA                                     0x10B6
#define CL_ARGB                                     0x10B7
#define CL_INTENSITY                                0x10B8
#define CL_LUMINANCE                                0x10B9
#define CL_Rx                                       0x10BA
#define CL_RGx                                      0x10BB
#define CL_RGBx                                     0x10BC
#define CL_DEPTH                                    0x10BD
#define CL_DEPTH_STENCIL                            0x10BE
#define CL_sRGB                                     0x10BF
#define CL_sRGBx                                    0x10C0
#define CL_sRGBA                                    0x10C1
#define CL_sBGRA                                    0x10C2
#define CL_ABGR                                     0x10C3

	/* cl_channel_type */
#define CL_SNORM_INT8                               0x10D0
#define CL_SNORM_INT16                              0x10D1
#define CL_UNORM_INT8                               0x10D2
#define CL_UNORM_INT16                              0x10D3
#define CL_UNORM_SHORT_565                          0x10D4
#define CL_UNORM_SHORT_555                          0x10D5
#define CL_UNORM_INT_101010                         0x10D6
#define CL_SIGNED_INT8                              0x10D7
#define CL_SIGNED_INT16                             0x10D8
#define CL_SIGNED_INT32                             0x10D9
#define CL_UNSIGNED_INT8                            0x10DA
#define CL_UNSIGNED_INT16                           0x10DB
#define CL_UNSIGNED_INT32                           0x10DC
#define CL_HALF_FLOAT                               0x10DD
#define CL_FLOAT                                    0x10DE
#define CL_UNORM_INT24                              0x10DF
#define CL_UNORM_INT_101010_2                       0x10E0

	/* cl_mem_object_type */
#define CL_MEM_OBJECT_BUFFER                        0x10F0
#define CL_MEM_OBJECT_IMAGE2D                       0x10F1
#define CL_MEM_OBJECT_IMAGE3D                       0x10F2
#define CL_MEM_OBJECT_IMAGE2D_ARRAY                 0x10F3
#define CL_MEM_OBJECT_IMAGE1D                       0x10F4
#define CL_MEM_OBJECT_IMAGE1D_ARRAY                 0x10F5
#define CL_MEM_OBJECT_IMAGE1D_BUFFER                0x10F6
#define CL_MEM_OBJECT_PIPE                          0x10F7

	/* cl_mem_info */
#define CL_MEM_TYPE                                 0x1100
#define CL_MEM_FLAGS                                0x1101
#define CL_MEM_SIZE                                 0x1102
#define CL_MEM_HOST_PTR                             0x1103
#define CL_MEM_MAP_COUNT                            0x1104
#define CL_MEM_REFERENCE_COUNT                      0x1105
#define CL_MEM_CONTEXT                              0x1106
#define CL_MEM_ASSOCIATED_MEMOBJECT                 0x1107
#define CL_MEM_OFFSET                               0x1108
#define CL_MEM_USES_SVM_POINTER                     0x1109

	/* cl_image_info */
#define CL_IMAGE_FORMAT                             0x1110
#define CL_IMAGE_ELEMENT_SIZE                       0x1111
#define CL_IMAGE_ROW_PITCH                          0x1112
#define CL_IMAGE_SLICE_PITCH                        0x1113
#define CL_IMAGE_WIDTH                              0x1114
#define CL_IMAGE_HEIGHT                             0x1115
#define CL_IMAGE_DEPTH                              0x1116
#define CL_IMAGE_ARRAY_SIZE                         0x1117
#define CL_IMAGE_BUFFER                             0x1118
#define CL_IMAGE_NUM_MIP_LEVELS                     0x1119
#define CL_IMAGE_NUM_SAMPLES                        0x111A

	/* cl_pipe_info */
#define CL_PIPE_PACKET_SIZE                         0x1120
#define CL_PIPE_MAX_PACKETS                         0x1121

	/* cl_addressing_mode */
#define CL_ADDRESS_NONE                             0x1130
#define CL_ADDRESS_CLAMP_TO_EDGE                    0x1131
#define CL_ADDRESS_CLAMP                            0x1132
#define CL_ADDRESS_REPEAT                           0x1133
#define CL_ADDRESS_MIRRORED_REPEAT                  0x1134

	/* cl_filter_mode */
#define CL_FILTER_NEAREST                           0x1140
#define CL_FILTER_LINEAR                            0x1141

	/* cl_sampler_info */
#define CL_SAMPLER_REFERENCE_COUNT                  0x1150
#define CL_SAMPLER_CONTEXT                          0x1151
#define CL_SAMPLER_NORMALIZED_COORDS                0x1152
#define CL_SAMPLER_ADDRESSING_MODE                  0x1153
#define CL_SAMPLER_FILTER_MODE                      0x1154
#define CL_SAMPLER_MIP_FILTER_MODE                  0x1155
#define CL_SAMPLER_LOD_MIN                          0x1156
#define CL_SAMPLER_LOD_MAX                          0x1157

	/* cl_map_flags - bitfield */
#define CL_MAP_READ                                 (1 << 0)
#define CL_MAP_WRITE                                (1 << 1)
#define CL_MAP_WRITE_INVALIDATE_REGION              (1 << 2)

	/* cl_program_info */
#define CL_PROGRAM_REFERENCE_COUNT                  0x1160
#define CL_PROGRAM_CONTEXT                          0x1161
#define CL_PROGRAM_NUM_DEVICES                      0x1162
#define CL_PROGRAM_DEVICES                          0x1163
#define CL_PROGRAM_SOURCE                           0x1164
#define CL_PROGRAM_BINARY_SIZES                     0x1165
#define CL_PROGRAM_BINARIES                         0x1166
#define CL_PROGRAM_NUM_KERNELS                      0x1167
#define CL_PROGRAM_KERNEL_NAMES                     0x1168
#define CL_PROGRAM_IL                               0x1169
#define CL_PROGRAM_SCOPE_GLOBAL_CTORS_PRESENT       0x116A
#define CL_PROGRAM_SCOPE_GLOBAL_DTORS_PRESENT       0x116B

	/* cl_program_build_info */
#define CL_PROGRAM_BUILD_STATUS                     0x1181
#define CL_PROGRAM_BUILD_OPTIONS                    0x1182
#define CL_PROGRAM_BUILD_LOG                        0x1183
#define CL_PROGRAM_BINARY_TYPE                      0x1184
#define CL_PROGRAM_BUILD_GLOBAL_VARIABLE_TOTAL_SIZE 0x1185

	/* cl_program_binary_type */
#define CL_PROGRAM_BINARY_TYPE_NONE                 0x0
#define CL_PROGRAM_BINARY_TYPE_COMPILED_OBJECT      0x1
#define CL_PROGRAM_BINARY_TYPE_LIBRARY              0x2
#define CL_PROGRAM_BINARY_TYPE_EXECUTABLE           0x4

	/* cl_build_status */
#define CL_BUILD_SUCCESS                            0
#define CL_BUILD_NONE                               -1
#define CL_BUILD_ERROR                              -2
#define CL_BUILD_IN_PROGRESS                        -3

	/* cl_kernel_info */
#define CL_KERNEL_FUNCTION_NAME                     0x1190
#define CL_KERNEL_NUM_ARGS                          0x1191
#define CL_KERNEL_REFERENCE_COUNT                   0x1192
#define CL_KERNEL_CONTEXT                           0x1193
#define CL_KERNEL_PROGRAM                           0x1194
#define CL_KERNEL_ATTRIBUTES                        0x1195
#define CL_KERNEL_MAX_NUM_SUB_GROUPS                0x11B9
#define CL_KERNEL_COMPILE_NUM_SUB_GROUPS            0x11BA

	/* cl_kernel_arg_info */
#define CL_KERNEL_ARG_ADDRESS_QUALIFIER             0x1196
#define CL_KERNEL_ARG_ACCESS_QUALIFIER              0x1197
#define CL_KERNEL_ARG_TYPE_NAME                     0x1198
#define CL_KERNEL_ARG_TYPE_QUALIFIER                0x1199
#define CL_KERNEL_ARG_NAME                          0x119A

	/* cl_kernel_arg_address_qualifier */
#define CL_KERNEL_ARG_ADDRESS_GLOBAL                0x119B
#define CL_KERNEL_ARG_ADDRESS_LOCAL                 0x119C
#define CL_KERNEL_ARG_ADDRESS_CONSTANT              0x119D
#define CL_KERNEL_ARG_ADDRESS_PRIVATE               0x119E

	/* cl_kernel_arg_access_qualifier */
#define CL_KERNEL_ARG_ACCESS_READ_ONLY              0x11A0
#define CL_KERNEL_ARG_ACCESS_WRITE_ONLY             0x11A1
#define CL_KERNEL_ARG_ACCESS_READ_WRITE             0x11A2
#define CL_KERNEL_ARG_ACCESS_NONE                   0x11A3

	/* cl_kernel_arg_type_qualifier */
#define CL_KERNEL_ARG_TYPE_NONE                     0
#define CL_KERNEL_ARG_TYPE_CONST                    (1 << 0)
#define CL_KERNEL_ARG_TYPE_RESTRICT                 (1 << 1)
#define CL_KERNEL_ARG_TYPE_VOLATILE                 (1 << 2)
#define CL_KERNEL_ARG_TYPE_PIPE                     (1 << 3)

	/* cl_kernel_work_group_info */
#define CL_KERNEL_WORK_GROUP_SIZE                   0x11B0
#define CL_KERNEL_COMPILE_WORK_GROUP_SIZE           0x11B1
#define CL_KERNEL_LOCAL_MEM_SIZE                    0x11B2
#define CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE 0x11B3
#define CL_KERNEL_PRIVATE_MEM_SIZE                  0x11B4
#define CL_KERNEL_GLOBAL_WORK_SIZE                  0x11B5

	/* cl_kernel_sub_group_info */
#define CL_KERNEL_MAX_SUB_GROUP_SIZE_FOR_NDRANGE    0x2033
#define CL_KERNEL_SUB_GROUP_COUNT_FOR_NDRANGE       0x2034
#define CL_KERNEL_LOCAL_SIZE_FOR_SUB_GROUP_COUNT    0x11B8

	/* cl_kernel_exec_info */
#define CL_KERNEL_EXEC_INFO_SVM_PTRS                0x11B6
#define CL_KERNEL_EXEC_INFO_SVM_FINE_GRAIN_SYSTEM   0x11B7

	/* cl_event_info  */
#define CL_EVENT_COMMAND_QUEUE                      0x11D0
#define CL_EVENT_COMMAND_TYPE                       0x11D1
#define CL_EVENT_REFERENCE_COUNT                    0x11D2
#define CL_EVENT_COMMAND_EXECUTION_STATUS           0x11D3
#define CL_EVENT_CONTEXT                            0x11D4

	/* cl_command_type */
#define CL_COMMAND_NDRANGE_KERNEL                   0x11F0
#define CL_COMMAND_TASK                             0x11F1
#define CL_COMMAND_NATIVE_KERNEL                    0x11F2
#define CL_COMMAND_READ_BUFFER                      0x11F3
#define CL_COMMAND_WRITE_BUFFER                     0x11F4
#define CL_COMMAND_COPY_BUFFER                      0x11F5
#define CL_COMMAND_READ_IMAGE                       0x11F6
#define CL_COMMAND_WRITE_IMAGE                      0x11F7
#define CL_COMMAND_COPY_IMAGE                       0x11F8
#define CL_COMMAND_COPY_IMAGE_TO_BUFFER             0x11F9
#define CL_COMMAND_COPY_BUFFER_TO_IMAGE             0x11FA
#define CL_COMMAND_MAP_BUFFER                       0x11FB
#define CL_COMMAND_MAP_IMAGE                        0x11FC
#define CL_COMMAND_UNMAP_MEM_OBJECT                 0x11FD
#define CL_COMMAND_MARKER                           0x11FE
#define CL_COMMAND_ACQUIRE_GL_OBJECTS               0x11FF
#define CL_COMMAND_RELEASE_GL_OBJECTS               0x1200
#define CL_COMMAND_READ_BUFFER_RECT                 0x1201
#define CL_COMMAND_WRITE_BUFFER_RECT                0x1202
#define CL_COMMAND_COPY_BUFFER_RECT                 0x1203
#define CL_COMMAND_USER                             0x1204
#define CL_COMMAND_BARRIER                          0x1205
#define CL_COMMAND_MIGRATE_MEM_OBJECTS              0x1206
#define CL_COMMAND_FILL_BUFFER                      0x1207
#define CL_COMMAND_FILL_IMAGE                       0x1208
#define CL_COMMAND_SVM_FREE                         0x1209
#define CL_COMMAND_SVM_MEMCPY                       0x120A
#define CL_COMMAND_SVM_MEMFILL                      0x120B
#define CL_COMMAND_SVM_MAP                          0x120C
#define CL_COMMAND_SVM_UNMAP                        0x120D

	/* command execution status */
#define CL_COMPLETE                                 0x0
#define CL_RUNNING                                  0x1
#define CL_SUBMITTED                                0x2
#define CL_QUEUED                                   0x3

	/* cl_buffer_create_type  */
#define CL_BUFFER_CREATE_TYPE_REGION                0x1220

	/* cl_profiling_info  */
#define CL_PROFILING_COMMAND_QUEUED                 0x1280
#define CL_PROFILING_COMMAND_SUBMIT                 0x1281
#define CL_PROFILING_COMMAND_START                  0x1282
#define CL_PROFILING_COMMAND_END                    0x1283
#define CL_PROFILING_COMMAND_COMPLETE               0x1284

	/********************************************************************************************************/

	/* Platform API */
	using clGetPlatformIDsPTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_uint  num_entries, cl_platform_id *  platforms, cl_uint *  num_platforms) CL_API_SUFFIX__VERSION_1_0;
	extern clGetPlatformIDsPTR cload_clGetPlatformIDs;
	inline CL_API_ENTRY cl_int CL_API_CALL clGetPlatformIDs(cl_uint  num_entries, cl_platform_id *  platforms, cl_uint *  num_platforms) CL_API_SUFFIX__VERSION_1_0
	{
		return cload_clGetPlatformIDs(num_entries, platforms, num_platforms);
	}


	using clGetPlatformInfoPTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_platform_id  platform, cl_platform_info  param_name, size_t  param_value_size, void *  param_value, size_t *  param_value_size_ret) CL_API_SUFFIX__VERSION_1_0;
	extern clGetPlatformInfoPTR cload_clGetPlatformInfo;
	inline CL_API_ENTRY cl_int CL_API_CALL clGetPlatformInfo(cl_platform_id  platform, cl_platform_info  param_name, size_t  param_value_size, void *  param_value, size_t *  param_value_size_ret) CL_API_SUFFIX__VERSION_1_0
	{
		return cload_clGetPlatformInfo(platform, param_name, param_value_size, param_value, param_value_size_ret);
	}


	/* Device APIs */
	using clGetDeviceIDsPTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_platform_id  platform, cl_device_type  device_type, cl_uint  num_entries, cl_device_id *  devices, cl_uint *  num_devices) CL_API_SUFFIX__VERSION_1_0;
	extern clGetDeviceIDsPTR cload_clGetDeviceIDs;
	inline CL_API_ENTRY cl_int CL_API_CALL clGetDeviceIDs(cl_platform_id  platform, cl_device_type  device_type, cl_uint  num_entries, cl_device_id *  devices, cl_uint *  num_devices) CL_API_SUFFIX__VERSION_1_0
	{
		return cload_clGetDeviceIDs(platform, device_type, num_entries, devices, num_devices);
	}


	using clGetDeviceInfoPTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_device_id  device, cl_device_info  param_name, size_t  param_value_size, void *  param_value, size_t *  param_value_size_ret) CL_API_SUFFIX__VERSION_1_0;
	extern clGetDeviceInfoPTR cload_clGetDeviceInfo;
	inline CL_API_ENTRY cl_int CL_API_CALL clGetDeviceInfo(cl_device_id  device, cl_device_info  param_name, size_t  param_value_size, void *  param_value, size_t *  param_value_size_ret) CL_API_SUFFIX__VERSION_1_0
	{
		return cload_clGetDeviceInfo(device, param_name, param_value_size, param_value, param_value_size_ret);
	}


	using clCreateSubDevicesPTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_device_id  in_device, const cl_device_partition_property *  properties, cl_uint  num_devices, cl_device_id *  out_devices, cl_uint *  num_devices_ret) CL_API_SUFFIX__VERSION_1_2;
	extern clCreateSubDevicesPTR cload_clCreateSubDevices;
	inline CL_API_ENTRY cl_int CL_API_CALL clCreateSubDevices(cl_device_id  in_device, const cl_device_partition_property *  properties, cl_uint  num_devices, cl_device_id *  out_devices, cl_uint *  num_devices_ret) CL_API_SUFFIX__VERSION_1_2
	{
		return cload_clCreateSubDevices(in_device, properties, num_devices, out_devices, num_devices_ret);
	}


	using clRetainDevicePTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_device_id  device) CL_API_SUFFIX__VERSION_1_2;
	extern clRetainDevicePTR cload_clRetainDevice;
	inline CL_API_ENTRY cl_int CL_API_CALL clRetainDevice(cl_device_id  device) CL_API_SUFFIX__VERSION_1_2
	{
		return cload_clRetainDevice(device);
	}


	using clReleaseDevicePTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_device_id  device) CL_API_SUFFIX__VERSION_1_2;
	extern clReleaseDevicePTR cload_clReleaseDevice;
	inline CL_API_ENTRY cl_int CL_API_CALL clReleaseDevice(cl_device_id  device) CL_API_SUFFIX__VERSION_1_2
	{
		return cload_clReleaseDevice(device);
	}


	using clSetDefaultDeviceCommandQueuePTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_context  context, cl_device_id  device, cl_command_queue  command_queue) CL_API_SUFFIX__VERSION_2_1;
	extern clSetDefaultDeviceCommandQueuePTR cload_clSetDefaultDeviceCommandQueue;
	inline CL_API_ENTRY cl_int CL_API_CALL clSetDefaultDeviceCommandQueue(cl_context  context, cl_device_id  device, cl_command_queue  command_queue) CL_API_SUFFIX__VERSION_2_1
	{
		return cload_clSetDefaultDeviceCommandQueue(context, device, command_queue);
	}


	using clGetDeviceAndHostTimerPTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_device_id  device, cl_ulong*  device_timestamp, cl_ulong*  host_timestamp) CL_API_SUFFIX__VERSION_2_1;
	extern clGetDeviceAndHostTimerPTR cload_clGetDeviceAndHostTimer;
	inline CL_API_ENTRY cl_int CL_API_CALL clGetDeviceAndHostTimer(cl_device_id  device, cl_ulong*  device_timestamp, cl_ulong*  host_timestamp) CL_API_SUFFIX__VERSION_2_1
	{
		return cload_clGetDeviceAndHostTimer(device, device_timestamp, host_timestamp);
	}


	using clGetHostTimerPTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_device_id  device, cl_ulong *  host_timestamp) CL_API_SUFFIX__VERSION_2_1;
	extern clGetHostTimerPTR cload_clGetHostTimer;
	inline CL_API_ENTRY cl_int CL_API_CALL clGetHostTimer(cl_device_id  device, cl_ulong *  host_timestamp) CL_API_SUFFIX__VERSION_2_1
	{
		return cload_clGetHostTimer(device, host_timestamp);
	}



	/* Context APIs  */
	using clCreateContextPTR = CL_API_ENTRY cl_context(CL_API_CALL *)(const cl_context_properties *  properties, cl_uint  num_devices, const cl_device_id *  devices, void (CL_CALLBACK *  pfn_notify)(const char *, const void *, size_t, void *), void *  user_data, cl_int *  errcode_ret) CL_API_SUFFIX__VERSION_1_0;
	extern clCreateContextPTR cload_clCreateContext;
	inline CL_API_ENTRY cl_context CL_API_CALL clCreateContext(const cl_context_properties *  properties, cl_uint  num_devices, const cl_device_id *  devices, void (CL_CALLBACK *  pfn_notify)(const char *, const void *, size_t, void *), void *  user_data, cl_int *  errcode_ret) CL_API_SUFFIX__VERSION_1_0
	{
		return cload_clCreateContext(properties, num_devices, devices, pfn_notify, user_data, errcode_ret);
	}


	using clCreateContextFromTypePTR = CL_API_ENTRY cl_context(CL_API_CALL *)(const cl_context_properties *  properties, cl_device_type  device_type, void (CL_CALLBACK *  pfn_notify)(const char *, const void *, size_t, void *), void *  user_data, cl_int *  errcode_ret) CL_API_SUFFIX__VERSION_1_0;
	extern clCreateContextFromTypePTR cload_clCreateContextFromType;
	inline CL_API_ENTRY cl_context CL_API_CALL clCreateContextFromType(const cl_context_properties *  properties, cl_device_type  device_type, void (CL_CALLBACK *  pfn_notify)(const char *, const void *, size_t, void *), void *  user_data, cl_int *  errcode_ret) CL_API_SUFFIX__VERSION_1_0
	{
		return cload_clCreateContextFromType(properties, device_type, pfn_notify, user_data, errcode_ret);
	}


	using clRetainContextPTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_context  context) CL_API_SUFFIX__VERSION_1_0;
	extern clRetainContextPTR cload_clRetainContext;
	inline CL_API_ENTRY cl_int CL_API_CALL clRetainContext(cl_context  context) CL_API_SUFFIX__VERSION_1_0
	{
		return cload_clRetainContext(context);
	}


	using clReleaseContextPTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_context  context) CL_API_SUFFIX__VERSION_1_0;
	extern clReleaseContextPTR cload_clReleaseContext;
	inline CL_API_ENTRY cl_int CL_API_CALL clReleaseContext(cl_context  context) CL_API_SUFFIX__VERSION_1_0
	{
		return cload_clReleaseContext(context);
	}


	using clGetContextInfoPTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_context  context, cl_context_info  param_name, size_t  param_value_size, void *  param_value, size_t *  param_value_size_ret) CL_API_SUFFIX__VERSION_1_0;
	extern clGetContextInfoPTR cload_clGetContextInfo;
	inline CL_API_ENTRY cl_int CL_API_CALL clGetContextInfo(cl_context  context, cl_context_info  param_name, size_t  param_value_size, void *  param_value, size_t *  param_value_size_ret) CL_API_SUFFIX__VERSION_1_0
	{
		return cload_clGetContextInfo(context, param_name, param_value_size, param_value, param_value_size_ret);
	}


	/* Command Queue APIs */
	using clCreateCommandQueueWithPropertiesPTR = CL_API_ENTRY cl_command_queue(CL_API_CALL *)(cl_context  context, cl_device_id  device, const cl_queue_properties *  properties, cl_int *  errcode_ret) CL_API_SUFFIX__VERSION_2_0;
	extern clCreateCommandQueueWithPropertiesPTR cload_clCreateCommandQueueWithProperties;
	inline CL_API_ENTRY cl_command_queue CL_API_CALL clCreateCommandQueueWithProperties(cl_context  context, cl_device_id  device, const cl_queue_properties *  properties, cl_int *  errcode_ret) CL_API_SUFFIX__VERSION_2_0
	{
		return cload_clCreateCommandQueueWithProperties(context, device, properties, errcode_ret);
	}


	using clRetainCommandQueuePTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_command_queue  command_queue) CL_API_SUFFIX__VERSION_1_0;
	extern clRetainCommandQueuePTR cload_clRetainCommandQueue;
	inline CL_API_ENTRY cl_int CL_API_CALL clRetainCommandQueue(cl_command_queue  command_queue) CL_API_SUFFIX__VERSION_1_0
	{
		return cload_clRetainCommandQueue(command_queue);
	}


	using clReleaseCommandQueuePTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_command_queue  command_queue) CL_API_SUFFIX__VERSION_1_0;
	extern clReleaseCommandQueuePTR cload_clReleaseCommandQueue;
	inline CL_API_ENTRY cl_int CL_API_CALL clReleaseCommandQueue(cl_command_queue  command_queue) CL_API_SUFFIX__VERSION_1_0
	{
		return cload_clReleaseCommandQueue(command_queue);
	}


	using clGetCommandQueueInfoPTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_command_queue  command_queue, cl_command_queue_info  param_name, size_t  param_value_size, void *  param_value, size_t *  param_value_size_ret) CL_API_SUFFIX__VERSION_1_0;
	extern clGetCommandQueueInfoPTR cload_clGetCommandQueueInfo;
	inline CL_API_ENTRY cl_int CL_API_CALL clGetCommandQueueInfo(cl_command_queue  command_queue, cl_command_queue_info  param_name, size_t  param_value_size, void *  param_value, size_t *  param_value_size_ret) CL_API_SUFFIX__VERSION_1_0
	{
		return cload_clGetCommandQueueInfo(command_queue, param_name, param_value_size, param_value, param_value_size_ret);
	}


	/* Memory Object APIs */
	using clCreateBufferPTR = CL_API_ENTRY cl_mem(CL_API_CALL *)(cl_context  context, cl_mem_flags  flags, size_t  size, void *  host_ptr, cl_int *  errcode_ret) CL_API_SUFFIX__VERSION_1_0;
	extern clCreateBufferPTR cload_clCreateBuffer;
	inline CL_API_ENTRY cl_mem CL_API_CALL clCreateBuffer(cl_context  context, cl_mem_flags  flags, size_t  size, void *  host_ptr, cl_int *  errcode_ret) CL_API_SUFFIX__VERSION_1_0
	{
		return cload_clCreateBuffer(context, flags, size, host_ptr, errcode_ret);
	}


	using clCreateSubBufferPTR = CL_API_ENTRY cl_mem(CL_API_CALL *)(cl_mem  buffer, cl_mem_flags  flags, cl_buffer_create_type  buffer_create_type, const void *  buffer_create_info, cl_int *  errcode_ret) CL_API_SUFFIX__VERSION_1_1;
	extern clCreateSubBufferPTR cload_clCreateSubBuffer;
	inline CL_API_ENTRY cl_mem CL_API_CALL clCreateSubBuffer(cl_mem  buffer, cl_mem_flags  flags, cl_buffer_create_type  buffer_create_type, const void *  buffer_create_info, cl_int *  errcode_ret) CL_API_SUFFIX__VERSION_1_1
	{
		return cload_clCreateSubBuffer(buffer, flags, buffer_create_type, buffer_create_info, errcode_ret);
	}


	using clCreateImagePTR = CL_API_ENTRY cl_mem(CL_API_CALL *)(cl_context  context, cl_mem_flags  flags, const cl_image_format *  image_format, const cl_image_desc *  image_desc, void *  host_ptr, cl_int *  errcode_ret) CL_API_SUFFIX__VERSION_1_2;
	extern clCreateImagePTR cload_clCreateImage;
	inline CL_API_ENTRY cl_mem CL_API_CALL clCreateImage(cl_context  context, cl_mem_flags  flags, const cl_image_format *  image_format, const cl_image_desc *  image_desc, void *  host_ptr, cl_int *  errcode_ret) CL_API_SUFFIX__VERSION_1_2
	{
		return cload_clCreateImage(context, flags, image_format, image_desc, host_ptr, errcode_ret);
	}


	using clCreatePipePTR = CL_API_ENTRY cl_mem(CL_API_CALL *)(cl_context  context, cl_mem_flags  flags, cl_uint  pipe_packet_size, cl_uint  pipe_max_packets, const cl_pipe_properties *  properties, cl_int *  errcode_ret) CL_API_SUFFIX__VERSION_2_0;
	extern clCreatePipePTR cload_clCreatePipe;
	inline CL_API_ENTRY cl_mem CL_API_CALL clCreatePipe(cl_context  context, cl_mem_flags  flags, cl_uint  pipe_packet_size, cl_uint  pipe_max_packets, const cl_pipe_properties *  properties, cl_int *  errcode_ret) CL_API_SUFFIX__VERSION_2_0
	{
		return cload_clCreatePipe(context, flags, pipe_packet_size, pipe_max_packets, properties, errcode_ret);
	}


	using clRetainMemObjectPTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_mem  memobj) CL_API_SUFFIX__VERSION_1_0;
	extern clRetainMemObjectPTR cload_clRetainMemObject;
	inline CL_API_ENTRY cl_int CL_API_CALL clRetainMemObject(cl_mem  memobj) CL_API_SUFFIX__VERSION_1_0
	{
		return cload_clRetainMemObject(memobj);
	}


	using clReleaseMemObjectPTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_mem  memobj) CL_API_SUFFIX__VERSION_1_0;
	extern clReleaseMemObjectPTR cload_clReleaseMemObject;
	inline CL_API_ENTRY cl_int CL_API_CALL clReleaseMemObject(cl_mem  memobj) CL_API_SUFFIX__VERSION_1_0
	{
		return cload_clReleaseMemObject(memobj);
	}


	using clGetSupportedImageFormatsPTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_context  context, cl_mem_flags  flags, cl_mem_object_type  image_type, cl_uint  num_entries, cl_image_format *  image_formats, cl_uint *  num_image_formats) CL_API_SUFFIX__VERSION_1_0;
	extern clGetSupportedImageFormatsPTR cload_clGetSupportedImageFormats;
	inline CL_API_ENTRY cl_int CL_API_CALL clGetSupportedImageFormats(cl_context  context, cl_mem_flags  flags, cl_mem_object_type  image_type, cl_uint  num_entries, cl_image_format *  image_formats, cl_uint *  num_image_formats) CL_API_SUFFIX__VERSION_1_0
	{
		return cload_clGetSupportedImageFormats(context, flags, image_type, num_entries, image_formats, num_image_formats);
	}


	using clGetMemObjectInfoPTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_mem  memobj, cl_mem_info  param_name, size_t  param_value_size, void *  param_value, size_t *  param_value_size_ret) CL_API_SUFFIX__VERSION_1_0;
	extern clGetMemObjectInfoPTR cload_clGetMemObjectInfo;
	inline CL_API_ENTRY cl_int CL_API_CALL clGetMemObjectInfo(cl_mem  memobj, cl_mem_info  param_name, size_t  param_value_size, void *  param_value, size_t *  param_value_size_ret) CL_API_SUFFIX__VERSION_1_0
	{
		return cload_clGetMemObjectInfo(memobj, param_name, param_value_size, param_value, param_value_size_ret);
	}


	using clGetImageInfoPTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_mem  image, cl_image_info  param_name, size_t  param_value_size, void *  param_value, size_t *  param_value_size_ret) CL_API_SUFFIX__VERSION_1_0;
	extern clGetImageInfoPTR cload_clGetImageInfo;
	inline CL_API_ENTRY cl_int CL_API_CALL clGetImageInfo(cl_mem  image, cl_image_info  param_name, size_t  param_value_size, void *  param_value, size_t *  param_value_size_ret) CL_API_SUFFIX__VERSION_1_0
	{
		return cload_clGetImageInfo(image, param_name, param_value_size, param_value, param_value_size_ret);
	}


	using clGetPipeInfoPTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_mem  pipe, cl_pipe_info  param_name, size_t  param_value_size, void *  param_value, size_t *  param_value_size_ret) CL_API_SUFFIX__VERSION_2_0;
	extern clGetPipeInfoPTR cload_clGetPipeInfo;
	inline CL_API_ENTRY cl_int CL_API_CALL clGetPipeInfo(cl_mem  pipe, cl_pipe_info  param_name, size_t  param_value_size, void *  param_value, size_t *  param_value_size_ret) CL_API_SUFFIX__VERSION_2_0
	{
		return cload_clGetPipeInfo(pipe, param_name, param_value_size, param_value, param_value_size_ret);
	}



	using clSetMemObjectDestructorCallbackPTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_mem  memobj, void (CL_CALLBACK * pfn_notify)(cl_mem  memobj, void* user_data), void * user_data) CL_API_SUFFIX__VERSION_1_1;
	extern clSetMemObjectDestructorCallbackPTR cload_clSetMemObjectDestructorCallback;
	inline CL_API_ENTRY cl_int CL_API_CALL clSetMemObjectDestructorCallback(cl_mem  memobj, void (CL_CALLBACK * pfn_notify)(cl_mem  memobj, void* user_data), void * user_data) CL_API_SUFFIX__VERSION_1_1
	{
		return cload_clSetMemObjectDestructorCallback(memobj, pfn_notify, user_data);
	}


	/* SVM Allocation APIs */
	using clSVMAllocPTR = CL_API_ENTRY void * (CL_API_CALL *)(cl_context  context, cl_svm_mem_flags  flags, size_t  size, cl_uint  alignment) CL_API_SUFFIX__VERSION_2_0;
	extern clSVMAllocPTR cload_clSVMAlloc;
	inline CL_API_ENTRY void * CL_API_CALL clSVMAlloc(cl_context  context, cl_svm_mem_flags  flags, size_t  size, cl_uint  alignment) CL_API_SUFFIX__VERSION_2_0
	{
		return cload_clSVMAlloc(context, flags, size, alignment);
	}


	using clSVMFreePTR = CL_API_ENTRY void (CL_API_CALL *)(cl_context  context, void *  svm_pointer) CL_API_SUFFIX__VERSION_2_0;
	extern clSVMFreePTR cload_clSVMFree;
	inline CL_API_ENTRY void CL_API_CALL clSVMFree(cl_context  context, void *  svm_pointer) CL_API_SUFFIX__VERSION_2_0
	{
		return cload_clSVMFree(context, svm_pointer);
	}


	/* Sampler APIs */
	using clCreateSamplerWithPropertiesPTR = CL_API_ENTRY cl_sampler(CL_API_CALL *)(cl_context  context, const cl_sampler_properties *  normalized_coords, cl_int *  errcode_ret) CL_API_SUFFIX__VERSION_2_0;
	extern clCreateSamplerWithPropertiesPTR cload_clCreateSamplerWithProperties;
	inline CL_API_ENTRY cl_sampler CL_API_CALL clCreateSamplerWithProperties(cl_context  context, const cl_sampler_properties *  normalized_coords, cl_int *  errcode_ret) CL_API_SUFFIX__VERSION_2_0
	{
		return cload_clCreateSamplerWithProperties(context, normalized_coords, errcode_ret);
	}


	using clRetainSamplerPTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_sampler  sampler) CL_API_SUFFIX__VERSION_1_0;
	extern clRetainSamplerPTR cload_clRetainSampler;
	inline CL_API_ENTRY cl_int CL_API_CALL clRetainSampler(cl_sampler  sampler) CL_API_SUFFIX__VERSION_1_0
	{
		return cload_clRetainSampler(sampler);
	}


	using clReleaseSamplerPTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_sampler  sampler) CL_API_SUFFIX__VERSION_1_0;
	extern clReleaseSamplerPTR cload_clReleaseSampler;
	inline CL_API_ENTRY cl_int CL_API_CALL clReleaseSampler(cl_sampler  sampler) CL_API_SUFFIX__VERSION_1_0
	{
		return cload_clReleaseSampler(sampler);
	}


	using clGetSamplerInfoPTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_sampler  sampler, cl_sampler_info  param_name, size_t  param_value_size, void *  param_value, size_t *  param_value_size_ret) CL_API_SUFFIX__VERSION_1_0;
	extern clGetSamplerInfoPTR cload_clGetSamplerInfo;
	inline CL_API_ENTRY cl_int CL_API_CALL clGetSamplerInfo(cl_sampler  sampler, cl_sampler_info  param_name, size_t  param_value_size, void *  param_value, size_t *  param_value_size_ret) CL_API_SUFFIX__VERSION_1_0
	{
		return cload_clGetSamplerInfo(sampler, param_name, param_value_size, param_value, param_value_size_ret);
	}


	/* Program Object APIs  */
	using clCreateProgramWithSourcePTR = CL_API_ENTRY cl_program(CL_API_CALL *)(cl_context  context, cl_uint  count, const char **  strings, const size_t *  lengths, cl_int *  errcode_ret) CL_API_SUFFIX__VERSION_1_0;
	extern clCreateProgramWithSourcePTR cload_clCreateProgramWithSource;
	inline CL_API_ENTRY cl_program CL_API_CALL clCreateProgramWithSource(cl_context  context, cl_uint  count, const char **  strings, const size_t *  lengths, cl_int *  errcode_ret) CL_API_SUFFIX__VERSION_1_0
	{
		return cload_clCreateProgramWithSource(context, count, strings, lengths, errcode_ret);
	}


	using clCreateProgramWithBinaryPTR = CL_API_ENTRY cl_program(CL_API_CALL *)(cl_context  context, cl_uint  num_devices, const cl_device_id *  device_list, const size_t *  lengths, const unsigned char **  binaries, cl_int *  binary_status, cl_int *  errcode_ret) CL_API_SUFFIX__VERSION_1_0;
	extern clCreateProgramWithBinaryPTR cload_clCreateProgramWithBinary;
	inline CL_API_ENTRY cl_program CL_API_CALL clCreateProgramWithBinary(cl_context  context, cl_uint  num_devices, const cl_device_id *  device_list, const size_t *  lengths, const unsigned char **  binaries, cl_int *  binary_status, cl_int *  errcode_ret) CL_API_SUFFIX__VERSION_1_0
	{
		return cload_clCreateProgramWithBinary(context, num_devices, device_list, lengths, binaries, binary_status, errcode_ret);
	}


	using clCreateProgramWithBuiltInKernelsPTR = CL_API_ENTRY cl_program(CL_API_CALL *)(cl_context  context, cl_uint  num_devices, const cl_device_id *  device_list, const char *  kernel_names, cl_int *  errcode_ret) CL_API_SUFFIX__VERSION_1_2;
	extern clCreateProgramWithBuiltInKernelsPTR cload_clCreateProgramWithBuiltInKernels;
	inline CL_API_ENTRY cl_program CL_API_CALL clCreateProgramWithBuiltInKernels(cl_context  context, cl_uint  num_devices, const cl_device_id *  device_list, const char *  kernel_names, cl_int *  errcode_ret) CL_API_SUFFIX__VERSION_1_2
	{
		return cload_clCreateProgramWithBuiltInKernels(context, num_devices, device_list, kernel_names, errcode_ret);
	}


	using clCreateProgramWithILPTR = CL_API_ENTRY cl_program(CL_API_CALL *)(cl_context  context, const void*  il, size_t  length, cl_int*  errcode_ret) CL_API_SUFFIX__VERSION_2_1;
	extern clCreateProgramWithILPTR cload_clCreateProgramWithIL;
	inline CL_API_ENTRY cl_program CL_API_CALL clCreateProgramWithIL(cl_context  context, const void*  il, size_t  length, cl_int*  errcode_ret) CL_API_SUFFIX__VERSION_2_1
	{
		return cload_clCreateProgramWithIL(context, il, length, errcode_ret);
	}



	using clRetainProgramPTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_program  program) CL_API_SUFFIX__VERSION_1_0;
	extern clRetainProgramPTR cload_clRetainProgram;
	inline CL_API_ENTRY cl_int CL_API_CALL clRetainProgram(cl_program  program) CL_API_SUFFIX__VERSION_1_0
	{
		return cload_clRetainProgram(program);
	}


	using clReleaseProgramPTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_program  program) CL_API_SUFFIX__VERSION_1_0;
	extern clReleaseProgramPTR cload_clReleaseProgram;
	inline CL_API_ENTRY cl_int CL_API_CALL clReleaseProgram(cl_program  program) CL_API_SUFFIX__VERSION_1_0
	{
		return cload_clReleaseProgram(program);
	}


	using clBuildProgramPTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_program  program, cl_uint  num_devices, const cl_device_id *  device_list, const char *  options, void (CL_CALLBACK *  pfn_notify)(cl_program  program, void *  user_data), void *  user_data) CL_API_SUFFIX__VERSION_1_0;
	extern clBuildProgramPTR cload_clBuildProgram;
	inline CL_API_ENTRY cl_int CL_API_CALL clBuildProgram(cl_program  program, cl_uint  num_devices, const cl_device_id *  device_list, const char *  options, void (CL_CALLBACK *  pfn_notify)(cl_program  program, void *  user_data), void *  user_data) CL_API_SUFFIX__VERSION_1_0
	{
		return cload_clBuildProgram(program, num_devices, device_list, options, pfn_notify, user_data);
	}


	using clCompileProgramPTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_program  program, cl_uint  num_devices, const cl_device_id *  device_list, const char *  options, cl_uint  num_input_headers, const cl_program *  input_headers, const char **  header_include_names, void (CL_CALLBACK *  pfn_notify)(cl_program  program, void *  user_data), void *  user_data) CL_API_SUFFIX__VERSION_1_2;
	extern clCompileProgramPTR cload_clCompileProgram;
	inline CL_API_ENTRY cl_int CL_API_CALL clCompileProgram(cl_program  program, cl_uint  num_devices, const cl_device_id *  device_list, const char *  options, cl_uint  num_input_headers, const cl_program *  input_headers, const char **  header_include_names, void (CL_CALLBACK *  pfn_notify)(cl_program  program, void *  user_data), void *  user_data) CL_API_SUFFIX__VERSION_1_2
	{
		return cload_clCompileProgram(program, num_devices, device_list, options, num_input_headers, input_headers, header_include_names, pfn_notify, user_data);
	}


	using clLinkProgramPTR = CL_API_ENTRY cl_program(CL_API_CALL *)(cl_context  context, cl_uint  num_devices, const cl_device_id *  device_list, const char *  options, cl_uint  num_input_programs, const cl_program *  input_programs, void (CL_CALLBACK *  pfn_notify)(cl_program  program, void *  user_data), void *  user_data, cl_int *  errcode_ret) CL_API_SUFFIX__VERSION_1_2;
	extern clLinkProgramPTR cload_clLinkProgram;
	inline CL_API_ENTRY cl_program CL_API_CALL clLinkProgram(cl_context  context, cl_uint  num_devices, const cl_device_id *  device_list, const char *  options, cl_uint  num_input_programs, const cl_program *  input_programs, void (CL_CALLBACK *  pfn_notify)(cl_program  program, void *  user_data), void *  user_data, cl_int *  errcode_ret) CL_API_SUFFIX__VERSION_1_2
	{
		return cload_clLinkProgram(context, num_devices, device_list, options, num_input_programs, input_programs, pfn_notify, user_data, errcode_ret);
	}


	using clSetProgramReleaseCallbackPTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_program  program, void (CL_CALLBACK *  pfn_notify)(cl_program  program, void *  user_data), void *  user_data) CL_API_SUFFIX__VERSION_2_2;
	extern clSetProgramReleaseCallbackPTR cload_clSetProgramReleaseCallback;
	inline CL_API_ENTRY cl_int CL_API_CALL clSetProgramReleaseCallback(cl_program  program, void (CL_CALLBACK *  pfn_notify)(cl_program  program, void *  user_data), void *  user_data) CL_API_SUFFIX__VERSION_2_2
	{
		return cload_clSetProgramReleaseCallback(program, pfn_notify, user_data);
	}


	using clSetProgramSpecializationConstantPTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_program  program, cl_uint  spec_id, size_t  spec_size, const void*  spec_value) CL_API_SUFFIX__VERSION_2_2;
	extern clSetProgramSpecializationConstantPTR cload_clSetProgramSpecializationConstant;
	inline CL_API_ENTRY cl_int CL_API_CALL clSetProgramSpecializationConstant(cl_program  program, cl_uint  spec_id, size_t  spec_size, const void*  spec_value) CL_API_SUFFIX__VERSION_2_2
	{
		return cload_clSetProgramSpecializationConstant(program, spec_id, spec_size, spec_value);
	}



	using clUnloadPlatformCompilerPTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_platform_id  platform) CL_API_SUFFIX__VERSION_1_2;
	extern clUnloadPlatformCompilerPTR cload_clUnloadPlatformCompiler;
	inline CL_API_ENTRY cl_int CL_API_CALL clUnloadPlatformCompiler(cl_platform_id  platform) CL_API_SUFFIX__VERSION_1_2
	{
		return cload_clUnloadPlatformCompiler(platform);
	}


	using clGetProgramInfoPTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_program  program, cl_program_info  param_name, size_t  param_value_size, void *  param_value, size_t *  param_value_size_ret) CL_API_SUFFIX__VERSION_1_0;
	extern clGetProgramInfoPTR cload_clGetProgramInfo;
	inline CL_API_ENTRY cl_int CL_API_CALL clGetProgramInfo(cl_program  program, cl_program_info  param_name, size_t  param_value_size, void *  param_value, size_t *  param_value_size_ret) CL_API_SUFFIX__VERSION_1_0
	{
		return cload_clGetProgramInfo(program, param_name, param_value_size, param_value, param_value_size_ret);
	}


	using clGetProgramBuildInfoPTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_program  program, cl_device_id  device, cl_program_build_info  param_name, size_t  param_value_size, void *  param_value, size_t *  param_value_size_ret) CL_API_SUFFIX__VERSION_1_0;
	extern clGetProgramBuildInfoPTR cload_clGetProgramBuildInfo;
	inline CL_API_ENTRY cl_int CL_API_CALL clGetProgramBuildInfo(cl_program  program, cl_device_id  device, cl_program_build_info  param_name, size_t  param_value_size, void *  param_value, size_t *  param_value_size_ret) CL_API_SUFFIX__VERSION_1_0
	{
		return cload_clGetProgramBuildInfo(program, device, param_name, param_value_size, param_value, param_value_size_ret);
	}


	/* Kernel Object APIs */
	using clCreateKernelPTR = CL_API_ENTRY cl_kernel(CL_API_CALL *)(cl_program  program, const char *  kernel_name, cl_int *  errcode_ret) CL_API_SUFFIX__VERSION_1_0;
	extern clCreateKernelPTR cload_clCreateKernel;
	inline CL_API_ENTRY cl_kernel CL_API_CALL clCreateKernel(cl_program  program, const char *  kernel_name, cl_int *  errcode_ret) CL_API_SUFFIX__VERSION_1_0
	{
		return cload_clCreateKernel(program, kernel_name, errcode_ret);
	}


	using clCreateKernelsInProgramPTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_program  program, cl_uint  num_kernels, cl_kernel *  kernels, cl_uint *  num_kernels_ret) CL_API_SUFFIX__VERSION_1_0;
	extern clCreateKernelsInProgramPTR cload_clCreateKernelsInProgram;
	inline CL_API_ENTRY cl_int CL_API_CALL clCreateKernelsInProgram(cl_program  program, cl_uint  num_kernels, cl_kernel *  kernels, cl_uint *  num_kernels_ret) CL_API_SUFFIX__VERSION_1_0
	{
		return cload_clCreateKernelsInProgram(program, num_kernels, kernels, num_kernels_ret);
	}


	using clCloneKernelPTR = CL_API_ENTRY cl_kernel(CL_API_CALL *)(cl_kernel  source_kernel, cl_int*  errcode_ret) CL_API_SUFFIX__VERSION_2_1;
	extern clCloneKernelPTR cload_clCloneKernel;
	inline CL_API_ENTRY cl_kernel CL_API_CALL clCloneKernel(cl_kernel  source_kernel, cl_int*  errcode_ret) CL_API_SUFFIX__VERSION_2_1
	{
		return cload_clCloneKernel(source_kernel, errcode_ret);
	}


	using clRetainKernelPTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_kernel  kernel) CL_API_SUFFIX__VERSION_1_0;
	extern clRetainKernelPTR cload_clRetainKernel;
	inline CL_API_ENTRY cl_int CL_API_CALL clRetainKernel(cl_kernel  kernel) CL_API_SUFFIX__VERSION_1_0
	{
		return cload_clRetainKernel(kernel);
	}


	using clReleaseKernelPTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_kernel  kernel) CL_API_SUFFIX__VERSION_1_0;
	extern clReleaseKernelPTR cload_clReleaseKernel;
	inline CL_API_ENTRY cl_int CL_API_CALL clReleaseKernel(cl_kernel  kernel) CL_API_SUFFIX__VERSION_1_0
	{
		return cload_clReleaseKernel(kernel);
	}


	using clSetKernelArgPTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_kernel  kernel, cl_uint  arg_index, size_t  arg_size, const void *  arg_value) CL_API_SUFFIX__VERSION_1_0;
	extern clSetKernelArgPTR cload_clSetKernelArg;
	inline CL_API_ENTRY cl_int CL_API_CALL clSetKernelArg(cl_kernel  kernel, cl_uint  arg_index, size_t  arg_size, const void *  arg_value) CL_API_SUFFIX__VERSION_1_0
	{
		return cload_clSetKernelArg(kernel, arg_index, arg_size, arg_value);
	}


	using clSetKernelArgSVMPointerPTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_kernel  kernel, cl_uint  arg_index, const void *  arg_value) CL_API_SUFFIX__VERSION_2_0;
	extern clSetKernelArgSVMPointerPTR cload_clSetKernelArgSVMPointer;
	inline CL_API_ENTRY cl_int CL_API_CALL clSetKernelArgSVMPointer(cl_kernel  kernel, cl_uint  arg_index, const void *  arg_value) CL_API_SUFFIX__VERSION_2_0
	{
		return cload_clSetKernelArgSVMPointer(kernel, arg_index, arg_value);
	}


	using clSetKernelExecInfoPTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_kernel  kernel, cl_kernel_exec_info  param_name, size_t  param_value_size, const void *  param_value) CL_API_SUFFIX__VERSION_2_0;
	extern clSetKernelExecInfoPTR cload_clSetKernelExecInfo;
	inline CL_API_ENTRY cl_int CL_API_CALL clSetKernelExecInfo(cl_kernel  kernel, cl_kernel_exec_info  param_name, size_t  param_value_size, const void *  param_value) CL_API_SUFFIX__VERSION_2_0
	{
		return cload_clSetKernelExecInfo(kernel, param_name, param_value_size, param_value);
	}


	using clGetKernelInfoPTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_kernel  kernel, cl_kernel_info  param_name, size_t  param_value_size, void *  param_value, size_t *  param_value_size_ret) CL_API_SUFFIX__VERSION_1_0;
	extern clGetKernelInfoPTR cload_clGetKernelInfo;
	inline CL_API_ENTRY cl_int CL_API_CALL clGetKernelInfo(cl_kernel  kernel, cl_kernel_info  param_name, size_t  param_value_size, void *  param_value, size_t *  param_value_size_ret) CL_API_SUFFIX__VERSION_1_0
	{
		return cload_clGetKernelInfo(kernel, param_name, param_value_size, param_value, param_value_size_ret);
	}


	using clGetKernelArgInfoPTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_kernel  kernel, cl_uint  arg_indx, cl_kernel_arg_info  param_name, size_t  param_value_size, void *  param_value, size_t *  param_value_size_ret) CL_API_SUFFIX__VERSION_1_2;
	extern clGetKernelArgInfoPTR cload_clGetKernelArgInfo;
	inline CL_API_ENTRY cl_int CL_API_CALL clGetKernelArgInfo(cl_kernel  kernel, cl_uint  arg_indx, cl_kernel_arg_info  param_name, size_t  param_value_size, void *  param_value, size_t *  param_value_size_ret) CL_API_SUFFIX__VERSION_1_2
	{
		return cload_clGetKernelArgInfo(kernel, arg_indx, param_name, param_value_size, param_value, param_value_size_ret);
	}


	using clGetKernelWorkGroupInfoPTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_kernel  kernel, cl_device_id  device, cl_kernel_work_group_info  param_name, size_t  param_value_size, void *  param_value, size_t *  param_value_size_ret) CL_API_SUFFIX__VERSION_1_0;
	extern clGetKernelWorkGroupInfoPTR cload_clGetKernelWorkGroupInfo;
	inline CL_API_ENTRY cl_int CL_API_CALL clGetKernelWorkGroupInfo(cl_kernel  kernel, cl_device_id  device, cl_kernel_work_group_info  param_name, size_t  param_value_size, void *  param_value, size_t *  param_value_size_ret) CL_API_SUFFIX__VERSION_1_0
	{
		return cload_clGetKernelWorkGroupInfo(kernel, device, param_name, param_value_size, param_value, param_value_size_ret);
	}


	using clGetKernelSubGroupInfoPTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_kernel  kernel, cl_device_id  device, cl_kernel_sub_group_info  param_name, size_t  input_value_size, const void* input_value, size_t  param_value_size, void*  param_value, size_t*  param_value_size_ret) CL_API_SUFFIX__VERSION_2_1;
	extern clGetKernelSubGroupInfoPTR cload_clGetKernelSubGroupInfo;
	inline CL_API_ENTRY cl_int CL_API_CALL clGetKernelSubGroupInfo(cl_kernel  kernel, cl_device_id  device, cl_kernel_sub_group_info  param_name, size_t  input_value_size, const void* input_value, size_t  param_value_size, void*  param_value, size_t*  param_value_size_ret) CL_API_SUFFIX__VERSION_2_1
	{
		return cload_clGetKernelSubGroupInfo(kernel, device, param_name, input_value_size, input_value, param_value_size, param_value, param_value_size_ret);
	}



	/* Event Object APIs */
	using clWaitForEventsPTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_uint  num_events, const cl_event *  event_list) CL_API_SUFFIX__VERSION_1_0;
	extern clWaitForEventsPTR cload_clWaitForEvents;
	inline CL_API_ENTRY cl_int CL_API_CALL clWaitForEvents(cl_uint  num_events, const cl_event *  event_list) CL_API_SUFFIX__VERSION_1_0
	{
		return cload_clWaitForEvents(num_events, event_list);
	}


	using clGetEventInfoPTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_event  event, cl_event_info  param_name, size_t  param_value_size, void *  param_value, size_t *  param_value_size_ret) CL_API_SUFFIX__VERSION_1_0;
	extern clGetEventInfoPTR cload_clGetEventInfo;
	inline CL_API_ENTRY cl_int CL_API_CALL clGetEventInfo(cl_event  event, cl_event_info  param_name, size_t  param_value_size, void *  param_value, size_t *  param_value_size_ret) CL_API_SUFFIX__VERSION_1_0
	{
		return cload_clGetEventInfo(event, param_name, param_value_size, param_value, param_value_size_ret);
	}


	using clCreateUserEventPTR = CL_API_ENTRY cl_event(CL_API_CALL *)(cl_context  context, cl_int *  errcode_ret) CL_API_SUFFIX__VERSION_1_1;
	extern clCreateUserEventPTR cload_clCreateUserEvent;
	inline CL_API_ENTRY cl_event CL_API_CALL clCreateUserEvent(cl_context  context, cl_int *  errcode_ret) CL_API_SUFFIX__VERSION_1_1
	{
		return cload_clCreateUserEvent(context, errcode_ret);
	}


	using clRetainEventPTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_event  event) CL_API_SUFFIX__VERSION_1_0;
	extern clRetainEventPTR cload_clRetainEvent;
	inline CL_API_ENTRY cl_int CL_API_CALL clRetainEvent(cl_event  event) CL_API_SUFFIX__VERSION_1_0
	{
		return cload_clRetainEvent(event);
	}


	using clReleaseEventPTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_event  event) CL_API_SUFFIX__VERSION_1_0;
	extern clReleaseEventPTR cload_clReleaseEvent;
	inline CL_API_ENTRY cl_int CL_API_CALL clReleaseEvent(cl_event  event) CL_API_SUFFIX__VERSION_1_0
	{
		return cload_clReleaseEvent(event);
	}


	using clSetUserEventStatusPTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_event  event, cl_int  execution_status) CL_API_SUFFIX__VERSION_1_1;
	extern clSetUserEventStatusPTR cload_clSetUserEventStatus;
	inline CL_API_ENTRY cl_int CL_API_CALL clSetUserEventStatus(cl_event  event, cl_int  execution_status) CL_API_SUFFIX__VERSION_1_1
	{
		return cload_clSetUserEventStatus(event, execution_status);
	}


	using clSetEventCallbackPTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_event  event, cl_int  command_exec_callback_type, void (CL_CALLBACK *  pfn_notify)(cl_event, cl_int, void *), void *  user_data) CL_API_SUFFIX__VERSION_1_1;
	extern clSetEventCallbackPTR cload_clSetEventCallback;
	inline CL_API_ENTRY cl_int CL_API_CALL clSetEventCallback(cl_event  event, cl_int  command_exec_callback_type, void (CL_CALLBACK *  pfn_notify)(cl_event, cl_int, void *), void *  user_data) CL_API_SUFFIX__VERSION_1_1
	{
		return cload_clSetEventCallback(event, command_exec_callback_type, pfn_notify, user_data);
	}


	/* Profiling APIs */
	using clGetEventProfilingInfoPTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_event  event, cl_profiling_info  param_name, size_t  param_value_size, void *  param_value, size_t *  param_value_size_ret) CL_API_SUFFIX__VERSION_1_0;
	extern clGetEventProfilingInfoPTR cload_clGetEventProfilingInfo;
	inline CL_API_ENTRY cl_int CL_API_CALL clGetEventProfilingInfo(cl_event  event, cl_profiling_info  param_name, size_t  param_value_size, void *  param_value, size_t *  param_value_size_ret) CL_API_SUFFIX__VERSION_1_0
	{
		return cload_clGetEventProfilingInfo(event, param_name, param_value_size, param_value, param_value_size_ret);
	}


	/* Flush and Finish APIs */
	using clFlushPTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_command_queue  command_queue) CL_API_SUFFIX__VERSION_1_0;
	extern clFlushPTR cload_clFlush;
	inline CL_API_ENTRY cl_int CL_API_CALL clFlush(cl_command_queue  command_queue) CL_API_SUFFIX__VERSION_1_0
	{
		return cload_clFlush(command_queue);
	}


	using clFinishPTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_command_queue  command_queue) CL_API_SUFFIX__VERSION_1_0;
	extern clFinishPTR cload_clFinish;
	inline CL_API_ENTRY cl_int CL_API_CALL clFinish(cl_command_queue  command_queue) CL_API_SUFFIX__VERSION_1_0
	{
		return cload_clFinish(command_queue);
	}


	/* Enqueued Commands APIs */
	using clEnqueueReadBufferPTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_command_queue  command_queue, cl_mem  buffer, cl_bool  blocking_read, size_t  offset, size_t  size, void *  ptr, cl_uint  num_events_in_wait_list, const cl_event *  event_wait_list, cl_event *  event) CL_API_SUFFIX__VERSION_1_0;
	extern clEnqueueReadBufferPTR cload_clEnqueueReadBuffer;
	inline CL_API_ENTRY cl_int CL_API_CALL clEnqueueReadBuffer(cl_command_queue  command_queue, cl_mem  buffer, cl_bool  blocking_read, size_t  offset, size_t  size, void *  ptr, cl_uint  num_events_in_wait_list, const cl_event *  event_wait_list, cl_event *  event) CL_API_SUFFIX__VERSION_1_0
	{
		return cload_clEnqueueReadBuffer(command_queue, buffer, blocking_read, offset, size, ptr, num_events_in_wait_list, event_wait_list, event);
	}


	using clEnqueueReadBufferRectPTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_command_queue  command_queue, cl_mem  buffer, cl_bool  blocking_read, const size_t *  buffer_offset, const size_t *  host_offset, const size_t *  region, size_t  buffer_row_pitch, size_t  buffer_slice_pitch, size_t  host_row_pitch, size_t  host_slice_pitch, void *  ptr, cl_uint  num_events_in_wait_list, const cl_event *  event_wait_list, cl_event *  event) CL_API_SUFFIX__VERSION_1_1;
	extern clEnqueueReadBufferRectPTR cload_clEnqueueReadBufferRect;
	inline CL_API_ENTRY cl_int CL_API_CALL clEnqueueReadBufferRect(cl_command_queue  command_queue, cl_mem  buffer, cl_bool  blocking_read, const size_t *  buffer_offset, const size_t *  host_offset, const size_t *  region, size_t  buffer_row_pitch, size_t  buffer_slice_pitch, size_t  host_row_pitch, size_t  host_slice_pitch, void *  ptr, cl_uint  num_events_in_wait_list, const cl_event *  event_wait_list, cl_event *  event) CL_API_SUFFIX__VERSION_1_1
	{
		return cload_clEnqueueReadBufferRect(command_queue, buffer, blocking_read, buffer_offset, host_offset, region, buffer_row_pitch, buffer_slice_pitch, host_row_pitch, host_slice_pitch, ptr, num_events_in_wait_list, event_wait_list, event);
	}


	using clEnqueueWriteBufferPTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_command_queue  command_queue, cl_mem  buffer, cl_bool  blocking_write, size_t  offset, size_t  size, const void *  ptr, cl_uint  num_events_in_wait_list, const cl_event *  event_wait_list, cl_event *  event) CL_API_SUFFIX__VERSION_1_0;
	extern clEnqueueWriteBufferPTR cload_clEnqueueWriteBuffer;
	inline CL_API_ENTRY cl_int CL_API_CALL clEnqueueWriteBuffer(cl_command_queue  command_queue, cl_mem  buffer, cl_bool  blocking_write, size_t  offset, size_t  size, const void *  ptr, cl_uint  num_events_in_wait_list, const cl_event *  event_wait_list, cl_event *  event) CL_API_SUFFIX__VERSION_1_0
	{
		return cload_clEnqueueWriteBuffer(command_queue, buffer, blocking_write, offset, size, ptr, num_events_in_wait_list, event_wait_list, event);
	}


	using clEnqueueWriteBufferRectPTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_command_queue  command_queue, cl_mem  buffer, cl_bool  blocking_write, const size_t *  buffer_offset, const size_t *  host_offset, const size_t *  region, size_t  buffer_row_pitch, size_t  buffer_slice_pitch, size_t  host_row_pitch, size_t  host_slice_pitch, const void *  ptr, cl_uint  num_events_in_wait_list, const cl_event *  event_wait_list, cl_event *  event) CL_API_SUFFIX__VERSION_1_1;
	extern clEnqueueWriteBufferRectPTR cload_clEnqueueWriteBufferRect;
	inline CL_API_ENTRY cl_int CL_API_CALL clEnqueueWriteBufferRect(cl_command_queue  command_queue, cl_mem  buffer, cl_bool  blocking_write, const size_t *  buffer_offset, const size_t *  host_offset, const size_t *  region, size_t  buffer_row_pitch, size_t  buffer_slice_pitch, size_t  host_row_pitch, size_t  host_slice_pitch, const void *  ptr, cl_uint  num_events_in_wait_list, const cl_event *  event_wait_list, cl_event *  event) CL_API_SUFFIX__VERSION_1_1
	{
		return cload_clEnqueueWriteBufferRect(command_queue, buffer, blocking_write, buffer_offset, host_offset, region, buffer_row_pitch, buffer_slice_pitch, host_row_pitch, host_slice_pitch, ptr, num_events_in_wait_list, event_wait_list, event);
	}


	using clEnqueueFillBufferPTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_command_queue  command_queue, cl_mem  buffer, const void *  pattern, size_t  pattern_size, size_t  offset, size_t  size, cl_uint  num_events_in_wait_list, const cl_event *  event_wait_list, cl_event *  event) CL_API_SUFFIX__VERSION_1_2;
	extern clEnqueueFillBufferPTR cload_clEnqueueFillBuffer;
	inline CL_API_ENTRY cl_int CL_API_CALL clEnqueueFillBuffer(cl_command_queue  command_queue, cl_mem  buffer, const void *  pattern, size_t  pattern_size, size_t  offset, size_t  size, cl_uint  num_events_in_wait_list, const cl_event *  event_wait_list, cl_event *  event) CL_API_SUFFIX__VERSION_1_2
	{
		return cload_clEnqueueFillBuffer(command_queue, buffer, pattern, pattern_size, offset, size, num_events_in_wait_list, event_wait_list, event);
	}


	using clEnqueueCopyBufferPTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_command_queue  command_queue, cl_mem  src_buffer, cl_mem  dst_buffer, size_t  src_offset, size_t  dst_offset, size_t  size, cl_uint  num_events_in_wait_list, const cl_event *  event_wait_list, cl_event *  event) CL_API_SUFFIX__VERSION_1_0;
	extern clEnqueueCopyBufferPTR cload_clEnqueueCopyBuffer;
	inline CL_API_ENTRY cl_int CL_API_CALL clEnqueueCopyBuffer(cl_command_queue  command_queue, cl_mem  src_buffer, cl_mem  dst_buffer, size_t  src_offset, size_t  dst_offset, size_t  size, cl_uint  num_events_in_wait_list, const cl_event *  event_wait_list, cl_event *  event) CL_API_SUFFIX__VERSION_1_0
	{
		return cload_clEnqueueCopyBuffer(command_queue, src_buffer, dst_buffer, src_offset, dst_offset, size, num_events_in_wait_list, event_wait_list, event);
	}


	using clEnqueueCopyBufferRectPTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_command_queue  command_queue, cl_mem  src_buffer, cl_mem  dst_buffer, const size_t *  src_origin, const size_t *  dst_origin, const size_t *  region, size_t  src_row_pitch, size_t  src_slice_pitch, size_t  dst_row_pitch, size_t  dst_slice_pitch, cl_uint  num_events_in_wait_list, const cl_event *  event_wait_list, cl_event *  event) CL_API_SUFFIX__VERSION_1_1;
	extern clEnqueueCopyBufferRectPTR cload_clEnqueueCopyBufferRect;
	inline CL_API_ENTRY cl_int CL_API_CALL clEnqueueCopyBufferRect(cl_command_queue  command_queue, cl_mem  src_buffer, cl_mem  dst_buffer, const size_t *  src_origin, const size_t *  dst_origin, const size_t *  region, size_t  src_row_pitch, size_t  src_slice_pitch, size_t  dst_row_pitch, size_t  dst_slice_pitch, cl_uint  num_events_in_wait_list, const cl_event *  event_wait_list, cl_event *  event) CL_API_SUFFIX__VERSION_1_1
	{
		return cload_clEnqueueCopyBufferRect(command_queue, src_buffer, dst_buffer, src_origin, dst_origin, region, src_row_pitch, src_slice_pitch, dst_row_pitch, dst_slice_pitch, num_events_in_wait_list, event_wait_list, event);
	}


	using clEnqueueReadImagePTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_command_queue  command_queue, cl_mem  image, cl_bool  blocking_read, const size_t *  origin, const size_t *  region, size_t  row_pitch, size_t  slice_pitch, void *  ptr, cl_uint  num_events_in_wait_list, const cl_event *  event_wait_list, cl_event *  event) CL_API_SUFFIX__VERSION_1_0;
	extern clEnqueueReadImagePTR cload_clEnqueueReadImage;
	inline CL_API_ENTRY cl_int CL_API_CALL clEnqueueReadImage(cl_command_queue  command_queue, cl_mem  image, cl_bool  blocking_read, const size_t *  origin, const size_t *  region, size_t  row_pitch, size_t  slice_pitch, void *  ptr, cl_uint  num_events_in_wait_list, const cl_event *  event_wait_list, cl_event *  event) CL_API_SUFFIX__VERSION_1_0
	{
		return cload_clEnqueueReadImage(command_queue, image, blocking_read, origin, region, row_pitch, slice_pitch, ptr, num_events_in_wait_list, event_wait_list, event);
	}


	using clEnqueueWriteImagePTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_command_queue  command_queue, cl_mem  image, cl_bool  blocking_write, const size_t *  origin, const size_t *  region, size_t  input_row_pitch, size_t  input_slice_pitch, const void *  ptr, cl_uint  num_events_in_wait_list, const cl_event *  event_wait_list, cl_event *  event) CL_API_SUFFIX__VERSION_1_0;
	extern clEnqueueWriteImagePTR cload_clEnqueueWriteImage;
	inline CL_API_ENTRY cl_int CL_API_CALL clEnqueueWriteImage(cl_command_queue  command_queue, cl_mem  image, cl_bool  blocking_write, const size_t *  origin, const size_t *  region, size_t  input_row_pitch, size_t  input_slice_pitch, const void *  ptr, cl_uint  num_events_in_wait_list, const cl_event *  event_wait_list, cl_event *  event) CL_API_SUFFIX__VERSION_1_0
	{
		return cload_clEnqueueWriteImage(command_queue, image, blocking_write, origin, region, input_row_pitch, input_slice_pitch, ptr, num_events_in_wait_list, event_wait_list, event);
	}


	using clEnqueueFillImagePTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_command_queue  command_queue, cl_mem  image, const void *  fill_color, const size_t *  origin, const size_t *  region, cl_uint  num_events_in_wait_list, const cl_event *  event_wait_list, cl_event *  event) CL_API_SUFFIX__VERSION_1_2;
	extern clEnqueueFillImagePTR cload_clEnqueueFillImage;
	inline CL_API_ENTRY cl_int CL_API_CALL clEnqueueFillImage(cl_command_queue  command_queue, cl_mem  image, const void *  fill_color, const size_t *  origin, const size_t *  region, cl_uint  num_events_in_wait_list, const cl_event *  event_wait_list, cl_event *  event) CL_API_SUFFIX__VERSION_1_2
	{
		return cload_clEnqueueFillImage(command_queue, image, fill_color, origin, region, num_events_in_wait_list, event_wait_list, event);
	}


	using clEnqueueCopyImagePTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_command_queue  command_queue, cl_mem  src_image, cl_mem  dst_image, const size_t *  src_origin, const size_t *  dst_origin, const size_t *  region, cl_uint  num_events_in_wait_list, const cl_event *  event_wait_list, cl_event *  event) CL_API_SUFFIX__VERSION_1_0;
	extern clEnqueueCopyImagePTR cload_clEnqueueCopyImage;
	inline CL_API_ENTRY cl_int CL_API_CALL clEnqueueCopyImage(cl_command_queue  command_queue, cl_mem  src_image, cl_mem  dst_image, const size_t *  src_origin, const size_t *  dst_origin, const size_t *  region, cl_uint  num_events_in_wait_list, const cl_event *  event_wait_list, cl_event *  event) CL_API_SUFFIX__VERSION_1_0
	{
		return cload_clEnqueueCopyImage(command_queue, src_image, dst_image, src_origin, dst_origin, region, num_events_in_wait_list, event_wait_list, event);
	}


	using clEnqueueCopyImageToBufferPTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_command_queue  command_queue, cl_mem  src_image, cl_mem  dst_buffer, const size_t *  src_origin, const size_t *  region, size_t  dst_offset, cl_uint  num_events_in_wait_list, const cl_event *  event_wait_list, cl_event *  event) CL_API_SUFFIX__VERSION_1_0;
	extern clEnqueueCopyImageToBufferPTR cload_clEnqueueCopyImageToBuffer;
	inline CL_API_ENTRY cl_int CL_API_CALL clEnqueueCopyImageToBuffer(cl_command_queue  command_queue, cl_mem  src_image, cl_mem  dst_buffer, const size_t *  src_origin, const size_t *  region, size_t  dst_offset, cl_uint  num_events_in_wait_list, const cl_event *  event_wait_list, cl_event *  event) CL_API_SUFFIX__VERSION_1_0
	{
		return cload_clEnqueueCopyImageToBuffer(command_queue, src_image, dst_buffer, src_origin, region, dst_offset, num_events_in_wait_list, event_wait_list, event);
	}


	using clEnqueueCopyBufferToImagePTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_command_queue  command_queue, cl_mem  src_buffer, cl_mem  dst_image, size_t  src_offset, const size_t *  dst_origin, const size_t *  region, cl_uint  num_events_in_wait_list, const cl_event *  event_wait_list, cl_event *  event) CL_API_SUFFIX__VERSION_1_0;
	extern clEnqueueCopyBufferToImagePTR cload_clEnqueueCopyBufferToImage;
	inline CL_API_ENTRY cl_int CL_API_CALL clEnqueueCopyBufferToImage(cl_command_queue  command_queue, cl_mem  src_buffer, cl_mem  dst_image, size_t  src_offset, const size_t *  dst_origin, const size_t *  region, cl_uint  num_events_in_wait_list, const cl_event *  event_wait_list, cl_event *  event) CL_API_SUFFIX__VERSION_1_0
	{
		return cload_clEnqueueCopyBufferToImage(command_queue, src_buffer, dst_image, src_offset, dst_origin, region, num_events_in_wait_list, event_wait_list, event);
	}


	using clEnqueueMapBufferPTR = CL_API_ENTRY void * (CL_API_CALL *)(cl_command_queue  command_queue, cl_mem  buffer, cl_bool  blocking_map, cl_map_flags  map_flags, size_t  offset, size_t  size, cl_uint  num_events_in_wait_list, const cl_event *  event_wait_list, cl_event *  event, cl_int *  errcode_ret) CL_API_SUFFIX__VERSION_1_0;
	extern clEnqueueMapBufferPTR cload_clEnqueueMapBuffer;
	inline CL_API_ENTRY void * CL_API_CALL clEnqueueMapBuffer(cl_command_queue  command_queue, cl_mem  buffer, cl_bool  blocking_map, cl_map_flags  map_flags, size_t  offset, size_t  size, cl_uint  num_events_in_wait_list, const cl_event *  event_wait_list, cl_event *  event, cl_int *  errcode_ret) CL_API_SUFFIX__VERSION_1_0
	{
		return cload_clEnqueueMapBuffer(command_queue, buffer, blocking_map, map_flags, offset, size, num_events_in_wait_list, event_wait_list, event, errcode_ret);
	}


	using clEnqueueMapImagePTR = CL_API_ENTRY void * (CL_API_CALL *)(cl_command_queue  command_queue, cl_mem  image, cl_bool  blocking_map, cl_map_flags  map_flags, const size_t *  origin, const size_t *  region, size_t *  image_row_pitch, size_t *  image_slice_pitch, cl_uint  num_events_in_wait_list, const cl_event *  event_wait_list, cl_event *  event, cl_int *  errcode_ret) CL_API_SUFFIX__VERSION_1_0;
	extern clEnqueueMapImagePTR cload_clEnqueueMapImage;
	inline CL_API_ENTRY void * CL_API_CALL clEnqueueMapImage(cl_command_queue  command_queue, cl_mem  image, cl_bool  blocking_map, cl_map_flags  map_flags, const size_t *  origin, const size_t *  region, size_t *  image_row_pitch, size_t *  image_slice_pitch, cl_uint  num_events_in_wait_list, const cl_event *  event_wait_list, cl_event *  event, cl_int *  errcode_ret) CL_API_SUFFIX__VERSION_1_0
	{
		return cload_clEnqueueMapImage(command_queue, image, blocking_map, map_flags, origin, region, image_row_pitch, image_slice_pitch, num_events_in_wait_list, event_wait_list, event, errcode_ret);
	}


	using clEnqueueUnmapMemObjectPTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_command_queue  command_queue, cl_mem  memobj, void *  mapped_ptr, cl_uint  num_events_in_wait_list, const cl_event *  event_wait_list, cl_event *  event) CL_API_SUFFIX__VERSION_1_0;
	extern clEnqueueUnmapMemObjectPTR cload_clEnqueueUnmapMemObject;
	inline CL_API_ENTRY cl_int CL_API_CALL clEnqueueUnmapMemObject(cl_command_queue  command_queue, cl_mem  memobj, void *  mapped_ptr, cl_uint  num_events_in_wait_list, const cl_event *  event_wait_list, cl_event *  event) CL_API_SUFFIX__VERSION_1_0
	{
		return cload_clEnqueueUnmapMemObject(command_queue, memobj, mapped_ptr, num_events_in_wait_list, event_wait_list, event);
	}


	using clEnqueueMigrateMemObjectsPTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_command_queue  command_queue, cl_uint  num_mem_objects, const cl_mem *  mem_objects, cl_mem_migration_flags  flags, cl_uint  num_events_in_wait_list, const cl_event *  event_wait_list, cl_event *  event) CL_API_SUFFIX__VERSION_1_2;
	extern clEnqueueMigrateMemObjectsPTR cload_clEnqueueMigrateMemObjects;
	inline CL_API_ENTRY cl_int CL_API_CALL clEnqueueMigrateMemObjects(cl_command_queue  command_queue, cl_uint  num_mem_objects, const cl_mem *  mem_objects, cl_mem_migration_flags  flags, cl_uint  num_events_in_wait_list, const cl_event *  event_wait_list, cl_event *  event) CL_API_SUFFIX__VERSION_1_2
	{
		return cload_clEnqueueMigrateMemObjects(command_queue, num_mem_objects, mem_objects, flags, num_events_in_wait_list, event_wait_list, event);
	}


	using clEnqueueNDRangeKernelPTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_command_queue  command_queue, cl_kernel  kernel, cl_uint  work_dim, const size_t *  global_work_offset, const size_t *  global_work_size, const size_t *  local_work_size, cl_uint  num_events_in_wait_list, const cl_event *  event_wait_list, cl_event *  event) CL_API_SUFFIX__VERSION_1_0;
	extern clEnqueueNDRangeKernelPTR cload_clEnqueueNDRangeKernel;
	inline CL_API_ENTRY cl_int CL_API_CALL clEnqueueNDRangeKernel(cl_command_queue  command_queue, cl_kernel  kernel, cl_uint  work_dim, const size_t *  global_work_offset, const size_t *  global_work_size, const size_t *  local_work_size, cl_uint  num_events_in_wait_list, const cl_event *  event_wait_list, cl_event *  event) CL_API_SUFFIX__VERSION_1_0
	{
		return cload_clEnqueueNDRangeKernel(command_queue, kernel, work_dim, global_work_offset, global_work_size, local_work_size, num_events_in_wait_list, event_wait_list, event);
	}


	using clEnqueueNativeKernelPTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_command_queue  command_queue, void (CL_CALLBACK * user_func)(void *), void *  args, size_t  cb_args, cl_uint  num_mem_objects, const cl_mem *  mem_list, const void **  args_mem_loc, cl_uint  num_events_in_wait_list, const cl_event *  event_wait_list, cl_event *  event) CL_API_SUFFIX__VERSION_1_0;
	extern clEnqueueNativeKernelPTR cload_clEnqueueNativeKernel;
	inline CL_API_ENTRY cl_int CL_API_CALL clEnqueueNativeKernel(cl_command_queue  command_queue, void (CL_CALLBACK * user_func)(void *), void *  args, size_t  cb_args, cl_uint  num_mem_objects, const cl_mem *  mem_list, const void **  args_mem_loc, cl_uint  num_events_in_wait_list, const cl_event *  event_wait_list, cl_event *  event) CL_API_SUFFIX__VERSION_1_0
	{
		return cload_clEnqueueNativeKernel(command_queue, user_func, args, cb_args, num_mem_objects, mem_list, args_mem_loc, num_events_in_wait_list, event_wait_list, event);
	}


	using clEnqueueMarkerWithWaitListPTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_command_queue  command_queue, cl_uint  num_events_in_wait_list, const cl_event *  event_wait_list, cl_event *  event) CL_API_SUFFIX__VERSION_1_2;
	extern clEnqueueMarkerWithWaitListPTR cload_clEnqueueMarkerWithWaitList;
	inline CL_API_ENTRY cl_int CL_API_CALL clEnqueueMarkerWithWaitList(cl_command_queue  command_queue, cl_uint  num_events_in_wait_list, const cl_event *  event_wait_list, cl_event *  event) CL_API_SUFFIX__VERSION_1_2
	{
		return cload_clEnqueueMarkerWithWaitList(command_queue, num_events_in_wait_list, event_wait_list, event);
	}


	using clEnqueueBarrierWithWaitListPTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_command_queue  command_queue, cl_uint  num_events_in_wait_list, const cl_event *  event_wait_list, cl_event *  event) CL_API_SUFFIX__VERSION_1_2;
	extern clEnqueueBarrierWithWaitListPTR cload_clEnqueueBarrierWithWaitList;
	inline CL_API_ENTRY cl_int CL_API_CALL clEnqueueBarrierWithWaitList(cl_command_queue  command_queue, cl_uint  num_events_in_wait_list, const cl_event *  event_wait_list, cl_event *  event) CL_API_SUFFIX__VERSION_1_2
	{
		return cload_clEnqueueBarrierWithWaitList(command_queue, num_events_in_wait_list, event_wait_list, event);
	}


	using clEnqueueSVMFreePTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_command_queue  command_queue, cl_uint  num_svm_pointers, void *  svm_pointers, void (CL_CALLBACK * pfn_free_func)(cl_command_queue  queue, cl_uint  num_svm_pointers, void *  svm_pointers, void *  user_data), void *  user_data, cl_uint  num_events_in_wait_list, const cl_event *  event_wait_list, cl_event *  event) CL_API_SUFFIX__VERSION_2_0;
	extern clEnqueueSVMFreePTR cload_clEnqueueSVMFree;
	inline CL_API_ENTRY cl_int CL_API_CALL clEnqueueSVMFree(cl_command_queue  command_queue, cl_uint  num_svm_pointers, void *  svm_pointers, void (CL_CALLBACK * pfn_free_func)(cl_command_queue  queue, cl_uint  num_svm_pointers, void *  svm_pointers, void *  user_data), void *  user_data, cl_uint  num_events_in_wait_list, const cl_event *  event_wait_list, cl_event *  event) CL_API_SUFFIX__VERSION_2_0
	{
		return cload_clEnqueueSVMFree(command_queue, num_svm_pointers, svm_pointers, pfn_free_func, user_data, num_events_in_wait_list, event_wait_list, event);
	}


	using clEnqueueSVMMemcpyPTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_command_queue  command_queue, cl_bool  blocking_copy, void *  dst_ptr, const void *  src_ptr, size_t  size, cl_uint  num_events_in_wait_list, const cl_event *  event_wait_list, cl_event *  event) CL_API_SUFFIX__VERSION_2_0;
	extern clEnqueueSVMMemcpyPTR cload_clEnqueueSVMMemcpy;
	inline CL_API_ENTRY cl_int CL_API_CALL clEnqueueSVMMemcpy(cl_command_queue  command_queue, cl_bool  blocking_copy, void *  dst_ptr, const void *  src_ptr, size_t  size, cl_uint  num_events_in_wait_list, const cl_event *  event_wait_list, cl_event *  event) CL_API_SUFFIX__VERSION_2_0
	{
		return cload_clEnqueueSVMMemcpy(command_queue, blocking_copy, dst_ptr, src_ptr, size, num_events_in_wait_list, event_wait_list, event);
	}


	using clEnqueueSVMMemFillPTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_command_queue  command_queue, void *  svm_ptr, const void *  pattern, size_t  pattern_size, size_t  size, cl_uint  num_events_in_wait_list, const cl_event *  event_wait_list, cl_event *  event) CL_API_SUFFIX__VERSION_2_0;
	extern clEnqueueSVMMemFillPTR cload_clEnqueueSVMMemFill;
	inline CL_API_ENTRY cl_int CL_API_CALL clEnqueueSVMMemFill(cl_command_queue  command_queue, void *  svm_ptr, const void *  pattern, size_t  pattern_size, size_t  size, cl_uint  num_events_in_wait_list, const cl_event *  event_wait_list, cl_event *  event) CL_API_SUFFIX__VERSION_2_0
	{
		return cload_clEnqueueSVMMemFill(command_queue, svm_ptr, pattern, pattern_size, size, num_events_in_wait_list, event_wait_list, event);
	}


	using clEnqueueSVMMapPTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_command_queue  command_queue, cl_bool  blocking_map, cl_map_flags  flags, void *  svm_ptr, size_t  size, cl_uint  num_events_in_wait_list, const cl_event *  event_wait_list, cl_event *  event) CL_API_SUFFIX__VERSION_2_0;
	extern clEnqueueSVMMapPTR cload_clEnqueueSVMMap;
	inline CL_API_ENTRY cl_int CL_API_CALL clEnqueueSVMMap(cl_command_queue  command_queue, cl_bool  blocking_map, cl_map_flags  flags, void *  svm_ptr, size_t  size, cl_uint  num_events_in_wait_list, const cl_event *  event_wait_list, cl_event *  event) CL_API_SUFFIX__VERSION_2_0
	{
		return cload_clEnqueueSVMMap(command_queue, blocking_map, flags, svm_ptr, size, num_events_in_wait_list, event_wait_list, event);
	}


	using clEnqueueSVMUnmapPTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_command_queue  command_queue, void *  svm_ptr, cl_uint  num_events_in_wait_list, const cl_event *  event_wait_list, cl_event *  event) CL_API_SUFFIX__VERSION_2_0;
	extern clEnqueueSVMUnmapPTR cload_clEnqueueSVMUnmap;
	inline CL_API_ENTRY cl_int CL_API_CALL clEnqueueSVMUnmap(cl_command_queue  command_queue, void *  svm_ptr, cl_uint  num_events_in_wait_list, const cl_event *  event_wait_list, cl_event *  event) CL_API_SUFFIX__VERSION_2_0
	{
		return cload_clEnqueueSVMUnmap(command_queue, svm_ptr, num_events_in_wait_list, event_wait_list, event);
	}


	using clEnqueueSVMMigrateMemPTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_command_queue  command_queue, cl_uint  num_svm_pointers, const void **  svm_pointers, const size_t *  sizes, cl_mem_migration_flags  flags, cl_uint  num_events_in_wait_list, const cl_event *  event_wait_list, cl_event *  event) CL_API_SUFFIX__VERSION_2_1;
	extern clEnqueueSVMMigrateMemPTR cload_clEnqueueSVMMigrateMem;
	inline CL_API_ENTRY cl_int CL_API_CALL clEnqueueSVMMigrateMem(cl_command_queue  command_queue, cl_uint  num_svm_pointers, const void **  svm_pointers, const size_t *  sizes, cl_mem_migration_flags  flags, cl_uint  num_events_in_wait_list, const cl_event *  event_wait_list, cl_event *  event) CL_API_SUFFIX__VERSION_2_1
	{
		return cload_clEnqueueSVMMigrateMem(command_queue, num_svm_pointers, svm_pointers, sizes, flags, num_events_in_wait_list, event_wait_list, event);
	}



	/* Extension function access
	*
	* Returns the extension function address for the given function name,
	* or NULL if a valid function can not be found.  The client must
	* check to make sure the address is not NULL, before using or
	* calling the returned function address.
	*/
	using clGetExtensionFunctionAddressForPlatformPTR = CL_API_ENTRY void * (CL_API_CALL *)(cl_platform_id  platform, const char *  func_name) CL_API_SUFFIX__VERSION_1_2;
	extern clGetExtensionFunctionAddressForPlatformPTR cload_clGetExtensionFunctionAddressForPlatform;
	inline CL_API_ENTRY void * CL_API_CALL clGetExtensionFunctionAddressForPlatform(cl_platform_id  platform, const char *  func_name) CL_API_SUFFIX__VERSION_1_2
	{
		return cload_clGetExtensionFunctionAddressForPlatform(platform, func_name);
	}



	/* Deprecated OpenCL 1.1 APIs */
	using clCreateImage2DPTR = CL_API_ENTRY CL_EXT_PREFIX__VERSION_1_1_DEPRECATED cl_mem(CL_API_CALL *)(cl_context  context, cl_mem_flags  flags, const cl_image_format *  image_format, size_t  image_width, size_t  image_height, size_t  image_row_pitch, void *  host_ptr, cl_int *  errcode_ret) CL_EXT_SUFFIX__VERSION_1_1_DEPRECATED;
	extern clCreateImage2DPTR cload_clCreateImage2D;
	inline CL_API_ENTRY CL_EXT_PREFIX__VERSION_1_1_DEPRECATED cl_mem CL_API_CALL clCreateImage2D(cl_context  context, cl_mem_flags  flags, const cl_image_format *  image_format, size_t  image_width, size_t  image_height, size_t  image_row_pitch, void *  host_ptr, cl_int *  errcode_ret) CL_EXT_SUFFIX__VERSION_1_1_DEPRECATED
	{
		return cload_clCreateImage2D(context, flags, image_format, image_width, image_height, image_row_pitch, host_ptr, errcode_ret);
	}


	using clCreateImage3DPTR = CL_API_ENTRY CL_EXT_PREFIX__VERSION_1_1_DEPRECATED cl_mem(CL_API_CALL *)(cl_context  context, cl_mem_flags  flags, const cl_image_format *  image_format, size_t  image_width, size_t  image_height, size_t  image_depth, size_t  image_row_pitch, size_t  image_slice_pitch, void *  host_ptr, cl_int *  errcode_ret) CL_EXT_SUFFIX__VERSION_1_1_DEPRECATED;
	extern clCreateImage3DPTR cload_clCreateImage3D;
	inline CL_API_ENTRY CL_EXT_PREFIX__VERSION_1_1_DEPRECATED cl_mem CL_API_CALL clCreateImage3D(cl_context  context, cl_mem_flags  flags, const cl_image_format *  image_format, size_t  image_width, size_t  image_height, size_t  image_depth, size_t  image_row_pitch, size_t  image_slice_pitch, void *  host_ptr, cl_int *  errcode_ret) CL_EXT_SUFFIX__VERSION_1_1_DEPRECATED
	{
		return cload_clCreateImage3D(context, flags, image_format, image_width, image_height, image_depth, image_row_pitch, image_slice_pitch, host_ptr, errcode_ret);
	}


	using clEnqueueMarkerPTR = CL_API_ENTRY CL_EXT_PREFIX__VERSION_1_1_DEPRECATED cl_int(CL_API_CALL *)(cl_command_queue  command_queue, cl_event *  event) CL_EXT_SUFFIX__VERSION_1_1_DEPRECATED;
	extern clEnqueueMarkerPTR cload_clEnqueueMarker;
	inline CL_API_ENTRY CL_EXT_PREFIX__VERSION_1_1_DEPRECATED cl_int CL_API_CALL clEnqueueMarker(cl_command_queue  command_queue, cl_event *  event) CL_EXT_SUFFIX__VERSION_1_1_DEPRECATED
	{
		return cload_clEnqueueMarker(command_queue, event);
	}


	using clEnqueueWaitForEventsPTR = CL_API_ENTRY CL_EXT_PREFIX__VERSION_1_1_DEPRECATED cl_int(CL_API_CALL *)(cl_command_queue  command_queue, cl_uint  num_events, const cl_event *  event_list) CL_EXT_SUFFIX__VERSION_1_1_DEPRECATED;
	extern clEnqueueWaitForEventsPTR cload_clEnqueueWaitForEvents;
	inline CL_API_ENTRY CL_EXT_PREFIX__VERSION_1_1_DEPRECATED cl_int CL_API_CALL clEnqueueWaitForEvents(cl_command_queue  command_queue, cl_uint  num_events, const cl_event *  event_list) CL_EXT_SUFFIX__VERSION_1_1_DEPRECATED
	{
		return cload_clEnqueueWaitForEvents(command_queue, num_events, event_list);
	}


	using clEnqueueBarrierPTR = CL_API_ENTRY CL_EXT_PREFIX__VERSION_1_1_DEPRECATED cl_int(CL_API_CALL *)(cl_command_queue  command_queue) CL_EXT_SUFFIX__VERSION_1_1_DEPRECATED;
	extern clEnqueueBarrierPTR cload_clEnqueueBarrier;
	inline CL_API_ENTRY CL_EXT_PREFIX__VERSION_1_1_DEPRECATED cl_int CL_API_CALL clEnqueueBarrier(cl_command_queue  command_queue) CL_EXT_SUFFIX__VERSION_1_1_DEPRECATED
	{
		return cload_clEnqueueBarrier(command_queue);
	}


	using clUnloadCompilerPTR = CL_API_ENTRY CL_EXT_PREFIX__VERSION_1_1_DEPRECATED cl_int(CL_API_CALL *)(void) CL_EXT_SUFFIX__VERSION_1_1_DEPRECATED;
	extern clUnloadCompilerPTR cload_clUnloadCompiler;
	inline CL_API_ENTRY CL_EXT_PREFIX__VERSION_1_1_DEPRECATED cl_int CL_API_CALL clUnloadCompiler(void) CL_EXT_SUFFIX__VERSION_1_1_DEPRECATED
	{
		return cload_clUnloadCompiler();
	}


	using clGetExtensionFunctionAddressPTR = CL_API_ENTRY CL_EXT_PREFIX__VERSION_1_1_DEPRECATED void * (CL_API_CALL *)(const char *  func_name) CL_EXT_SUFFIX__VERSION_1_1_DEPRECATED;
	extern clGetExtensionFunctionAddressPTR cload_clGetExtensionFunctionAddress;
	inline CL_API_ENTRY CL_EXT_PREFIX__VERSION_1_1_DEPRECATED void * CL_API_CALL clGetExtensionFunctionAddress(const char *  func_name) CL_EXT_SUFFIX__VERSION_1_1_DEPRECATED
	{
		return cload_clGetExtensionFunctionAddress(func_name);
	}


	/* Deprecated OpenCL 2.0 APIs */
	using clCreateCommandQueuePTR = CL_API_ENTRY CL_EXT_PREFIX__VERSION_1_2_DEPRECATED cl_command_queue(CL_API_CALL *)(cl_context  context, cl_device_id  device, cl_command_queue_properties  properties, cl_int *  errcode_ret) CL_EXT_SUFFIX__VERSION_1_2_DEPRECATED;
	extern clCreateCommandQueuePTR cload_clCreateCommandQueue;
	inline CL_API_ENTRY CL_EXT_PREFIX__VERSION_1_2_DEPRECATED cl_command_queue CL_API_CALL clCreateCommandQueue(cl_context  context, cl_device_id  device, cl_command_queue_properties  properties, cl_int *  errcode_ret) CL_EXT_SUFFIX__VERSION_1_2_DEPRECATED
	{
		return cload_clCreateCommandQueue(context, device, properties, errcode_ret);
	}



	using clCreateSamplerPTR = CL_API_ENTRY CL_EXT_PREFIX__VERSION_1_2_DEPRECATED cl_sampler(CL_API_CALL *)(cl_context  context, cl_bool  normalized_coords, cl_addressing_mode  addressing_mode, cl_filter_mode  filter_mode, cl_int *  errcode_ret) CL_EXT_SUFFIX__VERSION_1_2_DEPRECATED;
	extern clCreateSamplerPTR cload_clCreateSampler;
	inline CL_API_ENTRY CL_EXT_PREFIX__VERSION_1_2_DEPRECATED cl_sampler CL_API_CALL clCreateSampler(cl_context  context, cl_bool  normalized_coords, cl_addressing_mode  addressing_mode, cl_filter_mode  filter_mode, cl_int *  errcode_ret) CL_EXT_SUFFIX__VERSION_1_2_DEPRECATED
	{
		return cload_clCreateSampler(context, normalized_coords, addressing_mode, filter_mode, errcode_ret);
	}


	using clEnqueueTaskPTR = CL_API_ENTRY CL_EXT_PREFIX__VERSION_1_2_DEPRECATED cl_int(CL_API_CALL *)(cl_command_queue  command_queue, cl_kernel  kernel, cl_uint  num_events_in_wait_list, const cl_event *  event_wait_list, cl_event *  event) CL_EXT_SUFFIX__VERSION_1_2_DEPRECATED;
	extern clEnqueueTaskPTR cload_clEnqueueTask;
	inline CL_API_ENTRY CL_EXT_PREFIX__VERSION_1_2_DEPRECATED cl_int CL_API_CALL clEnqueueTask(cl_command_queue  command_queue, cl_kernel  kernel, cl_uint  num_events_in_wait_list, const cl_event *  event_wait_list, cl_event *  event) CL_EXT_SUFFIX__VERSION_1_2_DEPRECATED
	{
		return cload_clEnqueueTask(command_queue, kernel, num_events_in_wait_list, event_wait_list, event);
	}


#ifdef __cplusplus
}
#endif

#endif  /* __OPENCL_CL_H */

/**********************************************************************************
* Copyright (c) 2008-2015 The Khronos Group Inc.
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and/or associated documentation files (the
* "Materials"), to deal in the Materials without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Materials, and to
* permit persons to whom the Materials are furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Materials.
*
* MODIFICATIONS TO THIS FILE MAY MEAN IT NO LONGER ACCURATELY REFLECTS
* KHRONOS STANDARDS. THE UNMODIFIED, NORMATIVE VERSIONS OF KHRONOS
* SPECIFICATIONS AND HEADER INFORMATION ARE LOCATED AT
*    https://www.khronos.org/registry/
*
* THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.
**********************************************************************************/

#ifndef __OPENCL_CL_GL_H
#define __OPENCL_CL_GL_H

#ifdef __cplusplus
extern "C" {
#endif

	typedef cl_uint     cl_gl_object_type;
	typedef cl_uint     cl_gl_texture_info;
	typedef cl_uint     cl_gl_platform_info;
	typedef struct __GLsync *cl_GLsync;

	/* cl_gl_object_type = 0x2000 - 0x200F enum values are currently taken           */
#define CL_GL_OBJECT_BUFFER                     0x2000
#define CL_GL_OBJECT_TEXTURE2D                  0x2001
#define CL_GL_OBJECT_TEXTURE3D                  0x2002
#define CL_GL_OBJECT_RENDERBUFFER               0x2003
#define CL_GL_OBJECT_TEXTURE2D_ARRAY            0x200E
#define CL_GL_OBJECT_TEXTURE1D                  0x200F
#define CL_GL_OBJECT_TEXTURE1D_ARRAY            0x2010
#define CL_GL_OBJECT_TEXTURE_BUFFER             0x2011

	/* cl_gl_texture_info           */
#define CL_GL_TEXTURE_TARGET                    0x2004
#define CL_GL_MIPMAP_LEVEL                      0x2005
#define CL_GL_NUM_SAMPLES                       0x2012


	using clCreateFromGLBufferPTR = CL_API_ENTRY cl_mem(CL_API_CALL *)(cl_context  context, cl_mem_flags  flags, cl_GLuint  bufobj, int *  errcode_ret) CL_API_SUFFIX__VERSION_1_0;
	extern clCreateFromGLBufferPTR cload_clCreateFromGLBuffer;
	inline CL_API_ENTRY cl_mem CL_API_CALL clCreateFromGLBuffer(cl_context  context, cl_mem_flags  flags, cl_GLuint  bufobj, int *  errcode_ret) CL_API_SUFFIX__VERSION_1_0
	{
		return cload_clCreateFromGLBuffer(context, flags, bufobj, errcode_ret);
	}


	using clCreateFromGLTexturePTR = CL_API_ENTRY cl_mem(CL_API_CALL *)(cl_context  context, cl_mem_flags  flags, cl_GLenum  target, cl_GLint  miplevel, cl_GLuint  texture, cl_int *  errcode_ret) CL_API_SUFFIX__VERSION_1_2;
	extern clCreateFromGLTexturePTR cload_clCreateFromGLTexture;
	inline CL_API_ENTRY cl_mem CL_API_CALL clCreateFromGLTexture(cl_context  context, cl_mem_flags  flags, cl_GLenum  target, cl_GLint  miplevel, cl_GLuint  texture, cl_int *  errcode_ret) CL_API_SUFFIX__VERSION_1_2
	{
		return cload_clCreateFromGLTexture(context, flags, target, miplevel, texture, errcode_ret);
	}


	using clCreateFromGLRenderbufferPTR = CL_API_ENTRY cl_mem(CL_API_CALL *)(cl_context  context, cl_mem_flags  flags, cl_GLuint  renderbuffer, cl_int *  errcode_ret) CL_API_SUFFIX__VERSION_1_0;
	extern clCreateFromGLRenderbufferPTR cload_clCreateFromGLRenderbuffer;
	inline CL_API_ENTRY cl_mem CL_API_CALL clCreateFromGLRenderbuffer(cl_context  context, cl_mem_flags  flags, cl_GLuint  renderbuffer, cl_int *  errcode_ret) CL_API_SUFFIX__VERSION_1_0
	{
		return cload_clCreateFromGLRenderbuffer(context, flags, renderbuffer, errcode_ret);
	}


	using clGetGLObjectInfoPTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_mem  memobj, cl_gl_object_type *  gl_object_type, cl_GLuint *  gl_object_name) CL_API_SUFFIX__VERSION_1_0;
	extern clGetGLObjectInfoPTR cload_clGetGLObjectInfo;
	inline CL_API_ENTRY cl_int CL_API_CALL clGetGLObjectInfo(cl_mem  memobj, cl_gl_object_type *  gl_object_type, cl_GLuint *  gl_object_name) CL_API_SUFFIX__VERSION_1_0
	{
		return cload_clGetGLObjectInfo(memobj, gl_object_type, gl_object_name);
	}


	using clGetGLTextureInfoPTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_mem  memobj, cl_gl_texture_info  param_name, size_t  param_value_size, void *  param_value, size_t *  param_value_size_ret) CL_API_SUFFIX__VERSION_1_0;
	extern clGetGLTextureInfoPTR cload_clGetGLTextureInfo;
	inline CL_API_ENTRY cl_int CL_API_CALL clGetGLTextureInfo(cl_mem  memobj, cl_gl_texture_info  param_name, size_t  param_value_size, void *  param_value, size_t *  param_value_size_ret) CL_API_SUFFIX__VERSION_1_0
	{
		return cload_clGetGLTextureInfo(memobj, param_name, param_value_size, param_value, param_value_size_ret);
	}


	using clEnqueueAcquireGLObjectsPTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_command_queue  command_queue, cl_uint  num_objects, const cl_mem *  mem_objects, cl_uint  num_events_in_wait_list, const cl_event *  event_wait_list, cl_event *  event) CL_API_SUFFIX__VERSION_1_0;
	extern clEnqueueAcquireGLObjectsPTR cload_clEnqueueAcquireGLObjects;
	inline CL_API_ENTRY cl_int CL_API_CALL clEnqueueAcquireGLObjects(cl_command_queue  command_queue, cl_uint  num_objects, const cl_mem *  mem_objects, cl_uint  num_events_in_wait_list, const cl_event *  event_wait_list, cl_event *  event) CL_API_SUFFIX__VERSION_1_0
	{
		return cload_clEnqueueAcquireGLObjects(command_queue, num_objects, mem_objects, num_events_in_wait_list, event_wait_list, event);
	}


	using clEnqueueReleaseGLObjectsPTR = CL_API_ENTRY cl_int(CL_API_CALL *)(cl_command_queue  command_queue, cl_uint  num_objects, const cl_mem *  mem_objects, cl_uint  num_events_in_wait_list, const cl_event *  event_wait_list, cl_event *  event) CL_API_SUFFIX__VERSION_1_0;
	extern clEnqueueReleaseGLObjectsPTR cload_clEnqueueReleaseGLObjects;
	inline CL_API_ENTRY cl_int CL_API_CALL clEnqueueReleaseGLObjects(cl_command_queue  command_queue, cl_uint  num_objects, const cl_mem *  mem_objects, cl_uint  num_events_in_wait_list, const cl_event *  event_wait_list, cl_event *  event) CL_API_SUFFIX__VERSION_1_0
	{
		return cload_clEnqueueReleaseGLObjects(command_queue, num_objects, mem_objects, num_events_in_wait_list, event_wait_list, event);
	}



	/* Deprecated OpenCL 1.1 APIs */
	using clCreateFromGLTexture2DPTR = CL_API_ENTRY CL_EXT_PREFIX__VERSION_1_1_DEPRECATED cl_mem(CL_API_CALL *)(cl_context  context, cl_mem_flags  flags, cl_GLenum  target, cl_GLint  miplevel, cl_GLuint  texture, cl_int *  errcode_ret) CL_EXT_SUFFIX__VERSION_1_1_DEPRECATED;
	extern clCreateFromGLTexture2DPTR cload_clCreateFromGLTexture2D;
	inline CL_API_ENTRY CL_EXT_PREFIX__VERSION_1_1_DEPRECATED cl_mem CL_API_CALL clCreateFromGLTexture2D(cl_context  context, cl_mem_flags  flags, cl_GLenum  target, cl_GLint  miplevel, cl_GLuint  texture, cl_int *  errcode_ret) CL_EXT_SUFFIX__VERSION_1_1_DEPRECATED
	{
		return cload_clCreateFromGLTexture2D(context, flags, target, miplevel, texture, errcode_ret);
	}


	using clCreateFromGLTexture3DPTR = CL_API_ENTRY CL_EXT_PREFIX__VERSION_1_1_DEPRECATED cl_mem(CL_API_CALL *)(cl_context  context, cl_mem_flags  flags, cl_GLenum  target, cl_GLint  miplevel, cl_GLuint  texture, cl_int *  errcode_ret) CL_EXT_SUFFIX__VERSION_1_1_DEPRECATED;
	extern clCreateFromGLTexture3DPTR cload_clCreateFromGLTexture3D;
	inline CL_API_ENTRY CL_EXT_PREFIX__VERSION_1_1_DEPRECATED cl_mem CL_API_CALL clCreateFromGLTexture3D(cl_context  context, cl_mem_flags  flags, cl_GLenum  target, cl_GLint  miplevel, cl_GLuint  texture, cl_int *  errcode_ret) CL_EXT_SUFFIX__VERSION_1_1_DEPRECATED
	{
		return cload_clCreateFromGLTexture3D(context, flags, target, miplevel, texture, errcode_ret);
	}


	/* cl_khr_gl_sharing extension  */

#define cl_khr_gl_sharing 1

	typedef cl_uint     cl_gl_context_info;

	/* Additional Error Codes  */
#define CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR  -1000

	/* cl_gl_context_info  */
#define CL_CURRENT_DEVICE_FOR_GL_CONTEXT_KHR    0x2006
#define CL_DEVICES_FOR_GL_CONTEXT_KHR           0x2007

	/* Additional cl_context_properties  */
#define CL_GL_CONTEXT_KHR                       0x2008
#define CL_EGL_DISPLAY_KHR                      0x2009
#define CL_GLX_DISPLAY_KHR                      0x200A
#define CL_WGL_HDC_KHR                          0x200B
#define CL_CGL_SHAREGROUP_KHR                   0x200C

	using clGetGLContextInfoKHRPTR = CL_API_ENTRY cl_int(CL_API_CALL *)(const cl_context_properties *  properties, cl_gl_context_info  param_name, size_t  param_value_size, void *  param_value, size_t *  param_value_size_ret) CL_API_SUFFIX__VERSION_1_0;
	extern clGetGLContextInfoKHRPTR cload_clGetGLContextInfoKHR;
	inline CL_API_ENTRY cl_int CL_API_CALL clGetGLContextInfoKHR(const cl_context_properties *  properties, cl_gl_context_info  param_name, size_t  param_value_size, void *  param_value, size_t *  param_value_size_ret) CL_API_SUFFIX__VERSION_1_0
	{
		return cload_clGetGLContextInfoKHR(properties, param_name, param_value_size, param_value, param_value_size_ret);
	}


	typedef CL_API_ENTRY cl_int(CL_API_CALL *clGetGLContextInfoKHR_fn)(
		const cl_context_properties * properties,
		cl_gl_context_info            param_name,
		size_t                        param_value_size,
		void *                        param_value,
		size_t *                      param_value_size_ret);

	bool cloadInit();
	void cloadTerminate();
#ifdef __cplusplus
}
#endif

#endif  /* __OPENCL_CL_GL_H */
