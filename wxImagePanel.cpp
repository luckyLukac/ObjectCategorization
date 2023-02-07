#include "wxImagePanel.hpp"

namespace GUI {
	BEGIN_EVENT_TABLE(wxImagePanel, wxPanel)
	EVT_PAINT(wxImagePanel::paintEvent)
	EVT_SIZE(wxImagePanel::onSize)
	END_EVENT_TABLE()

	// Render function.
	void wxImagePanel::render(wxDC& dc) {
		// Resetting the panel and appending the correct parameters to it.
		dc.Clear();
		SetBackgroundStyle(wxBG_STYLE_PAINT);
		dc.GetSize(&width, &height);

		// If a F4 plot flag is set, a shape is plotted to the panel.
		if (F4PlotFlag) {
			sweep->plotInput(dc);        // Plotting the chain code shape.
			sweep->plotBoundingBox(dc);  // Plotting the bounding box of the object.
		}

		// If a segment flag is set, 
		if (segmentFlag) {
			sweep->plotInput(dc);        // Plotting the chain code shape.
			sweep->plotBoundingBox(dc);  // Plotting the bounding box of the object.
			sweep->plotChains(dc);       // Plotting the segments of the sweep algorithm.
		}
	}



	// Main costructor of the image panel.
	wxImagePanel::wxImagePanel(wxFrame* parent) :
		wxPanel(parent),
		width(0),
		height(0),
		sweep(nullptr)
	{
		SetBackgroundColour(*wxWHITE);  // Setting background color to white.
	}



	// Event for painting of the panel.
	void wxImagePanel::paintEvent(wxPaintEvent& evt) {
		wxPaintDC dc(this);
		render(dc);
	}

	// Event for refreshing the panel on size.
	void wxImagePanel::onSize(wxSizeEvent& event) {
		Refresh();
		event.Skip();
	}



	// Setting the F4 flag.
	void wxImagePanel::setF4Flag() {
		F4PlotFlag = true;
	}

	// Setting the segment flag.
	void wxImagePanel::setSegmentFlag() {
		segmentFlag = true;
	}

	// Clearing all flags.
	void wxImagePanel::clearAllFlags() {
		F4PlotFlag = false;
		segmentFlag = false;
	}



	// Setting the sweeping pointer.
	void wxImagePanel::setSweepPointer(LineSweeping* s) {
		sweep = s;
	}
}