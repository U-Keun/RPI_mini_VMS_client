#include <iostream>
#include <pthread.h>
#include <json/json.h>

#include "rtsp_server.h"
#include "gst_util.h"
#include "httplib.h"

using namespace std;
using namespace httplib;
using namespace Json;

#define DEFAULT_RTSP_PORT "8554"

void* http_server_thread(void* arg) {
	const char* port = DEFAULT_RTSP_PORT;
    Server svr;

    svr.Post("/cam", [=](const Request& req, Response& res) {
		CharReaderBuilder reader;
		Value root;
		string errs;

		const string body = req.body;
		istringstream s(body);

		if (parseFromStream(reader, s, &root, &errs)) {
			if (root["cam"] == "on") {
				if (!start_rtsp_server(port)) {
					cerr << "Failed to start RTSP server.\n";
					return -1;
				}	
			} else if (root["cam"] == "off") {
				stop_rtsp_server();
			}
		}

		Value response;
		response["message"] = "received";
		StreamWriterBuilder writer;
		string jsonResponse = writeString(writer, response);

		res.set_content(jsonResponse, "application/json");
    });

    svr.listen("0.0.0.0", 8080);
    return nullptr;
}

int main(int argc, char** argv) {
    // GStreamer 초기화
    if (!initialize_gstreamer(argc, argv)) {
        cerr << "GStreamer initialization failed.\n";
        return -1;
    }

	pthread_t http_thread;
	if (pthread_create(&http_thread, nullptr, http_server_thread, nullptr)) {
        cerr << "Failed to create HTTP server thread.\n";
        return -1;
    }	

	pthread_join(http_thread, nullptr);

	return 0;
}
