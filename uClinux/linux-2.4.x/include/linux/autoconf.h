/*
 * Automatically generated by make menuconfig: don't edit
 */
#define AUTOCONF_INCLUDED
#define CONFIG_ARM 1
#undef  CONFIG_SBUS
#define CONFIG_UID16 1
#define CONFIG_RWSEM_GENERIC_SPINLOCK 1
#define CONFIG_UCLINUX 1
#define MAGIC_ROM_PTR 1

/*
 * Code maturity level options
 */
#undef  CONFIG_EXPERIMENTAL
#undef  CONFIG_OBSOLETE

/*
 * Loadable module support
 */
#undef  CONFIG_MODULES

/*
 * System Type
 */
#undef  CONFIG_ARCH_DSC21
#undef  CONFIG_ARCH_C5471
#undef  CONFIG_ARCH_CNXT
#undef  CONFIG_ARCH_NETARM
#undef  CONFIG_ARCH_TA7S
#undef  CONFIG_ARCH_TA7V
#undef  CONFIG_ARCH_SWARM
#define CONFIG_ARCH_SAMSUNG 1
#undef  CONFIG_ARCH_ATMEL
#undef  CONFIG_CPU_BIG_ENDIAN
#define CONFIG_SET_MEM_PARAM 1
#define DRAM_BASE 0x0c000000
#define DRAM_SIZE 0x00800000
#define FLASH_MEM_BASE 0x00000000
#define FLASH_SIZE 0x00200000
#define CONFIG_RAMKERNEL 1
#undef  CONFIG_ROMKERNEL
#undef  CONFIG_BOARD_SMDK40100
#define CONFIG_BOARD_MBA44 1
#undef  CONFIG_BOARD_EVS3C4530HEI
#undef  CONFIG_BOARD_S3C2500REFRGP
#undef  CONFIG_BOARD_SMDK2500
#undef  CONFIG_BOARD_SNDS100
#define CONFIG_SPU_NAME "S3C44B0X"
#define CONFIG_CPU_S3C44B0X 1
#define CONFIG_CPU_ARM710 1
#define CONFIG_CPU_32v4 1
#define CONFIG_CPU_32 1
#undef  CONFIG_CPU_26
#define CONFIG_NO_PGT_CACHE 1
#define CONFIG_CPU_WITH_CACHE 1
#undef  CONFIG_CPU_WITH_MCR_INSTRUCTION
#define CONFIG_ARM_CLK (64000000)

/*
 * General setup
 */
#undef  CONFIG_HOTPLUG
#undef  CONFIG_PCMCIA
#define CONFIG_NET 1
#undef  CONFIG_SYSVIPC
#undef  CONFIG_REDUCED_MEMORY
#undef  CONFIG_BSD_PROCESS_ACCT
#undef  CONFIG_SYSCTL
#undef  CONFIG_NWFPE
#define CONFIG_KCORE_ELF 1
#undef  CONFIG_KCORE_AOUT
#define CONFIG_BINFMT_FLAT 1
#undef  CONFIG_BINFMT_ZFLAT
#define CONFIG_KERNEL_ELF 1
#undef  CONFIG_ARTHUR
#undef  CONFIG_CMDLINE_BOOL
#undef  CONFIG_LEDS
#undef  CONFIG_ALIGNMENT_TRAP
#undef  CONFIG_RAM_ATTACHED_ROMFS

/*
 * Networking options
 */
#define CONFIG_PACKET 1
#undef  CONFIG_PACKET_MMAP
#undef  CONFIG_NETLINK_DEV
#undef  CONFIG_NETFILTER
#undef  CONFIG_FILTER
#undef  CONFIG_UNIX
#define CONFIG_INET 1
#undef  CONFIG_IP_MULTICAST
#undef  CONFIG_IP_ADVANCED_ROUTER
#undef  CONFIG_IP_PNP
#undef  CONFIG_NET_ARP_LIMIT
#undef  CONFIG_NET_IPIP
#undef  CONFIG_NET_IPGRE
#undef  CONFIG_INET_ECN
#undef  CONFIG_SYN_COOKIES
#undef  CONFIG_VLAN_8021Q
#undef  CONFIG_IPX
#undef  CONFIG_ATALK

/*
 * Appletalk devices
 */
#undef  CONFIG_DEV_APPLETALK
#undef  CONFIG_DECNET
#undef  CONFIG_BRIDGE

/*
 * QoS and/or fair queueing
 */
#undef  CONFIG_NET_SCHED
#undef  CONFIG_IPSEC

/*
 * Network testing
 */
#undef  CONFIG_NET_PKTGEN

/*
 * Network device support
 */
#define CONFIG_NETDEVICES 1

/*
 * ARCnet devices
 */
