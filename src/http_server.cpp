#include <json/json.h>
#include "httplib.h"

#include "http_server.h"
#include "rtsp_server.h"

using namespace std;
using namespace httplib;
using namespace Json;

void register_camera();
void control_camera(Server&);

void start_http_server() {
	Server server;
	
	control_camera(server);

	server.listen("0.0.0.0", 8080);
}

void register_camera() {

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
			} else if (root["camera"] == "off") {
				stop_streaming();
			}
		}

		Value response;
		response["message"] = "success";
		StreamWriterBuilder writer;
		string jsonResponse = writeString(writer, response);

		res.set_content(jsonResponse, "application/json");
	});
}



