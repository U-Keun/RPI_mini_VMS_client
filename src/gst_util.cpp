#include "gst_util.h"
#include <gst/gst.h>
#include <glib.h>
#include <iostream>

using namespace std;

bool initialize_gstreamer(int argc, char** argv) {
    GOptionContext *optctx;
    GError *error = NULL;

    optctx = g_option_context_new("RTSP Server\n");
    g_option_context_add_group(optctx, gst_init_get_option_group());
    
    if (!g_option_context_parse(optctx, &argc, &argv, &error)) {
        cerr << "Error parsing options: " << error->message << endl;
        g_error_free(error);
        g_option_context_free(optctx);
        return false;
    }

    g_option_context_free(optctx);
    return true;
}
