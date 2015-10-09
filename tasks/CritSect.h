/*
 * CritSect.h
 *
 *  Created on: 09 окт. 2015 г.
 *      Author: anisimov.e
 */

#ifndef TASKS_CRITSECT_H_
#define TASKS_CRITSECT_H_

#include "intrinsics.h"

class CritSect
{
public:
	CritSect();
	virtual ~CritSect();
private:
	__istate_t m_statusReg;
};

#endif /* TASKS_CRITSECT_H_ */
