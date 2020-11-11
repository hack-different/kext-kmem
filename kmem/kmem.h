//
//  kmem.h
//  kmem
//
//  Created by Rick Mark on 11/8/20.
//

#ifndef kmem_h
#define kmem_h

#include <IOKit/IOLib.h>

#include "kmem_api.h"

#define IODebug(fmt, ...) IOLog(("kmem: " fmt "\n"), ##__VA_ARGS__)

#endif /* kmem_h */
