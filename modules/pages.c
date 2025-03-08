#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/gfp.h>
#include <linux/fs.h>
#include <linux/types.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Chladas");
MODULE_DESCRIPTION("Page allocation");

/**
 * Prints the virtual and physical addresses of a given page.
 */
static void print_page_addresses(struct page *page, const char *label)
{
    void *vaddr = page_address(page);
    phys_addr_t paddr = virt_to_phys(vaddr);

    printk(KERN_INFO "%s: Virtual address: %px, Physical address: %pa\n",
           label, vaddr, &paddr);
}

static int __init page_alloc_init(void)
{
    struct page *page;
    int i, order = 2;
    int num_pages = 1 << order;  // 2^order pages

    printk(KERN_INFO "Module LOADED\n");

    /** Allocating pages */
    page = alloc_pages(GFP_KERNEL, order);
    if (!page) {
        printk(KERN_ERR "Page allocation failed!\n");
        return -ENOMEM;
    }

    printk(KERN_INFO "Address of the first allocated page struct: %px\n", page);
    printk(KERN_INFO "Size of struct page: %zu\n", sizeof(struct page));

    for (i = 0; i < num_pages; i++) {
        char label[32];
        snprintf(label, sizeof(label), "Page %d", i);
        print_page_addresses(page + i, label);
    }

    /** Accessing the first page in memory */
#ifdef CONFIG_FLATMEM
    struct page *first_page = (struct page *)contig_page_data.node_mem_map;
    print_page_addresses(first_page, "First system page");
#else
    printk(KERN_WARNING "contig_page_data.node_mem_map is not available on this system.\n");
#endif

    /** Freeing pages */
    __free_pages(page, order);

    return 0;
}

static void __exit page_alloc_exit(void)
{
    printk(KERN_INFO "Module REMOVED\n");
}

module_init(page_alloc_init);
module_exit(page_alloc_exit);

