#include "MainApp.h"

MainApp *app;
extern "C" void startup() {
	app = new MainApp;
	app->start();
}