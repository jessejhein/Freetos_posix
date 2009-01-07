/*
 * httpd-hget.c
 */
 
#include <stdlib.h>
#include <string.h>
#include <laser_ctrl.h>

#include "httpd-fsdata.h"

/***************************************************************************
 * Declare the cgi files: Copy from fsdata.c
 ***************************************************************************/
extern struct httpd_fsdata_file file_cgi_stat0_shtml[];
extern struct httpd_fsdata_file file_cgi_stat1_shtml[];
extern struct httpd_fsdata_file file_cgi_curset0_shtml[];
extern struct httpd_fsdata_file file_cgi_curset1_shtml[];
extern struct httpd_fsdata_file file_cgi_tec0_shtml[];
extern struct httpd_fsdata_file file_cgi_tec1_shtml[];
extern struct httpd_fsdata_file file_cgi_pwro_shtml[];
extern struct httpd_fsdata_file file_cgi_pwri_shtml[];

/***************************************************************************
 * Define the arguments encountered in submission of a form
 ***************************************************************************/
#define MAX_ARG     5
//cmd: /cgi/stat1.shtml?status0=Enable
#define STATUS0     "status0"
#define STATUS1     "status1"
#define DISABLE     "Disable"
#define ENABLE      "Enable"
//cmd: /cgi/curset0.shtml?cur0=200.00
#define CUR0        "cur0"
#define CUR1        "cur1"

/*---------------------------------------------------------------------------*/
void 
httpd_hget(char *str)
{   
    char* argv[MAX_ARG];
    char delim[] = "?=& ";
    int argc = 0;
    
    //--------------------
    //Breakdown the string
    //--------------------
    argv[argc++] = strtok(str, delim);
    while(1){
        argv[argc] = strtok(NULL, delim);
        if(argv[argc] == NULL || argc > MAX_ARG - 2){
            break;
        }
        argc++;
    }
    //---------------
    //Perform actions
    //---------------
    //file is /cgi/stat0.shtml?status0=Enable
    if(strcmp(argv[0], ((struct httpd_fsdata_file *)file_cgi_stat0_shtml)->name) == 0 ){
        if( strcmp(argv[1], STATUS0) == 0){
            if( strcmp(argv[2], DISABLE) == 0 ){
                laser_ctrl.enable[0] = 0;
                //TODO: add execution here
            } 
            else if( strcmp(argv[2], ENABLE) == 0 ){
                laser_ctrl.enable[0] = 1;                
                //TODO: add execution here
            } 
        }        
    }
    //file is /cgi/stat1.shtml?status1=Enable
    else if(strcmp(argv[0], ((struct httpd_fsdata_file *)file_cgi_stat1_shtml)->name) == 0 ){
        if( strcmp(argv[1], STATUS1) == 0){
            if( strcmp(argv[2], DISABLE) == 0 ){
                laser_ctrl.enable[1] = 0;
                //TODO: add execution here                
            } 
            else if( strcmp(argv[2], ENABLE) == 0 ){
                laser_ctrl.enable[1] = 1;
                //TODO: add execution here                
            } 
        }
    }
    //file is /cgi/curset0.shtml?cur0=200.00
    else if(strcmp(argv[0], ((struct httpd_fsdata_file *)file_cgi_curset0_shtml)->name) == 0 ){
        if( strcmp(argv[1], CUR0) == 0 ){
            laser_set.current_set[0] = atof(argv[2]);
            //TODO: add execution here  
        }
    }
    //file is /cgi/curset1.shtml?cur1=200.00
    else if(strcmp(argv[0], ((struct httpd_fsdata_file *)file_cgi_curset1_shtml)->name) == 0 ){
        if( strcmp(argv[1], CUR1) == 0 ){
            laser_set.current_set[1] = atof(argv[2]);
            //TODO: add execution here  
        }
    }
}
