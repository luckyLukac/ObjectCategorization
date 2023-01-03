#pragma once

#include "LineSweeping.hpp"
#include "stdafx.h"


namespace GUI {
	class wxImagePanel : public wxPanel {
		DECLARE_EVENT_TABLE();
	private:
		int width;				                // Width of the panel.
		int height;				                // Height of the panel.
		bool F4PlotFlag = false;                // Flag for plotting the F4 chain code.
		bool segmentFlag = false;               // Flag for drawing of a segment.
		LineSweeping* sweep = nullptr;			// Line sweeping object.

		void render(wxDC& dc);                  // Render function.

	public:
		wxImagePanel(wxFrame* parent);          // Main constructor of the image panel.

		void paintEvent(wxPaintEvent& evt);     // Event for painting of the panel.
		void onSize(wxSizeEvent& event);        // Event for refreshing the panel on size.

		void setF4Flag();					    // Setting the F4 flag.
		void setSegmentFlag();				    // Setting the segment flag.
		void clearAllFlags();				    // Clearing all flags.

		void setSweepPointer(LineSweeping* s);  // Setting the sweeping pointer.
	};
}