#undef  CONFIG_ARCNET
#undef  CONFIG_DUMMY
#undef  CONFIG_BONDING
#undef  CONFIG_EQUALIZER
#undef  CONFIG_TUN

/*
 * Ethernet (10 or 100Mbit)
 */
#define CONFIG_NET_ETHERNET 1
#undef  CONFIG_ARM_AM79C961A
#undef  CONFIG_ARM_CIRRUS
#undef  CONFIG_SUNLANCE
#undef  CONFIG_SUNBMAC
#undef  CONFIG_SUNQE
#undef  CONFIG_SUNGEM
#undef  CONFIG_NET_VENDOR_3COM
#undef  CONFIG_LANCE
#undef  CONFIG_NET_VENDOR_SMC
#undef  CONFIG_NET_VENDOR_RACAL
#define CONFIG_NET_ISA 1
#undef  CONFIG_E2100
#undef  CONFIG_EWRK3
#undef  CONFIG_EEXPRESS
#undef  CONFIG_EEXPRESS_PRO
#undef  CONFIG_HPLAN_PLUS
#undef  CONFIG_HPLAN
#undef  CONFIG_LP486E
#undef  CONFIG_ETH16I
#define CONFIG_NE2000 1
#undef  CONFIG_NE2000_16b
#undef  CONFIG_NET_PCI
#undef  CONFIG_NET_POCKET
#undef  CONFIG_FEC
#undef  CONFIG_CS89x0
#undef  CONFIG_UCCS8900

/*
 * Ethernet (1000 Mbit)
 */
#undef  CONFIG_ACENIC
#undef  CONFIG_DL2K
#undef  CONFIG_E1000
#undef  CONFIG_MYRI_SBUS
#undef  CONFIG_NS83820
#undef  CONFIG_HAMACHI
#undef  CONFIG_YELLOWFIN
#undef  CONFIG_R8169
#undef  CONFIG_SK98LIN
#undef  CONFIG_TIGON3
#undef  CONFIG_FDDI
#undef  CONFIG_PLIP
#undef  CONFIG_PPP
#undef  CONFIG_SLIP

/*
 * Wireless LAN (non-hamradio)
 */
#undef  CONFIG_NET_RADIO

/*
 * Token Ring devices
 */
#undef  CONFIG_TR
#undef  CONFIG_NET_FC

/*
 * Wan interfaces
 */
#undef  CONFIG_WAN

/*
 * Amateur Radio support
 */
#undef  CONFIG_HAMRADIO

/*
 * IrDA (infrared) support
 */
#undef  CONFIG_IRDA

/*
 * ATA/IDE/MFM/RLL support
 */
#undef  CONFIG_IDE
#undef  CONFIG_BLK_DEV_IDE_MODES
#undef  CONFIG_BLK_DEV_HD

/*
 * SCSI support
 */
#undef  CONFIG_SCSI

/*
 * ISDN subsystem
 */
#undef  CONFIG_ISDN

/*
 * Parallel port support
 */
#undef  CONFIG_PARPORT

/*
 * Memory Technology Devices (MTD)
 */
#undef  CONFIG_MTD

/*
 * Plug and Play configuration
 */
#undef  CONFIG_PNP
#undef  CONFIG_ISAPNP

/*
 * Block devices
 */
#undef  CONFIG_BLK_DEV_FD
#undef  CONFIG_BLK_DEV_XD
#undef  CONFIG_PARIDE
#undef  CONFIG_BLK_CPQ_DA
#undef  CONFIG_BLK_CPQ_CISS_DA
#undef  CONFIG_CISS_SCSI_TAPE
#undef  CONFIG_CISS_MONITOR_THREAD
#undef  CONFIG_BLK_DEV_DAC960
#undef  CONFIG_BLK_DEV_UMEM
#undef  CONFIG_BLK_DEV_LOOP
#undef  CONFIG_BLK_DEV_NBD
#define CONFIG_BLK_DEV_RAM 1
#define CONFIG_BLK_DEV_RAM_SIZE (1024)
#undef  CONFIG_BLK_DEV_INITRD
#undef  CONFIG_BLK_DEV_RAMDISK_DATA
#define CONFIG_BLK_DEV_BLKMEM 1
#define CONFIG_NOFLASH 1
#undef  CONFIG_AMDFLASH
#undef  CONFIG_INTELFLASH
#undef  CONFIG_BLK_STATS

/*
 * File systems
 */
