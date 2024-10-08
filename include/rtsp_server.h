#ifndef RTSP_SERVER_H
#define RTSP_SERVER_H

typedef struct _GMainLoop GMainLoop;

struct server_data {
	GMainLoop* loop;
	const char *port;
};

bool start_rtsp_server(const char*);
void stop_rtsp_server(server_data*);

#endif // RTSP_SERVER_H
