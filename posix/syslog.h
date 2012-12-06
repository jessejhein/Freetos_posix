/**
 * \file
 * System Log File
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

#ifndef SYSLOG_H_
#define SYSLOG_H_  1

/**
 * \brief Create a System Log File under the FS_ROOT directory
 * \retval 1 File created
 * \retval 0 Cannot create file
 */
extern int syslog_open (void);


/**
 * \brief Close the System Log File (if available)
 */
extern void syslog_close (void);


/**
 * \brief Append a message in System Log File (if available)
 * \param msg Pointer to message
 * \retval 1 File in used
 * \retval 0 Message appended/No file available
 * \remarks The message is preceded by a time-stamp and ends with a Windows newline
 */
extern int syslog_append (char* msg);

#endif /* SYSLOG_H_ */
