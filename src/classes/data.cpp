#define DATA_PATH "/data/persist.data"

#include <string>
#include "FS.h"
#include "LittleFS.h"

class Data {
  private:
    // All data to keep
    std::string wifiId, wifiPassword; // Lines 1, 2
    std::string username, userId; // Lines 3, 4

  public:
    Data(){};
    ~Data() {};

    void load() {
      File file = LittleFS.open(DATA_PATH, "r");
      if (!file) {
        Serial.println("Failed to open persistent data");
        return;
      }

      wifiId = file.read();
      wifiPassword = file.read();

      username = file.read();
      userId = file.read();

      Serial.printf("Loaded data: %s, %s, %s, %s\n", wifiId, wifiPassword, username, userId);

      file.close();
    }

    void save() {
      LittleFS.remove(DATA_PATH);

      File file = LittleFS.open(DATA_PATH, "w+");
      file.printf("%s\n%s\n%s\n%s",wifiId,wifiPassword,username,userId);
      file.close();
    }

    std::string getWiFiId() {
      return wifiId;
    }

    std::string getWiFiPassword() {
      return wifiPassword;
    }

    std::string getUsername() {
      return username;
    }

    std::string getUserId() {
      return userId;
    }

    void setWiFiId(std::string value) {
      wifiId = value;
    }

    void setWifiPassword(std::string value) {
      wifiPassword = value;
    }

    void setUsername(std::string value) {
      username = value;
    }

    void setUserId(std::string value) {
      userId = value;
    }
};
