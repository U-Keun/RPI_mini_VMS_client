#include <iostream>

#include "rtsp_server.h"
#include "gst_util.h"

using namespace std;

#define DEFAULT_RTSP_PORT "8554"

int main(int argc, char** argv) {
    const char* port = DEFAULT_RTSP_PORT;

    // GStreamer 초기화
    if (!initialize_gstreamer(argc, argv)) {
        cerr << "GStreamer initialization failed.\n";
        return -1;
    }

    // RTSP 서버 실행
    if (!start_rtsp_server(port)) {
        cerr << "Failed to start RTSP server.\n";
        return -1;
    }	
	return 0;

}
