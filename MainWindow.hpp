#pragma once

#include <cstddef>
#include <vector>

#include "LineSweeping.hpp"
#include "stdafx.h"
#include "wxImagePanel.hpp"


class MainWindow : public wxFrame {

private:
	unsigned int xWindowSize = 1000;                    // Window size by X coordinate.
	unsigned int yWindowSize = 600;                     // Window size by Y coordinate.
	GUI::wxImagePanel* image = nullptr;                 // Image panel.
	LineSweeping sweep;									// Line sweeping object.

public:
	MainWindow();                                       // Constructor of the window.

	void onLoadF4(wxCommandEvent& event);               // Loading the F4 chain code.

	std::vector<std::byte> readFile(std::string file);  // Reading a file.
};


// Enum class for events.
enum class EventID {
	ID_LoadF4,
	ID_Multisweep
};