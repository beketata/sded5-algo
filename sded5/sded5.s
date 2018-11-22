
#include "at91sam9g20/AT91SAM9G20.h"

#define USB_ADDR 0x10000000

Pin MACRO
		DC32 \1, \2
		DC8 \3
		DC8 \4
		DC8 \5
		DC8 \6
	ENDM

FILL MACRO
		rept \2
			DC8 \1
		endr
	ENDM

#define C

	MODULE  ?cstartup

	; Forward declaration of sections.
	SECTION IRQ_STACK:DATA:NOROOT(2)
	SECTION CSTACK:DATA:NOROOT(3)

//------------------------------------------------------------------------------
//         Startup routine
//------------------------------------------------------------------------------

	SECTION .vectors:CODE:NOROOT(2)

	PUBLIC	AlgoBinStart
	PUBLIC  __iar_program_start

	EXTERN	?main
	EXTERN	USB_ISR_SPU
	EXTERN	SMC_Setup
	EXTERN	DBGU_Init
	EXTERN	SDRAM_Init

	EXTERN	PIO_Configure

	ARM

; ---------------------------------------------------------------------------
__iar_program_start
AlgoBinStart

		B				resetHandler
; ---------------------------------------------------------------------------

loc_20000004				; CODE XREF: ROM:loc_20000004
		B				loc_20000004
; ---------------------------------------------------------------------------

loc_20000008				; CODE XREF: ROM:loc_20000008
		B				loc_20000008
; ---------------------------------------------------------------------------

loc_2000000C				; CODE XREF: ROM:loc_2000000C
		B				loc_2000000C
; ---------------------------------------------------------------------------

loc_20000010				; CODE XREF: ROM:loc_20000010
		B				loc_20000010
; ---------------------------------------------------------------------------

loc_20000014				; CODE XREF: ROM:loc_20000014
		B				loc_20000014
; ---------------------------------------------------------------------------

loc_20000018				; CODE XREF: ROM:loc_20000018
		B				loc_20000018
; ---------------------------------------------------------------------------

loc_2000001C				; CODE XREF: ROM:loc_2000001C
		B				loc_2000001C

; =============== S U B	R O U T	I N E =======================================


Vectors:

		LDR			PC, off_20000040

undefVector
		LDR			PC, off_20000044

swiVector
		LDR			PC, off_20000048

prefetchAbortVector
		LDR			PC, off_2000004C

dataAbortVector
		LDR			PC, off_20000050

reservedVector
		NOP

