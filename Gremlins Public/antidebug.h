#pragma once

namespace antidbg
{
	void init();
	int debugstring();
	int hidethread();
	int remotepresent();
	int contextthread();
	bool attached_dbg();
	//bool peb_dbg();
	bool handle_dbg();
	bool window_dbg();
	void SetupMain();
	void roild();
}