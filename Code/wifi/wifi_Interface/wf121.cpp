/*
 * wf121.cpp
 *
 *  Created on: 27.08.2015
 *      Author: tmikschl
 */

#include "wf121.h"


// Define for Debug Messages
//#define WIFI_DEBUG
//#define UART_BAUDRATE 2000000
#define UART_BAUDRATE 115200


HAL_UART *wifi_uart;
Semaphore sem_wlan_state;

BGLIB_DEFINE();


char buffer[BGLIB_MSG_MAXLEN];

UDPMsg wf121rxMsg;


WF121::RecvThread::RecvThread(WF121 *_wf121, HAL_UART *_uart) : Thread("WF121 Recv Thread",1003) {
	wf121 = _wf121;
	uart =_uart;
}

void WF121::RecvThread::run() {
	this->suspendCallerUntil(END_OF_TIME);
	while(true) {

		uart->suspendUntilDataReady();
		if(wf121->internal_state == wlan_state_inactive) {
			this->suspendCallerUntil(END_OF_TIME);
		}
		if(getMsg(buffer, BGLIB_MSG_HEADER_LEN) < 0) {
			#ifdef WIFI_DEBUG
				PRINTF("Wifi - Sync Error?\n\r");
			#endif
			while(uart->isDataReady())uart->getcharNoWait();
			if(wf121->internal_state !=  wlan_state_ready) {
				wf121->internal_state = wlan_state_hello;
				wifi_cmd_system_hello();
			}
			continue;
		}
		if(	getMsg(&(buffer[BGLIB_MSG_HEADER_LEN]),BGLIB_MSG_LEN(buffer)) <0) {
			#ifdef WIFI_DEBUG
				PRINTF("Wifi - Sync Error?\n\r");
			#endif
			while(uart->isDataReady())uart->getcharNoWait();
			if(wf121->internal_state !=  wlan_state_ready) {
				wf121->internal_state = wlan_state_hello;
				wifi_cmd_system_hello();
			}
			continue;
		}
		PROTECT_WITH_SEMAPHORE(sem_wlan_state) {
			wf121->processMsg();
		}
	}
}

int WF121::RecvThread::getMsg(char *buf, int len) {

	//long long time;
	for(int i=0;i<len;i++) {
		//time = NOW();
		//while(!uart->isDataReady() && NOW() < time + 10 * MILLISECONDS);
		uart->suspendUntilDataReady(NOW() + 10* MILLISECONDS);
		int c;
		c=uart->getcharNoWait();
		if(c<0) {
			return -1;
		}
		buf[i]=(char)c;
	}
	return len;
}

void wifiOutput(uint8 len1,uint8* data1,uint16 len2,uint8* data2) {
	int i=0;
	for(i=0;i<len1;i++){
		while(wifi_uart->putcharNoWait(data1[i])<0);
	}
	for(i=0;i<len2;i++){
		while(wifi_uart->putcharNoWait(data2[i])<0);
	}

}

WF121::WF121(HAL_UART *_uart) : recvthread(this,_uart){
		uart = _uart;
		wifi_uart = _uart;
		internal_state = wlan_state_inactive;
}

int WF121::init(const char *_ssid, const char *_pw) {
	ssid = _ssid;
	pw = _pw;


	uart->init(UART_BAUDRATE); //115200
	uart->config(UART_PARAMETER_HW_FLOW_CONTROL,1);

	BGLIB_INITIALIZE(wifiOutput);
	while(uart->isDataReady())uart->getcharNoWait();

	PROTECT_WITH_SEMAPHORE(sem_wlan_state) {
		if(internal_state == wlan_state_inactive) {
			internal_state = wlan_state_hello;
			recvthread.resume();
		} else {
			internal_state = wlan_state_hello;
		}
	}
	#ifdef WIFI_DEBUG
		PRINTF("Wifi State - Hello\n\r");
	#endif
	wifi_cmd_system_hello();
	AT(NOW()+300*MILLISECONDS);
	while(internal_state == wlan_state_hello) {
		while(uart->isDataReady())uart->getcharNoWait();
		#ifdef WIFI_DEBUG
			PRINTF("Wifi - Retry Hello\n\r");
		#endif
		wifi_cmd_system_hello();
		AT(NOW()+300*MILLISECONDS);
	}

	return 0;

}

int WF121::status() {
	return internal_state;
}

