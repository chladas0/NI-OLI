#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/gfp.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Chladas");
MODULE_DESCRIPTION("SLAB Cache");

#define RECORD_SIZE 128
#define NUM_RECORDS 4
#define LARGE_RECORD_SIZE 8192

/**
 * Constructor for initializing memory with 0x55
 */
void record_constructor(void *ptr) {
    memset(ptr, 0x55, RECORD_SIZE); /** Fill allocated memory with 0x55 */
}

static struct kmem_cache *cachep, *large_cachep;
static void *records[NUM_RECORDS];
static void *large_record;

static int __init kmem_cache_init(void) {

    printk(KERN_INFO "Module LOADED");

    int i;

    /** Create a slab cache for records of size 128 bytes */
    cachep = kmem_cache_create("record_cache", RECORD_SIZE, 0, SLAB_HWCACHE_ALIGN, record_constructor);
    if (!cachep) {
        printk(KERN_ERR "kmem_cache_create failed\n");
        return -ENOMEM;
    }

    /** Allocate 4 records from the cache */
    for (i = 0; i < NUM_RECORDS; i++) {
        records[i] = kmem_cache_alloc(cachep, GFP_KERNEL);
        if (!records[i]) {
            printk(KERN_ERR "kmem_cache_alloc failed for record %d\n", i);
            kmem_cache_destroy(cachep);
            return -ENOMEM;
        }

        /** Print the virtual address of the allocated record */
        printk(KERN_INFO "Address of record %d: %px\n", i, records[i]);

        /** Print the first byte of the allocated record (should be 0x55) */
        printk(KERN_INFO "First byte of record %d: %x\n", i, ((unsigned char *)records[i])[0]);
    }

    /** Create a larger item size cache (greater than one page) */
    large_cachep = kmem_cache_create("large_record_cache", LARGE_RECORD_SIZE, 0, SLAB_HWCACHE_ALIGN, record_constructor);
    if (!large_cachep) {
        printk(KERN_ERR "kmem_cache_create for large cache failed\n");
        kmem_cache_destroy(cachep);
        return -ENOMEM;
    }

    /** Allocate a single large record */
    large_record = kmem_cache_alloc(large_cachep, GFP_KERNEL);
    if (!large_record) {
        printk(KERN_ERR "kmem_cache_alloc failed for large record\n");
        kmem_cache_destroy(cachep);
        kmem_cache_destroy(large_cachep);
        return -ENOMEM;
    }

    printk(KERN_INFO "Allocated large record.\n");

    /** Print the virtual address of the large record */
    printk(KERN_INFO "Address of large record: %px\n", large_record);

    /** Print the first byte of the large record (should be 0x55) */
    printk(KERN_INFO "First byte of large record: %x\n", ((unsigned char *)large_record)[0]);

    return 0;
}

static void __exit kmem_cache_exit(void) {
    int i;

    printk(KERN_INFO "Module REMOVED");

    /** Free all the records */
    for (i = 0; i < NUM_RECORDS; i++) {
        kmem_cache_free(cachep, records[i]);
    }

    /** Free the large record */
    kmem_cache_free(large_cachep, large_record);

    /** Destroy the caches */
    kmem_cache_destroy(cachep);
    kmem_cache_destroy(large_cachep);
}

module_init(kmem_cache_init);
module_exit(kmem_cache_exit);

