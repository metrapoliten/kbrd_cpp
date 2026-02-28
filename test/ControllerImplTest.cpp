#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "impl/ControllerImpl.h"
#include "impl/ModelImpl.h"
#include "interfaces/IModel.h"
#include "interfaces/IController.h"
#include "ChipException.h"
#include "MockHidApi.hh"

using testing::_;
using testing::Return;
using testing::AtLeast;

class ControllerImplTest : public testing::Test {
  protected:
    MockHidApi mock;
    void SetUp() override { mockHid = &mock; }
    void TearDown() override { mockHid = nullptr; };
};

class ModelImplTest : public testing::Test {
protected:
  MockHidApi mock;
  void SetUp() override { mockHid = &mock; }
  void TearDown() override { mockHid = nullptr; };
};

class MockModel : public IModel {
  public:
  MOCK_METHOD(hid_device*, getChipHandler, (), (const, override));
  MOCK_METHOD(uint8_t, getBrightness, (), (override));
  MOCK_METHOD(Color, getRGB, (), (override));
};

class MockController : public IController {
public:
  MOCK_METHOD(void, setBrightness, (uint16_t lvl), (const, override));
  MOCK_METHOD(void, setColor, (Color color), (const, override));
};

TEST_F(ControllerImplTest, setBrightnessSendsCorrectData) {
  auto const model{ std::make_shared<MockModel>() };
  ControllerImpl const controller(model);
  auto const dummy_dev = reinterpret_cast<hid_device*>(0x1);
  constexpr auto brightness{50};
  EXPECT_CALL(*model, getChipHandler()).WillRepeatedly(Return(dummy_dev));

  EXPECT_CALL(mock, hid_send_feature_report(dummy_dev, _, 9))
     .WillOnce([brightness](hid_device*, const unsigned char* data, size_t) {
         EXPECT_EQ(data[0], 0x09); // kBrightnessByte
         EXPECT_EQ(data[1], 0x02); // kActionByte
         EXPECT_EQ(data[2], brightness);
         return 9;
     });

  controller.setBrightness(brightness);
}

TEST_F(ModelImplTest, throwsOnFailedDeviceOpen) {
  EXPECT_CALL(mock, hid_init()).Times(1);
  EXPECT_CALL(mock, hid_open(0x048d, 0x6006, _)).WillOnce(Return(nullptr));
  EXPECT_CALL(mock, hid_error(nullptr)).Times(1);

  EXPECT_THROW({
      ModelImpl model;
  }, ChipException);
}

TEST_F(ModelImplTest, ConstructorThrowsOnFailedInit) {
  EXPECT_CALL(mock, hid_init()).WillOnce(Return(-1));
  EXPECT_CALL(mock, hid_error(nullptr)).Times(1);

  EXPECT_THROW({
      ModelImpl model;
  }, ChipException);
}

TEST_F(ModelImplTest, GetBrightnessReturnsMockedValue) {
  constexpr auto brightness {85};
  auto const dev = reinterpret_cast<hid_device*>(0x1);

  EXPECT_CALL(mock, hid_init()).Times(1);

  EXPECT_CALL(mock, hid_open(_, _, _)).WillOnce(Return(dev));
  ON_CALL(mock, hid_send_feature_report(dev, _, 9))
  .WillByDefault([](hid_device*, const unsigned char*, size_t) {
    return 9;
  });
  EXPECT_CALL(mock, hid_get_feature_report(dev, _, 9))
    .Times(AtLeast(2))
    .WillRepeatedly([](hid_device*, unsigned char* data, size_t) {
      data[5] = brightness;
      return 9;
    });
  ModelImpl model;
  EXPECT_EQ(model.getBrightness(), brightness);
}