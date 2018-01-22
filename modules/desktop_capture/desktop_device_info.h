/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this file,
* You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef WEBRTC_MODULES_DESKTOP_CAPTURE_DEVICE_INFO_H_
#define WEBRTC_MODULES_DESKTOP_CAPTURE_DEVICE_INFO_H_

#include <map>
#include "modules/desktop_capture/desktop_capture_types.h"

namespace webrtc {

class DesktopDisplayDevice {
public:
  DesktopDisplayDevice();
  ~DesktopDisplayDevice();

  void setScreenId(const ScreenId screenId);
  void setDeviceName(const char *deviceNameUTF8);
  void setUniqueIdName(const char *deviceUniqueIdUTF8);
  void setPid(pid_t pid);

  ScreenId getScreenId();
  const char *getDeviceName();
  const char *getUniqueIdName();
  pid_t getPid();

  DesktopDisplayDevice& operator= (DesktopDisplayDevice& other);

protected:
  ScreenId screenId_;
  char* deviceNameUTF8_;
  char* deviceUniqueIdUTF8_;
  pid_t pid_;
};

typedef std::map<intptr_t,DesktopDisplayDevice*> DesktopDisplayDeviceList;

class DesktopDeviceInfo {
public:
  virtual ~DesktopDeviceInfo() {};

  virtual int32_t Init() = 0;
  virtual int32_t Refresh() = 0;
  virtual int32_t getDisplayDeviceCount() = 0;
  virtual int32_t getDesktopDisplayDeviceInfo(int32_t nIndex,
                                              DesktopDisplayDevice & desktopDisplayDevice) = 0;
  virtual int32_t getWindowCount() = 0;
  virtual int32_t getWindowInfo(int32_t nindex,
                                DesktopDisplayDevice &windowDevice) = 0;
};

class DesktopDeviceInfoImpl : public DesktopDeviceInfo {
public:
  DesktopDeviceInfoImpl();
  ~DesktopDeviceInfoImpl();

  virtual int32_t Init();
  virtual int32_t Refresh();
  virtual int32_t getDisplayDeviceCount();
  virtual int32_t getDesktopDisplayDeviceInfo(int32_t nIndex,
                                              DesktopDisplayDevice & desktopDisplayDevice);
  virtual int32_t getWindowCount();
  virtual int32_t getWindowInfo(int32_t nindex,
                                DesktopDisplayDevice &windowDevice);
  static DesktopDeviceInfo * Create();
protected:
  DesktopDisplayDeviceList desktop_display_list_;
  DesktopDisplayDeviceList desktop_window_list_;

  void CleanUp();
  void CleanUpWindowList();
  void CleanUpApplicationList();
  void CleanUpScreenList();

  void InitializeWindowList();
  virtual void InitializeScreenList() = 0;

  void RefreshWindowList();
  void RefreshScreenList();

};
};

#endif
