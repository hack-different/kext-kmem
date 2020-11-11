/* add your code here */

#ifndef IOKernelMemoryApeture_hpp
#define IOKernelMemoryApeture_hpp

#include <IOKit/IOLib.h>
#include <IOKit/IOService.h>
#include "kmem.h"

class IOKernelMemoryApeture : IOService {
    OSDeclareDefaultStructors(IOKernelMemoryApeture)
    
public:
    virtual bool init(OSDictionary *dictionary = 0) override;
    virtual void free(void) override;
    
    virtual bool start(IOService *provider) override;
    virtual void stop(IOService *provider) override;
    
};

#endif
