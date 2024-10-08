#include <gst/gst.h>
#include <gst/rtsp-server/rtsp-server.h>
#include <iostream>
#include <pthread.h>

#include "rtsp_server.h"

using namespace std;

void* start_rtsp_server_thread(void* arg) {
	server_data* data = static_cast<server_data*>(arg);
	GstRTSPServer *server;
    GstRTSPMountPoints *mounts;
    GstRTSPMediaFactory *factory;

    cout << "RTSP Server starting on port " << data->port << '\n';

	data->loop = g_main_loop_new(NULL, FALSE);

	server = gst_rtsp_server_new();
    g_object_set(server, "service", data->port, NULL);

    mounts = gst_rtsp_server_get_mount_points(server);

    factory = gst_rtsp_media_factory_new();

    gst_rtsp_media_factory_set_launch(factory, "(libcamerasrc ! video/x-raw,format=YUY2,width=640,height=480,framerate=45/1 ! videoconvert ! video/x-raw,format=I420 ! x264enc ! rtph264pay name=pay0 pt=96 )");

    gst_rtsp_mount_points_add_factory(mounts, "/test", factory);

    g_object_unref(mounts);

	if (gst_rtsp_server_attach(server, NULL) == 0) {
		cerr << "Failed to attach the server on port " << data->port << "\n";
        g_main_loop_unref(data->loop);
        return nullptr;
	}

	cout << "Stream ready at rtsp://RPI_IP_Address:" << data->port << "/test\n";

    g_main_loop_run(data->loop);

    g_main_loop_unref(data->loop);
    data->loop = nullptr;

    return nullptr;
}

bool start_rtsp_server(const char* port) {
	server_data* data = new server_data{ nullptr, port };
	pthread_t thread_id;

	if (pthread_create(&thread_id, nullptr, start_rtsp_server_thread, data) != 0) {
        cerr << "Failed to create thread for RTSP server\n";
        delete data;
        return false;
    }	

	pthread_detach(thread_id);
    
    return true;
}

void stop_rtsp_server(server_data* data) {
	if (data && data->loop) {
		cout << "Stopping RTSP Server on port " << data->port << '\n';
        g_main_loop_quit(data->loop);
    } else {
        cerr << "RTSP server is not running.\n";
    }
}
