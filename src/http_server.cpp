#include <json/json.h>
#include "httplib.h"
#include <iostream>

#include "http_server.h"
#include "rtsp_server.h"

using namespace std;
using namespace httplib;
using namespace Json;

#define PORT 8080

void check_cam_status(Server&);
void register_camera(Server&);
void control_camera(Server&);

bool streaming_on = false;

void start_http_server() {
	Server server;

	// GET request
	check_cam_status(server);

	// POST request
	register_camera(server);
	control_camera(server);

	server.listen("0.0.0.0", PORT);
}

void check_cam_status(Server& server) {
	server.Get("/cam-status", [](const Request& req, Response& res) {
		Value response;
		if (streaming_on) {
			response["message"] = "running";
		} else response["message"] = "stopped";
		StreamWriterBuilder writer;
		string jsonResponse = writeString(writer, response);

		res.set_content(jsonResponse, "application/json");
	});
}

void register_camera(Server& server) {
	server.Post("/register", [](const Request& req, Response& res) {
		Value response;
		response["message"] = "registered";
		response["name"] = "U-Keun";
		response["port"] = "8554";
		response["mount"] = "/stream";
		StreamWriterBuilder writer;
		string jsonResponse = writeString(writer, response);

		res.set_content(jsonResponse, "application/json");
	});
}

void control_camera(Server& server) {
	server.Post("/cam", [](const Request& req, Response& res) {
		CharReaderBuilder reader;
		Value root;
		string errs;
		
		const string body = req.body;
		istringstream s(body);

		if (parseFromStream(reader, s, &root, &errs)) {
			if (root["camera"] == "on") {
				start_streaming();
				streaming_on = true;
			} else if (root["camera"] == "off") {
				stop_streaming();
				streaming_on = false;
			}
		}

		Value response;
		response["message"] = "success";
		StreamWriterBuilder writer;
		string jsonResponse = writeString(writer, response);

		res.set_content(jsonResponse, "application/json");
	});
}
