/**
 * \addtogroup drivers Drivers
 * @{
 * 
 * Implementation of Drivers for DsPic
 */

/**
 * \defgroup kb Keyboard
 * @{
 */

/**
 * \file
 * Keyboard Driver
 * \author Dennis Tsang <dennis@amonics.com>
 */


#ifndef KB_H_
#define KB_H_

#ifdef KB_MOD

/**
 * \brief get keyboard port ready
 * \param flags accessing mode
 * \retval -1 error write mode is selected (errno = EROFS)
 * \retval 0 OK
 */
extern int kb_open (int flags);


/**
 * \brief copy the received data to buffer
 * \param buf pointer to buffer for reading
 * \retval -1 error not opened for read operation (errno = EBADF)
 * \retval 0 indicating no data is available
 * \retval 1 indicating 1 byte has been read
 */
extern int kb_read (unsigned char *buf);


#ifdef KB_PUSH_KEY
/**
 * \brief detect push key
 * \remarks used in idle task
 */
extern void kb_push_key (void);
#endif /* KB_PUSH_KEY */


#ifdef KB_FN_KEY
/**
 * \brief detect push key
 * \remarks used in idle task
 */
extern void kb_fn_key (void);
#endif /* KB_FN_KEY */

#endif /* KB_MOD */

#endif /* KB_H_ */

/** @} */
/** @} */
