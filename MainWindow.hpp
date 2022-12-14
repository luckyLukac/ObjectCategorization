#pragma once

#include <cstddef>
#include <vector>

#include "LineSweeping.hpp"
#include "stdafx.h"
#include "wxImagePanel.hpp"


class MainWindow : public wxFrame {

private:
	unsigned int xWindowSize = 1000;         // Window size by X coordinate.
	unsigned int yWindowSize = 1000;         // Window size by Y coordinate.
	GUI::wxImagePanel* image = nullptr;      // Image panel.
	LineSweeping sweep;						 // Line sweeping object.

public:
	MainWindow();                            // Constructor of the window.

	void loadF4(wxCommandEvent& event);      // Loading the F4 chain code.
	void multiSweep(wxCommandEvent& event);  // Multi sweep algorithm event for object categorization.
};


// Enum class for events.
enum class EventID {
	ID_LoadF4,
	ID_Multisweep
};