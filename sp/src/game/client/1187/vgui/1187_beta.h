

#ifndef ELEVENEIGHTYSEVEN_BETA_INTERFACE_H
#define ELEVENEIGHTYSEVEN_BETA_INTERFACE_H

#ifdef _WIN32
#pragma once
#endif

#include <vgui/IVGUI.h>

class I1187Beta
{
public:
	virtual void		Create(vgui::VPANEL parent) = 0;
	virtual void		Destroy(void) = 0;
};

extern I1187Beta* g_p1187beta;

#endif // ELEVENEIGHTYSEVEN_BETA_INTERFACE_H