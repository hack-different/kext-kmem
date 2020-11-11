//
//  kmem_api.h
//  kmem
//
//  Created by Rick Mark on 11/8/20.
//

#ifndef kmem_api_h
#define kmem_api_h

extern const char* kIOKernelMemoryApeture;
extern const char* kIOKernelMemoryUserClient;

extern const uint32_t kIOKernelMemoryApetureDefault;


enum {
    kIOKernelMemoryApetureMethodReadVirtual,
    kIOKernelMemoryApetureMethodReadPhysical,

    kIOKernelMemoryApetureMethodCount
};


typedef struct {
    void* address;
    size_t size;
} kmem_read_t;


#endif /* kmem_api_h */
