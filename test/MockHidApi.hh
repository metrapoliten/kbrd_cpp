#pragma once
#include <gmock/gmock.h>
#include <hidapi.h>

class MockHidApi {
public:
  MOCK_METHOD(int, hid_init, ());
  MOCK_METHOD(int, hid_exit, ());
  MOCK_METHOD(hid_device *, hid_open,
              (unsigned short vendor_id, unsigned short product_id,
               const wchar_t *serial_number));
  MOCK_METHOD(void, hid_close, (hid_device * dev));
  MOCK_METHOD(int, hid_send_feature_report,
              (hid_device * dev, const unsigned char *data, size_t length));
  MOCK_METHOD(int, hid_get_feature_report,
              (hid_device * dev, unsigned char *data, size_t length));
  MOCK_METHOD(const wchar_t *, hid_error, (hid_device * dev));
};

extern MockHidApi *mockHid;
