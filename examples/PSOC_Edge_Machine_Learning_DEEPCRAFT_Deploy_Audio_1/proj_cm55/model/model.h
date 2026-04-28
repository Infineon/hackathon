/*
* ImagiNet Compiler 5.10.4880.0+131a2bec9927153a4680e62c816983072c846a58
* Copyright © 2023- Imagimob AB, All Rights Reserved.
* 
* Generated at 04/23/2026 16:35:51 UTC. Any changes will be lost.
* 
* Model ID  02a284ef-b369-4bd0-b0f8-a1953f584b29
* 
* Memory    Size                      Efficiency
* Buffers   20485 bytes (RAM)         100 %
* State     101600 bytes (RAM)        100 %
* Readonly  33488 bytes (Flash)       100 %
* 
* Exported functions:
* 
*  @description: Try read data from model.
*  @param data_out Output features. Output float[5].
*  @return IPWIN_RET_SUCCESS (0) or IPWIN_RET_NODATA (-1), IPWIN_RET_ERROR (-2), IPWIN_RET_STREAMEND (-3)
*  int IMAI_dequeue(float *data_out);
* 
*  @description: Try write data to model.
*  @param data_in Input features. Input float[2].
*  @return IPWIN_RET_SUCCESS (0) or IPWIN_RET_NODATA (-1), IPWIN_RET_ERROR (-2), IPWIN_RET_STREAMEND (-3)
*  int IMAI_enqueue(const float *data_in);
* 
*  @description: Closes and flushes streams, free any heap allocated memory.
*  void IMAI_finalize(void);
* 
*  @description: Resets windows and neural networks(i.e. RNNs) to initial state.
*  @return IPWIN_RET_SUCCESS (0) or IPWIN_RET_NODATA (-1), IPWIN_RET_ERROR (-2), IPWIN_RET_STREAMEND (-3)
*  int IMAI_soft_reset(void);
* 
*  @description: Initializes buffers to initial state.
*  @return IPWIN_RET_SUCCESS (0) or IPWIN_RET_NODATA (-1), IPWIN_RET_ERROR (-2), IPWIN_RET_STREAMEND (-3)
*  int IMAI_init(void);
* 
* 
* Disclaimer:
*   The generated code relies on the optimizations done by the C compiler.
*   For example many for-loops of length 1 must be removed by the optimizer.
*   This can only be done if the functions are inlined and simplified.
*   Check disassembly if unsure.
*   tl;dr Compile using gcc with -O3 or -Ofast
* 
* Notes:
* 	-> This code was generated with DEEPCRAFT Studio using:
* 		ml-coretools 3.1.0.9404.
* 		tensorflow 2.19.0.
* 		ethos-u-vela 4.5.0.
* 	-> This code requires the following Modus Toolbox libraries (add them to your
* 	project using the Library Manager):
* 		ml-middleware 3.1.0.
* 		ml-tflite-micro 3.1.0.
*/

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "mtb_ml_model.h"
#define IMAI_API_USER_DEFINED

typedef int8_t q7_t;         // 8-bit fractional data type in Q1.7 format.
typedef int16_t q15_t;       // 16-bit fractional data type in Q1.15 format.
typedef int32_t q31_t;       // 32-bit fractional data type in Q1.31 format.
typedef int64_t q63_t;       // 64-bit fractional data type in Q1.63 format.
typedef float timestamp_t;

// Model GUID (16 bytes)
#define IMAI_MODEL_ID {0xef, 0x84, 0xa2, 0x02, 0x69, 0xb3, 0xd0, 0x4b, 0xb0, 0xf8, 0xa1, 0x95, 0x3f, 0x58, 0x4b, 0x29}


// First nibble is bit encoding, second nibble is number of bytes
#define IMAGINET_TYPES_NONE	(0x0)
#define IMAGINET_TYPES_FLOAT32	(0x14)
#define IMAGINET_TYPES_FLOAT64	(0x18)
#define IMAGINET_TYPES_INT8	(0x21)
#define IMAGINET_TYPES_INT16	(0x22)
#define IMAGINET_TYPES_INT32	(0x24)
#define IMAGINET_TYPES_INT64	(0x28)
#define IMAGINET_TYPES_Q7	(0x31)
#define IMAGINET_TYPES_Q15	(0x32)
#define IMAGINET_TYPES_Q31	(0x34)
#define IMAGINET_TYPES_BOOL	(0x41)
#define IMAGINET_TYPES_STRING	(0x54)
#define IMAGINET_TYPES_D8	(0x61)
#define IMAGINET_TYPES_D16	(0x62)
#define IMAGINET_TYPES_D32	(0x64)
#define IMAGINET_TYPES_UINT8	(0x71)
#define IMAGINET_TYPES_UINT16	(0x72)
#define IMAGINET_TYPES_UINT32	(0x74)
#define IMAGINET_TYPES_UINT64	(0x78)


// data_out [5] (20 bytes)
#define IMAI_DATA_OUT_RANK (1)
#define IMAI_DATA_OUT_SHAPE ((int[]){5})
#define IMAI_DATA_OUT_COUNT (5)
#define IMAI_DATA_OUT_TYPE float
#define IMAI_DATA_OUT_TYPE_ID IMAGINET_TYPES_FLOAT32
#define IMAI_DATA_OUT_SHIFT 0
#define IMAI_DATA_OUT_OFFSET 0
#define IMAI_DATA_OUT_SCALE 1
#define IMAI_DATA_OUT_SYMBOLS {"{unlabeled}", "East", "South", "West", "Nord"}

