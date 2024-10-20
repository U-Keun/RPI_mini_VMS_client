#ifndef CAMERA_COMTROLLER_H
#define CAMERA_CONTROLLER_H

#include <json/json.h>

class RTSPServer;

class CameraController {
public:
	CameraController(RTSPServer&);
	void startStreaming();
    void stopStreaming();
    bool isStreaming() const;
    Json::Value getStatus() const;
    Json::Value registerCamera() const;

private:
	RTSPServer& camera;
	bool streaming_on;
};

#endif // CAMERA_CONTROLLER_H
