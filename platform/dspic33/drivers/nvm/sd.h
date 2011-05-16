/**
 * \addtogroup drivers Drivers
 * @{
 *
 * Implementation of SD Card Driver
 */

/**
 * \defgroup file_system File System
 * @{
 */

/**
 * \file
 * SD Card Driver
 * \author Dennis Tsang <dennis@amonics.com>
 */

/*
 * Copyright (C) 2011  Dennis Tsang <dennis@amonics.com>
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

#ifndef SD_H_
#define SD_H_

//SD commands, many of these are not used here
#define GO_IDLE_STATE                   0
#define SEND_OP_COND                    1
#define SEND_IF_COND                    8
#define SEND_CSD                        9
#define STOP_TRANSMISSION               12
#define SEND_STATUS                     13
#define SET_BLOCK_LEN                   16
#define READ_SINGLE_BLOCK               17
#define READ_MULTIPLE_BLOCKS            18
#define WRITE_SINGLE_BLOCK              24
#define WRITE_MULTIPLE_BLOCKS           25
#define ERASE_BLOCK_START_ADDR          32
#define ERASE_BLOCK_END_ADDR            33
#define ERASE_SELECTED_BLOCKS           38
#define SD_SEND_OP_COND                 41   //ACMD
#define APP_CMD                         55
#define READ_OCR                        58
#define CRC_ON_OFF                      59

#define ON                              1
#define OFF                             0

#endif /* SD_H_ */
