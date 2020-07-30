// 
// 
// 

#include "E78LoraWan_Module.h"

String E78LoraWan_module::sendRawCommand(String command)
{
	delay(100);
	while (_serial.available())
		_serial.read();
	_serial.println(command);
	String ret = _serial.read();
	ret.trim();

	return ret;
}

E78LoraWan_module::E78LoraWan_module(Stream& serial):
_serial(serial)
{
	_serial.setTimeout(2000);
}

bool E78LoraWan_module::initOTAA(String AppEUI, String AppKey, String DevEUI)
{
	_nwkskey = "0";
	String receivedData;
			//clear serial buffer
	while (_serial.available())
		_serial.read();
	
	// If the Device EUI was given as a parameter, use it
	// otherwise use the Hardware EUI.
	if (DevEUI.length() == 16)
	{
		_deveui = DevEUI;
	}
	sendRawCommand("AT+CDEVEUI=" + _deveui);
	//TO DO , write enumeration to return multiple errors like error on deveui, ...

	// A valid length App EUI was given. Use it.
	if (AppEUI.length() == 16)
	{
		_appeui = AppEUI;
		sendRawCommand("AT+CAPPEUI=" + _appeui);
	}

	// A valid length App Key was give. Use it.
	if (AppKey.length() == 32)
	{
		_appskey = AppKey; //reuse the same variable as for ABP
		sendRawCommand("AT+CAPPKEY" + _appskey);
	}

	// Power set for the module

	// TTN does not yet support Adaptive Data Rate.
	// Using it is also only necessary in limited situations.
	// Therefore disable it by default.
	sendRawCommand("AT+CADR=0");
	_serial.setTimeout(30000);
	sendRawCommand("AT+CSAVE");

	//OTAA with maximum power
	setTXpower(0);

	//start join with OTAA
	sendRawCommand("AT+CJOINMODE=0");
	/*
	* To jpin the Lora Network some settings can be set further
	* paramerter 1:
		1 = start Join proces again in case the automatic join has allready joined the module
		0 = stop Join disable network
	* parameter 2:
		1 = Module will join automaticaly on start up
		0 = Module will not join automaticaly
	* parameter 3:
		Time in seconds between each join attempt
	* parameter 4:
		Join attempst before fail
	*/
	sendRawCommand("AT+CJOIN=1,1,10,1");

	bool joined = false;

	// Only try fourtimes to join, then return and let the user handle it.
	for (int i = 0; i < 3 && !joined; i++)
	{
		sendRawCommand("AT+CJOIN=1,1,10,1");
		receivedData = _serial.readStringUntil('N');
		if (receivedData.endsWith("CJOIN"))
		{
			joined = true;
			delay(10000);
		}
		else
		{
			delay(10000);
		}
	}
	_serial.setTimeout(2000);
	_serial.flush();
	return joined;
}


bool E78LoraWan_module::setTXpower(int power)
{
	bool succes;
	String response = "";
	if ((power >= 0) && (power <= 7))
	{
		response = sendRawCommand("AT+CTXP" + power);
		response.trim();
		if (response.equals("OK")) succes = true;
		else succes = false;
	}
	else succes = false;

	return succes;
}
