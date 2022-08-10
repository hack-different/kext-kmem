//
//  IOKernelMemoryUserClient.cpp
//  kmem
//
//  Created by Rick Mark on 11/8/20.
//

#include "IOKernelMemoryUserClient.hpp"

#if __LP64__
extern struct mach_header_64 __dso_handle;
#else
extern struct mach_header __dso_handle;
#endif

#define super IOUserClient


OSDefineMetaClassAndStructors(IOKernelMemoryApetureUserClient, IOUserClient)

const IOExternalMethodDispatch IOKernelMemoryApetureUserClient::s_methods[kIOKernelMemoryApetureMethodCount] = {
    {(IOExternalMethodAction)&IOKernelMemoryApetureUserClient::actionMethodReadVirtual, 3, 0, 8, 0},
    {(IOExternalMethodAction)&IOKernelMemoryApetureUserClient::actionMethodReadPhysical, 3, 0, 0, 0},
    {(IOExternalMethodAction)&IOKernelMemoryApetureUserClient::actionMethodHeaderAddress, 0, 0, 1, 0},
    {(IOExternalMethodAction)&IOKernelMemoryApetureUserClient::actionMethodCodeAddress, 0, 0, 1, 0},
};

bool IOKernelMemoryApetureUserClient::initWithTask(task_t owningTask, void *securityToken, UInt32 type, OSDictionary *properties) {
    IOLog("kmem: Executing 'IOKernelMemoryApetureUserClient::initWithTask()'.\n");
    
    
    if (!owningTask) {
        return false;
    }
    
    if (!super::initWithTask(owningTask, securityToken, type, properties)) {
        return false;
    }
    
    this->m_owner = owningTask;
    
    return true;
}

bool IOKernelMemoryApetureUserClient::start(IOService *provider) {
    IODebug("Executing 'IOKernelMemoryApetureUserClient::start()'.");
    
    if (!super::start(provider)) {
        return false;
    }
    
    this->m_apeture = OSDynamicCast(IOKernelMemoryApeture, provider);
    if (!this->m_apeture) {
        return false;
    }
    
    return true;
}

void IOKernelMemoryApetureUserClient::stop(IOService *provider) {
    IODebug("Executing 'IOKernelMemoryApetureUserClient::stop()'.");
    super::stop(provider);
}

IOReturn IOKernelMemoryApetureUserClient::clientClose() {
    if (this->m_apeture) {
        this->close((IOService*)this->m_apeture);
        this->m_apeture = nullptr;
    }
    
    if (this->m_owner) {
        this->m_owner = nullptr;
    }
    
    this->terminate();
    
    return kIOReturnSuccess;
}

IOReturn IOKernelMemoryApetureUserClient::externalMethod(uint32_t selector, IOExternalMethodArguments *arguments, IOExternalMethodDispatch *dispatch, OSObject *target, void *reference) {
    
    if (selector >= kIOKernelMemoryApetureMethodCount) {
        return kIOReturnUnsupported;
    }
    
    dispatch = (IOExternalMethodDispatch*)&s_methods[selector];
    target = this;
    reference = nullptr;
    
    return super::externalMethod(selector, arguments, dispatch, target, reference);
}

IOReturn IOKernelMemoryApetureUserClient::actionMethodReadVirtual(IOKernelMemoryApetureUserClient *target, void *reference,
                                                  IOExternalMethodArguments *arguments) {
    return target->methodReadVirtual(arguments);
}

IOReturn IOKernelMemoryApetureUserClient::actionMethodReadPhysical(IOKernelMemoryApetureUserClient *target, void *reference,
                                                   IOExternalMethodArguments *arguments) {
    return target->methodReadPhysical(arguments);
}

IOReturn IOKernelMemoryApetureUserClient::actionMethodHeaderAddress(IOKernelMemoryApetureUserClient *target, void *reference,
                                                  IOExternalMethodArguments *arguments) {
    return target->methodHeaderAddress(arguments);
}

