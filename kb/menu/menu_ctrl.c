/*
 * Copyright (c) 2005 cheng chung yan <yan@amonics.com>
 * menu control
 * 
 * Description
 * 
	Pre-define
	* user must define each node and its node no., it is hardcode
	* if end of node in file defined(extend node no.), if press "ENTER", go back to parent
	
	Level0, no.   Level1, no.          Level2, no. 
	C(file2)  --------------+--- B (node1) 
	                        +--- A
	                
	B(file1)  --------------+--- C (node2)
	                        +--- B --------------+--- B (node3)
	                                             +--- A
	                        +--- A
	                
	A(file0) 
	(node0)
	
	Data Structure
		NODE
		* max. local files      = MAX LOCAL FILE NUM.
		* local file no.        = 0=<x=<max. local files
		* connected             = 0 as no connect to child, 1 as connected to child
		* parent node no.       = 0 as top; otherwise parent node no.
		* FILE X max. local files
		        * FILE 0
		        * FILE 1
		        *  ...
		FILE
		* extend node no.        =4 0 as un-extenedable; node no. as extenedable
	
		NODE node[MAX_NODE_NUM]

	Methods	
		key-in
		* event comes
		* find out location and menu status
		* do location corresponding task
		
		display
		* spec. location 
		        * screen will not auto. gen.
		        * so programmer must implement screen by yourselve, so can specific the location
		* find out menu status in that location
		* display location corresponding task
 * 
 */

// including, get data linking from others ===============================================
//	base standard i/o
#include <stdio.h>

//	data =================================================================================
//		global temp. variable, atomic usage
unsigned char tmp;
//		menu
//			parameters
//				MAX_NODE -- how many nodes inside this menu
//				CAP_MAX_FILE_NUM_IN_NODE -- capacity of max. file no. inside this node
//											eg. 2 as 2 bits for max. 4 of files
//				CAP_MAX_NUM_NODE -- capacity of max. no. of node
#define	MAX_NODE					4
#define	CAP_MAX_FILE_NUM_IN_NODE	2
#define	CAP_MAX_NUM_NODE			2
//			node
struct Node {
	unsigned max_files		: CAP_MAX_FILE_NUM_IN_NODE;		// the node can contains this max. no. of file
	unsigned files_no		: CAP_MAX_FILE_NUM_IN_NODE;		// index of file inside a node
	unsigned connect_child	: 1;
	unsigned parent_node	: CAP_MAX_NUM_NODE;				// parent node no.(should be index no. inside the node array)
															// 0 as top or parent as Node0; otherwise parent node index
	unsigned char* file;									// file, i.e. pt to file array of this node
															// 0 as no connections to child; any no. as connected
															// inside the file array, pt to child node if any no., 0 as un-extenedable in this file
															// this pt + files_no indicates which file inside this node
};
struct Node node[MAX_NODE];
unsigned char exist_node_num = 0;

//			change up a file no.
#define	up_exist_file_no()		(node[exist_node_num].files_no>=(node[exist_node_num].max_files-1)) ? node[exist_node_num].files_no=0 : node[exist_node_num].files_no++
//			change down a file no.
#define	down_exist_file_no()	(node[exist_node_num].files_no==0) ? node[exist_node_num].files_no=(node[exist_node_num].max_files-1) : node[exist_node_num].files_no--

// methods ===============================================================================

//	is child here in this node inside this menu
//	return : 1 as selected, 0 as non-selected
unsigned char is_child_here(unsigned char node_num, unsigned char file)
{
	if ((node[node_num].connect_child&&node[node_num].files_no==file)) return 1;
	else return 0;
}

//	is step in this item inside this menu
unsigned char is_step_in_this_item_menu(unsigned char node_num, unsigned char file)
{
	if ((node[node_num].files_no==file)&&(node[node_num].connect_child==0)) return 1;
	else return 0;
}

//	change to next node, usually used by ENTER cmd
void next_node(void)
{
	//if (node[exist_node_num].connect_child==0) {					// this situation must not exist, since this method is called inside the sub-menu of this connected node
		if (node[exist_node_num].file!=0)							// if file ptr is 0, i.e. no child any more
			tmp = *(node[exist_node_num].file + node[exist_node_num].files_no);
		else tmp = 0;
		if (tmp==0)	{												// no child
			exist_node_num = node[exist_node_num].parent_node;		//	change the node no. 1st
			node[exist_node_num].connect_child = 0;					//	then reset this parent node to non-connected
		} else {													// has child
			node[exist_node_num].connect_child = 1;					//	must set the old node connected flag
			exist_node_num = tmp;									//	then change the node no.
		}
	//} else {
	//}
}

