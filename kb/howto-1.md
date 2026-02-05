## Method 1: ioremap() - Map physical to virtual address (Recommended)

```c
#include <linux/io.h>

void __iomem *virt_addr;
phys_addr_t phys_addr = 0x20200000;  // Physical address (e.g., GPIO base)
size_t size = 0x1000;  // 4KB

// Map physical address to virtual address
virt_addr = ioremap(phys_addr, size);
if (!virt_addr) {
    pr_err("Failed to map physical address\n");
    return -ENOMEM;
}

// Read/write using ioread/iowrite
u32 value = ioread32(virt_addr + 0x34);  // Read register at offset 0x34
iowrite32(0x12345678, virt_addr + 0x38); // Write to offset 0x38

// Or use readl/writel (common alternative)
value = readl(virt_addr + 0x34);
writel(0x12345678, virt_addr + 0x38);

// Unmap when done
iounmap(virt_addr);
```

## Method 2: Using memory barriers for proper ordering

```c
#include <linux/io.h>

void __iomem *base;

// Map
base = ioremap(0x20200000, 0x1000);

// Read with memory barrier
u32 val = ioread32(base);
rmb();  // Read memory barrier

// Write with memory barrier
wmb();  // Write memory barrier
iowrite32(0xFF, base + 4);
```

## Method 3: Direct pointer access (for already mapped memory)

```c
volatile u32 *gpio_reg;

// Cast virtual address to pointer
gpio_reg = (volatile u32 *)virt_addr;

// Read/write
u32 val = *gpio_reg;
*gpio_reg = 0x12345678;
```

## Complete example - GPIO driver

```c
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/io.h>

// BCM2835 GPIO physical addresses
#define BCM2835_GPIO_BASE   0x20200000  // RPi 1/2/3
#define BCM2711_GPIO_BASE   0xFE200000  // RPi 4
#define GPIO_SIZE           0x1000

struct gpio_driver {
  void __iomem *gpio_base;
  phys_addr_t phys_addr;
};

static struct gpio_driver *gdrv;

// GPIO register offsets
#define GPFSEL0     0x00
#define GPSET0      0x1C
#define GPCLR0      0x28
#define GPLEV0      0x34

static void gpio_set_output(int pin) {
  u32 reg_offset = GPFSEL0 + (pin / 10) * 4;
  u32 bit_offset = (pin % 10) * 3;
	u32 val;

  // Read current value
  val = ioread32(gdrv->gpio_base + reg_offset);

  // Clear and set function (001 = output)
  val &= ~(7 << bit_offset);
	val |= (1 << bit_offset);

  // Write back
  iowrite32(val, gdrv->gpio_base + reg_offset);
}

static void gpio_set(int pin, int value) {
  u32 reg = value ? GPSET0 : GPCLR0;
  iowrite32(1 << pin, gdrv->gpio_base + reg);
}

static int gpio_get(int pin) {
  u32 val = ioread32(gdrv->gpio_base + GPLEV0);
  return (val & (1 << pin)) ? 1 : 0;
}

static int __init gpio_init(void) {
  gdrv = kzalloc(sizeof(*gdrv), GFP_KERNEL);
  if (!gdrv)
    return -ENOMEM;

  // Determine physical address based on platform
  // (In real driver, detect this from device tree)
	gdrv->phys_addr = BCM2835_GPIO_BASE;

  // Map physical address
  gdrv->gpio_base = ioremap(gdrv->phys_addr, GPIO_SIZE);
	if (!gdrv->gpio_base) {
	  pr_err("Failed to map GPIO registers\n");
		kfree(gdrv);
		return -ENOMEM;
	}

  pr_info("GPIO mapped at physical 0x%llx to virtual %p\n",
                (u64)gdrv->phys_addr, gdrv->gpio_base);

  // Example: Set GPIO 4 as output and set it high
  gpio_set_output(4);
	gpio_set(4, 1);

  pr_info("GPIO 4 set high\n");

  return 0;
}

static void __exit gpio_exit(void) {
  // Set GPIO 4 low before exit
  gpio_set(4, 0);

  // Unmap
  if (gdrv->gpio_base)
	  iounmap(gdrv->gpio_base);

  kfree(gdrv);
  pr_info("GPIO driver unloaded\n");
}

module_init(gpio_init);
module_exit(gpio_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("GPIO Physical Address Mapping Example");
```

## Method 4: Using devm_ioremap() (managed resource)

```c
#include <linux/platform_device.h>
#include <linux/io.h>

static int my_probe(struct platform_device *pdev) {
  void __iomem *base;
  struct resource *res;

  // Get resource from device tree
  res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res) {
    dev_err(&pdev->dev, "No memory resource\n");
		return -ENODEV;
	}

  // Map resource (automatically unmapped on driver removal)
  base = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(base))
	  return PTR_ERR(base);

  // Use base...
  u32 val = readl(base);

  // No need to call iounmap - managed automatically
  return 0;
}
```

## Access methods comparison:

| Function | Use Case |
|----------|----------|
| `ioremap()` | General I/O memory mapping |
| `ioremap_nocache()` | Non-cacheable mapping (obsolete, use ioremap) |
| `devm_ioremap()` | Managed mapping (auto cleanup) |
| `devm_ioremap_resource()` | Managed + resource handling |
| `ioread32()` / `iowrite32()` | Portable I/O access |
| `readl()` / `writel()` | Legacy I/O access (32-bit) |
| `readb()` / `writeb()` | 8-bit I/O access |
| `readw()` / `writew()` | 16-bit I/O access |

## Important notes:

1. **Always use `void __iomem *`** for mapped addresses (sparse checking)
2. **Always `iounmap()`** when done (unless using `devm_*`)
3. **Use `ioread*/iowrite*`** or `readl/writel`, not direct pointer access
4. **Check return value** of ioremap (can fail)
5. **Use barriers** (`rmb()`, `wmb()`, `mb()`) for ordering when needed

**Recommended:** Use `ioremap()` + `ioread*/iowrite*` for reliable, portable physical memory access in kernel drivers.
