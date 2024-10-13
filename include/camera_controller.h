#ifndef CAMERA_COMTROLLER_H
#define CAMERA_CONTROLLER_H

#include <json/json.h>

class CameraController {
public:
	CameraController();
	void startStreaming();
    void stopStreaming();
    bool isStreaming() const;
    Json::Value getStatus() const;
    Json::Value registerCamera() const;

private:
	bool streaming_on;
};

#endif // CAMERA_CONTROLLER_H
