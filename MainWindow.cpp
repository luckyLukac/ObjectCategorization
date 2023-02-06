#include <chrono>
#include <fstream>
#include <iterator>
#include <regex>
#include <vector>

#include "HelperFunctions.hpp"
#include "MainWindow.hpp"


MainWindow::MainWindow() : 
	wxFrame(nullptr, wxID_ANY, "Object categorization") 
{
	SetSize(xWindowSize, yWindowSize);  // Setting the window size.
	SetBackgroundColour(wxColor(240, 240, 240));
	wxSizerFlags::DisableConsistencyChecks();

	// Creating an image panel.
	image = new GUI::wxImagePanel(this);
	sweep.setDrawPanel(image);

	/* GENERATED CODE 
	** DO NOT TOUCH!!!
	*/
	this->SetSizeHints(wxDefaultSize, wxDefaultSize);
	wxBoxSizer* lytWindow;
	lytWindow = new wxBoxSizer(wxHORIZONTAL);
	//image = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	lytWindow->Add(image, 1, wxEXPAND | wxALL, 5);
	wxBoxSizer* lytControls;
	lytControls = new wxBoxSizer(wxVERTICAL);
	wxStaticBoxSizer* gbxChainCodeLoading;
	gbxChainCodeLoading = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, wxT("Chain code loading")), wxVERTICAL);
	wxBoxSizer* lytChainCodeLoading;
	lytChainCodeLoading = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* lytChainCodeLoadingFile;
	lytChainCodeLoadingFile = new wxBoxSizer(wxHORIZONTAL);
	lblChainCodeLoading = new wxStaticText(gbxChainCodeLoading->GetStaticBox(), wxID_ANY, wxT("File:"), wxDefaultPosition, wxDefaultSize, 0);
	lblChainCodeLoading->Wrap(-1);
	lytChainCodeLoadingFile->Add(lblChainCodeLoading, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	tbxChainCodeLoading = new wxTextCtrl(gbxChainCodeLoading->GetStaticBox(), wxID_ANY, wxT("./F8 Datasets/fcced_shuttle.txt"), wxDefaultPosition, wxDefaultSize, 0);
	tbxChainCodeLoading->Enable(false);
	tbxChainCodeLoading->SetMinSize(wxSize(200, -1));
	lytChainCodeLoadingFile->Add(tbxChainCodeLoading, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

	btnSearchChainCode = new wxButton(gbxChainCodeLoading->GetStaticBox(), 0, wxT("Search"), wxDefaultPosition, wxDefaultSize, 0);
	
	btnSearchChainCode->SetMaxSize(wxSize(50, -1));
	lytChainCodeLoadingFile->Add(btnSearchChainCode, 1, wxALL, 5);
	lytChainCodeLoading->Add(lytChainCodeLoadingFile, 1, wxEXPAND, 5);
	wxBoxSizer* lytChainCodeLoadingRotation;
	lytChainCodeLoadingRotation = new wxBoxSizer(wxHORIZONTAL);
	lblChainCodeLoadingRotation = new wxStaticText(gbxChainCodeLoading->GetStaticBox(), wxID_ANY, wxT("Rotation:"), wxDefaultPosition, wxDefaultSize, 0);
	lblChainCodeLoadingRotation->Wrap(-1);
	lytChainCodeLoadingRotation->Add(lblChainCodeLoadingRotation, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	sbxChainCodeLoadingRotation = new wxSpinCtrl(gbxChainCodeLoading->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 7, 0);
	lytChainCodeLoadingRotation->Add(sbxChainCodeLoadingRotation, 1, wxALIGN_CENTER | wxALL | wxEXPAND, 5);
	lytChainCodeLoading->Add(lytChainCodeLoadingRotation, 1, wxEXPAND, 5);

	btnChainCodeLoad = new wxButton(gbxChainCodeLoading->GetStaticBox(), 1, wxT("Load chain code"), wxDefaultPosition, wxDefaultSize, 0);
	
	lytChainCodeLoading->Add(btnChainCodeLoad, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
	gbxChainCodeLoading->Add(lytChainCodeLoading, 1, wxEXPAND, 5);
	lytControls->Add(gbxChainCodeLoading, 1, wxEXPAND, 5);
	wxStaticBoxSizer* gbxMultisweep;
	gbxMultisweep = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, wxT("Multisweep algorithm")), wxVERTICAL);
	wxBoxSizer* lytMultisweep;
	lytMultisweep = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* lytMultisweepOutput;
	lytMultisweepOutput = new wxBoxSizer(wxHORIZONTAL);
	lblMultisweepOutput = new wxStaticText(gbxMultisweep->GetStaticBox(), wxID_ANY, wxT("Output file:"), wxDefaultPosition, wxDefaultSize, 0);
	lblMultisweepOutput->Wrap(-1);
	lytMultisweepOutput->Add(lblMultisweepOutput, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	tbxMultisweepOutput = new wxTextCtrl(gbxMultisweep->GetStaticBox(), wxID_ANY, wxT("test1"), wxDefaultPosition, wxDefaultSize, 0);
	tbxMultisweepOutput->SetMinSize(wxSize(200, -1));
	lytMultisweepOutput->Add(tbxMultisweepOutput, 1, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	lytMultisweep->Add(lytMultisweepOutput, 1, wxEXPAND, 5);

	btnMultisweep = new wxButton(gbxMultisweep->GetStaticBox(), 2, wxT("Run"), wxDefaultPosition, wxDefaultSize, 0);
	
	lytMultisweep->Add(btnMultisweep, 0, wxALIGN_CENTER | wxALL, 5);
	gbxMultisweep->Add(lytMultisweep, 1, wxEXPAND, 5);
	lytControls->Add(gbxMultisweep, 0, wxEXPAND, 5);
	wxStaticBoxSizer* gbxObjectComparison;
	gbxObjectComparison = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, wxT("Object comparison")), wxVERTICAL);
	wxBoxSizer* lytObjectComparisonFile1;
	lytObjectComparisonFile1 = new wxBoxSizer(wxHORIZONTAL);
	lblObjectComparisonFile1 = new wxStaticText(gbxObjectComparison->GetStaticBox(), wxID_ANY, wxT("File 1:"), wxDefaultPosition, wxDefaultSize, 0);
	lblObjectComparisonFile1->Wrap(-1);
	lytObjectComparisonFile1->Add(lblObjectComparisonFile1, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	tbxObjectComparisonFile1 = new wxTextCtrl(gbxObjectComparison->GetStaticBox(), wxID_ANY, wxT("./Results/test1.txt"), wxDefaultPosition, wxDefaultSize, 0);
	//tbxObjectComparisonFile1->Enable(false);
	tbxObjectComparisonFile1->SetMinSize(wxSize(200, -1));
	lytObjectComparisonFile1->Add(tbxObjectComparisonFile1, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

	btnObjectComparisonFile1 = new wxButton(gbxObjectComparison->GetStaticBox(), 3, wxT("Search"), wxDefaultPosition, wxDefaultSize, 0);
	
	btnObjectComparisonFile1->SetMaxSize(wxSize(50, -1));
	lytObjectComparisonFile1->Add(btnObjectComparisonFile1, 0, wxALL, 5);
	gbxObjectComparison->Add(lytObjectComparisonFile1, 1, wxEXPAND, 5);
	wxBoxSizer* lytObjectComparisonFile2;
	lytObjectComparisonFile2 = new wxBoxSizer(wxHORIZONTAL);
	lblObjectComparisonFile2 = new wxStaticText(gbxObjectComparison->GetStaticBox(), wxID_ANY, wxT("File 2:"), wxDefaultPosition, wxDefaultSize, 0);
	lblObjectComparisonFile2->Wrap(-1);
	lytObjectComparisonFile2->Add(lblObjectComparisonFile2, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	tbxObjectComparisonFile2 = new wxTextCtrl(gbxObjectComparison->GetStaticBox(), wxID_ANY, wxT("./Results/test2.txt"), wxDefaultPosition, wxDefaultSize, 0);
	//tbxObjectComparisonFile2->Enable(false);
	tbxObjectComparisonFile2->SetMinSize(wxSize(200, -1));
	lytObjectComparisonFile2->Add(tbxObjectComparisonFile2, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

	btnObjectComparisonFile2 = new wxButton(gbxObjectComparison->GetStaticBox(), 4, wxT("Search"), wxDefaultPosition, wxDefaultSize, 0);
	
	btnObjectComparisonFile2->SetMaxSize(wxSize(50, -1));
	lytObjectComparisonFile2->Add(btnObjectComparisonFile2, 0, wxALL, 5);
	gbxObjectComparison->Add(lytObjectComparisonFile2, 1, wxEXPAND, 5);

	btnObjectComparison = new wxButton(gbxObjectComparison->GetStaticBox(), 5, wxT("Compare"), wxDefaultPosition, wxDefaultSize, 0);
	
	gbxObjectComparison->Add(btnObjectComparison, 0, wxALIGN_CENTER | wxALL, 5);
	lytControls->Add(gbxObjectComparison, 0, wxEXPAND, 5);
	lytWindow->Add(lytControls, 0, wxALL, 5);
	this->SetSizer(lytWindow);
	this->Layout();
	this->Centre(wxBOTH);
	

	// Events.
	btnSearchChainCode->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &MainWindow::selectChainCode, this, static_cast<int>(EventID::selectFile));
	btnChainCodeLoad->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &MainWindow::loadF8, this, static_cast<int>(EventID::loadF8));
	btnMultisweep->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &MainWindow::multiSweep, this, static_cast<int>(EventID::multisweep));
	btnObjectComparisonFile1->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &MainWindow::selectFirstObjectResult, this, static_cast<int>(EventID::selectFirstObjectResult));
	btnObjectComparisonFile2->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &MainWindow::selectSecondObjectResult, this, static_cast<int>(EventID::selectSecondObjectResult));
	btnObjectComparison->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &MainWindow::compareResults, this, static_cast<int>(EventID::compareResults));
}