int main()
{
	
//	variables =========================================================

//		kb
    char key_in = 0;
    unsigned char state = 0;
    #define	KB_NONE_EVENT		0
    #define	KB_UP_EVENT			KB_NONE_EVENT+1
    #define	KB_DOWN_EVENT		KB_UP_EVENT+1
    #define	KB_ENTER_EVENT		KB_DOWN_EVENT+1
    #define	KB_QUIT_EVENT		KB_ENTER_EVENT+1
    unsigned char event = KB_NONE_EVENT;
    
//		menu
	#define	NODE0_MAX_FILE				3
	unsigned char node0_files[NODE0_MAX_FILE];
	node0_files[0] = 0;					// node0 file0 pts to no-node
	node0_files[1] = 2;					// node0 file1 pts to node2
	node0_files[2] = 1;					// node0 file2 pts to node1
	#define	NODE1_MAX_FILE				2
//	unsigned char node1_files[NODE1_MAX_FILE];
//	node1_files[0] = 0;
//	node1_files[1] = 0;
	#define	NODE2_MAX_FILE				3
	unsigned char node2_files[NODE2_MAX_FILE];
	node2_files[0] = 0;
	node2_files[1] = 3;
	node2_files[2] = 0;
	#define	NODE3_MAX_FILE				2
//	unsigned char node3_files[NODE3_MAX_FILE];
//	node3_files[0] = 0;
//	node3_files[1] = 0;

	node[0].max_files = NODE0_MAX_FILE;	// only have 3 items in main menu
	node[0].files_no = 0;				// init. pt to item 0
	node[0].connect_child = 0;			// init. no any connect to child node
	node[0].parent_node = 0;			// no parent, since it is top
	node[0].file = node0_files;
	node[1].max_files = NODE1_MAX_FILE;
	node[1].files_no = 0;
	node[1].connect_child = 0;
	node[1].parent_node = 0;
	node[1].file = 0;
	node[2].max_files = NODE2_MAX_FILE;
	node[2].files_no = 0;
	node[2].connect_child = 0;
	node[2].parent_node = 0;
	node[2].file = node2_files;
	node[3].max_files = NODE3_MAX_FILE;
	node[3].files_no = 0;
	node[3].connect_child = 0;
	node[3].parent_node = 2;
	node[3].file = 0;

//		screen
	unsigned int disp_cnt = 0;

//	init ==============================================================
    init_keyboard();
    
//	main looping ======================================================
    do {

//		detect kb =====================================================
//			Enter			key_in = 10
//			DOWN			key_in = 27
//							key_in = 91
//							key_in = 66
//			UP				key_in = 27
//							key_in = 91
//							key_in = 65
    	if (kbhit()==1) {
    		key_in = readch();
    		switch (key_in) {
    			case 10 : 
    				event = KB_ENTER_EVENT;
					state = 0;
    				break;
    			case 27 : 
    				state++;
    				break;
    			case 91 : 
    				state++;
    				break;
    			case 65 : 
					if (state==2) {
						event = KB_UP_EVENT;
						state = 0;
					}
    				break;
    			case 66 : 
					if (state==2) {
						event = KB_DOWN_EVENT;
						state = 0;
					}
    				break;
    			case 113 : 
					event = KB_QUIT_EVENT;
    				break;
    			default :
    				state = 0;
    				//printf("key_in = %d\n", key_in);
    				break;
    		}
    	}
    	
//		use event =======================================================
   		switch (event) {
   			case KB_UP_EVENT : 
   				//printf("event = KB_UP_EVENT\n");
				up_exist_file_no();
   				event = KB_NONE_EVENT;
   				break;
   			case KB_DOWN_EVENT : 
   				//printf("event = KB_DOWN_EVENT\n");
				down_exist_file_no();
   				event = KB_NONE_EVENT;
   				break;
   			case KB_ENTER_EVENT : 
   				//printf("event = KB_ENTER_EVENT\n");
   				next_node();
   				event = KB_NONE_EVENT;
   				break;
   			case KB_QUIT_EVENT : 
   				//printf("event = KB_QUIT_EVENT\n");
   				break;
   		}
    	
//		screen =======================================================
		if (disp_cnt>5000) {
			
			printf("\033[2J");						// clr terminal screen

	   		printf("C");
			if (is_step_in_this_item_menu(0, 2)) {
				printf(" *\n");
			} else if (is_child_here(0,2)) {
				printf("  \n");

		   		printf("	C.B");
				if (is_step_in_this_item_menu(1, 1)) {
					printf("	 *\n");
				} else printf("	  \n");
		   		
		   		printf("	C.A");
				if (is_step_in_this_item_menu(1, 0)) {
					printf("	 *\n");
				} else printf("	  \n");
		   		
			} else printf("  \n");
	   		
	   		printf("B");
			if (is_step_in_this_item_menu(0, 1)) {
				printf(" *\n");
			} else if (is_child_here(0,1)) {
				printf("  \n");

		   		printf("	B.C");
				if (is_step_in_this_item_menu(2, 2)) {
					printf("	 *\n");
				} else printf("	  \n");
		   		
		   		printf("	B.B");
				if (is_step_in_this_item_menu(2, 1)) {
					printf("	 *\n");
				} else if (is_child_here(2,1)) {
					printf("  \n");
					
			   		printf("		B.B.B");
					if (is_step_in_this_item_menu(3, 1)) {
						printf("	 *\n");
					} else printf("	  \n");
			   		
			   		printf("		B.B.A");
					if (is_step_in_this_item_menu(3, 0)) {
						printf("	 *\n");
					} else printf("	  \n");
			   		
				} else printf("	  \n");
		   		
		   		printf("	B.A");
				if (is_step_in_this_item_menu(2, 0)) {
					printf("	 *\n");
				} else printf("	  \n");
		   		
			} else printf("  \n");
	   		
	   		printf("A");
			if (is_step_in_this_item_menu(0, 0)) printf(" *\n");
	   		else printf("  \n");
	   		
	   		printf("\n");
	   		disp_cnt = 0;
		} else disp_cnt++;
   		
    } while (event!=KB_QUIT_EVENT);
    
//	close ==============================================================
    close_keyboard();

	return 0;
}
