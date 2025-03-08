#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Chladas");
MODULE_DESCRIPTION("Test kmalloc allocations with different flags");

#define ALLOC_COUNT 4

void *alloc_blocks[ALLOC_COUNT];

struct {
    size_t size;
    gfp_t flags;
} alloc_params[ALLOC_COUNT] = {
    {1024, GFP_DMA},
    {10000, GFP_DMA},
    {500000, GFP_KERNEL},
    {1024, GFP_KERNEL}
};

const char* gfp_flags_to_string(gfp_t flags) {
    if (flags == GFP_DMA)
        return "GFP_DMA";
    else if (flags == GFP_KERNEL)
        return "GFP_KERNEL";
    else
        return "Unknown Flag";
}

static int __init kmalloc_test_init(void) {
    int i;
    
    printk(KERN_INFO "Module LOADED...\n");

    for (i = 0; i < ALLOC_COUNT; i++) {
        alloc_blocks[i] = kmalloc(alloc_params[i].size, alloc_params[i].flags);
        if (!alloc_blocks[i]) {
            printk(KERN_ERR "kmalloc failed for size %zu with flags %s\n", 
                   alloc_params[i].size, gfp_flags_to_string(alloc_params[i].flags));
        } else {
            printk(KERN_INFO "Allocated %zu bytes with flags %s at %px\n",
                   alloc_params[i].size, gfp_flags_to_string(alloc_params[i].flags), alloc_blocks[i]);
        }
    }

    printk(KERN_INFO "Check /proc/slabinfo for changes.\n");
    return 0;
}

static void __exit kmalloc_test_exit(void) {
    int i;

    printk(KERN_INFO "Cleaning up kmalloc test module...\n");

    for (i = 0; i < ALLOC_COUNT; i++) {
        if (alloc_blocks[i]) {
            printk(KERN_INFO "Freeing memory at %px\n", alloc_blocks[i]);
            kfree(alloc_blocks[i]);
        }
    }

    printk(KERN_INFO "Module REMOVED...\n");
}

module_init(kmalloc_test_init);
module_exit(kmalloc_test_exit);

