#include "mbed.h"
#include "rtos.h"
#include "ble/BLE.h"
#include "ble/Gap.h"
#include "SecurityManager.h"
#include "events/mbed_events.h"
#include "BLE_HID/HIDServiceBase.h"
#include "ble/services/BatteryService.h"
#include "BLE_HID/BLEDeviceInformationService.h"
#include "BLEDevice.h"

/**
 * This program implements a complete HID-over-Gatt Profile:
 *  - HID is provided by MouseService
 *  - Battery Service
 *  - Device Information Service
 *
 * Complete strings can be sent over BLE using printf. Please note, however, than a 12char string
 * will take about 500ms to transmit, principally because of the limited notification rate in BLE.
 * KeyboardService uses a circular buffer to store the strings to send, and calls to putc will fail
 * once this buffer is full. This will result in partial strings being sent to the client.
 */
 
rtos::Thread t;

typedef ble_error_t (Gap::*disconnect_call_t)(ble::connection_handle_t, ble::local_disconnection_reason_t);
const static disconnect_call_t disconnect_call = &Gap::disconnect;

void BLEDevice::schedule_ble_events(BLE::OnEventsToProcessCallbackContext *context) {
    _event_queue.call(mbed::Callback<void()>(&context->ble, &BLE::processEvents));
}

BLEDevice::BLEDevice(BLEKeyboard keyboard, BLE &ble): 
    _battery_level(50),
    device_name("Example"),
    manufacturersName("ARM"),
    _ble(ble),
    _event_queue(event_queue),
    _battery_uuid(GattService::UUID_BATTERY_SERVICE),
    _battery_service(_ble, _battery_level),
    _handle(0),
    hid_device(1),
    _adv_data_builder(_adv_buffer),
     ifconnected(false){
     }
BLEDevice::BLEDevice(BLEMouse mouse, BLE &ble):
    _battery_level(50),
    device_name("Example"),
    manufacturersName("ARM"),
    _ble(ble),
    _event_queue(event_queue),
    _battery_uuid(GattService::UUID_BATTERY_SERVICE),
    _battery_service(_ble, _battery_level),
    _handle(0),
    hid_device(2),
    _adv_data_builder(_adv_buffer),
     ifconnected(false)
{}

    void BLEDevice::start(){
        _ble.onEventsToProcess(
            makeFunctionPointer(this, &BLEDevice::schedule_ble_events)
        );

        _ble.securityManager().setSecurityManagerEventHandler(this);
        _ble.gap().setEventHandler(this);
        _ble.init(this, &BLEDevice::on_init_complete);
		t.start(mbed::callback(&_event_queue, &events::EventQueue::dispatch_forever));

    }
	
 
    bool BLEDevice::isConnected(){
        return ifconnected;
    }
    

    void BLEDevice::setManufacturerName(const char *manufacturersName2){
        manufacturersName = manufacturersName2;
    }

    void BLEDevice::setDeviceName(const char *device_name2){
        device_name = device_name2;
    }

    void BLEDevice::setBatteryLevel(uint8_t _battery_level){
        _battery_service.updateBatteryLevel(_battery_level);
    }

    void BLEDevice::on_init_complete(BLE::InitializationCompleteCallbackContext *params){
        if (params->error != BLE_ERROR_NONE) {
            return;
        }

        ble_error_t error;
        /* If the security manager is required this needs to be called before any
         * calls to the Security manager happen. */
        error = _ble.securityManager().init(
            true,
            false,
            SecurityManager::IO_CAPS_NONE,
            NULL,
            false,
            NULL
        );

        if (error) {
            //printf("Error during init %d\r\n", error);
            return;
        }

        error = _ble.securityManager().preserveBondingStateOnReset(true);

        if (error) {
            //printf("Error during preserveBondingStateOnReset %d\r\n", error);
        }


        start_advertising();
    }

    void setManufacturerInfo(BLE &ble, const char *manufacturersName)
{

    PnPID_t pnpID;
    pnpID.vendorID_source = 0x2; // from the USB Implementer's Forum
    pnpID.vendorID = 0x0D28; // NXP
    pnpID.productID = 0x0204; // CMSIS-DAP (well, it's a keyboard but oh well)
    pnpID.productVersion = 0x0100; // v1.0
    BLEDeviceInformationService deviceInfo(ble, manufacturersName, "m1", "abc", "def", "ghi", "jkl", &pnpID);

}

    void BLEDevice::start_advertising(){

        ble::AdvertisingParameters adv_parameters(
            ble::advertising_type_t::CONNECTABLE_UNDIRECTED,
            ble::adv_interval_t(ble::millisecond_t(100))
        );
        adv_parameters.setTxPower(ble::advertising_power_t(-10));

        setManufacturerInfo(_ble, manufacturersName);

        _adv_data_builder.setFlags();
        _adv_data_builder.setName(device_name);
        if(hid_device == 1){
        _adv_data_builder.setAppearance(ble::adv_data_appearance_t::KEYBOARD);
        }
        else if(hid_device == 2){
        _adv_data_builder.setAppearance(ble::adv_data_appearance_t::MOUSE);
        }
        
      _adv_data_builder.setLocalServiceList(mbed::make_Span(&_battery_uuid, 1));
        
    

        ble_error_t error = _ble.gap().setAdvertisingParameters(
            GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED,
            adv_parameters
        );

        if (error) {
            // print_error(error, "_ble.gap().setAdvertisingParameters() failed");
            return;
        }

        error = _ble.gap().setAdvertisingPayload(
            ble::LEGACY_ADVERTISING_HANDLE,
            _adv_data_builder.getAdvertisingData()
        );

        if (error) {
            //print_error(error, "_ble.gap().setAdvertisingPayload() failed");
            return;
        }

        /* Start advertising */

        error = _ble.gap().startAdvertising(ble::LEGACY_ADVERTISING_HANDLE);

        if (error) {
            //print_error(error, "_ble.gap().startAdvertising() failed");
            return;
        }

        _ble.securityManager().setPairingRequestAuthorisation(false);
    }


    void BLEDevice::onDisconnectionComplete(const ble::DisconnectionCompleteEvent&) {
        ifconnected = false;
        _ble.gap().startAdvertising(ble::LEGACY_ADVERTISING_HANDLE);
		_ble.securityManager().setPairingRequestAuthorisation(false);
    }

    void BLEDevice::onConnectionComplete(const ble::ConnectionCompleteEvent &event) {
        ifconnected = true;

        ble_error_t error;

         _handle = event.getConnectionHandle();

              error = _ble.securityManager().setLinkSecurity(
            _handle,
            SecurityManager::SECURITY_MODE_ENCRYPTION_NO_MITM
        );

        if (error) {
            //printf("Error during SM::setLinkSecurity %d\r\n", error);
            return;
        }

    }



     void BLEDevice::pairingRequest(
        ble::connection_handle_t connectionHandle
    ) {
        //printf("Pairing requested - authorising\r\n");
        _ble.securityManager().acceptPairingRequest(connectionHandle);
    }

        void BLEDevice::pairingResult(
        ble::connection_handle_t connectionHandle,
        SecurityManager::SecurityCompletionStatus_t result
    ) {
        if (result == SecurityManager::SEC_STATUS_SUCCESS) {
            //printf("Pairing successful\r\n");
        } else {
            //printf("Pairing failed\r\n");
        }
    }


void BLEDevice::begin()
{
    BLEDevice::start();
}

