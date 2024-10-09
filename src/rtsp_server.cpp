#include <gst/gst.h>
#include <gst/rtsp-server/rtsp-server.h>
#include <iostream>
#include <thread>
#include <mutex>

#include "rtsp_server.h"

using namespace std;

GMainLoop* loop = nullptr;
GstRTSPServer* server = nullptr;
GstRTSPMediaFactory* factory = nullptr;
GstRTSPMedia* media = nullptr;
bool is_streaming = false;
mutex media_mutex;

static void media_configure(GstRTSPMediaFactory* factory, GstRTSPMedia* new_media, gpointer user_data) {
	lock_guard<mutex> lock(media_mutex);
	media = new_media;

    GstElement* element = gst_rtsp_media_get_element(media);

    if (!is_streaming) {
        gst_element_set_state(element, GST_STATE_PLAYING);
    } else {
		gst_element_set_state(element, GST_STATE_NULL);
	}

    gst_object_unref(element);

	g_signal_connect(media, "unprepared", G_CALLBACK(+[](GstRTSPMedia* unprepared_media, gpointer user_data) {
		lock_guard<mutex> lock(media_mutex);
		if (media == unprepared_media) media = nullptr;
	}), nullptr);
}

bool start_rtsp_server(const char* port) {
	loop = g_main_loop_new(NULL, FALSE);

    server = gst_rtsp_server_new();
    g_object_set(server, "service", port, NULL);

    GstRTSPMountPoints* mounts = gst_rtsp_server_get_mount_points(server);
    factory = gst_rtsp_media_factory_new();

	gst_rtsp_media_factory_set_launch(factory, "(libcamerasrc ! video/x-raw,format=YUY2,width=640,height=480,framerate=45/1 ! videoconvert ! video/x-raw,format=I420 ! x264enc ! rtph264pay name=pay0 pt=96 )");
    gst_rtsp_media_factory_set_shared(factory, TRUE);

    g_signal_connect(factory, "media-configure", G_CALLBACK(media_configure), NULL);

    gst_rtsp_mount_points_add_factory(mounts, "/test", factory);
    g_object_unref(mounts);

    if (gst_rtsp_server_attach(server, NULL) == 0) {
        cerr << "Failed to attach the server on port " << port << "\n";
        return false;
    }
	
	cout << "Stream ready at rtsp://<RPI_IP_Address>:" << port << "/test\n";

    thread main_loop_thread([]() {
        g_main_loop_run(loop);
    });
    main_loop_thread.detach();

    return true;
}

void start_streaming() {
    if (!is_streaming) {
        cout << "Starting streaming.\n";
        is_streaming = true;
		
		lock_guard<mutex> lock(media_mutex);
		if (media) {
			GstElement* element = gst_rtsp_media_get_element(media);
			gst_element_set_state(element, GST_STATE_PLAYING);
			gst_object_unref(element);
		}
    } else {
        cout << "Streaming is already running.\n";
    }
}

void stop_streaming() {
    if (is_streaming) {
        cout << "Stopping streaming.\n";
        is_streaming = false;
		
		lock_guard<mutex> lock(media_mutex);
		if (media) {
			GstElement* element = gst_rtsp_media_get_element(media);
			gst_element_set_state(element, GST_STATE_NULL);
			gst_object_unref(element);
		}
    } else {
        cout << "Streaming is not running.\n";
    }
}

void stop_rtsp_server() {
    if (loop) {
        cout << "Stopping RTSP Server.\n";
        g_main_loop_quit(loop);
        g_main_loop_unref(loop);
        loop = nullptr;
    }

    if (server) {
        g_object_unref(server);
        server = nullptr;
    }

    if (factory) {
        g_object_unref(factory);
        factory = nullptr;
    }
	
	lock_guard<mutex> lock(media_mutex);
	media = nullptr;
}
