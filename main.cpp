#include <iostream>

#include "gst_util.h"
#include "rtsp_server.h"
#include "http_server.h"

using namespace std;

#define DEFAULT_RTSP_PORT "8554"

int main(int argc, char** argv) {
    // GStreamer 초기화
    if (!initialize_gstreamer(argc, argv)) {
        cerr << "GStreamer initialization failed.\n";
        return -1;
    }
	
	// RTSP 서버 시작
	if (!start_rtsp_server(DEFAULT_RTSP_PORT)) {
        cerr << "Failed to start RTSP server.\n";
        return -1;
    }

	start_http_server();

	return 0;
}
