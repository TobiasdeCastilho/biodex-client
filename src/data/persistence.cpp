#pragma once

#define DATA_PATH "/data/persist.data"

#include <string>
#include "FS.h"
#include "LittleFS.h"

class Data {
  private:
    std::string wifiId, wifiPassword;
    std::string username, userId;

  public:
    Data(){};
    ~Data() {};

    void load() {
      if (!LittleFS.exists(DATA_PATH)) return;

      File file = LittleFS.open(DATA_PATH, "r");
      if (!file) return;

      wifiId = file.readStringUntil('\n').c_str();
      wifiPassword = file.readStringUntil('\n').c_str();
      username = file.readStringUntil('\n').c_str();
      userId = file.readStringUntil('\n').c_str();

      file.close();
    }

    void save() {
      LittleFS.remove(DATA_PATH);

      Serial.printf("New Values: %s\n%s\n%s\n%s",wifiId,wifiPassword,username,userId);

      File file = LittleFS.open(DATA_PATH, "w+");
      file.printf("%s\n%s\n%s\n%s\n",
        wifiId.c_str(),
        wifiPassword.c_str(),
        username.c_str(),
        userId.c_str()
      );
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
