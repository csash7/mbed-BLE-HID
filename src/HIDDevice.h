#include "mbed.h"
#include "ble/BLE.h"
#include "ble/services/BatteryService.h"
#include "events/mbed_events.h"
#include "BLE_HID/mouse.h"
#include "BLE_HID/keyboard.h"

static events::EventQueue event_queue(/* event count */ 16 * EVENTS_EVENT_SIZE);


class BLEDevice : public ble::Gap::EventHandler,
                 public SecurityManager::EventHandler
                 {


public:
    BLE &_ble;
    BLEDevice(BLEKeyboard keyboard, BLE &ble = BLE::Instance());
    BLEDevice(BLEMouse mouse, BLE &ble = BLE::Instance());
    void begin();

    bool isConnected(void);
    void setManufacturerName(const char *manufacturersName2);
    void setDeviceName(const char *device_name2);
    void setBatteryLevel(uint8_t _battery_level);

private:

    void schedule_ble_events(BLE::OnEventsToProcessCallbackContext *context);
    events::EventQueue &_event_queue = event_queue;

    uint8_t _battery_level;
    const char *manufacturersName;
    const char *device_name;

    UUID _battery_uuid;
    BatteryService _battery_service;
    uint8_t hid_device;
    uint8_t _adv_buffer[ble::LEGACY_ADVERTISING_MAX_SIZE];
    ble::AdvertisingDataBuilder _adv_data_builder;
    ble::connection_handle_t _handle;
    bool ifconnected;

private:
    void start(void);
    void on_init_complete(BLE::InitializationCompleteCallbackContext *params);
    void start_advertising();
    virtual void onDisconnectionComplete(const ble::DisconnectionCompleteEvent&);
    virtual void onConnectionComplete(const ble::ConnectionCompleteEvent &event);
    virtual void pairingResult(
        ble::connection_handle_t connectionHandle,
        SecurityManager::SecurityCompletionStatus_t result
    );
    virtual void pairingRequest(
        ble::connection_handle_t connectionHandle
    );

};
