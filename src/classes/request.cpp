#include <string>
#include <WiFi.h>
#include <WiFiClient.h>
#include <iomanip>
#include <sstream>

typedef enum {
    CONNECTED,	    
    CONNECTING,
    RETRYING,
    FAILED
} WifiState;
typedef std::function<void(WifiState)> WifiStateCallback;

typedef struct {
	std::string id;
	double latitude;
	double longitude;
	std::string imageData;
} IdentifyRequest;
typedef struct {
	std::string name;
	std::string description;	
} IdentifyResponse;

class Comunication {

	public:
		Comunication() {}
		~Comunication() {
			WiFi.disconnect(true);
			WiFi.persistent(false);
		}	

		void connectWiFi(std::string ssid, std::string password, WifiStateCallback callback) {
	    if(WiFi.status() == WL_CONNECTED) {
	      callback(CONNECTED);
	      return;
	    }
			
			callback(CONNECTING);
	    WiFi.begin(ssid.c_str(), password.c_str());

	    int tryLimit = 10;
	    while (WiFi.status() != WL_CONNECTED && tryLimit > 0) {				
	      callback(CONNECTING);
	      delay(1000);
	      tryLimit--;
	    }

			switch (WiFi.status()) {
	      case WL_CONNECTED:
	        callback(CONNECTED);
	        break;	        
	      default:
	        callback(FAILED);
	        break;
	    }
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
			
			if (request.id.length() != 36) {		
				response.name = "Erro";
				response.description = "ID inválido";
				return response;
			}

			std::string requestData = 
				"idt" +                  // 3 chars
				request.id +             // 36 chars
				requestLatitude.str() +  // 10 chars
				requestLongitude.str() + // 10 chars
				request.imageData;       // Variable content
			
			WiFiClient client;
			while (!client.connect("example.com", 80)) delay(200);
			client.print(requestData.c_str());
			std::string responseStr = client.readStringUntil('\0').c_str();	
			
			response.name = "Sample Name";
			response.description = "Sample Description";
			return response;
		}	
};