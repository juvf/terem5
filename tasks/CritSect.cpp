/*
 * CritSect.cpp
 *
 *  Created on: 09 окт. 2015 г.
 *      Author: anisimov.e
 */

#include "CritSect.h"
static __istate_t m_statusReg;
void enterCritSect()
{
	m_statusReg = __get_interrupt_state();
	 __disable_interrupt();
}

void exitCritSect()
{
	 __set_interrupt_state(m_statusReg);
}

