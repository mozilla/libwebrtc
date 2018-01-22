/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "modules/desktop_capture/desktop_device_info.h"

#include <cstddef>
#include <cstdlib>
#include <cstdio>
#include <cstring>

namespace webrtc {

static inline void SetStringMember(char **member, const char *value) {
  if (!value) {
    return;
  }

  if (*member) {
    delete [] *member;
    *member = NULL;
  }

  size_t  nBufLen = strlen(value) + 1;
  char *buffer = new char[nBufLen];
  memcpy(buffer, value, nBufLen - 1);
  buffer[nBufLen - 1] = '\0';
  *member = buffer;
}

DesktopDisplayDevice::DesktopDisplayDevice() {
  screenId_ = kInvalidScreenId;
  deviceUniqueIdUTF8_ = NULL;
  deviceNameUTF8_ = NULL;
  pid_ = 0;
}

DesktopDisplayDevice::~DesktopDisplayDevice() {
  screenId_ = kInvalidScreenId;

  if (deviceUniqueIdUTF8_){
    delete [] deviceUniqueIdUTF8_;
  }

  if (deviceNameUTF8_){
    delete [] deviceNameUTF8_;
  }

  deviceUniqueIdUTF8_ = NULL;
  deviceNameUTF8_ = NULL;
}

void DesktopDisplayDevice::setScreenId(const ScreenId screenId) {
  screenId_ = screenId;
}

void DesktopDisplayDevice::setDeviceName(const char *deviceNameUTF8) {
  SetStringMember(&deviceNameUTF8_, deviceNameUTF8);
}

void DesktopDisplayDevice::setUniqueIdName(const char *deviceUniqueIdUTF8) {
  SetStringMember(&deviceUniqueIdUTF8_, deviceUniqueIdUTF8);
}

void DesktopDisplayDevice::setPid(const int pid) {
  pid_ = pid;
}

ScreenId DesktopDisplayDevice::getScreenId() {
  return screenId_;
}

const char *DesktopDisplayDevice::getDeviceName() {
  return deviceNameUTF8_;
}

const char *DesktopDisplayDevice::getUniqueIdName() {
  return deviceUniqueIdUTF8_;
}

pid_t DesktopDisplayDevice::getPid() {
  return pid_;
}

DesktopDisplayDevice& DesktopDisplayDevice::operator= (DesktopDisplayDevice& other) {
  if (&other == this) {
    return *this;
  }
  screenId_ = other.getScreenId();
  setUniqueIdName(other.getUniqueIdName());
  setDeviceName(other.getDeviceName());
  pid_ = other.getPid();

  return *this;
}

DesktopDeviceInfoImpl::DesktopDeviceInfoImpl() {
}

DesktopDeviceInfoImpl::~DesktopDeviceInfoImpl() {
  CleanUp();
}

int32_t DesktopDeviceInfoImpl::getDisplayDeviceCount() {
  return desktop_display_list_.size();
}

int32_t DesktopDeviceInfoImpl::getDesktopDisplayDeviceInfo(int32_t nIndex,
                                                           DesktopDisplayDevice & desktopDisplayDevice) {
  if(nIndex < 0 || nIndex >= desktop_display_list_.size()) {
    return -1;
  }

  std::map<intptr_t,DesktopDisplayDevice*>::iterator iter = desktop_display_list_.begin();
  std::advance (iter, nIndex);
  DesktopDisplayDevice * pDesktopDisplayDevice = iter->second;
  if(pDesktopDisplayDevice) {
    desktopDisplayDevice = (*pDesktopDisplayDevice);
  }

  return 0;
}

int32_t DesktopDeviceInfoImpl::getWindowCount() {
  return desktop_window_list_.size();
}
int32_t DesktopDeviceInfoImpl::getWindowInfo(int32_t nIndex,
                                             DesktopDisplayDevice &windowDevice) {
  if (nIndex < 0 || nIndex >= desktop_window_list_.size()) {
    return -1;
  }

  std::map<intptr_t, DesktopDisplayDevice *>::iterator itr = desktop_window_list_.begin();
  std::advance(itr, nIndex);
  DesktopDisplayDevice * pWindow = itr->second;
  if (!pWindow) {
    return -1;
  }

  windowDevice = (*pWindow);
  return 0;
}

void DesktopDeviceInfoImpl::CleanUp() {
  CleanUpScreenList();
  CleanUpWindowList();
}

int32_t DesktopDeviceInfoImpl::Init() {
  InitializeScreenList();
  InitializeWindowList();

  return 0;
}
int32_t DesktopDeviceInfoImpl::Refresh() {
  RefreshScreenList();
  RefreshWindowList();

  return 0;
}

void DesktopDeviceInfoImpl::CleanUpWindowList() {
  std::map<intptr_t, DesktopDisplayDevice *>::iterator iterWindow;
  for (iterWindow = desktop_window_list_.begin(); iterWindow != desktop_window_list_.end(); iterWindow++) {
    DesktopDisplayDevice *pWindow = iterWindow->second;
    delete pWindow;
    iterWindow->second = NULL;
  }
  desktop_window_list_.clear();
}
void DesktopDeviceInfoImpl::InitializeWindowList() {
  std::unique_ptr<DesktopCapturer> pWinCap = DesktopCapturer::CreateWindowCapturer(DesktopCaptureOptions::CreateDefault());
  DesktopCapturer::SourceList list;
  if (pWinCap && pWinCap->GetSourceList(&list)) {
    DesktopCapturer::SourceList::iterator itr;
    for (itr = list.begin(); itr != list.end(); itr++) {
      DesktopDisplayDevice *pWinDevice = new DesktopDisplayDevice;
      if (!pWinDevice) {
        continue;
      }

      pWinDevice->setScreenId(itr->id);
      pWinDevice->setDeviceName(itr->title.c_str());
      pWinDevice->setPid(itr->pid);

      char idStr[BUFSIZ];
#if WEBRTC_WIN
      _snprintf_s(idStr, sizeof(idStr), sizeof(idStr) - 1, "%ld", pWinDevice->getScreenId());
#else
      snprintf(idStr, sizeof(idStr), "%ld", pWinDevice->getScreenId());
#endif
      pWinDevice->setUniqueIdName(idStr);
      desktop_window_list_[pWinDevice->getScreenId()] = pWinDevice;
    }
  }
}
void DesktopDeviceInfoImpl::RefreshWindowList() {
  CleanUpWindowList();
  InitializeWindowList();
}

void DesktopDeviceInfoImpl::CleanUpScreenList() {
  std::map<intptr_t,DesktopDisplayDevice*>::iterator iterDevice;
  for (iterDevice=desktop_display_list_.begin(); iterDevice != desktop_display_list_.end(); iterDevice++){
    DesktopDisplayDevice *pDesktopDisplayDevice = iterDevice->second;
    delete pDesktopDisplayDevice;
    iterDevice->second = NULL;
  }
  desktop_display_list_.clear();
 }
void DesktopDeviceInfoImpl::RefreshScreenList() {
  CleanUpScreenList();
  InitializeScreenList();
}
}
