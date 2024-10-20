#include <iostream>
#include <thread>

#include "rtsp_server.h"

using namespace std;

RTSPServer::RTSPServer(const char* port) 
: loop(nullptr), server(nullptr), factory(nullptr), media(nullptr), is_streaming(false), port(port) {}

RTSPServer::~RTSPServer() {
	stop();
}

void RTSPServer::on_media_unprepared(GstRTSPMedia* unprepared_media, gpointer user_data) {
	auto* self = static_cast<RTSPServer*>(user_data);
	lock_guard<mutex> lock(self->media_mutex);
	if (self->media == unprepared_media) self->media = nullptr;
}

void RTSPServer::media_configure(GstRTSPMediaFactory* factory, GstRTSPMedia* new_media) {
	lock_guard<mutex> lock(media_mutex);
	media = new_media;

	GstElement* element = gst_rtsp_media_get_element(media);

    if (!is_streaming) {
        gst_element_set_state(element, GST_STATE_PLAYING);
    } else {
		gst_element_set_state(element, GST_STATE_NULL);
	}

    gst_object_unref(element);

	g_signal_connect(media, "unprepared", G_CALLBACK(&RTSPServer::on_media_unprepared), this);
}

void RTSPServer::start() {
	loop = g_main_loop_new(NULL, FALSE);

    server = gst_rtsp_server_new();
    g_object_set(server, "service", port, NULL);

    GstRTSPMountPoints* mounts = gst_rtsp_server_get_mount_points(server);
    factory = gst_rtsp_media_factory_new();

	gst_rtsp_media_factory_set_launch(factory, "(libcamerasrc ! video/x-raw,format=YUY2,width=640,height=480,framerate=45/1 ! videoconvert ! video/x-raw,format=I420 ! x264enc ! rtph264pay name=pay0 pt=96 )");

    gst_rtsp_media_factory_set_shared(factory, TRUE);

    g_signal_connect(factory, "media-configure", G_CALLBACK(media_configure_callback), this);

    gst_rtsp_mount_points_add_factory(mounts, "/stream", factory);
    g_object_unref(mounts);

    if (gst_rtsp_server_attach(server, NULL) == 0) {
        cerr << "Failed to attach the server on port " << port << "\n";
        exit(1);
    }
	
	cout << "Stream ready at rtsp://<RPI_IP_Address>:" << port << "/stream\n";

    thread main_loop_thread([this]() {
        g_main_loop_run(loop);
    });
    main_loop_thread.detach();
}

void RTSPServer::start_streaming() {
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

void RTSPServer::stop_streaming() {
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

void RTSPServer::stop() {
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
