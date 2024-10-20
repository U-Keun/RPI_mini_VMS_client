#include "camera_controller.h"
#include "rtsp_server.h"

CameraController::CameraController(RTSPServer& camera) : camera(camera), streaming_on(false) {
	camera.start();
}

void CameraController::startStreaming() {
	camera.start_streaming();
	streaming_on = true;
}

void CameraController::stopStreaming() {
	camera.stop_streaming();
	streaming_on = false;
}

bool CameraController::isStreaming() const {
	return streaming_on;
}

Json::Value CameraController::getStatus() const {
	Json::Value response;
    response["message"] = streaming_on ? "running" : "stopped";
    return response;
}

Json::Value CameraController::registerCamera() const {
	Json::Value response;
	response["message"] = "registered";
    response["name"] = "U-Keun";
    response["port"] = "8554";
    response["mount"] = "/stream";
    return response;
}
