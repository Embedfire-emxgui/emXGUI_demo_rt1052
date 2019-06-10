/*===================================================================*/
/*                                                                   */
/*  K6502.h : Header file for K6502                                  */
/*                                                                   */
/*  2000/05/29  InfoNES Project ( based on pNesX )                   */
/*                                                                   */
/*===================================================================*/

#ifndef K6502_H_INCLUDED
#define K6502_H_INCLUDED

#include "InfoNES_Types.h"

#ifndef NULL
#define NULL 0
#endif

/* 6502 Flags */
#define FLAG_C 0x01
#define FLAG_Z 0x02
#define FLAG_I 0x04
#define FLAG_D 0x08
#define FLAG_B 0x10
#define FLAG_R 0x20
#define FLAG_V 0x40
#define FLAG_N 0x80


/* Interrupt Vectors */
#define VECTOR_NMI   0xfffa
#define VECTOR_RESET 0xfffc
#define VECTOR_IRQ   0xfffe
#define NMI_REQ  CPU->NMI_State = 0;// NMI Request
#define IRQ_REQ  CPU->IRQ_State = 0;// IRQ Request
#define BASE_STACK   0x100/* Stack Address */


typedef struct // Value and Flag Data
{
  NBYTE byValue;
  NBYTE byFlag;
}value_table;

typedef struct // Value and Flag Data
{
// 6502 Register
    NWORD PC;
    NBYTE SP;
    NBYTE F;
    NBYTE A;
    NBYTE X;
    NBYTE Y;
    NBYTE IRQ_State;// The state of the IRQ pin
    NBYTE IRQ_Wiring;// Wiring of the IRQ pin
    NBYTE NMI_State;// The state of the NMI pin
    NBYTE NMI_Wiring;// Wiring of the NMI pin
    NWORD g_wPassedClocks;// The number of the clocks that it passed
    NBYTE g_byTestTable[ 256 ];// A table for the test
    value_table g_ASLTable[ 256 ];// A table for ASL
    value_table g_LSRTable[ 256 ];// A table for LSR
    value_table g_ROLTable[ 2 ][ 256 ];// A table for ROL
    value_table g_RORTable[ 2 ][ 256 ];// A table for ROR
}CpuResource;






// Emulator Operation
void K6502_Init(void);
void K6502_Reset(void);
void K6502_Set_Int_Wiring( NBYTE byNMI_Wiring, NBYTE byIRQ_Wiring );
void K6502_Step( register NWORD wClocks );

// I/O Operation (User definition)
static  NBYTE K6502_Read( NWORD wAddr);
static  NWORD K6502_ReadW( NWORD wAddr );
static  NWORD K6502_ReadW2( NWORD wAddr );
static  NBYTE K6502_ReadZp( NBYTE byAddr );
static  NWORD K6502_ReadZpW( NBYTE byAddr );
static  NBYTE K6502_ReadAbsX(void);
static  NBYTE K6502_ReadAbsY(void);
static  NBYTE K6502_ReadIY(void);

static  void K6502_Write( NWORD wAddr, NBYTE byData );
static  void K6502_WriteW( NWORD wAddr, NWORD wData );

// The state of the IRQ pin
//extern NBYTE IRQ_State;

// The state of the NMI pin
//extern NBYTE NMI_State;

// The number of the clocks that it passed
extern NWORD g_wPassedClocks;
extern CpuResource *CPU;


#endif /* !K6502_H_INCLUDED */
