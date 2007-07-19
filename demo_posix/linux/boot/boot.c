/************************************************************************************************
 * File: 			boot.c
 * Description:		boot up file Linux
 ***********************************************************************************************
 * DESCRIPTION:
 * 	1) 	vUserMain() should contains the creation of application threads/coroutine
 ***********************************************************************************************/

int main( void )
{
	/* Configure any hardware. */
	vSetupHardware();

	/* Create the main task. */
	vUserMain();

	/* Will only reach here if there is insufficient heap available to start
	the scheduler. */
	return 0;
}


