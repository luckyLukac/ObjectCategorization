#include "App.hpp"
#include "MainWindow.hpp"


wxIMPLEMENT_APP(App);  // Implementation of the app.


// Initialization method for the app.
bool App::OnInit() {
	// Creating a new window.
	MainWindow* window = new MainWindow();
	window->Show(true);

	return true;
}