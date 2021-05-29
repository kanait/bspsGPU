////////////////////////////////////////////////////////////////////
//
// $Id: mydef.h,v 1.3 2004/01/15 18:11:09 kanai Exp $
//
// Copyright (c) 2002 by Keio Research Institute at SFC
// All rights reserved. 
//
////////////////////////////////////////////////////////////////////

#ifndef _MYDEF_H
#define _MYDEF_H

#define mycomment(c) (c == '\n' || c == '!' || c == '%' || c == '#' || c == '*')

#define foreach(a,b,c) for(a::iterator c=(b).begin();(c)!=(b).end();(c)++)
#define foreachP(a,b,c) for(a::iterator c=(b).begin();(c)!=(b).end();(c)++)

//
// Ç±Ç±Ç©ÇÁêÊÇÕÇ¢ÇÁÇ»Ç¢Ç©Ç‡ÇµÇÍÇ»Ç¢
//

#define NULLID		-1
typedef	short Id;

#define ZEROEPS		1.0e-06
#define TRIANGLE	3
#define RECTANGLE	4

#define nXYZ            3

// color
#define PNTBLUE		2
#define PNTGREEN	1
#define PNTRED		0

#define EDGEWHITE	2
#define EDGEBLUE	1
#define EDGERED		0

#define FACEBLUE	2
#define FACEGREEN	1
#define FACERED		0

#define LOOPBLUE        1
#define LOOPRED         0

#define SQRT2           1.41421356
#define SQRT3           1.7320508
#define SQRT6           2.44948974

#endif // _MYDEF_H