void WF121::reset() {
	PROTECT_WITH_SEMAPHORE(sem_wlan_state) {
		internal_state = wlan_state_inactive;
		wifi_cmd_system_reset(0);
	}
}

int WF121::write(UDPMsg *msg) {
	if(internal_state != wlan_state_ready || (udp_endpoint == -1 && tcp_endpoint == -1)) {
		return -1;
	}

	if(udp_endpoint != -1) {
		wifi_cmd_endpoint_send(udp_endpoint,msg->length,&msg->data);
	}
	if(tcp_endpoint != -1) {
		wifi_cmd_endpoint_send(tcp_endpoint,msg->length,&msg->data);
	}
	return msg->length;
}

void WF121::processMsg() {

	switch(BGLIB_MSG_ID(buffer)) {
	case wifi_rsp_system_hello_id:
		if(internal_state == wlan_state_hello) {
			#ifdef WIFI_DEBUG
				PRINTF("Wifi State - Reset\n\r");
			#endif
			internal_state = wlan_state_reset;
			wifi_cmd_system_reset(0);
		}
		break;
	case wifi_evt_system_boot_id:
		#ifdef WIFI_DEBUG
			PRINTF("Wifi Boot - Major: %d Minor: %d Patch: %d Build: %d\n\r",BGLIB_MSG(buffer)->evt_system_boot.major,BGLIB_MSG(buffer)->evt_system_boot.minor,BGLIB_MSG(buffer)->evt_system_boot.patch,BGLIB_MSG(buffer)->evt_system_boot.build);
		#endif
		if(internal_state == wlan_state_reset) {
			#ifdef WIFI_DEBUG
				PRINTF("Wifi State - Wifi On\n\r");
			#endif
			internal_state = wlan_state_wifi_on;
			wifi_cmd_sme_wifi_on();
		} else {
			#ifdef WIFI_DEBUG
				PRINTF("WiFi - Reset - Reestablishing Connection\n\r");
			#endif
			internal_state = wlan_state_wifi_on;
			wifi_cmd_sme_wifi_on();
		}
		break;
	case wifi_rsp_sme_wifi_on_id:
		if(internal_state == wlan_state_wifi_on) {
			if(BGLIB_MSG(buffer)->rsp_sme_wifi_on.result == wifi_err_success) {
				#ifdef WIFI_DEBUG
					PRINTF("Wifi State - Wait Wifi On\n\r");
				#endif
				wifi_cmd_system_set_max_power_saving_state(0);
				internal_state = wlan_state_wait_wifi_on;
			} else {
				#ifdef WIFI_DEBUG
					PRINTF("WiFi - error turning Wifi on - retry from start\n\r");
				#endif
				internal_state = wlan_state_hello;
				wifi_cmd_system_hello();
			}
		}
		break;
	case wifi_rsp_system_set_max_power_saving_state_id:
		#ifdef WIFI_DEBUG
			PRINTF("Wifi - power saving level changed\n\r");
		#endif
		break;
	case wifi_evt_sme_wifi_is_on_id:
		if(internal_state == wlan_state_wait_wifi_on) {
			if(BGLIB_MSG(buffer)->evt_sme_wifi_is_on.result == wifi_err_success) {
				#ifdef WIFI_DEBUG
					PRINTF("Wifi State - Set Pw\n\r");
				#endif
				internal_state = wlan_state_set_pw;
				wifi_cmd_sme_set_password(strlen(pw),pw);
			} else {
				#ifdef WIFI_DEBUG
					PRINTF("WiFi - error waiting for wifi on - retry from start\n\r");
				#endif
				internal_state = wlan_state_hello;
				wifi_cmd_system_hello();
			}
		}
		break;
	case wifi_rsp_sme_set_password_id:
		if(internal_state == wlan_state_set_pw) {
			if(BGLIB_MSG(buffer)->rsp_sme_set_password.status == wifi_err_success) {
				#ifdef WIFI_DEBUG
					PRINTF("Wifi State - Connect\n\r");
				#endif
				internal_state = wlan_state_connect;
				wifi_cmd_sme_connect_ssid(strlen(ssid),ssid);
			} else {
				#ifdef WIFI_DEBUG
					PRINTF("WiFi - error setting password - retry from start\n\r");
				#endif
				internal_state = wlan_state_hello;
				wifi_cmd_system_hello();
			}
		}
		break;
	case wifi_rsp_sme_connect_ssid_id:
		if(internal_state == wlan_state_connect) {
			if(BGLIB_MSG(buffer)->rsp_sme_connect_ssid.result == wifi_err_success) {
				#ifdef WIFI_DEBUG
					PRINTF("Wifi State - Wait Connect\n\r");
				#endif
				internal_state = wlan_state_wait_connect;
			} else {
				#ifdef WIFI_DEBUG
					PRINTF("WiFi - error connecting - retry from start\n\r");
				#endif
				internal_state = wlan_state_hello;
				wifi_cmd_system_hello();
			}
		}
		break;
	case wifi_evt_sme_connect_failed_id:
		#ifdef WIFI_DEBUG
			PRINTF("Wifi State - Connect Failed - Retry from Start\n\r");
		#endif
		internal_state = wlan_state_hello;
		wifi_cmd_system_hello();
		break;
	case wifi_evt_sme_connected_id:
		if(internal_state == wlan_state_wait_connect) {
			if(BGLIB_MSG(buffer)->evt_sme_connected.status == wifi_err_success) {
				#ifdef WIFI_DEBUG
					PRINTF("Wifi - connected\n\r");
				#endif
				/*internal_state = wlan_state_udp_connect;
				wifi_cmd_tcpip_udp_connect(WIFI_DESTINATION,12345,0);*/
			} else {
				#ifdef WIFI_DEBUG
					PRINTF("WiFi - error connecting - retry from start\n\r");
				#endif
				internal_state = wlan_state_hello;
				wifi_cmd_system_hello();
			}
		}
		break;
	case wifi_evt_sme_interface_status_id:
		#ifdef WIFI_DEBUG
			PRINTF("Wifi - Interface Status: %s\n\r",BGLIB_MSG(buffer)->evt_sme_interface_status.status?"up":"down");
		#endif
		if(BGLIB_MSG(buffer)->evt_sme_interface_status.status == 0) {
			#ifdef WIFI_DEBUG
				PRINTF("WiFi - Interface down - retry from start\n\r");
			#endif
			internal_state = wlan_state_hello;
			wifi_cmd_system_hello();
		} else {
			internal_state = wlan_state_ready;
			if(httpServerEnabled) {
				startHTTPServer();
			} if(udpConnectionEnabled) {
				startUDPConnection();
			} if(tcpConnectionEnabled) {
				startTCPConnection();
			}
		}
		break;
	case wifi_rsp_tcpip_udp_connect_id:
		#ifdef WIFI_DEBUG
			PRINTF("Wifi - UDP connect: %s\n\r",(BGLIB_MSG(buffer)->rsp_tcpip_udp_connect.result == wifi_err_success)?"success":"error");
		#endif
		if(BGLIB_MSG(buffer)->rsp_tcpip_udp_connect.result == wifi_err_success) {
			udp_endpoint=BGLIB_MSG(buffer)->rsp_tcpip_udp_connect.endpoint;
			wifi_cmd_tcpip_start_udp_server(udp_port,-1);
		}
		break;
	case wifi_rsp_tcpip_start_udp_server_id:
		#ifdef WIFI_DEBUG
			PRINTF("Wifi - Start UDP Server: %s\n\r",(BGLIB_MSG(buffer)->rsp_tcpip_start_udp_server.result == wifi_err_success)?"success":"error");
		#endif
                wifi_cmd_tcpip_udp_bind(udp_endpoint,udp_port);
		break;
	case wifi_evt_tcpip_udp_data_id:
		wf121rxMsg.length = BGLIB_MSG(buffer)->evt_tcpip_udp_data.data.len;
		memcpy(&wf121rxMsg.data,&BGLIB_MSG(buffer)->evt_tcpip_udp_data.data.data,wf121rxMsg.length);
		if(!rxFifo.put(wf121rxMsg)) {
			PRINTF("Wifi - Recevive Buffer full\n\r");
		}
		udp_msgs_recv++;
		upCallDataReady();
		break;
	case wifi_rsp_tcpip_tcp_connect_id:
		#ifdef WIFI_DEBUG
			PRINTF("Wifi - TCP connect: %s\n\r",(BGLIB_MSG(buffer)->rsp_tcpip_tcp_connect.result == wifi_err_success)?"success":"error");
		#endif
		if(BGLIB_MSG(buffer)->rsp_tcpip_tcp_connect.result == wifi_err_success) {
			tcp_endpoint=BGLIB_MSG(buffer)->rsp_tcpip_tcp_connect.endpoint;
		}
		break;
	case wifi_evt_endpoint_data_id:
		wf121rxMsg.length = BGLIB_MSG(buffer)->evt_endpoint_data.data.len;
		memcpy(&wf121rxMsg.data,&BGLIB_MSG(buffer)->evt_endpoint_data.data.data,wf121rxMsg.length);
		if(!rxFifo.put(wf121rxMsg)) {
			PRINTF("Wifi - Recevive Buffer full\n\r");
		}
		tcp_msgs_recv++;
		upCallDataReady();
		break;
	case wifi_rsp_endpoint_send_id:
		#ifdef WIFI_DEBUG
			if(BGLIB_MSG(buffer)->rsp_endpoint_send.result == wifi_err_invalid_command ) {
				PRINTF("Wifi Syntax Error: Command Not Recognized\n\r");
			} else if(BGLIB_MSG(buffer)->rsp_endpoint_send.result == wifi_err_timeout) {
				PRINTF("Wifi Syntax Error: TimeOut\n\r");
			} else if(BGLIB_MSG(buffer)->rsp_endpoint_send.result != wifi_err_success ) {
				PRINTF("Wifi - Send error %d \n\r",BGLIB_MSG(buffer)->rsp_endpoint_send.result);
			}
		#endif
		if(BGLIB_MSG(buffer)->rsp_endpoint_send.endpoint == udp_endpoint) {
			udp_msgs_send++;
		}
		if(BGLIB_MSG(buffer)->rsp_endpoint_send.endpoint == tcp_endpoint) {
			tcp_msgs_send++;
		}
		upCallWriteFinished();
		break;
	case wifi_evt_endpoint_syntax_error_id:
		#ifdef WIFI_DEBUG
			if(BGLIB_MSG(buffer)->evt_endpoint_syntax_error.result == wifi_err_invalid_command ) {
				PRINTF("Wifi Syntax Error: Command Not Recognized\n\r");
			} else if(BGLIB_MSG(buffer)->evt_endpoint_syntax_error.result == wifi_err_timeout) {
				PRINTF("Wifi Syntax Error: TimeOut\n\r");
			} else {
				PRINTF("Wifi Syntax Error\n\r");
			}
		#endif
		break;
	case wifi_evt_tcpip_endpoint_status_id:
		#ifdef WIFI_DEBUG
			PRINTF("Wifi - TcpIp Endpoint Status changed\n\r");
		#endif
		break;
    case wifi_evt_endpoint_status_id:
		#ifdef WIFI_DEBUG
			PRINTF("Wifi - Endpoint Status changed\n\r");
		#endif
		break;
    case wifi_evt_endpoint_closing_id:
		#ifdef WIFI_DEBUG
    		PRINTF("Wifi - Endpoint closing\n\r");
		#endif
    	wifi_cmd_endpoint_close(BGLIB_MSG(buffer)->evt_endpoint_closing.endpoint);
    	break;
    case wifi_rsp_endpoint_close_id:
		#ifdef WIFI_DEBUG
    		PRINTF("Wifi - Endpoint closed\n\r");
		#endif
    	if(tcpConnectionEnabled) {
    		startTCPConnection();
    	}
    	break;
    case wifi_evt_tcpip_configuration_id:
		#ifdef WIFI_DEBUG
			PRINTF("Wifi - TcpIp configuration changed - IP: %d.%d.%d.%d\n\r",BGLIB_MSG(buffer)->evt_tcpip_configuration.address.a[0],BGLIB_MSG(buffer)->evt_tcpip_configuration.address.a[1],BGLIB_MSG(buffer)->evt_tcpip_configuration.address.a[2],BGLIB_MSG(buffer)->evt_tcpip_configuration.address.a[3]);
		#endif
		address = BGLIB_MSG(buffer)->evt_tcpip_configuration.address;
		netmask = BGLIB_MSG(buffer)->evt_tcpip_configuration.netmask;
		gateway = BGLIB_MSG(buffer)->evt_tcpip_configuration.gateway;
		dhcp = BGLIB_MSG(buffer)->evt_tcpip_configuration.use_dhcp;
		break;
	case wifi_evt_tcpip_dns_configuration_id:
		#ifdef WIFI_DEBUG
			PRINTF("Wifi - DNS Configuration changed: Server #%d\n\r",BGLIB_MSG(buffer)->evt_tcpip_dns_configuration.index);
		#endif
		break;

	case wifi_rsp_https_enable_id:
		#ifdef WIFI_DEBUG
			PRINTF("Wifi - HTTPS enabled\n\r");
		#endif
		break;
	case wifi_rsp_https_add_path_id:
		#ifdef WIFI_DEBUG
			PRINTF("Wifi - HTTPS added path\n\r");
		#endif
		break;
	case wifi_evt_https_api_request_id:
		#ifdef WIFI_DEBUG
			PRINTF("Wifi - HTTPS Request #%ld\n\r",BGLIB_MSG(buffer)->evt_https_api_request.request);
		#endif
		if(httpServerEnabled && internal_state == wlan_state_ready) {
			httpHandler->handleRequest(BGLIB_MSG(buffer)->evt_https_api_request.request,&(BGLIB_MSG(buffer)->evt_https_api_request.resource),BGLIB_MSG(buffer)->evt_https_api_request.method);
		}
		break;
	case wifi_evt_https_api_request_header_id:
		#ifdef WIFI_DEBUG
			PRINTF("Wifi - HTTPS Request Header #%ld\n\r",BGLIB_MSG(buffer)->evt_https_api_request_header.request);
		#endif
		break;
	case wifi_evt_https_api_request_finished_id:
		#ifdef WIFI_DEBUG
			PRINTF("Wifi - HTTPS Request finished #%ld\n\r",BGLIB_MSG(buffer)->evt_https_api_request_finished.request);
		#endif
		break;
	case wifi_rsp_https_api_response_id:
		#ifdef WIFI_DEBUG
			PRINTF("Wifi - HTTPS Response\n\r");
		#endif
		break;
	case wifi_rsp_https_api_response_finish_id:
		#ifdef WIFI_DEBUG
			PRINTF("Wifi - HTTPS Response finished\n\r");
		#endif
		break;
	default:
		#ifdef WIFI_DEBUG
			PRINTF("Unhandled Msg: %d\n\r",(int)BGLIB_MSG_ID(buffer));
			while(uart->isDataReady())uart->getcharNoWait();
		#endif
		break;
	}
}

