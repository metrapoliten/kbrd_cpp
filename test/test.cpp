#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "ChipException.h"
#include "MockHidApi.hh"
#include "impl/ControllerImpl.h"
#include "impl/ModelImpl.h"
#include "impl/ViewImpl.h"
#include "interfaces/IController.h"
#include "interfaces/IModel.h"
#include "operator_uc.h"

using testing::_;
using testing::AtLeast;
using testing::Return;
using ::testing::StrictMock;

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
  MOCK_METHOD(hid_device *, getChipHandler, (), (const, override));
  MOCK_METHOD(uint8_t, getBrightness, (), (override));
  MOCK_METHOD(Color, getRGB, (), (override));
};

class MockController : public IController {
public:
  MOCK_METHOD(void, setBrightness, (uint16_t lvl), (const, override));
  MOCK_METHOD(void, setColor, (Color color), (const, override));
};

class ViewImplTest : public testing::Test {
protected:
  void SetUp() override {
    _origCinBuf = std::cin.rdbuf();
    _origCoutBuf = std::cout.rdbuf();
    std::cin.rdbuf(_cin.rdbuf());
    std::cout.rdbuf(_cout.rdbuf());

    _mockModel = std::make_shared<StrictMock<MockModel>>();
    _mockController = std::make_shared<StrictMock<MockController>>();
    _view = std::make_unique<ViewImpl>(_mockController, _mockModel);
  }

  void TearDown() override {
    std::cin.rdbuf(_origCinBuf);
    std::cout.rdbuf(_origCoutBuf);
  }

  std::shared_ptr<MockModel> _mockModel;
  std::shared_ptr<MockController> _mockController;
  std::unique_ptr<ViewImpl> _view;

  std::stringstream _cin;
  std::stringstream _cout;
  std::streambuf *_origCinBuf;
  std::streambuf *_origCoutBuf;
};

