/**
 * \addtogroup drivers Drivers
 * @{
 *
 * Implementation of MMC Card Driver
 */

/**
 * \defgroup mmc_driver MMC Card Driver
 * @{
 *
 * MMC Card Driver using SPI Interface
 * \li Reference: Part 1 Physical Layer Simplified Specification Version 3.01, Section 7
 *
 * This module implements the interface required by FATFS defined in diskio.h
 */

/**
 * \file
 * MMC Card Driver
 * \author Dennis Tsang <dennis@amonics.com>
 */

/*
 * Copyright (C) 2012  Dennis Tsang <dennis@amonics.com>
 *
 * This file is part of freertos_posix.
 *
 * freertos_posix is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * freertos_posix is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with freertos_posix.  If not, see <http://www.gnu.org/licenses/>
 */

#ifndef MMC_H_
#define MMC_H_

#include <diskio.h>


//SD Commands, See section 7.3.1.3, Response are defined in section 7.3.2
#define CMD0_GO_IDLE_STATE              (0)
#define CMD1_SEND_OP_COND               (1)
#define CMD8_SEND_IF_COND               (8)
#define CMD9_SEND_CSD                   (9)
#define CMD10_SEND_CID                  (10)
#define CMD12_STOP_TRANSMISSION         (12)
#define ACMD13_SD_STATUS                (13|0x80)
#define CMD16_SET_BLOCK_LEN             (16)
#define CMD17_READ_SINGLE_BLOCK         (17)
#define CMD18_READ_MULTIPLE_BLOCKS      (18)
#define ACMD23_SET_WR_BLK_ERASE_COUNT   (23|0x80)
#define CMD24_WRITE_SINGLE_BLOCK        (24)
#define CMD25_WRITE_MULTIPLE_BLOCKS     (25)
#define ACMD41_SD_SEND_OP_COND          (41|0x80)
#define CMD55_APP_CMD                   (55)
#define CMD58_READ_OCR                  (58)


/**
 * \brief Initialise disk drive
 * \param drv Drive ID
 * \return Status of MMC
 * \remarks CMD0 -> CMD8 -> (CMD58) -> CMD41 -> CMD58
 */
extern DSTATUS disk_initialize (BYTE drv);


/**
 * \brief The disk_status function returns the current disk status.
 * \param drv Specifies the physical drive number to be confirmed.
 * \return Disk status
 */
extern DSTATUS disk_status (BYTE drv);


/**
 * \brief The disk_read function reads sector(s) from the disk drive.
 * \param drv Specifies the physical drive number.
 * \param buf Pointer to the byte array to store the read data. The size of buffer must be in sector size * sector count.
 * \param sector Specifies the start sector number in logical block address (LBA).
 * \param count Specifies number of sectors to read. The value can be 1 to 128. Generally, a multiple sector transfer request must not be split into single sector transactions to the device, or you may not get good read performance.
 * \return Result status of read operation
 */
extern DRESULT disk_read (BYTE drv, BYTE *buf, DWORD sector, BYTE count);


/**
 * \brief The disk_write writes sector(s) to the disk.
 * \param drv Specifies the physical drive number.
 * \param buf Pointer to the byte array to be written.
 * \param sector Specifies the start sector number in logical block address (LBA).
 * \param count Specifies the number of sectors to write. The value can be 1 to 128. Generally, a multiple sector transfer request must not be split into single sector transactions to the device, or you will never get good write performance.
 * \return Result status of write operation
 */
extern DRESULT disk_write (BYTE drv, const BYTE *buf, DWORD sector, BYTE count);


/**
 * \brief The disk_ioctl function controls device specified features and miscellaneous functions other than disk read/write
 * \param drv Drive ID
 * \param cmd Specifies the command code
 * \param buf Pointer to the parameter buffer depends on the command code. When it is not used, specify a NULL pointer.
 * \return
 */
extern DRESULT disk_ioctl (BYTE drv, BYTE cmd, void* buf);


/**
 * \brief The get_fattime function gets current time.
 * \return Current time is returned with packed into a DWORD value. The bit field is as follows:
 * \remarks If a zero is returned, the file will not have a valid time.
 * \verbatim
    bit31:25
      Year from 1980 (0..127)
    bit24:21
      Month (1..12)
    bit20:16
      Day in month(1..31)
    bit15:11
      Hour (0..23)
    bit10:5
      Minute (0..59)
    bit4:0
      Second / 2 (0..29)
    \endverbatim
 */
extern DWORD get_fattime (void);


//-------------------------------------------------------------------------------------------------
/**
 * \brief Check whether the MMC card has been inserted
 * \remarks This function needs is called periodically in Idle Task
 */
extern void mmc_check_card_insert (void);


/**
 * \brief Initialise the card detect pin of MMC
 * \remarks This function is implemented externally
 */
extern void mmc_card_detect_pin_init (void);


/**
 * \brief Read MMC card detection pin to determine if MMC is inserted into socket
 * \retval 1 Card inserted
 * \retval 0 No Card inserted
 * \remarks This function is implemented externally
 */
extern int mmc_card_detect (void);

#endif /* MMC_H_ */

/** @} */
/** @} */