irqVector
		LDR			PC, [PC,#-0xF20]

fiqHandler
		LDR			PC, [PC,#-0xF20]


; ---------------------------------------------------------------------------
off_20000040	DCD resetHandler	; DATA XREF: Vectors
off_20000044	DCD undefVector_0	; DATA XREF: undefVector
off_20000048	DCD swiVector_0		; DATA XREF: swiVector
off_2000004C	DCD prefetchAbortVector_0 ; DATA XREF: prefetchAbortVector
off_20000050	DCD dataAbortVector_0	; DATA XREF: dataAbortVector
; ---------------------------------------------------------------------------

		B				resetHandler

undefVector_0
		B				undefVector_0

swiVector_0
		B				swiVector_0

prefetchAbortVector_0
		B				prefetchAbortVector_0

dataAbortVector_0
		B				dataAbortVector_0

sub_20000068
		B				sub_20000068

sub_2000006C
		B				sub_20000074

sub_20000070
		B				sub_20000070

; =============== S U B	R O U T	I N E =======================================


sub_20000074				; CODE XREF: sub_2000006C
		SUB			LR, LR,	#4
		STMFD		SP!, {LR}
		MRS			LR, SPSR
		STMFD		SP!, {R0,LR}
		LDR			LR, =AT91C_BASE_AIC ; (AIC) Source	Mode Register
		LDR			R0, [LR,#0x100]
		STR			LR, [LR,#0x100]
		MSR			CPSR_c,	#0x13
		STMFD		SP!, {R1-R3,R12,LR}
		MOV			LR, PC
		BX			R0
		LDMFD		SP!, {R1-R3,R12,LR}
		MSR			CPSR_c,	#0x92
		LDR			LR, =AT91C_BASE_AIC ; (AIC) Source	Mode Register
		STR			LR, [LR,#0x130]
		LDMFD		SP!, {R0,LR}
		MSR			SPSR_cxsf, LR
		LDMFD		SP!, {PC}^

; End of function sub_20000074


; =============== S U B	R O U T	I N E =======================================

resetHandler

		LDR			R0, =Vectors

		MSR			CPSR_c,	#0xD3
		LDR			SP, =0x301000
		STMFD		SP!, {R0}

		LDR			R0, =LowLevelInit
		MOV			LR, PC
		BX			R0

		LDMFD		SP!, {R0}
		LDR			R0, =Vectors
		MOV			R1, #0
		MOV			R2, #0x200

loc_200000E8
		CMP			R1, R2
		LDRCC		R3, [R0],#4
		STRCC		R3, [R1],#4
		BCC			loc_200000E8

		MOV			R0, #0x2000
		MSR			CPSR_c,	#0xD1				; FIQ
		MOV			SP, R0
		SUB			R0, R0,	#0x34

		MSR			CPSR_c,	#0xD2				; IRQ
		MOV			SP, R0
		SUB			R0, R0,	#0x60

		MSR			CPSR_c,	#0xD7				; Abort
		MOV			SP, R0
		SUB			R0, R0,	#4

		MSR			CPSR_c,	#0xDB				; Undefined
		MOV			SP, R0
		SUB			R0, R0,	#4

		MSR			CPSR_c,	#0x13				; Supervisor
		MOV			SP, R0

		LDR			R0, =sub_200019BC
		MOV			LR, PC
		BX			R0

		MOV			R0, #0
		MCR			p15, 0,	R0,c7,c7, 0
		MCR			p15, 0,	R0,c7,c10, 4
		MCR			p15, 0,	R0,c8,c7, 0
		MOV			R0, #0x300000
		MCR			p15, 0,	R0,c2,c0, 0
		MOV			R0, #0xFFFFFFFF
		MCR			p15, 0,	R0,c3,c0, 0
		MRC			p15, 0,	R0,c1,c0, 0
		ORR			R0, R0,	#4
		ORR			R0, R0,	#0x1000
		ORR			R0, R0,	#1
		MCR			p15, 0,	R0,c1,c0, 0

		NOP
		NOP
		NOP

		LDR			R0, =?main ; argc
		BX			R0 ; main

; End of function resetHandler


; =============== S U B	R O U T	I N E =======================================

; Attributes: noreturn

sub_2000018C				; CODE XREF: sub_2000018C
		B				sub_2000018C
; End of function sub_2000018C


; =============== S U B	R O U T	I N E =======================================


sub_200003DC				; CODE XREF: sub_20000584+20
					; sub_20000584+34 ...
		MOV			R2, #0x30000000
		STRH		R0, [R2,#6]
		MOV			R2, #0x30000000
		STRH		R1, [R2,#4]
		BX			LR
; End of function sub_200003DC


; =============== S U B	R O U T	I N E =======================================


LowLevelInit				; CODE XREF: resetHandler+18

		STMFD		SP!, {R4,LR}

		LDR			R0, dword_20001A94
		MOV			R1, #0
		STR			R0, [R1,#AT91C_CKGR_MOR] ; (CKGR) Main Oscillator Register
		NOP

loc_20001814				; CODE XREF: LowLevelInit+20
		MOV			R0, #0
		LDR			R0, [R0,#AT91C_PMC_SR] ; Status	Register
		TST			R0, #1
		BEQ			loc_20001814
		LDR			R0, dword_20001A98
		MOV			R1, #0
		STR			R0, [R1,#AT91C_PMC_MCKR] ; (PMC) Master	Clock Register
		NOP

loc_20001834				; CODE XREF: LowLevelInit+40
		MOV			R0, #0
		LDR			R0, [R0,#AT91C_PMC_SR] ; Status	Register
		TST			R0, #8
		BEQ			loc_20001834

		LDR			R0, dword_20001A9C
		MOV			R1, #0
		STR			R0, [R1,#AT91C_PMC_PLLAR] ; (PMC) PLL A	Register
		NOP

loc_20001854				; CODE XREF: LowLevelInit+60
		MOV			R0, #0
		LDR			R0, [R0,#AT91C_PMC_SR] ; Status	Register
		TST			R0, #2
		BEQ			loc_20001854
		LDR			R0, dword_20001AA0
		MOV			R1, #0
		STR			R0, [R1,#AT91C_PMC_PLLBR] ; PMC) PLL B Register
		NOP

loc_20001874				; CODE XREF: LowLevelInit+80
		MOV			R0, #0
		LDR			R0, [R0,#AT91C_PMC_SR] ; Status	Register
		TST			R0, #4
		BEQ			loc_20001874
		NOP

loc_20001888				; CODE XREF: LowLevelInit+94
		MOV			R0, #0
		LDR			R0, [R0,#AT91C_PMC_SR] ; Status	Register
		TST			R0, #8
		BEQ			loc_20001888
		LDR			R0, dword_20001AA4
		MOV			R1, #0
		STR			R0, [R1,#AT91C_PMC_MCKR] ; (PMC) Master	Clock Register
		NOP

loc_200018A8				; CODE XREF: LowLevelInit+B4
		MOV			R0, #0
		LDR			R0, [R0,#AT91C_PMC_SR] ; Status	Register
		TST			R0, #8
		BEQ			loc_200018A8
		MOV			R0, #0xFFFFFFFF
		MOV			R1, #0
		STR			R0, [R1,#AT91C_AIC_IDCR] ; (AIC) Interrupt Disable Command Register
		LDR			R0, off_20001AA8
		MOV			R1, #0
		STR			R0, [R1,#AT91C_AIC_SVR]	; (AIC)	Source Vector Register
		MOV			R4, #1

loc_200018D4				; CODE XREF: LowLevelInit+E8
		CMP			R4, #0x1F
		BCS			loc_200018FC
		B				loc_200018EC
; ---------------------------------------------------------------------------

loc_200018E0				; CODE XREF: LowLevelInit+F8
		ADD			R0, R4,	#1
		AND			R4, R0,	#0xFF
		B				loc_200018D4
; ---------------------------------------------------------------------------

loc_200018EC				; CODE XREF: LowLevelInit+DC
		LDR			R0, off_20001AAC
		LDR			R1, off_20001AB0
		STR			R0, [R1,R4,LSL#2]
		B				loc_200018E0
; ---------------------------------------------------------------------------

loc_200018FC				; CODE XREF: LowLevelInit+D8
		LDR			R0, =USB_ISR_SPU
		MOV			R1, #0
		STR			R0, [R1,#AT91C_AIC_SPU]	; Spurious Interrupt Vector Register
		MOV			R4, #0

loc_2000190C				; CODE XREF: LowLevelInit+120
		CMP			R4, #8
		BCS			loc_20001934
		B				loc_20001924
; ---------------------------------------------------------------------------

loc_20001918				; CODE XREF: LowLevelInit+130
		ADD			R0, R4,	#1
		AND			R4, R0,	#0xFF
		B				loc_2000190C
; ---------------------------------------------------------------------------

loc_20001924				; CODE XREF: LowLevelInit+114
		MOV			R0, #0
		MOV			R1, #0
		STR			R0, [R1,#AT91C_AIC_EOICR] ; (AIC) End of Interrupt Command Register
		B				loc_20001918
; ---------------------------------------------------------------------------

loc_20001934				; CODE XREF: LowLevelInit+110
		MOV			R0, #0x8000
		MOV			R1, #0
		STR			R0, [R1,#AT91C_WDTC_WDMR] ; Watchdog Mode Register
		SUB			R0, R0,	#0xA000
		LDR			R0, [R0,#0xF00]
		CMP			R0, #0
		BNE			loc_20001954
		BL			sub_200031E8

loc_20001954				; CODE XREF: LowLevelInit+14C
		MOV			R0, #0
		LDR			R0, [R0,#AT91C_RTTC_RTMR] ; (RTTC) Real-time Mode Register
		BIC			R0, R0,	#0x30000
		MOV			R1, #0
		STR			R0, [R1,#AT91C_RTTC_RTMR] ; (RTTC) Real-time Mode Register
		MOV			R0, #0
		LDR			R0, [R0,#AT91C_PITC_PIMR] ; (PITC) Period Interval Mode	Register
		BIC			R0, R0,	#0x2000000
		MOV			R1, #0
		STR			R0, [R1,#AT91C_PITC_PIMR] ; (PITC) Period Interval Mode	Register
		MOV			R1, #1	; a2
		LDR			R0, off_20001AB8 ; pio
		BL			PIO_Configure
		MOV			R1, #1	; a2
		LDR			R0, off_20001ABC ; pio
		BL			PIO_Configure
		BL			SMC_Setup
		MOV			R1, #1	; a2
		LDR			R0, off_20001AC0 ; pio
		BL			PIO_Configure
		LDR			R2, dword_20001AC4
		LDR			R1, dword_20001AC8
		MOV			R0, #0x800
		BL			DBGU_Init
		BL			SDRAM_Init
		LDMFD		SP!, {R4,PC}
; End of function LowLevelInit


; =============== S U B	R O U T	I N E =======================================


sub_200019BC				; CODE XREF: resetHandler+80
					; DATA XREF: resetHandler+78 ...
		MOV			R2, #0x300000
		MOV			R0, #0

loc_200019C4				; CODE XREF: sub_200019BC+1C
		LDR			R3, dword_20001ACC
		CMP			R0, R3
		BCS			loc_200019F0
		B				loc_200019DC
; ---------------------------------------------------------------------------

loc_200019D4				; CODE XREF: sub_200019BC+30
		ADD			R0, R0,	#0x100000
		B				loc_200019C4
; ---------------------------------------------------------------------------

loc_200019DC				; CODE XREF: sub_200019BC+14
		MOV			R1, R0,LSR#20
		ORR			R3, R0,	#0xC00
		ORR			R3, R3,	#0x1A
		STR			R3, [R2,R1,LSL#2]
		B				loc_200019D4
; ---------------------------------------------------------------------------

loc_200019F0				; CODE XREF: sub_200019BC+10
		MOV			R0, #0x20000000

loc_200019F4				; CODE XREF: sub_200019BC+48
		CMP			R0, #0x22000000
		BCS			loc_20001A1C
		B				loc_20001A08
; ---------------------------------------------------------------------------

loc_20001A00				; CODE XREF: sub_200019BC+5C
		ADD			R0, R0,	#0x100000
		B				loc_200019F4
; ---------------------------------------------------------------------------

loc_20001A08				; CODE XREF: sub_200019BC+40
		MOV			R1, R0,LSR#20
		ORR			R3, R0,	#0xC00
		ORR			R3, R3,	#0x1A
		STR			R3, [R2,R1,LSL#2]
		B				loc_20001A00
; ---------------------------------------------------------------------------

loc_20001A1C				; CODE XREF: sub_200019BC+3C
		MOV			R0, #USB_ADDR

loc_20001A20				; CODE XREF: sub_200019BC+78
		LDR			R3, dword_20001AD0
		CMP			R0, R3
		BCS			loc_20001A4C
		B				loc_20001A38
; ---------------------------------------------------------------------------

loc_20001A30				; CODE XREF: sub_200019BC+8C
		ADD			R0, R0,	#0x100000
		B				loc_20001A20
; ---------------------------------------------------------------------------

loc_20001A38				; CODE XREF: sub_200019BC+70
		MOV			R1, R0,LSR#20
		ORR			R3, R0,	#0xC00
		ORR			R3, R3,	#0x12
		STR			R3, [R2,R1,LSL#2]
		B				loc_20001A30
; ---------------------------------------------------------------------------

loc_20001A4C				; CODE XREF: sub_200019BC+6C
		MOV			R0, #0x30000000

loc_20001A50				; CODE XREF: sub_200019BC+A8
		LDR			R3, dword_20001AD4
		CMP			R0, R3
		BCS			loc_20001A7C
		B				loc_20001A68
; ---------------------------------------------------------------------------

loc_20001A60				; CODE XREF: sub_200019BC+BC
		ADD			R0, R0,	#0x100000
		B				loc_20001A50
; ---------------------------------------------------------------------------

loc_20001A68				; CODE XREF: sub_200019BC+A0
		MOV			R1, R0,LSR#20
		ORR			R3, R0,	#0xC00
		ORR			R3, R3,	#0x12
		STR			R3, [R2,R1,LSL#2]
		B				loc_20001A60
; ---------------------------------------------------------------------------

loc_20001A7C				; CODE XREF: sub_200019BC+9C
		LDR			R3, dword_20001AD8
		MOV			R12, R3,LSR#18
		STR			R3, [R12,R2]
		ADD			R3, R3,	#0x600000
		STR			R3, [R2,#0x14]
		BX			LR
; End of function sub_200019BC


; =============== S U B R O U T I N E =======================================

; Attributes: noreturn

defaultFiqHandler                       ; DATA XREF: LowLevelInit+C4↓o
                                        ; ROM:off_20001AA8↓o
                NOP

loc_200017F4                            ; CODE XREF: defaultFiqHandler:loc_200017F4↓j
                B               loc_200017F4
; End of function defaultFiqHandler


; =============== S U B R O U T I N E =======================================

; Attributes: noreturn

defaultIrqHandler                       ; DATA XREF: LowLevelInit:loc_200018EC↓o
                                        ; ROM:off_20001AAC↓o
                NOP

loc_200017FC                            ; CODE XREF: defaultIrqHandler:loc_200017FC↓j
                B               loc_200017FC
; End of function defaultIrqHandler


; ---------------------------------------------------------------------------

dword_20001A94	DCD 0x4001		; DATA XREF: LowLevelInit+4
dword_20001A98	DCD 0x205		; DATA XREF: LowLevelInit+24
dword_20001A9C	DCD 0x202A3F01		; DATA XREF: LowLevelInit+44
dword_20001AA0	DCD 0x103A3F11		; DATA XREF: LowLevelInit+64
dword_20001AA4	DCD 0x206		; DATA XREF: LowLevelInit+98
off_20001AA8		DCD defaultFiqHandler	; DATA XREF: LowLevelInit+C4
off_20001AAC		DCD defaultIrqHandler	; DATA XREF: LowLevelInit:loc_200018EC
off_20001AB0		DCD AT91C_AIC_SVR	; DATA XREF: LowLevelInit+F0
					; (AIC)	Source Vector Register
; Pin *off_20001AB8
off_20001AB8		DCD _PC11	; DATA XREF: LowLevelInit+180
; Pin *off_20001ABC
off_20001ABC		DCD _PC13	; DATA XREF: LowLevelInit+18C
; Pin *off_20001AC0
off_20001AC0		DCD _PB14_15	; DATA XREF: LowLevelInit+19C
dword_20001AC4	DCD 0x5E7B800		; DATA XREF: LowLevelInit+1A4
dword_20001AC8	DCD 0x1C200		; DATA XREF: LowLevelInit+1A8
dword_20001ACC	DCD 0x3FFFFF		; DATA XREF: sub_200019BC:loc_200019C4
dword_20001AD0	DCD 0x100FFFFF		; DATA XREF: sub_200019BC:loc_20001A20
dword_20001AD4	DCD 0x300FFFFF		; DATA XREF: sub_200019BC:loc_20001A50
dword_20001AD8	DCD 0xFFF00C12		; DATA XREF: sub_200019BC:loc_20001A7C

; =============== S U B	R O U T	I N E =======================================


sub_200031E8				; CODE XREF: LowLevelInit+150
		MOV			R0, #3
		LDR			R1, =0xFFFFE000
		STR			R0, [R1,#0xF00]
		BX			LR
; End of function sub_200031E8


; ---------------------------------------------------------------------------

	
		SECTION SDRAM_DATAINIT_ALGO:CODE
		ARM
		
_PB14_15	      Pin 0x0000C000, AT91C_PIOB_PER, 3, 0, 0, 0
_PC11			      Pin 0x00000800, AT91C_PIOC_PER, 4, 0,	0, 0
_PC13			      Pin 0x00002000, AT91C_PIOC_PER, 4, 0, 1, 0

; ROM		ends


		END
