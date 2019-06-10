/*===================================================================*/
/*                                                                   */
/*  InfoNES_Types.h : Type definitions for InfoNES                   */
/*                                                                   */
/*  2000/5/4    InfoNES Project ( based on pNesX )                   */
/*                                                                   */
/*===================================================================*/

#ifndef InfoNES_TYPES_H_INCLUDED
#define InfoNES_TYPES_H_INCLUDED

/*-------------------------------------------------------------------*/
/*  Type definition                                                  */
/*-------------------------------------------------------------------*/
#ifndef NDWORD
typedef unsigned long  NDWORD;
#endif /* !NDWORD */

#ifndef NWORD
typedef unsigned short NWORD;
#endif /* !NWORD */

#ifndef NBYTE
typedef unsigned char  NBYTE;
#endif /* !NBYTE */

/*-------------------------------------------------------------------*/
/*  NULL definition                                                  */
/*-------------------------------------------------------------------*/
#ifndef NULL
#define NULL  0
#endif /* !NULL */

#endif /* !InfoNES_TYPES_H_INCLUDED */
