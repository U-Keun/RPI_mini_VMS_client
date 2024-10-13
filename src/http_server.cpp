#include <iostream>

#include "http_server.h"
#include "camera_controller.h"

using namespace httplib;
using namespace Json;
using namespace std;

HTTPServer::HTTPServer(int port, CameraController& camera_controller)
	: port(port), camera_controller(camera_controller) {}

void HTTPServer::start() {
	setupRoutes();
	server.listen("0.0.0.0", port);
}

void HTTPServer::setupRoutes() {
	server.Get("/cam-status", [this](const Request& req, Response& res) {
		getCamStatus(req, res);
	});
	server.Post("/register", [this](const Request& req, Response& res) {
		postRegisterCamera(req, res);
	});
	server.Post("/cam", [this](const Request& req, Response& res) {
		postControlCamera(req, res);
	});
}

void HTTPServer::getCamStatus(const Request& req, Response& res) {
	Value response = camera_controller.getStatus();
	StreamWriterBuilder writer;
	string jsonResponse = writeString(writer, response);
	res.set_content(jsonResponse, "application/json");
}

void HTTPServer::postRegisterCamera(const Request& req, Response& res) {
	Value response = camera_controller.registerCamera();
	StreamWriterBuilder writer;
	string jsonResponse = writeString(writer, response);
	res.set_content(jsonResponse, "application/json");
}

void HTTPServer::postControlCamera(const Request& req, Response& res) {
	CharReaderBuilder reader;
	Value root;
	string errs;
		
	const string body = req.body;
	istringstream s(body);

	if (parseFromStream(reader, s, &root, &errs)) {
		if (root["camera"] == "on") {
			camera_controller.startStreaming();
		} else if (root["camera"] == "off") {
			camera_controller.stopStreaming();
		} else {
			res.status = 400;
			return;
		}
	} else {
		res.status = 400;
		return;
	}

	Value response;
	response["message"] = "success";
	StreamWriterBuilder writer;
	string jsonResponse = writeString(writer, response);
	res.set_content(jsonResponse, "application/json");
}