TEST_F(ControllerImplTest, setBrightnessSendsCorrectData) {
  auto const model{std::make_shared<MockModel>()};
  ControllerImpl const controller(model);
  auto const dummy_dev = reinterpret_cast<hid_device *>(0x1);
  constexpr auto brightness{50};
  EXPECT_CALL(*model, getChipHandler()).WillRepeatedly(Return(dummy_dev));

  EXPECT_CALL(mock, hid_send_feature_report(dummy_dev, _, 9))
      .WillOnce([brightness](hid_device *, const unsigned char *data, size_t) {
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

  EXPECT_THROW({ ModelImpl model; }, ChipException);
}

TEST_F(ModelImplTest, ConstructorThrowsOnFailedInit) {
  EXPECT_CALL(mock, hid_init()).WillOnce(Return(-1));
  EXPECT_CALL(mock, hid_error(nullptr)).Times(1);

  EXPECT_THROW({ ModelImpl model; }, ChipException);
}

TEST_F(ModelImplTest, GetBrightnessReturnsMockedValue) {
  constexpr auto brightness{85};
  auto const dev = reinterpret_cast<hid_device *>(0x1);

  EXPECT_CALL(mock, hid_init()).Times(1);

  EXPECT_CALL(mock, hid_open(_, _, _)).WillOnce(Return(dev));
  EXPECT_CALL(mock, hid_send_feature_report(dev, _, 9))
      .WillRepeatedly(
          [](hid_device *, const unsigned char *, size_t) { return 9; });
  EXPECT_CALL(mock, hid_get_feature_report(dev, _, 9))
      .Times(AtLeast(2))
      .WillRepeatedly([](hid_device *, unsigned char *data, size_t) {
        data[5] = brightness;
        return 9;
      });
  ModelImpl model;
  EXPECT_EQ(model.getBrightness(), brightness);
}

TEST_F(ModelImplTest, GetRGBReturnsMockedValues) {
  constexpr uint8_t r{64};
  constexpr uint8_t g{128};
  constexpr uint8_t b{255};

  auto const dev{reinterpret_cast<hid_device *>(0x1)};
  EXPECT_CALL(mock, hid_init()).Times(1);
  EXPECT_CALL(mock, hid_open(_, _, _)).WillOnce(Return(dev));
  EXPECT_CALL(mock, hid_send_feature_report(dev, _, 9))
      .WillRepeatedly(
          [](hid_device *, const unsigned char *, size_t) { return 9; });
  EXPECT_CALL(mock, hid_get_feature_report(dev, _, 9))
      .Times(AtLeast(2))
      .WillRepeatedly([](hid_device *, unsigned char *data, size_t) {
        data[4] = r;
        data[5] = g;
        data[6] = b;
        return 9;
      });

  ModelImpl model;
  auto [res_r, res_g, res_b] = model.getRGB();

  EXPECT_EQ(res_r, r);
  EXPECT_EQ(res_g, g);
  EXPECT_EQ(res_b, b);
}

TEST(UtilTest, UnsignedCharLiteral) {
  constexpr auto val = 0x02_uc;
  EXPECT_EQ(val, 2);
}

TEST(UtilTest, ExceptionMessage) {
  ChipException const ex("Test error");
  EXPECT_STREQ(ex.what(), "Test error");
}

TEST(ColorTest, StructStorage) {
  constexpr Color c{1, 2, 3};
  EXPECT_EQ(c.R, 1);
  EXPECT_EQ(c.G, 2);
  EXPECT_EQ(c.B, 3);
}

TEST_F(ViewImplTest, ShowCurrentSettings) {
  EXPECT_CALL(*_mockModel, getBrightness()).WillOnce(Return(80));
  EXPECT_CALL(*_mockModel, getRGB()).WillOnce(Return(Color{10, 20, 30}));

  _view->showCurrentSettings();

  const std::string output = _cout.str();
  std::cout << output << std::endl;
  EXPECT_NE(output.find("Brightness: 80%"), std::string::npos);
  EXPECT_NE(output.find("RGB: 10 20 30"), std::string::npos)
      << output << std::endl;
}

TEST_F(ViewImplTest, RunMenuChangeBrightnessValid) {
  _cin << "b\n75\nq\n";
  EXPECT_CALL(*_mockController, setBrightness(75)).Times(1);
  _view->runMenu();
}

TEST_F(ViewImplTest, RunMenuChangeBrightnessInvalidNumber) {
  _cin << "b\nabc\nq\n";
  // No controller call expected
  EXPECT_CALL(*_mockController, setBrightness(_)).Times(0);
  _view->runMenu();
  std::string output = _cout.str();
  EXPECT_NE(output.find("INVALID ARGUMENT"), std::string::npos);
}

TEST_F(ViewImplTest, RunMenuChangeBrightnessOutOfRange) {
  _cin << "b\n200\nq\n";
  EXPECT_CALL(*_mockController, setBrightness(_)).Times(0);
  _view->runMenu();
  std::string output = _cout.str();
  EXPECT_NE(output.find("LEVEL OF BRIGHTNESS MUST BE FROM 0 TO 100"),
            std::string::npos);
}

TEST_F(ViewImplTest, RunMenuChangeColorInvalidTokens) {
  _cin << "m\n10.20\nq\n"; // only two tokens
  EXPECT_CALL(*_mockController, setColor(_)).Times(0);
  _view->runMenu();
  std::string output = _cout.str();
  EXPECT_NE(output.find("UNABLE TO GET THREE COLORS"), std::string::npos);
}

TEST_F(ViewImplTest, RunMenuChangeColorNonNumeric) {
  _cin << "m\n10.abc.30\nq\n";
  EXPECT_CALL(*_mockController, setColor(_)).Times(0);
  _view->runMenu();
  auto output = _cout.str();
  EXPECT_NE(output.find("INVALID ARGUMENT"), std::string::npos);
}

TEST_F(ViewImplTest, RunMenuChangeColorOutOfRange) {
  _cin << "m\n300.20.30\nq\n";
  EXPECT_CALL(*_mockController, setColor(_)).Times(0);
  _view->runMenu();
  auto output = _cout.str();
  EXPECT_NE(output.find("PRIMARY COLOR VALUE MUST BE FROM 0 TO 255"),
            std::string::npos);
}

TEST_F(ViewImplTest, RunMenuShowCurrentSettings) {
  _cin << "c\nq\n";
  EXPECT_CALL(*_mockModel, getBrightness()).WillOnce(Return(50));
  EXPECT_CALL(*_mockModel, getRGB()).WillOnce(Return(Color{1, 2, 3}));
  _view->runMenu();
  auto output = _cout.str();
  EXPECT_NE(output.find("Brightness: 50%"), std::string::npos);
  EXPECT_NE(output.find("RGB: 1 2 3"), std::string::npos);
}

TEST_F(ViewImplTest, RunMenuInvalidOption) {
  _cin << "x\nq\n";
  _view->runMenu();
  std::string output = _cout.str();
  EXPECT_NE(output.find("WRONG OPTION"), std::string::npos);
}

TEST_F(ViewImplTest, ClearCinThrowsOnEof) {
  _cin.setstate(std::ios::eofbit);
  EXPECT_THROW(_view->runMenu(), std::runtime_error);
}