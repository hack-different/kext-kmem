//
//  kmem.c
//  kmem
//
//  Created by Rick Mark on 11/7/20.
//

#include <mach/mach_types.h>

kern_return_t kmem_start(kmod_info_t * ki, void *d);
kern_return_t kmem_stop(kmod_info_t *ki, void *d);

kern_return_t kmem_start(kmod_info_t * ki, void *d)
{
    return KERN_SUCCESS;
}

kern_return_t kmem_stop(kmod_info_t *ki, void *d)
{
    return KERN_SUCCESS;
}
