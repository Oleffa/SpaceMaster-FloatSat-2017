/*
 * linkinterfacewf121.cpp
 *
 *  Created on: 31.08.2015
 *      Author: tmikschl
 */

#include "linkinterfacewf121.h"
UDPMsg txMsg;
UDPMsg rxMsg;

LinkinterfaceWF121::LinkinterfaceWF121(WF121 *_wf121) : Linkinterface(-1){
	wf121 = _wf121;
}

void LinkinterfaceWF121::init() {
	wf121->setIoEventReceiver(this);
}

void LinkinterfaceWF121::onWriteFinished() {
    if(threadToResume) threadToResume->resume();
}

void LinkinterfaceWF121::onDataReady() {
    if(threadToResume) threadToResume->resume();
}

bool LinkinterfaceWF121::sendNetworkMsg(NetworkMessage &outMsg)	{
	txMsg.length=outMsg.numberOfBytesToSend();

	memcpy(txMsg.data,&outMsg,txMsg.length);
	wf121->write(&txMsg);

    return true;
}




bool LinkinterfaceWF121::getNetworkMsg(NetworkMessage &inMsg,int32_t &numberOfReceivedBytes) {
	if(checkedLen > 0) {
		memcpy(&inMsg,&rxMsg.data[checkedLen],rxMsg.length-checkedLen);
		if(inMsg.numberOfBytesToSend() < rxMsg.length-checkedLen) {
			numberOfReceivedBytes = inMsg.numberOfBytesToSend();
			checkedLen += inMsg.numberOfBytesToSend();
		} else {
			numberOfReceivedBytes = rxMsg.length-checkedLen;
			checkedLen = 0;
		}
		return true;
	} else if(wf121->read(&rxMsg)) {
		memcpy(&inMsg,rxMsg.data,rxMsg.length);
		if(inMsg.numberOfBytesToSend() < rxMsg.length) {
			numberOfReceivedBytes = inMsg.numberOfBytesToSend();
			checkedLen = inMsg.numberOfBytesToSend();
		} else {
			numberOfReceivedBytes = rxMsg.length;
			checkedLen = 0;
		}
		return true;
	}
	return false;
}