// data_in [2] (8 bytes)
#define IMAI_DATA_IN_RANK (1)
#define IMAI_DATA_IN_SHAPE ((int[]){2})
#define IMAI_DATA_IN_COUNT (2)
#define IMAI_DATA_IN_TYPE float
#define IMAI_DATA_IN_TYPE_ID IMAGINET_TYPES_FLOAT32
#define IMAI_DATA_IN_SHIFT 0
#define IMAI_DATA_IN_OFFSET 0
#define IMAI_DATA_IN_SCALE 1
#define IMAI_DATA_IN_SYMBOLS { }

#define IMAI_KEY_MAX (8)

// Return codes
#define IMAI_RET_SUCCESS 0
#define IMAI_RET_NODATA -1
#define IMAI_RET_ERROR -2
#define IMAI_RET_STREAMEND -3

#define IPWIN_RET_SUCCESS 0
#define IPWIN_RET_NODATA -1
#define IPWIN_RET_ERROR -2
#define IPWIN_RET_STREAMEND -3

// Exported methods
int IMAI_dequeue(float *restrict data_out);
int IMAI_enqueue(const float *restrict data_in);
void IMAI_finalize(void);
int IMAI_soft_reset(void);
int IMAI_init(void);

// Call macros — invoke any exported function via a void* array
#define IMAI_DEQUEUE_PTR(a) IMAI_dequeue((float *)(a)[0])
#define IMAI_ENQUEUE_PTR(a) IMAI_enqueue((const float *)(a)[0])
#define IMAI_FINALIZE_PTR(a) IMAI_finalize()
#define IMAI_SOFT_RESET_PTR(a) IMAI_soft_reset()
#define IMAI_INIT_PTR(a) IMAI_init()

// Symbol IMAI_PROFILING must be defined to enable profiling of models
// Symbol IMAI_PROFILING_LOG will enable printing the raw outputs of neural networks
/// @brief This method will print the region profiling results
void IMAI_print_region_profiling(void);
#ifdef IMAI_PROFILING
/// @brief Implement this method to perform profiling, should populate the value pointed to by val with the current tick count
int IMAI_get_ticks(uint64_t *val);
/// @brief Only re-implement this method if you want to perform custom profiling of regions in the generated code
void IMAI_hook_region(bool entered, int32_t region_id);
#endif

/// @brief This method will print neural network inference profiling results
void IMAI_mtb_models_profile_log();
/// @brief This method will print neural network information
void IMAI_mtb_models_print_info();
#define IMAI_MAX_MTB_MODELS 4
extern int32_t IMAI_mtb_models_count;
extern mtb_ml_model_t* IMAI_mtb_models[IMAI_MAX_MTB_MODELS];

// Profiling regions
#ifdef IMAI_PROFILING
    #define IMAI_REGIONS_COUNT 2
    #define IMAI_REGIONS_NAMES {\
    	"PREPROCESSOR",\
    	"NETWORK",\
    }
    typedef enum {
    	IMAI_PREPROCESSOR = 0,
    	IMAI_NETWORK = 1,
    } IMAI_Region_t;
#else
    #define IMAI_REGIONS_COUNT 0
    #define IMAI_REGIONS_NAMES {}
    typedef enum {IMAI_REGIONS_EMPTY} IMAI_Region_t;
#endif

typedef enum {
    IMAI_PARAM_UNDEFINED = 0,
    IMAI_PARAM_INPUT = 1,
    IMAI_PARAM_OUTPUT = 2,
    IMAI_PARAM_REFERENCE = 3,
    IMAI_PARAM_HANDLE = 7,
    IMAI_PARAM_CALLBACK = 8,
} IMAI_param_attrib;

typedef char *label_text_t;

typedef struct {
    char* name;
    int size;
    label_text_t *labels;
} IMAI_shape_dim;

typedef struct {
    char* name;
    IMAI_param_attrib attrib;
    int32_t rank;
    IMAI_shape_dim *shape;
    int32_t count;
    int32_t type_id;
    float frequency;
    int shift;
    float scale;
    long offset;
} IMAI_param_def;

typedef enum {
    IMAI_FUNC_ATTRIB_NONE = 0,
    IMAI_FUNC_ATTRIB_CAN_FAIL = 1,
    IMAI_FUNC_ATTRIB_PUBLIC = 2,
    IMAI_FUNC_ATTRIB_INIT = 4,
    IMAI_FUNC_ATTRIB_DESTRUCTOR = 8,
} IMAI_func_attrib;

typedef struct {
    char* name;
    char* description;
    void* fn_ptr;
    IMAI_func_attrib attrib;
    int32_t param_count;
    IMAI_param_def *param_list;
} IMAI_func_def;

typedef struct {
    uint32_t size;
    uint32_t peak_usage;
} IMAI_mem_usage;

typedef enum {
    IMAI_API_TYPE_UNDEFINED = 0,
    IMAI_API_TYPE_FUNCTION = 1,
    IMAI_API_TYPE_QUEUE = 2,
    IMAI_API_TYPE_QUEUE_TIME = 3,
    IMAI_API_TYPE_USER_DEFINED = 4,
} IMAI_api_type;

typedef struct {
    uint32_t api_ver;
    uint8_t id[16];
    IMAI_api_type api_type;
    char* prefix;
    IMAI_mem_usage buffer_mem;
    IMAI_mem_usage static_mem;
    IMAI_mem_usage readonly_mem;
    int32_t func_count;
    IMAI_func_def *func_list;
} IMAI_api_def;

IMAI_api_def *IMAI_api(void);