void MainWindow::selectChainCode(wxCommandEvent& event) {
	// Creating a file dialog to choose the file with the F4 chain code..
	wxFileDialog fd(this, "Open TXT file", "C:\\Users\\Uporabnik\\source\\repos\\ObjectCategorization\\F8 Datasets\\", "", "TXT files (*.txt)|*.txt", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

	// If a user changed his mind, our job is over here.
	if (fd.ShowModal() == wxID_CANCEL) {
		return;
	}

	std::string file = fd.GetPath().ToStdString();
	const std::string replacement = "C:\\Users\\Uporabnik\\source\\repos\\ObjectCategorization";
	file.replace(file.find(replacement), replacement.size(), ".");

	tbxChainCodeLoading->SetValue(file);
}

void MainWindow::loadF8(wxCommandEvent& event) {
	// Reading a file with the F8 chain code.
	std::string file = tbxChainCodeLoading->GetValue().ToStdString();
	const uint rotation = sbxChainCodeLoadingRotation->GetValue();
	const bool success = sweep.readFileF8(file, rotation);

	// Converting the F8 chain code to coordinates and drawing it on the image panel.
	image->clearAllFlags();
	image->setF4Flag();
	image->setSweepPointer(&sweep);
	image->Refresh();
}

void MainWindow::multiSweep(wxCommandEvent& event) {
	// If a chain code is not set, we cannot sweep the object.
	// This is the end, my beautiful friend.
	if (!sweep.isChainCodeSet()) {
		wxMessageBox("Object has not been loaded.", "Warning", wxOK | wxICON_WARNING);
		return;
	}

	auto start = std::chrono::steady_clock::now();

	sweep.clearSegments();				      // Clearing potential previously calculated segments.
	sweep.fillShape();						  // Filling the object.

	// Sweeping the object.
	for (int i = 15; i <= 180; i += 45) {
		sweep.setAngleOfRotation(toRadians(i));   // Setting the angle of rotation for the sweep line.
		sweep.sweep();						      // Sweeping the object with the sweep line.
		sweep.extractChains();				      // Extraction of chains from the object.
	}

	auto end = std::chrono::steady_clock::now();

	const std::string filename = tbxMultisweepOutput->GetValue().ToStdString();
	FeatureVector featureVector = sweep.calculateFeatureVector();  // Calculation of a feature vector for the current object.
	featureVector.writeToFile(filename + ".txt", "./Results/");

	image->setSegmentFlag();
	image->Refresh(false);

	u128 time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	std::stringstream ss;
	ss << "Time: " << time << " ms";
	wxMessageBox(ss.str(), "", wxOK);
}

void MainWindow::selectFirstObjectResult(wxCommandEvent& event) {
	// Creating a file dialog to choose the file with the F4 chain code..
	wxFileDialog fd(this, "Open TXT file", "C:\\Users\\Uporabnik\\source\\repos\\ObjectCategorization\\Results\\", "", "TXT files (*.txt)|*.txt", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

	// If a user changed his mind, our job is over here.
	if (fd.ShowModal() == wxID_CANCEL) {
		return;
	}

	std::string file = fd.GetPath().ToStdString();
	const std::string replacement = "C:\\Users\\Uporabnik\\source\\repos\\ObjectCategorization";
	file.replace(file.find(replacement), replacement.size(), ".");

	tbxObjectComparisonFile1->SetValue(file);
}

void MainWindow::selectSecondObjectResult(wxCommandEvent& event) {
	// Creating a file dialog to choose the file with the F4 chain code..
	wxFileDialog fd(this, "Open TXT file", "C:\\Users\\Uporabnik\\source\\repos\\ObjectCategorization\\Results", "", "TXT files (*.txt)|*.txt", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

	// If a user changed his mind, our job is over here.
	if (fd.ShowModal() == wxID_CANCEL) {
		return;
	}

	std::string file = fd.GetPath().ToStdString();
	const std::string replacement = "C:\\Users\\Uporabnik\\source\\repos\\ObjectCategorization";
	file.replace(file.find(replacement), replacement.size(), ".");

	tbxObjectComparisonFile2->SetValue(file);
}

void MainWindow::compareResults(wxCommandEvent& event) {
	// Getting the names of the both files.
	const std::string file1 = tbxObjectComparisonFile1->GetValue().ToStdString();
	const std::string file2 = tbxObjectComparisonFile2->GetValue().ToStdString();

	std::ifstream if1(file1);
	std::ifstream if2(file2);

	// If one of the file streams is not open, we say farewell.
	if (!if1.is_open() || !if2.is_open()) {
		wxMessageBox("ERROR", "", wxOK);
		return;
	}

	// Buffers for both file streams.
	std::string str1;
	std::string str2;

	// Getting the chain count for each file.
	std::getline(if1, str1);
	std::getline(if2, str2);
	const uint chainCount1 = std::stoi(str1);
	const uint chainCount2 = std::stoi(str2);

	// If the chain count is different, the results are not the same. Rocket science.
	//if (chainCount1 != chainCount2) {
	//	wxMessageBox("NO", "", wxOK);
	//	return;
	//}

	// Reading the chain values.
	std::vector<double> chain1((std::istream_iterator<double>(if1)), std::istream_iterator<double>());
	std::vector<double> chain2((std::istream_iterator<double>(if2)), std::istream_iterator<double>());

	for (uint i = 0; i < static_cast<uint>(0.7 * chain1.size()); i++) {
		if (difference(chain1[i], chain2[i]) > 0.06) {
			wxMessageBox("NO", "", wxOK);
			return;
		}
	}

	wxMessageBox("YES", "", wxOK);
}