#include "djsw_gui_app.h"

#include "djsw_gui_app_wave1.h"

void OnGuiInit_App()
{
	djView* view;

	GetView(&view, 0);
	OnGuiInit_Wave1(view);
}