bool WF121::isDataReady(){
	return !rxFifo.isEmpty();
}

int WF121::read(UDPMsg *msg) {
	return rxFifo.get(*msg);
}

void WF121::startHTTPServer() {
	wifi_cmd_https_add_path(0,1,"/");
	wifi_cmd_https_add_path(1,4,"/api");
	wifi_cmd_https_enable(1,0,0);
}

void WF121::enableHTTPServer(HTTPHandler *_httpHandler) {
	httpHandler = _httpHandler;
	httpServerEnabled = true;
	PROTECT_WITH_SEMAPHORE(sem_wlan_state) {
		if(internal_state == wlan_state_ready) {
			startHTTPServer();
		}
	}
}
void WF121::startUDPConnection() {
	wifi_cmd_tcpip_udp_connect(udp_destination,udp_port,0);
}

void WF121::startTCPConnection() {
	if(tcp_destination == 0) {
		tcp_destination = gateway.u;
	}
	wifi_cmd_tcpip_tcp_connect(tcp_destination,tcp_port,-1);
}

void WF121::enableUDPConnection(uint32_t _udp_destination, uint32_t _udp_port) {
	udpConnectionEnabled = true;
	udp_destination = _udp_destination;
	udp_port = _udp_port;
	PROTECT_WITH_SEMAPHORE(sem_wlan_state) {
		if(internal_state == wlan_state_ready) {
			startUDPConnection();
		}
	}
}

void WF121::enableTCPConnection(uint32_t _tcp_destination, uint32_t _tcp_port) {
	tcpConnectionEnabled = true;
	tcp_destination = _tcp_destination;
	tcp_port = _tcp_port;
	PROTECT_WITH_SEMAPHORE(sem_wlan_state) {
		if(internal_state == wlan_state_ready) {
			startTCPConnection();
		}
	}
}

ipv4 WF121::getAdress() {
	return address;
}

ipv4 WF121::getGateway() {
	return gateway;
}

ipv4 WF121::getNetmask() {
	return netmask;
}

uint8_t WF121::getDHCP() {
	return dhcp;
}

const char * WF121::getSSID() {
	return ssid;
}

uint32_t WF121::getUDPMsgsRecv() {
	return udp_msgs_recv;
}


uint32_t WF121::getUDPMsgsSend() {
	return udp_msgs_send;
}
