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
	
	if (!start_rtsp_server(DEFAULT_RTSP_PORT)) {
        cerr << "Failed to start RTSP server.\n";
        return -1;
    }	

	Server svr;
    svr.Post("/cam", [&](const Request& req, Response& res) {
        CharReaderBuilder reader;
        Value root;
        string errs;

        const string body = req.body;
        istringstream s(body);

        if (parseFromStream(reader, s, &root, &errs)) {
            if (root["cam"] == "on") {
                start_streaming();
            } else if (root["cam"] == "off") {
                stop_streaming();
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
