//
//  IOKernelMemoryUserClient.hpp
//  kmem
//
//  Created by Rick Mark on 11/8/20.
//

#ifndef IOKernelMemoryUserClient_hpp
#define IOKernelMemoryUserClient_hpp


#include <IOKit/IOService.h>
#include <IOKit/IOUserClient.h>
#include "kmem.h"
#include "IOKernelMemoryApeture.hpp"

class IOKernelMemoryApetureUserClient : public IOUserClient {
    OSDeclareDefaultStructors(IOKernelMemoryApetureUserClient);
    
public:
    virtual bool initWithTask(task_t owningTask, void *securityToken,
                              UInt32 type, OSDictionary *properties) override;
    
    virtual bool start(IOService *provider) override;
    virtual void stop(IOService *provider) override;
    
    virtual IOReturn clientClose() override;
    
    virtual IOReturn externalMethod(uint32_t selector,
                                    IOExternalMethodArguments *arguments,
                                    IOExternalMethodDispatch *dispatch,
                                    OSObject *target, void *reference) override;
protected:
    /**
     * The following methods unpack/handle the given arguments and
     * call the related driver method.
     */
    virtual IOReturn methodReadVirtual(IOExternalMethodArguments *arguments);
    virtual IOReturn methodReadPhysical(IOExternalMethodArguments *arguments);

    static IOReturn actionMethodReadVirtual(IOKernelMemoryApetureUserClient *target, void *reference, IOExternalMethodArguments *arguments);
    static IOReturn actionMethodReadPhysical(IOKernelMemoryApetureUserClient *target, void *reference,IOExternalMethodArguments *arguments);
private:
    /**
     *  Method dispatch table.
     */
    static const IOExternalMethodDispatch s_methods[kIOKernelMemoryApetureMethodCount];
    
    /**
     *  Driver provider.
     */
    IOKernelMemoryApeture *m_apeture = nullptr;

    /**
     *  Userland subscriber.
     */
    OSAsyncReference64 *m_subscriber = nullptr;
    
    /**
     *  Task owner.
     */
    task_t m_owner;
};


#endif /* IOKernelMemoryUserClient_hpp */
