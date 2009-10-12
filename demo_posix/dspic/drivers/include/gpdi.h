/**
 * \addtogroup drivers Drivers
 * @{
 * 
 * Implementation of Drivers for DsPic
 */

/**
 * \defgroup gpdi General Purpose Digital Input
 * @{
 */

/**
 * \file
 * General Purpose Digital Input Driver
 * \author Dennis Tsang <dennis@amonics.com>
 */

#ifndef GPDI_H_
#define GPDI_H_

#ifdef GPDI_MOD

/**
 * \brief get GPDI port ready
 * \param flags accessing mode
 * \retval 0 ok
 */
extern int gpdi_open (int flags);


/**
 * \brief read the status to selected GPDI channel
 * \param buf pointer to buffer for reading
 * \retval -1 error not opened for read operation (errno = EBADF)
 * \retval 1 indicating 1 byte has been read
 */
extern int gpdi_read (unsigned char *buf);


/**
 * \brief change the configuration of GPDI module
 * \param request Request code - defined in ioctl.h (gpdi_SELECT_CH) 
 * \param argp channel id
 * \retval 0 success
 * \retval -1 error
 */
extern int gpdi_ioctl (int request, unsigned char* argp);


/**
 * \brief GPDI control job
 * \param arg no use
 */
void* gpdi_ctrl (void* arg);

#endif /* GPDI_MOD */

#endif /* GPDI_H_ */

/** @} */
/** @} */
