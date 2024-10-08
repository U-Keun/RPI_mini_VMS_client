#include <iostream>
#include <pthread.h>
#include <json/json.h>
#include <queue>
#include <string>

#include "rtsp_server.h"
#include "gst_util.h"
#include "httplib.h"

using namespace std;
using namespace httplib;
using namespace Json;

#define DEFAULT_RTSP_PORT "8554"

int main(int argc, char** argv) {
    // GStreamer 초기화
    if (!initialize_gstreamer(argc, argv)) {
        cerr << "GStreamer initialization failed.\n";
        return -1;
    }

	queue<string> ports;
	ports.push("8554");
	ports.push("8555");
	ports.push("8556");

    Server svr;
	svr.Post("/cam", [&](const Request& req, Response& res) {
		CharReaderBuilder reader;
		Value root;
		string errs;

		const string body = req.body;
		istringstream s(body);

		if (parseFromStream(reader, s, &root, &errs)) {
			if (root["cam"] == "on") {
				if (ports.empty()) {
					cout << "Not enough port..\n";
					return 0;
				}

				const char* port = ports.front().c_str();

				if (!start_rtsp_server(port)) {
					cerr << "Failed to start RTSP server.\n";
					return -1;
				}	

				ports.pop();
			} else if (root["cam"] == "off") {
				cout << "let us stop the server!\n";
				// stop_rtsp_server();
			}
		}

		Value response;
		response["message"] = "received";
		StreamWriterBuilder writer;
		string jsonResponse = writeString(writer, response);

		res.set_content(jsonResponse, "application/json");
    });

    svr.listen("0.0.0.0", 8080);
	return 0;
}
