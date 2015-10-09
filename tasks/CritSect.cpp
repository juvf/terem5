/*
 * CritSect.cpp
 *
 *  Created on: 09 окт. 2015 г.
 *      Author: anisimov.e
 */

#include "CritSect.h"

CritSect::CritSect()
	: m_statusReg(__get_interrupt_state())
{
	 __disable_interrupt();
}

CritSect::~CritSect()
{
	 __set_interrupt_state(m_statusReg);
}