IOReturn IOKernelMemoryApetureUserClient::actionMethodCodeAddress(IOKernelMemoryApetureUserClient *target, void *reference,
                                                  IOExternalMethodArguments *arguments) {
    return target->methodCodeAddress(arguments);
}

IOReturn IOKernelMemoryApetureUserClient::methodHeaderAddress(IOExternalMethodArguments *arguments) {
    IOLog("kmem: Executing 'IOKernelMemoryApetureUserClient::methodHeaderAddress()'.\n");
    arguments->scalarOutput[0] = (uint64_t)&__dso_handle;
    return kIOReturnSuccess;
}

__attribute__((noinline))
static uint64_t get_code_addr(void) {
    return (uint64_t)__builtin_return_address(0);
}

IOReturn IOKernelMemoryApetureUserClient::methodCodeAddress(IOExternalMethodArguments *arguments) {
    IOLog("kmem: Executing 'IOKernelMemoryApetureUserClient::methodCodeAddress()'.\n");
    arguments->scalarOutput[0] = get_code_addr();
    return kIOReturnSuccess;
}

IOReturn IOKernelMemoryApetureUserClient::methodReadVirtual(IOExternalMethodArguments *arguments) {
    IOLog("kmem: Executing 'IOKernelMemoryApetureUserClient::methodReadVirtual()'.\n");
    
    void* targetAddress = (void*)arguments->scalarInput[0];
    size_t targetSize = (size_t)arguments->scalarInput[1];
    mach_vm_address_t userMemoryBuffer = (mach_vm_address_t)arguments->scalarInput[2];

    IOMemoryDescriptor* userBuffer = IOMemoryDescriptor::withAddressRange(userMemoryBuffer, targetSize, kIOMemoryDirectionOut, this->m_owner);
    
    kern_return_t prepareResult = userBuffer->prepare(kIOMemoryDirectionOut);
    if (prepareResult != KERN_SUCCESS) {
        OSSafeReleaseNULL(userBuffer);
        return prepareResult;
    }
    
    IOMemoryMap* kernelMapping = userBuffer->map();
    if (kernelMapping == nullptr) {
        userBuffer->complete();
        OSSafeReleaseNULL(userBuffer);
        return kIOReturnVMError;
    }
    
//    arguments->scalarOutput[0] = kernelMapping->getAddress();
    arguments->scalarOutput[0] = 0xdeadbeef;
    arguments->scalarOutput[1] = (int64_t)targetAddress;
    arguments->scalarOutput[2] = userBuffer->getPhysicalAddress();
    arguments->scalarOutput[3] = kernelMapping->getVirtualAddress();
    arguments->scalarOutput[4] = kernelMapping->getSize();
//    arguments->scalarOutput[5] = kernelMapping->getAddress();
    arguments->scalarOutput[5] = 0xbaadc0de;
    memcpy(&arguments->scalarOutput[6], (void*)kernelMapping->getVirtualAddress(), sizeof(uint64_t));
    arguments->scalarOutput[7] = kernelMapping->getPhysicalAddress();


//    memset((void*)userBuffer->getVirtualAddress(), 0x41, targetSize);
    //memcpy((void*)kernelMapping->getVirtualAddress(), &markerValue, sizeof(uint64_t));
    
//    userBuffer->writeBytes(0, targetAddress, targetSize);
    memcpy((void*)kernelMapping->getVirtualAddress(), targetAddress, targetSize);
    
    kernelMapping->unmap();
    OSSafeReleaseNULL(kernelMapping);
    
    
    
    kern_return_t completeResult = userBuffer->complete();
    
    if (completeResult != KERN_SUCCESS) {
        OSSafeReleaseNULL(userBuffer);
        return completeResult;
    }
    
    OSSafeReleaseNULL(userBuffer);
    return kIOReturnSuccess;
}

IOReturn IOKernelMemoryApetureUserClient::methodReadPhysical(IOExternalMethodArguments *arguments) {
    IOLog("kmem: Executing 'IOKernelMemoryApetureUserClient::methodReadPhysical()'.\n");

    return kIOReturnSuccess;
}
