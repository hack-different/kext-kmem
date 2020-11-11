/* add your code here */

#include "IOKernelMemoryApeture.hpp"

#define super IOService

OSDefineMetaClassAndStructors(IOKernelMemoryApeture, IOService);


bool IOKernelMemoryApeture::start(IOService *provider) {
    IOLog("kmem: Executing 'IOKernelMemoryApeture::start()'.\n");
    
    bool superStart = super::start(provider);
    if (superStart) {
        IODebug("Calling 'IOKernelMemory:registerService()'.");
        registerService();
    }
    
    return superStart;
}


void IOKernelMemoryApeture::stop(IOService *provider) {
    IODebug("Executing 'IOKernelMemoryApeture:stop()'.");
    
    super::stop(provider);
}

bool IOKernelMemoryApeture::init(OSDictionary *dictionary) {
    IODebug("Executing 'IOKernelMemoryApeture:init()'.");
    
    return super::init(dictionary);
}

void IOKernelMemoryApeture::free() {
    IODebug("Executing 'IOKernelMemoryApeture:free()'.");
    
    super::free();
}
