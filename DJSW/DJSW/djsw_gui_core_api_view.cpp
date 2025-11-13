#include "djsw_gui_core_api.h"

#include <assert.h>

// -------------------- Global Method Implementations --------------------



// -------------------- Member Implementations --------------------
void djView::OnDrawBackground()
{
	djRectLTRB djViewport = { 0 };

	djViewport.right = this->viewport.width;
	djViewport.bottom = this->viewport.height;

	DrawRectangle(djViewport, this->backgroundColor);
}

void djView::OnDrawBounds()
{
	djRectLTRB djBounds = { 0 };

	if (this->bounds.left > 0)
	{
		djBounds.left = 0;
		djBounds.top = 0;
		djBounds.right = this->bounds.left;
		djBounds.bottom = this->viewport.height;
		DrawRectangle(djBounds, this->boundColor);
	}
	if (this->bounds.top > 0)
	{
		djBounds.left = 0;
		djBounds.top = 0;
		djBounds.right = this->viewport.width;
		djBounds.bottom = this->bounds.top;
		DrawRectangle(djBounds, this->boundColor);
	}
	if (this->bounds.right > 0)
	{
		djBounds.left = this->viewport.width - this->bounds.right;
		djBounds.top = 0;
		djBounds.right = this->viewport.width;
		djBounds.bottom = this->viewport.height;
		DrawRectangle(djBounds, this->boundColor);
	}
	if (this->bounds.bottom > 0)
	{
		djBounds.left = 0;
		djBounds.top = this->viewport.height - this->bounds.bottom;
		djBounds.right = this->viewport.width;
		djBounds.bottom = this->viewport.height;
		DrawRectangle(djBounds, this->boundColor);
	}
}
