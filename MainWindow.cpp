#include <fstream>

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
	// Sweep.SetDrawPanel(image);
	sizer->Add(image, 1, wxEXPAND);
	SetSizer(sizer);

	// Events.
	Bind(wxEVT_MENU, &MainWindow::onLoadF4, this, static_cast<int>(EventID::ID_LoadF4));
}



// Loading the F4 chain code.
void MainWindow::onLoadF4(wxCommandEvent& event) {
	// Creating a file dialog to choose the file with the F4 chain code..
	wxFileDialog fd(this, _("Open TXT file"), "", "", "ZXZ files (*.txt)|*.txt", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	
	// If a user changed his mind, our job is over here.
	if (fd.ShowModal() == wxID_CANCEL) {
		return;
	}

	// Reading a file with the F4 chain code.
	std::string file = fd.GetPath().ToStdString();
	std::vector<std::byte> chainCode = readFile(file);

	// Converting the F4 chain code to coordinates and drawing it on the image panel.
	sweep.setF4(chainCode);
	image->clearAllFlags();
	image->setF4Flag();
	image->setSweepPointer(&sweep);
	image->Refresh();
}



// Reading a file.
std::vector<std::byte> MainWindow::readFile(std::string file) {
	std::vector<std::byte> chainCode;
	
	// Opening a file.
	std::ifstream in(file);
	
	// If a file is not open, we do not panic but abort the process.
	if (!in.is_open()) {
		return chainCode;
	}

	// Reading a file character by character.
	char c;
	while (in.read(&c, 1)) {
		// If a value is 0, 1, 2 or 3, the value is added to the chain code vector.
		if (c == '0' || c == '1' || c == '2' || c == '3') {
			chainCode.push_back(static_cast<std::byte>(c));
		}
	}

	return chainCode;
}