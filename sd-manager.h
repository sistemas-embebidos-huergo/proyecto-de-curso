#ifndef SD_MANAGER_H
#define SD_MANAGER_H

#include "SD.h"

class SDManager
{
private:
  int CSPin;

public:
  SDManager(int CSPin);

  bool begin();

  void appendFile(const char *path, const char *message, const char *defaultHeaders);

  String readFile(const char *path);

  String **readEnvironmentVariables(int &count);
};

#endif
