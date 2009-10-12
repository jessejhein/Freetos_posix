/**
 * \file
 * Implementation of circular buffer
 * \author Cheng Chung Yan <yan@amonics.com>
 * \author Dennis Tsang <dennis@amonics.com>
 */

unsigned char
pre_wr_cir254buf (unsigned char wr, unsigned char rd, unsigned char max)
{
  unsigned char wr_backup = wr + 1;     //normal position of next char to write
  if (wr_backup == max) wr_backup = 0;  //if reach the end of buffer, loop back
  if (wr_backup == rd) return 255;      //Buffer is full
  else return wr_backup;
}

unsigned char
pre_rd_cir254buf (unsigned char wr, unsigned char rd, unsigned char max)
{
  unsigned char rd_backup;
  if (rd == wr) return 255;                     //all data read
  else
    {
      rd_backup = rd + 1;                       //normal position of next char to read
      if (rd_backup == max) rd_backup = 0;      //if reach the end of buffer, loop back
      return rd_backup;
    }
}

