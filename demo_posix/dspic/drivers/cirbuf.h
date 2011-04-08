/**
 * \file
 * Implementation of circular buffer
 * \author Cheng Chung Yan <yan@amonics.com>
 * \author Dennis Tsang <dennis@amonics.com>
 */

#ifndef CIRBUF_H_
#define CIRBUF_H_

/**
 * \brief Determine the next valid position to write to the buffer
 * \param wr
 * \param rd
 * \param max
 * \retval <254 next position of buffer
 * \retval 255 buffer is full
 * \remarks
 * \verbatim
   Design Concept
     Make a circular buffer template.
     this is only a template, not for real code to work, please implement this code into
     your application area.

               | wr ptr
    top        V                              bottom
     |<------------------------------------>| buffer                 wr and rd ptr, they both
                             ^                                       wr ptr must go prior rd         must go upward, then circular
                             | rd ptr
   \endverbatim
 */
extern unsigned char pre_wr_cir254buf (unsigned char wr, unsigned char rd, unsigned char max);

/**
 * \brief Determine the next valid position to read from the buffer
 * \param wr
 * \param rd
 * \param max
 * \retval <254 next position of buffer
 * \retval 255 no data
 */
extern unsigned char pre_rd_cir254buf (unsigned char wr, unsigned char rd, unsigned char max);

#endif /* CIRBUF_H_ */
