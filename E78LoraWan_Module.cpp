// 
// 
// 

#include "E78LoraWan_Module.h"

bool  E78LoraWan_module::wakeUp(){
    bool succes = 0;
    sendRawCommand("000000000D0A");
    succes = sendRawCommandConfirmed("AT+CLPM=0");
    return succes;  
}

bool E78LoraWan_module::enterDeepSleep(){
    bool succes = 0;
    succes = sendRawCommandConfirmed("AT+CLPM=1");
    return succes;  
}


String E78LoraWan_module::base16encode(String input)
{
	char charsOut[input.length() * 2 + 1];
	char charsIn[input.length() + 1];
	input.trim();
	input.toCharArray(charsIn, input.length() + 1);
	unsigned i = 0;
	for (i = 0; i < input.length() + 1; i++)
	{
		if (charsIn[i] == '\0') break;
		int value = int(charsIn[i]);
		char buffer[3];
		sprintf(buffer, "%02x", value);
		charsOut[2 * i] = buffer[0];
		charsOut[2 * i + 1] = buffer[1];
	}
	charsOut[2 * i] = '\0';
	String toReturn = String(charsOut);
	return toReturn;
}

String E78LoraWan_module::base16decode(String input)
{
	char charsIn[input.length() + 1];
	char charsOut[input.length() / 2 + 1];
	input.trim();
	input.toCharArray(charsIn, input.length() + 1);

	unsigned i = 0;
	for (i = 0; i < input.length() / 2 + 1; i++)
	{
		if (charsIn[i * 2] == '\0') break;
		if (charsIn[i * 2 + 1] == '\0') break;

		char toDo[2];
		toDo[0] = charsIn[i * 2];
		toDo[1] = charsIn[i * 2 + 1];
		int out = strtoul(toDo, 0, 16);

		if (out < 128)
		{
			charsOut[i] = char(out);
		}
	}
	charsOut[i] = '\0';
	return charsOut;
}

int E78LoraWan_module::txCnf(String data)
{
	return txCommand(data, true, true);
}

int E78LoraWan_module::txUncnf(String data)
{
	return txCommand(data, false, true);
}

int E78LoraWan_module::txCommand(String data, bool confirmed, bool shouldEncode)
{
	int testcounter;
	String temp = "";
  int confirm = 0;
	int Type = 0;
	String Link = "";
	int length;
	if (confirmed) {
   confirm = 1;
	}
	else {
		confirm = 0;
	}

	if (shouldEncode) {
		data = base16encode(data);
	}
	else data = data;
	length = data.length();
	temp = "AT+DTRX=" + String(confirm) + "," + String(_TXretrails) + "," + String(length) + "," + String(data);
	temp = sendRawCommand(temp);
  temp.trim();
  _returnstring = temp;
	if (temp.startsWith("ERR")) return TX_FAIL;
	else {
      while (_serial.available())
    _returnstring = _serial.read();
		if (temp.substring(14, 21).startsWith("ERR")) return TX_FAIL;
		else {
		return TX_SUCCESS;
		}
	}
}

String E78LoraWan_module::sendRawCommand(String command)
{
	delay(100);
	while (_serial.available())
		_serial.read();
	_serial.println(command);
	delay(50); 
	String ret = "";
	while (_serial.available())  ret=_serial.readString();
	return ret;
}

bool E78LoraWan_module::sendRawCommandConfirmed(String command)
{
	bool succes;
	delay(100);
	while (_serial.available())
		_serial.read();
	_serial.println(command);
	delay(50);
	String ret = "";
	while (_serial.available())  ret = _serial.readString();
	ret.trim();
	if (ret.equals("OK")) succes = true;
	else succes = false;
	return succes;
}

bool E78LoraWan_module::SetTxRetrails(int TXretrails)
{
	_TXretrails = TXretrails;
}

int E78LoraWan_module::GetTxRetrails()
{
	return _TXretrails;
}

int E78LoraWan_module::tx(String data)
{
	return txUncnf(data);
}

int E78LoraWan_module::txBytes(const byte * data, uint8_t size , bool confirm)
{
	char msgBuffer[size * 2 + 1];

	char buffer[3];
	for (unsigned i = 0; i < size; i++)
	{
		sprintf(buffer, "%02X", data[i]);
		memcpy(&msgBuffer[i], &buffer, sizeof(buffer));
	}
	String dataToTx(msgBuffer);
  if(confirm) return txCommand(dataToTx,true,false);
  else return txCommand(dataToTx,false,false);
}

E78LoraWan_module::E78LoraWan_module(Stream& serial):
_serial(serial)
{
	_serial.setTimeout(3000);
	
}

String E78LoraWan_module::sysver()
{
	String temp;
	temp = sendRawCommand("AT+CGMR?");
	return temp;
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
		sendRawCommand("AT+CAPPKEY=" + _appskey);
	}

	// Power set for the module

	// TTN does not yet support Adaptive Data Rate.
	// Using it is also only necessary in limited situations.
	// Therefore disable it by default.
	sendRawCommand("AT+CADR=0");

	// set send retrails to 5 (default)
	SetTxRetrails(_TXretrails);

	sendRawCommand("AT+CSAVE");

	//OTAA with maximum power
	setTXpower(0);

	//start join with OTAA
	sendRawCommand("AT+CJOINMODE=0");
	/*
	* To join the Lora Network some settings can be set further
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
	for (int i = 0; i <= 3 && !joined; i++)
	{
		receivedData = sendRawCommand("AT+CJOIN=1,1,10,1");
    //Serial.println(receivedData);
		int index=receivedData.indexOf("+");
    receivedData=receivedData.substring(index,receivedData.length());
		if (receivedData.startsWith("+CJOIN"))
		{
			joined = true;
			delay(25000);
     //Serial.println("test");
		}
		else
		{
			delay(25000);
		}
	}
	_serial.setTimeout(2000);
	_serial.flush();
	return joined;
}


bool E78LoraWan_module::setDR(int SF)
{
	bool succes;
	String temp = "";
	if ((SF >= 0) && (SF <= 5))
	{
		temp = "AT+CDATARATE=" + String(SF);
		// send the At command for power
		succes = sendRawCommandConfirmed(temp);
		//demand response
	}
	else succes = false;
	return succes;
}

int E78LoraWan_module::GetDR()
{
	String temp = "";
	int datarate = -1;
	temp = sendRawCommand("AT+CDATARATE?");
	temp.trim();
	if (temp.endsWith("OK")) {
		temp = temp.substring(11, 12);
		datarate = temp.toInt();
	}
	else datarate = -1;
	return datarate;
}


bool E78LoraWan_module::setTXpower(int power)
{
	bool succes;
	String temp = "";
	if ((power >= 0) && (power <= 7))
	{
		temp = "AT+CTXP=" + String(power);
		// send the At command for power
		succes = sendRawCommandConfirmed(temp);
	}
	else succes = false;

	return succes;
}

String E78LoraWan_module::getRx()
{
  
	String temp = "";
	temp = sendRawCommand("AT+DRX?");
  temp.trim();
		//get the length of the payload out of the AT command
    if(temp.startsWith("+DRX:")){
      int index = temp.indexOf(",",4);
      if(index >= 0){
			temp=temp.substring(index + 1, temp.length());
      temp=base16decode(temp);
 //     Serial.println("RX:"+temp);
		  return temp;
      }
      else return "";
    }
    else return "";
}


