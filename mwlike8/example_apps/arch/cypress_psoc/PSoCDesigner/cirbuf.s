	.module cirbuf.c
	.area text(rom, con, rel)
	.dbfile ./cirbuf.c
	.dbfunc e pre_wr_cir254buf _pre_wr_cir254buf fc
;      wr_backup -> X+0
;            max -> X-6
;             rd -> X-5
;             wr -> X-4
_pre_wr_cir254buf::
	.dbline -1
	push X
	mov X,SP
	add SP,1
	.dbline 24
; /*	Copyright (C) 2004 cheng chung yan <yan@amonics.com> for 8-bit system linlike8
;  *	lib C in linlike8 8 bit system
;  *
;  *	cirbuf.c
;  *
;  *	Design Concept
;  *		Make a circular buffer template.
;  *		this is only a template, not for real code to work, please implement this code into your application area.
;  *
;  *			| wr ptr
;  *		top	V			       bottom
;  *		|<------------------------------------>| buffer		wr and rd ptr, they both must go upward, then circular
;  *				^					wr ptr must go prior rd
;  *				| rd ptr
;  *
;  *  Testing Procedures
;  *	before you compile, you must copy whole directory of linlike8/ to your local home
;  *	compile this file in linux gcc environment : " make cirbuf_to_run.o"
;  *	testing by : "main_compile_dir/cirbuf_to_run.o"
;  *
;  */
; 
; unsigned char pre_wr_cir254buf(unsigned char wr, unsigned char rd, unsigned char max)
; {
	.dbline 25
; 	unsigned char wr_backup = wr + 1;
	mov A,[X-4]
	add A,1
	mov [X+0],A
	.dbline 26
; 	if (wr_backup==max) wr_backup = 0;
	mov A,[X+0]
	cmp A,[X-6]
	jnz L2
	.dbline 26
	mov [X+0],0
L2:
	.dbline 27
; 	if (wr_backup==rd) return 255;
	mov A,[X+0]
	cmp A,[X-5]
	jnz L4
	.dbline 27
	mov A,-1
	xjmp L1
L4:
	.dbline 28
; 	else return wr_backup;
	mov A,[X+0]
	.dbline -2
L1:
	add SP,-1
	pop X
	.dbline 0 ; func end
	ret
	.dbsym l wr_backup 0 c
	.dbsym l max -6 c
	.dbsym l rd -5 c
	.dbsym l wr -4 c
	.dbend
	.dbfunc e pre_rd_cir254buf _pre_rd_cir254buf fc
;      rd_backup -> X+0
;            max -> X-6
;             rd -> X-5
;             wr -> X-4
_pre_rd_cir254buf::
	.dbline -1
	push X
	mov X,SP
	add SP,1
	.dbline 32
; }
; 
; unsigned char pre_rd_cir254buf(unsigned char wr, unsigned char rd, unsigned char max)
; {
	.dbline 34
; 	unsigned char rd_backup;
; 	if (rd==wr) return 255;
	mov A,[X-5]
	cmp A,[X-4]
	jnz L7
	.dbline 34
	mov A,-1
	xjmp L6
L7:
	.dbline 35
; 	else {
	.dbline 36
; 		rd_backup = rd + 1;
	mov A,[X-5]
	add A,1
	mov [X+0],A
	.dbline 37
; 		if (rd_backup==max) rd_backup = 0;
	mov A,[X+0]
	cmp A,[X-6]
	jnz L9
	.dbline 37
	mov [X+0],0
L9:
	.dbline 38
; 		return rd_backup;
	mov A,[X+0]
	.dbline -2
L6:
	add SP,-1
	pop X
	.dbline 0 ; func end
	ret
	.dbsym l rd_backup 0 c
	.dbsym l max -6 c
	.dbsym l rd -5 c
	.dbsym l wr -4 c
	.dbend
