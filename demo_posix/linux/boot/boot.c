/**
 * \file
 * boot up file Linux
 * vUserMain() should contains the creation of application threads/coroutine
 * \author Dennis Tsang <dennis@amonics.com>
 */

/**
 * \brief main program
 */
int 
main(void)
{
  /* Configure any hardware. */
  vSetupHardware();

  /* Create the main task. */
  vUserMain();

  /* main thread never ends */
  while(1);
  
  return 0;
}
