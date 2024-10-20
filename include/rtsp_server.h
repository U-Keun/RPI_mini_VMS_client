#ifndef RTSP_SERVER_H
#define RTSP_SERVER_H

#include <gst/gst.h>
#include <gst/rtsp-server/rtsp-server.h>
#include <mutex>

class RTSPServer {
public:
	RTSPServer(const char*);
	~RTSPServer();
	void start();
	void start_streaming();
	void stop_streaming();
	void stop();

private:
	void media_configure(GstRTSPMediaFactory*, GstRTSPMedia*);
	static void media_configure_callback(GstRTSPMediaFactory* factory, GstRTSPMedia* new_media, gpointer user_data) {
		RTSPServer* self = static_cast<RTSPServer*>(user_data);
		self->media_configure(factory, new_media);
	}
	static void on_media_unprepared(GstRTSPMedia*, gpointer);
	
	GMainLoop* loop;
	GstRTSPServer* server;
	GstRTSPMediaFactory* factory;
	GstRTSPMedia* media;
	bool is_streaming;
	std::mutex media_mutex;
	const char* port;
};

#endif // RTSP_SERVER_H


