#include <iostream>

#include "gst_util.h"
#include "rtsp_server.h"
#include "camera_controller.h"
#include "http_server.h"

using namespace std;

#define RTSP_PORT "8554"
#define HTTP_PORT 8080

int main(int argc, char** argv) {
    // GStreamer 초기화
    if (!initialize_gstreamer(argc, argv)) {
        cerr << "GStreamer initialization failed.\n";
        return -1;
    }

	RTSPServer camera(RTSP_PORT);
	CameraController camera_controller(camera);
	HTTPServer server(HTTP_PORT, camera_controller);
	server.start();

	return 0;
}
