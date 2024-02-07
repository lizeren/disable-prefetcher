#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Zeren Li");
MODULE_DESCRIPTION("A module to disable HW prefetcher");

// Global variable to store the original value of CPUACTLR_EL1
static unsigned long original_cpuactlr_value;

static void disable_hw_prefetcher(void)
{
    unsigned long value;
    // Read the current value of CPUACTLR_EL1
    asm volatile("mrs %0, S3_1_C15_C2_0" : "=r" (value));
    // Save the original value
    original_cpuactlr_value = value;
    
    // Set bit 56 to disable the Load/Store HW prefetcher
    value |= (1UL << 56);
    
    // Write the modified value back to CPUACTLR_EL1
    asm volatile("msr S3_1_C15_C2_0, %0" : : "r" (value));

    printk(KERN_INFO "HW prefetcher disabled.\n");
}

static void enable_hw_prefetcher(void)
{
    // Restore the original value of CPUACTLR_EL1
    asm volatile("msr S3_1_C15_C2_0, %0" : : "r" (original_cpuactlr_value));
    printk(KERN_INFO "HW prefetcher restored.\n");
}

static int __init prefetcher_init(void)
{
    printk(KERN_INFO "Disabling HW prefetcher...\n");
    disable_hw_prefetcher();
    return 0; // Non-zero return means that the module couldn't be loaded.
}

static void __exit prefetcher_cleanup(void)
{
    enable_hw_prefetcher();
    printk(KERN_INFO "HW prefetcher module cleanup.\n");
}

module_init(prefetcher_init);
module_exit(prefetcher_cleanup);