#undef  CONFIG_QUOTA
#undef  CONFIG_QFMT_V2
#undef  CONFIG_AUTOFS_FS
#undef  CONFIG_AUTOFS4_FS
#undef  CONFIG_REISERFS_FS
#undef  CONFIG_REISERFS_CHECK
#undef  CONFIG_REISERFS_PROC_INFO
#undef  CONFIG_ADFS_FS
#undef  CONFIG_ADFS_FS_RW
#undef  CONFIG_AFFS_FS
#undef  CONFIG_HFS_FS
#undef  CONFIG_HFSPLUS_FS
#undef  CONFIG_BEFS_FS
#undef  CONFIG_BEFS_DEBUG
#undef  CONFIG_BFS_FS
#undef  CONFIG_EXT3_FS
#undef  CONFIG_JBD
#undef  CONFIG_JBD_DEBUG
#undef  CONFIG_FAT_FS
#undef  CONFIG_MSDOS_FS
#undef  CONFIG_UMSDOS_FS
#undef  CONFIG_VFAT_FS
#undef  CONFIG_EFS_FS
#undef  CONFIG_JFFS_FS
#undef  CONFIG_JFFS2_FS
#undef  CONFIG_CRAMFS
#undef  CONFIG_TMPFS
#define CONFIG_RAMFS 1
#define CONFIG_RAMFS 1
#undef  CONFIG_ISO9660_FS
#undef  CONFIG_JOLIET
#undef  CONFIG_ZISOFS
#undef  CONFIG_JFS_FS
#undef  CONFIG_JFS_DEBUG
#undef  CONFIG_JFS_STATISTICS
#undef  CONFIG_MINIX_FS
#undef  CONFIG_VXFS_FS
#undef  CONFIG_NTFS_FS
#undef  CONFIG_NTFS_RW
#undef  CONFIG_HPFS_FS
#define CONFIG_PROC_FS 1
#undef  CONFIG_DEVFS_FS
#undef  CONFIG_DEVFS_MOUNT
#undef  CONFIG_DEVFS_DEBUG
#undef  CONFIG_DEVPTS_FS
#undef  CONFIG_QNX4FS_FS
#undef  CONFIG_QNX4FS_RW
#define CONFIG_ROMFS_FS 1
#undef  CONFIG_EXT2_FS
#undef  CONFIG_SYSV_FS
#undef  CONFIG_UDF_FS
#undef  CONFIG_UDF_RW
#undef  CONFIG_UFS_FS
#undef  CONFIG_UFS_FS_WRITE

/*
 * Network File Systems
 */
#undef  CONFIG_CODA_FS
#undef  CONFIG_INTERMEZZO_FS
#define CONFIG_NFS_FS 1
#define CONFIG_NFS_V3 1
#undef  CONFIG_NFS_DIRECTIO
#undef  CONFIG_ROOT_NFS
#undef  CONFIG_NFSD
#undef  CONFIG_NFSD_V3
#undef  CONFIG_NFSD_TCP
#define CONFIG_SUNRPC 1
#define CONFIG_LOCKD 1
#define CONFIG_LOCKD_V4 1
#undef  CONFIG_SMB_FS
#undef  CONFIG_NCP_FS
#undef  CONFIG_NCPFS_PACKET_SIGNING
#undef  CONFIG_NCPFS_IOCTL_LOCKING
#undef  CONFIG_NCPFS_STRONG
#undef  CONFIG_NCPFS_NFS_NS
#undef  CONFIG_NCPFS_OS2_NS
#undef  CONFIG_NCPFS_SMALLDOS
#undef  CONFIG_NCPFS_NLS
#undef  CONFIG_NCPFS_EXTRAS
#undef  CONFIG_ZISOFS_FS

/*
 * Partition Types
 */
#undef  CONFIG_PARTITION_ADVANCED
#undef  CONFIG_SMB_NLS
#undef  CONFIG_NLS

/*
 * Character devices
 */
#define CONFIG_SERIAL_S3C44B0X 1
#undef  CONFIG_CONSOLE_NULL
#define CONFIG_CONSOLE_UART 1
#define CONFIG_SERIAL_S3C44B0X_CONSOLE 1

/*
 * Serial drivers
 */
