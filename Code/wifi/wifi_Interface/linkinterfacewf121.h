/*
 * linkinterfacewf121.h
 *
 *  Created on: 31.08.2015
 *      Author: tmikschl
 */

#ifndef SRC_LINKINTERFACEWF121_H_
#define SRC_LINKINTERFACEWF121_H_

#include <gateway/linkinterface.h>
#include <hal/genericIO.h>
#include "wf121.h"

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

class LinkinterfaceWF121: public Linkinterface,IOEventReceiver {
private:
	WF121 *wf121;
	int checkedLen;

public:
	LinkinterfaceWF121(WF121 *_wf121);
	void onDataReady();
	void onWriteFinished();
	void init();

	bool sendNetworkMsg(NetworkMessage &outMsg);
	bool getNetworkMsg(NetworkMessage &inMsg,int32_t &numberOfReceivedBytes);
};

#ifndef NO_RODOS_NAMESPACE
}
#endif

#endif /* SRC_LINKINTERFACEWF121_H_ */
