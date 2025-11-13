#pragma once

#include <string>
#include <WiFi.h>
#include <WiFiClient.h>
#include <iomanip>
#include <sstream>

#include "../globals/data.h"

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
	std::string imageData;
} IdentifyRequest;
typedef struct {
  std::string animalId;
	std::string name;
	std::string description;
	const char* audio;
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

    void sendRequest() {
      WiFiClient client;
			while (!client.connect(PROTOCOL_DEFAULT_HOST, PROTOCOL_DEFAULT_PORT)) delay(200);
			std::string data = requestData;
			Serial.printf("Message: %s", data.c_str());

			client.print(data.c_str());
			responseData = client.readStringUntil('\0').c_str();
    }

  public:
		Request() {}
		~Request() {}

		void requestInitialize(const InitializeRequest& request) {
			requestData =
			  "ini" +
				completeData(request.user, 30),
				completeData(request.password, 30);

			sendRequest();

			std::string userId = getFromResponse(0);
			data.setUserId(userId);

			Serial.printf("User Id: %s", userId.c_str());
		}

		IdentifyResponse requestIdentify(const IdentifyRequest& request) {
			IdentifyResponse response;

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

			requestData =
				"idt" +
				data.getUserId() +       // 36 chars
				requestLatitude.str() +  // 10 chars
				requestLongitude.str() + // 10 chars
				request.imageData;       // Variable content

			response.animalId = getFromResponse(32);
			response.name = getFromResponse(60);
			response.description = getFromResponse(400);
			response.audio = getFromResponse(0).c_str();

			return response;
		}
};