#undef  CONFIG_SERIAL_ANAKIN
#undef  CONFIG_SERIAL_ANAKIN_CONSOLE
#define CONFIG_SERIAL_S3C44B0X 1
#define CONFIG_SERIAL_S3C44B0X_CONSOLE 1
#define CONFIG_S3C44B0_DEFAULT_BAUDRATE (115200)
#undef  CONFIG_SERIAL_AMBA
#undef  CONFIG_SERIAL_AMBA_CONSOLE
#undef  CONFIG_SERIAL_CLPS711X
#undef  CONFIG_SERIAL_CLPS711X_CONSOLE
#undef  CONFIG_SERIAL_21285
#undef  CONFIG_SERIAL_21285_OLD
#undef  CONFIG_SERIAL_21285_CONSOLE
#undef  CONFIG_SERIAL_UART00
#undef  CONFIG_SERIAL_UART00_CONSOLE
#undef  CONFIG_SERIAL_SA1100
#undef  CONFIG_SERIAL_SA1100_CONSOLE
#undef  CONFIG_SERIAL_8250
#undef  CONFIG_SERIAL_8250_CONSOLE
#undef  CONFIG_SERIAL_8250_EXTENDED
#undef  CONFIG_SERIAL_8250_MANY_PORTS
#undef  CONFIG_SERIAL_8250_SHARE_IRQ
#undef  CONFIG_SERIAL_8250_DETECT_IRQ
#undef  CONFIG_SERIAL_8250_MULTIPORT
#undef  CONFIG_SERIAL_8250_HUB6
#define CONFIG_SERIAL_CORE 1
#define CONFIG_SERIAL_CORE_CONSOLE 1
#undef  CONFIG_LEDMAN
#undef  CONFIG_DS1302
#undef  CONFIG_VT
#undef  CONFIG_SERIAL
#undef  CONFIG_SERIAL_EXTENDED
#undef  CONFIG_SERIAL_NONSTANDARD
#undef  CONFIG_UNIX98_PTYS

/*
 * I2C support
 */
#undef  CONFIG_I2C

/*
 * Mice
 */
#undef  CONFIG_BUSMOUSE
#undef  CONFIG_MOUSE

/*
 * Joysticks
 */
#undef  CONFIG_INPUT_GAMEPORT
#undef  CONFIG_INPUT_NS558
#undef  CONFIG_INPUT_LIGHTNING
#undef  CONFIG_INPUT_PCIGAME
#undef  CONFIG_INPUT_CS461X
#undef  CONFIG_INPUT_EMU10K1
#undef  CONFIG_INPUT_SERIO
#undef  CONFIG_INPUT_SERPORT
#undef  CONFIG_INPUT_ANALOG
#undef  CONFIG_INPUT_A3D
#undef  CONFIG_INPUT_ADI
#undef  CONFIG_INPUT_COBRA
#undef  CONFIG_INPUT_GF2K
#undef  CONFIG_INPUT_GRIP
#undef  CONFIG_INPUT_INTERACT
#undef  CONFIG_INPUT_TMDC
#undef  CONFIG_INPUT_SIDEWINDER
#undef  CONFIG_INPUT_IFORCE_USB
#undef  CONFIG_INPUT_IFORCE_232
#undef  CONFIG_INPUT_WARRIOR
#undef  CONFIG_INPUT_MAGELLAN
#undef  CONFIG_INPUT_SPACEORB
#undef  CONFIG_INPUT_SPACEBALL
#undef  CONFIG_INPUT_STINGER
#undef  CONFIG_INPUT_DB9
#undef  CONFIG_INPUT_GAMECON
#undef  CONFIG_INPUT_TURBOGRAFX
#undef  CONFIG_QIC02_TAPE
#undef  CONFIG_IPMI_HANDLER
#undef  CONFIG_IPMI_PANIC_EVENT
#undef  CONFIG_IPMI_DEVICE_INTERFACE
#undef  CONFIG_IPMI_KCS
#undef  CONFIG_IPMI_WATCHDOG

/*
 * Controller Area Network Cards/Chips
 */
#undef  CONFIG_CAN4LINUX

/*
 * Watchdog Cards
 */
#undef  CONFIG_WATCHDOG
#undef  CONFIG_SCx200_GPIO
#undef  CONFIG_AMD_PM768
#undef  CONFIG_NVRAM
#undef  CONFIG_RTC
#undef  CONFIG_DTLK
#undef  CONFIG_R3964
#undef  CONFIG_APPLICOM

/*
 * Ftape, the floppy tape device driver
 */
#undef  CONFIG_FTAPE
#undef  CONFIG_AGP

/*
 * Direct Rendering Manager (XFree86 DRI support)
 */
#undef  CONFIG_DRM

/*
 * USB support
 */
#undef  CONFIG_USB

/*
 * Support for USB gadgets
 */
#undef  CONFIG_USB_GADGET

/*
 * I2O device support
 */
#undef  CONFIG_I2O
#undef  CONFIG_I2O_BLOCK
#undef  CONFIG_I2O_LAN
#undef  CONFIG_I2O_SCSI
#undef  CONFIG_I2O_PROC

/*
 * Kernel hacking
 */
#define CONFIG_FRAME_POINTER 1
#undef  CONFIG_REVISIT
#define CONFIG_DEBUG_ERRORS 1
#undef  CONFIG_DEBUG_USER
#define CONFIG_DEBUG_INFO 1
#undef  CONFIG_MAGIC_SYSRQ

/*
 * Cryptographic options
 */
#undef  CONFIG_CRYPTO

/*
 * Library routines
 */
#undef  CONFIG_CRC32
#undef  CONFIG_ZLIB_INFLATE
#undef  CONFIG_ZLIB_DEFLATE
