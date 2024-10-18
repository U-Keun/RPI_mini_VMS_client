#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include "httplib.h"

class CameraController;

class HTTPServer {
public:
	HTTPServer(int, CameraController&);
	void start();

private:
	void setupRoutes();
	void getCamStatus(const httplib::Request&, httplib::Response&);
	void postRegisterCamera(const httplib::Request&, httplib::Response&);
	void postControlCamera(const httplib::Request&, httplib::Response&);
	
	httplib::Server server;
	int port;
	CameraController& camera_controller;
};

#endif // HTTP_SERVER_H


