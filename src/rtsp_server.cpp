#include <gst/gst.h>
#include <gst/rtsp-server/rtsp-server.h>
#include <gio/gio.h>
#include <sys/socket.h>
#include <iostream>
#include <pthread.h>

#include "rtsp_server.h"

using namespace std;

GMainLoop* loop = nullptr;
GstRTSPServer* server = nullptr;

void* start_rtsp_server_thread(void* arg) {
	const char* port = static_cast<const char*>(arg);
    GstRTSPMountPoints *mounts;
    GstRTSPMediaFactory *factory;

    cout << "RTSP Server starting on port " << port << '\n';

	loop = g_main_loop_new(NULL, FALSE);
	server = gst_rtsp_server_new();
    g_object_set(server, "service", port, NULL);

    mounts = gst_rtsp_server_get_mount_points(server);
    factory = gst_rtsp_media_factory_new();

    gst_rtsp_media_factory_set_launch(factory, "(libcamerasrc ! video/x-raw,format=YUY2,width=640,height=480,framerate=45/1 ! videoconvert ! video/x-raw,format=I420 ! x264enc ! rtph264pay name=pay0 pt=96 )");

	gst_rtsp_media_factory_set_shared(factory, TRUE);
    gst_rtsp_mount_points_add_factory(mounts, "/test", factory);
    g_object_unref(mounts);

	if (gst_rtsp_server_attach(server, NULL) == 0) {
		cerr << "Failed to attach the server on port " << port << "\n";
		g_object_unref(server);
        g_main_loop_unref(loop);
        return nullptr;
	}

	cout << "Stream ready at rtsp://RPI_IP_Address:" << port << "/test\n";

    g_main_loop_run(loop);

	g_object_unref(server);
    g_main_loop_unref(loop);
    server = nullptr;
    loop = nullptr;

    return nullptr;
}

bool start_rtsp_server(const char* port) {
	pthread_t thread_id;

	if (pthread_create(&thread_id, nullptr, start_rtsp_server_thread, (void*)port) != 0) {
        cerr << "Failed to create thread for RTSP server\n";
        return false;
    }	

	pthread_detach(thread_id);
    
    return true;
}

void stop_rtsp_server() {
	if (loop) {
		cout << "Stopping RTSP Server.\n";
        g_main_loop_quit(loop);
    } else {
        cerr << "RTSP server is not running.\n";
    }
}
