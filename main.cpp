#include <gst/gst.h>
#include <gst/rtsp-server/rtsp-server.h>
#include <iostream>

using namespace std;

#define DEFAULT_RTSP_PORT "8554"

int main(int argc, char** argv) {
	GMainLoop *loop;
    GstRTSPServer *server;
    GstRTSPMountPoints *mounts;
    GstRTSPMediaFactory *factory;
    GOptionContext *optctx;
    GError *error = NULL;
    const char* port = DEFAULT_RTSP_PORT;

    cout << "RTSP Server starting...\n";

    optctx = g_option_context_new("RTSP Server\n");
    g_option_context_add_group(optctx, gst_init_get_option_group());
    if (!g_option_context_parse(optctx, &argc, &argv, &error)) {
        cerr << "Error parsing options: " << error->message << endl;
        return -1;
    }

    g_option_context_free(optctx);

    // 메인 루프 생성
    loop = g_main_loop_new(NULL, FALSE);

    // RTSP 서버 생성
    server = gst_rtsp_server_new();
    g_object_set(server, "service", port, NULL);

    // 마운트 포인트 생성
    mounts = gst_rtsp_server_get_mount_points(server);

    // RTSP 미디어 팩토리 생성
    factory = gst_rtsp_media_factory_new();

    // GStreamer 파이프라인 설정
    gst_rtsp_media_factory_set_launch(factory, "(libcamerasrc ! video/x-raw,format=YUY2,width=640,height=480,framerate=45/1 ! videoconvert ! x264enc ! rtph264pay name=pay0 pt=96 )");

    // 스트림 경로 설정
    gst_rtsp_mount_points_add_factory(mounts, "/test", factory);

    // 마운트 포인트 해제
    g_object_unref(mounts);

    // 서버 연결
    if (gst_rtsp_server_attach(server, NULL) == 0) {
        cerr << "Failed to attach the server\n";
        return -1;
    }

    // 서버 정보 출력
    cout << "Stream ready at rtsp://RPI_IP_Address:" << port << "/test\n";

    // 메인 루프 실행
    g_main_loop_run(loop);

    // 메인 루프 해제
    g_main_loop_unref(loop);


	return 0;

}
