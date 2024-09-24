#include "rtsp_server.h"
#include <gst/gst.h>
#include <gst/rtsp-server/rtsp-server.h>
#include <iostream>

using namespace std;

bool start_rtsp_server(const char* port) {
    GMainLoop *loop;
    GstRTSPServer *server;
    GstRTSPMountPoints *mounts;
    GstRTSPMediaFactory *factory;

    cout << "RTSP Server starting...\n";

    loop = g_main_loop_new(NULL, FALSE);

    server = gst_rtsp_server_new();
    g_object_set(server, "service", port, NULL);

    mounts = gst_rtsp_server_get_mount_points(server);

    factory = gst_rtsp_media_factory_new();

    gst_rtsp_media_factory_set_launch(factory, "(libcamerasrc ! video/x-raw,format=YUY2,width=640,height=480,framerate=45/1 ! videoconvert ! video/x-raw,format=I420 ! x264enc ! rtph264pay name=pay0 pt=96 )");

    gst_rtsp_mount_points_add_factory(mounts, "/test", factory);

    g_object_unref(mounts);

    if (gst_rtsp_server_attach(server, NULL) == 0) {
        cerr << "Failed to attach the server\n";
        return false;
    }

    cout << "Stream ready at rtsp://RPI_IP_Address:" << port << "/test\n";

    g_main_loop_run(loop);

    g_main_loop_unref(loop);

    return true;
}
