// E78LoraWan_Module.h

#ifndef _E78LORAWAN_MODULE_h
#define _E78LORAWAN_MODULE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
#endif

enum TX_RETURN_TYPE {
	TX_FAIL = 0,    // The transmission failed.
					// If you sent a confirmed message and it is not acked,
					// this will be the returned value.

	TX_SUCCESS = 1, // The transmission was successful.
					// Also the case when a confirmed message was acked.

	TX_WITH_RX = 2  // A downlink message was received after the transmission.
					// This also implies that a confirmed message is acked.
};

class E78LoraWan_module {
public:

	/*
	 * A simplified constructor taking only a Stream ({Software/Hardware}Serial) object.
	* The serial port should already be initialised when initialising this library.
	 */
	E78LoraWan_module(Stream& serial);

	/*
	 * Returns the AppSKey or AppKey used when initializing the radio.
	 * In the case of ABP this function will return the App Session Key.
	 * In the case of OTAA this function will return the App Key.
	 */
	String getAppkey();

	/*
	 * In the case of OTAA this function will return the Application EUI used
	 * to initialize the radio.
	 */
	String getAppeui();

	/*
	 * In the case of OTAA this function will return the Device EUI used to
	 * initialize the radio. .
	 */
	String getDeveui();

	/*
	 * Get the E78's hardware and firmware version number. 
	 */
	String sysver();

	/*
	* Initialise the RE78 and join a network using over the air activation.
	*
	* AppEUI: Application EUI as a HEX string.
	*         Example "70B3D57ED00001A6"
	* AppKey: Application key as a HEX string.
	*         Example "A23C96EE13804963F8C2BD6285448198"
	* DevEUI: Device EUI as a HEX string.
	*         Example "0011223344556677"
	* If the DevEUI parameter is omitted, the Hardware EUI from module will be used
	* If no keys, or invalid length keys, are provided, no keys
	* will be configured. If the module is already configured with some keys
	* they will be used. Otherwise the join will fail and this function
	* will return false.
	*/
	bool initOTAA(String AppEUI = "", String AppKey = "", String DevEUI = "");


	/*
	* Initialise the RN2xx3 and join a network using personalization.
	*
    * addr: The device address as a HEX string.
    *       Example "0203FFEE"
    * AppSKey: Application Session Key as a HEX string.
    *          Example "8D7FFEF938589D95AAD928C2E2E7E48F"
    * NwkSKey: Network Session Key as a HEX string.
    *          Example "AE17E567AECC8787F749A62F5541D522"
    */

	//bool initABP(String addr, String AppSKey, String NwkSKey);

	/*
	* Change the datarate at which the E78 transmits.
	* A value of between 0 and 5 can be specified,
	* 0 coresponding to a SF of 12 and 5 corresponding to 7
	* This can be overwritten by the network when using OTAA.
	* So to force a datarate, call this function after initOTAA().
	*/
	void setDR(int dr);

	/*
	* Get the data rate used by the transmissions
	* 0 coresponding to a SF of 12 and 5 corresponding to 7
	*/
	int GetDR();

	/*
	* Get the Module SNR of the last received packet. Helpful to debug link quality.
	*/
	int getSNR();

	/*
	* Get the transmit power the E78 will use to transmit messages.
	* use an integer between 0 and 7 , 0 corresponding to 17dBm and 7 to 3dBm.
	*Decrements of 2 are used.
	*/
	bool setTXpower(int power);

	/*
	* Returns the last downlink message HEX string.
	*/
	String getRx();



	/*
	* Encode an ASCII string to a HEX string as needed when passed
	* to the RN2xx3 module.
	*/
	String base16encode(String);

	/*
	* Decode a HEX string to an ASCII string. Useful to decode a
	* string received from the RN2xx3.
	*/
	String base16decode(String);

	/*
    * Send a raw command to the E78 module.
    * Returns the raw string as received back from the E78.
    * If the E78 replies with multiple line, only the first line will be returned.
    */
	String sendRawCommand(String command);

	/*
	* Transmit the provided data. The data is hex-encoded by this library,
	* so plain text can be provided.
	* This function is an alias for txUncnf().
	*
	* Parameter is an ascii text string.
	*/
	TX_RETURN_TYPE tx(String);

	/*
	* Transmit raw byte encoded data via LoRa WAN.
	* This method expects a raw byte array as first parameter.
	* The second parameter is the count of the bytes to send.
	*/
	TX_RETURN_TYPE txBytes(const byte*, uint8_t);

	/*
	* Do a confirmed transmission via LoRa WAN.
	*
	* Parameter is an ascii text string.
	*/
	TX_RETURN_TYPE txCnf(String);

	/*
	 * Do an unconfirmed transmission via LoRa WAN.
	 *
	 * Parameter is an ascii text string.
	 */
	TX_RETURN_TYPE txUncnf(String);

	/*
	 * Transmit the provided data using the provided command.
	 *
	 * String - the tx command to send
				can only be one of "mac tx cnf 1 " or "mac tx uncnf 1 "
	 * String - an ascii text string if bool is true. A HEX string if bool is false.
	 * bool - should the data string be hex encoded or not
	 */
	TX_RETURN_TYPE txCommand(String, String, bool);

private:

	Stream& _serial;

	//The default address to use on TTN if no address is defined.
	//This one falls in the "testing" address space.
	String _devAddr = "03FFBEEF";

	// if you want to use another DevEUI than the hardware one
	// use this deveui for LoRa WAN
	String _deveui = "0011223344556677";

	//the appeui to use for LoRa WAN
	String _appeui = "0";

	//the nwkskey to use for LoRa WAN
	String _nwkskey = "0";

	//the appskey/appkey to use for LoRa WAN
	String _appskey = "0";

	// The downlink messenge
	String _rxMessenge = "";

	void sendEncoded(String);

};


