/*
 *  drivers/mtd/nand/armsys_44b0x.c
 *
 *  Overview:
 *   This is a device driver for the NAND flash device found on the
 *   ARMSYS board which utilizes the Samsung K9F2808 part. This is
 *   a 128Mbit (16M x 8 bits or 8M x 16bit ) NAND flash device.
 */

#include <linux/slab.h>
#include <linux/module.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/partitions.h>
#include <asm/io.h>

/*
 * MTD structure for ARMSYS board
 */
static struct mtd_info *armsys_mtd = NULL;

/*
 * Values specific to the ARMSYS board (used with 44B0X processor)
 */
#define ARMSYS_IO_BASE	  0x01d20000	/* Start of 44B0X IO address space */
#define ARMSYS_FIO_BASE  0x2000000	/* Address where flash is mapped */
#define ARMSYS_PEDR	  0x0014	/*
					 * IO offset to Port C data register
					 * where the CLE, ALE and NCE pins
					 * are wired to.
					 */
#define ARMSYS_PEDDR	 0x0010 	/*
					 * IO offset to Port C control register
					 * where the CLE, ALE and NCE pins
					 * are wired to.
					 */


/*
 * Module stuff
 */

static int armsys_io_base = ARMSYS_IO_BASE;
static int armsys_fio_base = ARMSYS_FIO_BASE;
static int armsys_pedr = ARMSYS_PEDR;
static int armsys_peddr = ARMSYS_PEDDR;

#ifdef MODULE
MODULE_PARM(armsys_io_base, "i");
MODULE_PARM(armsys_fio_base, "i");
MODULE_PARM(armsys_pedr, "i");
MODULE_PARM(armsys_peddr, "i");

__setup("armsys_io_base=",armsys_io_base);
__setup("armsys_fio_base=",armsys_fio_base);
__setup("armsys_pedr=",armsys_pedr);
__setup("armsys_peddr=",armsys_peddr);
#endif

#ifdef CONFIG_MTD_PARTITIONS
/*
 * Define partitions for flash device
 */
const static struct mtd_partition partition_info[] = {
	{ name: "ARMSYS flash partition 1",
	  offset: 2*1024*1024,
	  size: 6*1024*1024 },
	{ name: "ARMSYS flash partition 2",
	  offset: 8*1024*1024,
	  size: 6*1024*1024 }
};
#define NUM_PARTITIONS 2
#endif

/* 
 *	hardware specific access to control-lines
*/
void armsys_hwcontrol(int cmd){

    switch(cmd){

	case NAND_CTL_SETCLE: (*(volatile unsigned *) (armsys_io_base + armsys_pedr)) |=  0x04; break;
	case NAND_CTL_CLRCLE: (*(volatile unsigned *) (armsys_io_base + armsys_pedr)) &= ~0x04; break;

	case NAND_CTL_SETALE: (*(volatile unsigned *) (armsys_io_base + armsys_pedr)) |=  0x08; break;
	case NAND_CTL_CLRALE: (*(volatile unsigned *) (armsys_io_base + armsys_pedr)) &= ~0x08; break;

	case NAND_CTL_SETNCE: (*(volatile unsigned *) (armsys_io_base + armsys_pedr)) &= ~0x02; break;
	case NAND_CTL_CLRNCE: (*(volatile unsigned *) (armsys_io_base + armsys_pedr)) |=  0x02; break;
    }
}

/*
 * Main initialization routine
 */
int __init armsys_init (void)
{
	struct nand_chip *this;

	/* Allocate memory for MTD device structure and private data */
	armsys_mtd = kmalloc (sizeof(struct mtd_info) + sizeof (struct nand_chip),
				GFP_KERNEL);
	if (!armsys_mtd) {
		printk ("Unable to allocate ARMSYS NAND MTD device structure.\n");
		return -ENOMEM;
	}

	/* Get pointer to private data */
	this = (struct nand_chip *) (&armsys_mtd[1]);

	/* Initialize structures */
	memset((char *) armsys_mtd, 0, sizeof(struct mtd_info));
	memset((char *) this, 0, sizeof(struct nand_chip));

	/* Link the private data with the MTD structure */
	armsys_mtd->priv = this;

	/*
	 * Set GPIO Port C control register so that the pins are configured
	 * to be outputs for controlling the NAND flash.
	 */
	(*(volatile unsigned *) (armsys_io_base + armsys_peddr)) = 0x0f00ff54;

	/* Set address of NAND IO lines */
	this->IO_ADDR_R = armsys_fio_base;
	this->IO_ADDR_W = armsys_fio_base;
	/* Set address of hardware control function */
	this->hwcontrol = armsys_hwcontrol;
	/* 15 us command delay time */
	this->chip_delay = 15;		

	/* Scan to find existence of the device */
	if (nand_scan (armsys_mtd)) {
		kfree (armsys_mtd);
		return -ENXIO;
	}

	/* Allocate memory for internal data buffer */
	this->data_buf = kmalloc (sizeof(u_char) * (armsys_mtd->oobblock + armsys_mtd->oobsize), GFP_KERNEL);
	if (!this->data_buf) {
		printk ("Unable to allocate NAND data buffer for ARMSYS.\n");
		kfree (armsys_mtd);
		return -ENOMEM;
	}

	/* Allocate memory for internal data buffer */
	this->data_cache = kmalloc (sizeof(u_char) * (armsys_mtd->oobblock + armsys_mtd->oobsize), GFP_KERNEL);
	if (!this->data_cache) {
		printk ("Unable to allocate NAND data cache for ARMSYS.\n");
		kfree (this->data_buf);
		kfree (armsys_mtd);
		return -ENOMEM;
	}
	this->cache_page = -1;

        INIT_LIST_HEAD(&this->wq.task_list);

	/* Register the partitions */
	add_mtd_partitions(armsys_mtd, (struct mtd_partition *)partition_info, NUM_PARTITIONS);

	/* Return happy */
	return 0;
}
module_init(armsys_init);

/*
 * Clean up routine
 */
#ifdef MODULE
static void __exit armsys_cleanup (void)
{
	struct nand_chip *this = (struct nand_chip *) &armsys_mtd[1];

	/* Unregister the device */
	del_mtd_device (armsys_mtd);

	/* Free internal data buffer */
	kfree (this->data_buf);
	kfree (this->page_cache);

	/* Free the MTD device structure */
	kfree (armsys_mtd);
}
module_exit(armsys_cleanup);
#endif

/*MODULE_LICENSE("GPL");
MODULE_AUTHOR("Steven J. Hill <sjhill@cotw.com");
MODULE_DESCRIPTION("Board-specific glue layer for NAND flash on ARMSYS board");*/
