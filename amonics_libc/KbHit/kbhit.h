/****************************************************************************
 *
 *      Get keystroke in Linux System
 * 		keystroke is standard including keyboard, console
 *
 *      Example(s)
 *      * step by step to use this module 
                include "/home/open_data/MainData/KnowledgeDb/ComputerSysDevelopment/apps/kb/KbHit/kbhit.h"
                char key_in;
                init_keyboard();                        // init. keyboard
                if (kbhit()==1) key_in = readch();      // if kbhit get a key, then call readch() to read that key
                close_keyboard();                       // release keyboard setting for kbhit function
	* in your linking, add
		$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJ) /home/open_data/MainData/KnowledgeDb/ComputerSysDevelopment/apps/kb/KbHit/kbhit.o
 *
 *      Related Functions:   
 *
 *      Valid Parameters:    
 *
 *      Note:
 *
 ****************************************************************************/

#ifndef KBHITh
#define KBHITh

void   init_keyboard(void);

//----------------------------------------------------------------------------
// Description :  close kbhit keyboard setting
// parameters :
// return :
//----------------------------------------------------------------------------
void   close_keyboard(void);

//----------------------------------------------------------------------------
// Description :  keyboard hit?
// parameters : 
// return : 
//      0 : no any key here
//      1 : keyboard gets a key, user can call readch() to collect this key
//----------------------------------------------------------------------------
int kbhit(void);

//----------------------------------------------------------------------------
// Description :  get one char
// parameters : 
// return : a char. from keyboard
//----------------------------------------------------------------------------
int readch(void);

#endif
