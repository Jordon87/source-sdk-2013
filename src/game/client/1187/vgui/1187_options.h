#include <vgui/VGUI.h>

namespace vgui
{
	class Panel;
}

class I1187Options
{
public:
	virtual void		Create(vgui::VPANEL parent) = 0;
	virtual void		Destroy(void) = 0;
};

extern I1187Options* SOptions;