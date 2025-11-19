#pragma once

#include <string>
#include <WiFi.h>
#include <WiFiClient.h>
#include <iomanip>
#include <sstream>
#include "esp_camera.h"

#include "../globals/data.h"

#include "../hardware/audio.cpp"

#define PROTOCOL_DEFAULT_PORT 4002
#define PROTOCOL_DEFAULT_HOST "10.0.0.196"

typedef enum {
  CONNECTED,
  CONNECTING,
  RETRYING,
  FAILED
} WifiState;
typedef std::function<void(WifiState)> WifiStateCallback;

typedef struct {
  std::string user;
  std::string password;
} InitializeRequest;
typedef struct {
  std::string id;
} InitializeResponse;

typedef struct {
	double latitude;
	double longitude;
} IdentifyRequest;
typedef struct {
	std::string name;
	std::string popularName;
	std::string description;
	uint8_t* audio;
	size_t audioSize;
} IdentifyResponse;

typedef struct {
  int page;
} ListRequest;
typedef struct {
  std::string animalId;
  std::string nome;
} ListItem;
typedef struct {
  int page;
  int amount;
  ListItem* items;
} ListResponse;

typedef struct {
  std::string animalId;
} SearchRequest;
typedef struct {
  std::string animalId;
  std::string name;
  std::string description;
  int amount;
} SearchResponse;

typedef struct {
  std::string animalId;
  int image;
} ImageSearchRequest;
typedef struct {
  char* image;
} ImageSeatchResponse;

class Request {
  private:
    std::string requestData, responseData;

    bool checkConnection(){
      return WiFi.status() == WL_CONNECTED;
    }

    std::string getFromResponse(int size){
      if(!size) return responseData;

      std::string result = responseData.substr(0,size - 1);
      responseData = responseData.substr(size);
      return result;
    };

    std::string completeData(std::string data, int size) {
      std::string result = data;
      for(int i = result.length(); i < size; i++)
        result += ' ';
      return result;
    }

    void sendRequest(const char* buffer, size_t len) {
      WiFiClient client;
			connectClient(client);

      size_t sent = 0;
      while(sent < len){
        sent += client.write((const uint8_t*)buffer + sent, len - sent);
      }
      responseData = client.readStringUntil('\0').c_str();
    }

    void connectClient(WiFiClient& client) {
      while (!client.connect(PROTOCOL_DEFAULT_HOST, PROTOCOL_DEFAULT_PORT)) delay(200);
    }

  public:
		Request() {}
		~Request() {}

		IdentifyResponse lastIdentification;

		void requestInitialize(const InitializeRequest& request) {
			if(!checkConnection()) return;

			requestData =
			  "ini" +
				completeData(request.user, 30) +
				completeData(request.password, 30);

			sendRequest(requestData.c_str(), requestData.length());

			std::string status = getFromResponse(3);
			if(!status.compare("000")) return;

			std::string userId = getFromResponse(0);

			data.setUserId(userId);
			data.setUsername(request.user);
			data.save();

			responseData.clear();
		}

		void requestIdentify(const IdentifyRequest& request, camera_fb_t* photo) {
      if(!checkConnection()) return;

      std::ostringstream requestLatitude;
      requestLatitude << std::showpos
          << std::fixed
          << std::setprecision(6)
          << std::setw(10)
          << std::setfill('0')
          << request.latitude;

      std::ostringstream requestLongitude;
      requestLongitude << std::showpos
          << std::fixed
          << std::setprecision(6)
          << std::setw(10)
          << std::setfill('0')
          << request.longitude;

      WiFiClient client;
      while (!client.connect(PROTOCOL_DEFAULT_HOST, PROTOCOL_DEFAULT_PORT)) {
        delay(200);
      }

      std::string requestData =
          "idt" +
          data.getUserId() +
          requestLatitude.str() +
          requestLongitude.str();

      client.print(requestData.c_str());

      const size_t chunkSize = 512;
      size_t sent = 0;
      while(sent < photo->len){
        size_t len = (photo->len - sent > chunkSize) ? chunkSize : (photo->len - sent);
        client.write((const uint8_t*)photo->buf + sent, len);
        sent += len;
        yield();
      }
      client.print('\0');

      responseData.clear();
      while (responseData.size() < 3 + 560 + 20) {
        if (client.available()) {
          responseData.push_back(client.read());
        } else {
          delay(1);
        }
      }

      std::string status  = getFromResponse(3);
      if (!status.compare("000")) return;

      lastIdentification.name.clear();
      lastIdentification.popularName.clear();
      lastIdentification.description.clear();
      delete lastIdentification.audio;

      lastIdentification.name = getFromResponse(60);
      lastIdentification.popularName = getFromResponse(100);
      lastIdentification.description = getFromResponse(400);
      std::string sizeStr = getFromResponse(10);
      responseData.clear();

      uint32_t fileSize = atoi(sizeStr.c_str());
      if(!fileSize) return;

      lastIdentification.audio = new uint8_t[fileSize];
      lastIdentification.audioSize = fileSize;

      client.read(lastIdentification.audio, fileSize);
      client.stop();
		}
};
