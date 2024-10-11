#include <json/json.h>
#include "httplib.h"
#include <iostream>

#include <unistd.h>
#include <sys/syscall.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include "http_server.h"
#include "rtsp_server.h"

using namespace std;
using namespace httplib;
using namespace Json;

#define PORT 8080
#define GPIO_CONTROL 454
#define GPIO_LED 18
#define ON 1
#define OFF 0
#define MYPID _IO(100, 1)

void check_cam_status(Server&);
void register_camera(Server&);
void control_camera(Server&);

bool streaming_on = false;

void sigio_handler(int signo, siginfo_t *info, void *context) {
	cout << "SIGIO is called\n";

	if (streaming_on) {
		stop_streaming();
		streaming_on = false;
		syscall(GPIO_CONTROL, GPIO_LED, OFF);
	} else {
		start_streaming();
		streaming_on = true;
		syscall(GPIO_CONTROL, GPIO_LED, ON);
	}
}

void start_http_server() {

	struct sigaction sa;

	memset(&sa, 0, sizeof(sa));
	sa.sa_sigaction = sigio_handler;
	sa.sa_flags = SA_SIGINFO;

	if (sigaction(SIGIO, &sa, NULL) == -1) {
		perror("sigaction");
		return;
	}

	int fd;
	fd = open("/dev/gpiobutton", O_RDWR);
	if (fd < 0) {
		perror("/dev/gpiobutton error");
		return;
	}
	
	pid_t mypid = getpid();
	ioctl(fd, MYPID, mypid);

	Server server;

	// GET request
	check_cam_status(server);

	// POST request
	register_camera(server);
	control_camera(server);

	server.listen("0.0.0.0", PORT);
}

void check_cam_status(Server& server) {
	server.Get("/cam-status", [](const Request& req, Response& res) {
		Value response;
		if (streaming_on) {
			response["message"] = "running";
		} else response["message"] = "stopped";
		StreamWriterBuilder writer;
		string jsonResponse = writeString(writer, response);

		res.set_content(jsonResponse, "application/json");
	});
}

void register_camera(Server& server) {
	server.Post("/register", [](const Request& req, Response& res) {
		Value response;
		response["message"] = "registered";
		response["name"] = "U-Keun";
		response["port"] = "8554";
		response["mount"] = "/stream";
		StreamWriterBuilder writer;
		string jsonResponse = writeString(writer, response);

		res.set_content(jsonResponse, "application/json");
	});
}

void control_camera(Server& server) {
	server.Post("/cam", [](const Request& req, Response& res) {
		CharReaderBuilder reader;
		Value root;
		string errs;
		
		const string body = req.body;
		istringstream s(body);

		if (parseFromStream(reader, s, &root, &errs)) {
			if (root["camera"] == "on") {
				start_streaming();
				streaming_on = true;
				syscall(GPIO_CONTROL, GPIO_LED, ON);
			} else if (root["camera"] == "off") {
				stop_streaming();
				streaming_on = false;
				syscall(GPIO_CONTROL, GPIO_LED, OFF);
			}
		}

		Value response;
		response["message"] = "success";
		StreamWriterBuilder writer;
		string jsonResponse = writeString(writer, response);

		res.set_content(jsonResponse, "application/json");
	});
}
