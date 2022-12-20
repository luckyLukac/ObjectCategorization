#include <fstream>

#include "HelperFunctions.hpp"
#include "MainWindow.hpp"


MainWindow::MainWindow() : 
	wxFrame(nullptr, wxID_ANY, "Object categorization") 
{
	SetSize(xWindowSize, yWindowSize);  // Setting the window size.

	// Creating menu items.
	wxMenu* menuFile = new wxMenu();
	menuFile->Append(static_cast<int>(EventID::ID_LoadF4), "Load F4 chain code", "Load F4 chain code");
	wxMenu* menuAction = new wxMenu();
	menuAction->Append(static_cast<int>(EventID::ID_Multisweep), "Multisweep", "MultiSweepLine", "Object categorization with multi-sweepline approach");

	// Creating a main menu.
	wxMenuBar* menu = new wxMenuBar();
	menu->Append(menuFile, "File");
	menu->Append(menuAction, "Actions");
	SetMenuBar(menu);

	// Creating an image panel.
	image = new GUI::wxImagePanel(this);
	wxSizer* sizer = new wxBoxSizer(wxVERTICAL);
	sweep.setDrawPanel(image);
	sizer->Add(image, 1, wxEXPAND);
	SetSizer(sizer);

	// Events.
	Bind(wxEVT_MENU, &MainWindow::loadF4, this, static_cast<int>(EventID::ID_LoadF4));          // Loading the chain code.
	Bind(wxEVT_MENU, &MainWindow::multiSweep, this, static_cast<int>(EventID::ID_Multisweep));  // Starting multisweep algorithm.
}



// Loading the F4 chain code.
void MainWindow::loadF4(wxCommandEvent& event) {
	// Creating a file dialog to choose the file with the F4 chain code..
	wxFileDialog fd(this, _("Open TXT file"), "", "", "ZXZ files (*.txt)|*.txt", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	
	// If a user changed his mind, our job is over here.
	if (fd.ShowModal() == wxID_CANCEL) {
		return;
	}

	// Reading a file with the F4 chain code.
	std::string file = fd.GetPath().ToStdString();
	bool success = sweep.readFile(file);

	// Converting the F4 chain code to coordinates and drawing it on the image panel.
	//sweep.setF4(chainCode);
	image->clearAllFlags();
	image->setF4Flag();
	image->setSweepPointer(&sweep);
	image->Refresh();
}

#include <chrono>

// Multi sweep algorithm event for object categorization.
void MainWindow::multiSweep(wxCommandEvent& event) {
	// If a chain code is not set, we cannot sweep the object.
	// This is the end, my beautiful friend.
	if (!sweep.isChainCodeSet()) {
		wxMessageBox("Object has not been loaded.", "Warning", wxOK | wxICON_WARNING);
		return;
	}
	

	// Sweeping the object.
	auto start = std::chrono::steady_clock::now();
	
	sweep.clearSegments();				      // Clearing potential previously calculated segments.
	sweep.fillShape();						  // Filling the object.


	for (int i = 0; i <= 90; i += 15) {
		sweep.setAngleOfRotation(toRadians(i));   // Setting the angle of rotation for the sweep line.
		sweep.sweep();						      // Sweeping the object with the sweep line.
		wxMessageBox("Das Ende", "Info", wxOK | wxICON_INFORMATION);
		image->Refresh();
	}
	

	auto end = std::chrono::steady_clock::now();
	auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	wxMessageBox(std::to_string(time) + " ms", "Info", wxOK | wxICON_INFORMATION);

}