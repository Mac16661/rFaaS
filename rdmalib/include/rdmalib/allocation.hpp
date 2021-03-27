
#ifndef __RDMALIB_EXECUTOR_MANAGER__
#define __RDMALIB_EXECUTOR_MANAGER__

#include <cstdint>

namespace rdmalib {

  struct AllocationRequest
  {

    // > 0: Number of cores to be allocated
    // < 0: client_id with negative sign, deallocation & disconnect request
    int16_t cores;
    int16_t input_buf_count;
    int32_t input_buf_size; 
    int32_t func_buf_size;
  };

}

#endif

