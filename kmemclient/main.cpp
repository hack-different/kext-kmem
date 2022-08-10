//
//  main.cpp
//  kmemclient
//
//  Created by Rick Mark on 11/8/20.
//

#include <iostream>
#include <IOKit/IOKitLib.h>
#include <mach/error.h>
#include <stdio.h>
#include "kmem_api.h"


const char* kIOKernelMemoryApeture = "IOKernelMemoryApeture";
const char* kIOKernelMemoryUserClient = "IOKernelMemoryUserClient";

const uint32_t kIOKernelMemoryApetureDefault = 0;



static const long hextable[] = {
   ['0'] = 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, // faster for most modern processors,
   ['A'] = 10, 11, 12, 13, 14, 15,       // for the space conscious, reduce to
   ['a'] = 10, 11, 12, 13, 14, 15        // signed char.
};

uint64_t parseHex(const char* input) {
    if (strlen(input) > 2 && input[0] == '0' && input[1] == 'x') {
        input += 2;
        uint64_t result = 0;
        while (*input && result >= 0) {
           result = (result << 4) | hextable[*input++];
        }
        
        return result;
    }
    
    return atoi(input);
}

int main(int argc, const char * argv[]) {
    if (argc != 3) {
        std::cout << "Usage: kmemclient <ADDRESS> <SIZE>\n";
        return -1;
    }
    
    uint64_t targetAddress = parseHex(argv[1]);
    uint32_t targetSize = (uint32_t)parseHex(argv[2]);
    
    io_service_t service;
    io_connect_t connection;
    
    uint64_t input[3];
    uint64_t output[8];
    uint32_t outputCount = 8;
    
    input[0] = targetAddress;
    input[1] = targetSize;
    
    service = IOServiceGetMatchingService(kIOMasterPortDefault, IOServiceMatching(kIOKernelMemoryApeture));

    
    if (service == IO_OBJECT_NULL) {
        std::cerr << "Could not find the kernel side component IOKernelMemoryApeture\n";
        return -2;
    }
    
    kern_return_t openResult = IOServiceOpen(service, mach_task_self(), kIOKernelMemoryApetureDefault, &connection);
    
    if (openResult != KERN_SUCCESS) {
        std::cerr << "Could not open the IOKernelMemoryApeture\n";
        return -3;
    }

    outputCount = 1;
    kern_return_t hdrResult = IOConnectCallScalarMethod(connection, kIOKernelMemoryApetureMethodGetKextHeaderAddress, nullptr, 0, output, &outputCount);
    if (hdrResult != KERN_SUCCESS) {
        char hexError[16];
        sprintf(hexError, "0x%X", hdrResult);
        std::cerr << "Unable to invoke hdr addr operation on the IOService: " << hexError << "\n";
        return -5;
    } else {
        printf("kext header address: %p\n", (void*)output[0]);
    }

    outputCount = 1;
    kern_return_t codeResult = IOConnectCallScalarMethod(connection, kIOKernelMemoryApetureMethodGetKextCodeAddress, nullptr, 0, output, &outputCount);
    if (codeResult != KERN_SUCCESS) {
        char hexError[16];
        sprintf(hexError, "0x%X", hdrResult);
        std::cerr << "Unable to invoke code addr operation on the IOService: " << hexError << "\n";
        return -5;
    } else {
        printf("code header address: %p\n", (void*)output[0]);
    }

#if 1
    
    outputCount = 8;
    void* outputBuffer = malloc(targetSize);
    if (!outputBuffer) {
        std::cerr << "Could not allocate user mode buffer with sufficant size.\n";
        return -4;
    }
    
    memset(outputBuffer, 0x00, targetSize);
    fprintf(stderr, "user mode buffer: 0x%llX\n", (uint64_t)outputBuffer);
    
    input[2] = (uint64_t)outputBuffer;
    
    
    
    kern_return_t readResult = IOConnectCallScalarMethod(connection, kIOKernelMemoryApetureMethodReadVirtual, input, 3, output, &outputCount);
    
    if (readResult != KERN_SUCCESS) {
        char hexError[16];
        sprintf(hexError, "0x%X", readResult);
        std::cerr << "Unable to invoke Read operation on the IOService: " << hexError << "\n";
        return -5;
    }
    
    fprintf(stderr, "would have written %d bytes from 0x%llX -> 0x%llX\n", targetSize, output[1], output[0]);
    
    fprintf(stderr, "user buffer physical address: 0x%llX\n", output[2]);
    fprintf(stderr, "virtual address: 0x%llX\n", output[3]);
    fprintf(stderr, "size: %lld\n", output[4]);
    fprintf(stderr, "address: 0x%llX\n", output[5]);
    //fprintf(stderr, "physical address: %llX\n", output[6]);
    fprintf(stderr, "physical address: 0x%llX\n", output[7]);
    /*
    arguments->scalarOutput[2] = userBuffer->getPhysicalAddress();
    arguments->scalarOutput[3] = kernelMapping->getVirtualAddress();
    arguments->scalarOutput[4] = kernelMapping->getSize();
    arguments->scalarOutput[5] = kernelMapping->getAddress();
    arguments->scalarOutput[7] = kernelMapping->getPhysicalAddress();
    */
    // Output the bytes to STDOUT
    
    fwrite(outputBuffer, 1, targetSize, stdout);
    fflush(stdout);
    
#endif
    
    return 0;
}
