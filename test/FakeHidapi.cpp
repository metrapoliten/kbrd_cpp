#include <hidapi.h>

#include "MockHidApi.hh"

MockHidApi* mockHid = nullptr;

extern "C" {
  int hid_init() { return mockHid ? mockHid->hid_init() : -1; }
  hid_device* hid_open(unsigned short vendor_id, unsigned short product_id, const wchar_t* serial_number) {
    return mockHid ? mockHid->hid_open(vendor_id, product_id, serial_number) : nullptr;
  }
  int hid_send_feature_report(hid_device* dev, const unsigned char* data, size_t length) {
    return mockHid ? mockHid->hid_send_feature_report(dev, data, length) : -1;
  }
  int hid_get_feature_report(hid_device* dev, unsigned char* data, size_t length) {
    return mockHid ? mockHid->hid_get_feature_report(dev, data, length) : -1;
  }
  const wchar_t* hid_error(hid_device* dev) {
    return mockHid ? mockHid->hid_error(dev) : L"Mock Error";
  }
  void hid_close([[maybe_unused]]hid_device* dev) {}
  int hid_exit() { return mockHid ? mockHid->hid_exit() : -1; }
}

