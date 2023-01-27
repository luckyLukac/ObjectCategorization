#pragma once

#include "LineSweeping.hpp"
#include "stdafx.h"
#include "wxImagePanel.hpp"


/// <summary>
/// Main window of the application.
/// </summary>
class MainWindow : public wxFrame {
private:
	uint xWindowSize = 1000;				 // Window size by X coordinate.
	uint yWindowSize = 1000;				 // Window size by Y coordinate.
	GUI::wxImagePanel* image = nullptr;      // Image panel.
	LineSweeping sweep;						 // Line sweeping object.

	wxStaticText* lblChainCodeLoading;
	wxTextCtrl* tbxChainCodeLoading;
	wxButton* btnSearchChainCode;
	wxStaticText* lblChainCodeLoadingRotation;
	wxSpinCtrl* sbxChainCodeLoadingRotation;
	wxButton* btnChainCodeLoad;
	wxStaticText* lblMultisweepOutput;
	wxTextCtrl* tbxMultisweepOutput;
	wxButton* btnMultisweep;
	wxStaticText* lblObjectComparisonFile1;
	wxTextCtrl* tbxObjectComparisonFile1;
	wxButton* btnObjectComparisonFile1;
	wxStaticText* lblObjectComparisonFile2;
	wxTextCtrl* tbxObjectComparisonFile2;
	wxButton* btnObjectComparisonFile2;
	wxButton* btnObjectComparison;


public:
	/// <summary>
	/// Constructor of the main window of the WX application.
	/// </summary>
	MainWindow();


	/// <summary>
	/// Selection of a chain code to be loaded.
	/// </summary>
	/// <param name="event">: captured event</param>
	void selectChainCode(wxCommandEvent& event);

	/// <summary>
	/// Event for loading the F8 chain code.
	/// </summary>
	/// <param name="event">: captured event</param>
	void loadF8(wxCommandEvent& event);

	/// <summary>
	/// Event for starting the multi-sweep algorithm for object categorization.
	/// </summary>
	/// <param name="event">: captured event</param>
	void multiSweep(wxCommandEvent& event);

	/// <summary>
	/// Selection of a first object result for a comparison.
	/// </summary>
	/// <param name="event">: captured event</param>
	void selectFirstObjectResult(wxCommandEvent& event);

	/// <summary>
	/// Selection of a second object result for a comparison.
	/// </summary>
	/// <param name="event">: captured event</param>
	void selectSecondObjectResult(wxCommandEvent& event);

	/// <summary>
	/// Comparing the two chain code algorithm results.
	/// </summary>
	/// <param name="event">: captured event</param>
	/// <returns>true if the two objects are the same, false otherwise</returns>
	void compareResults(wxCommandEvent& event);
};


/// <summary>
/// Enum class for events.
/// </summary>
enum class EventID {
	selectFile,
	loadF8,
	multisweep,
	selectFirstObjectResult,
	selectSecondObjectResult,
	compareResults
};