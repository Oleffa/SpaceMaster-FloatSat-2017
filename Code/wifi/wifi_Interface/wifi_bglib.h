#ifndef wifi_bglib_h
#define wifi_bglib_h

/*****************************************************************************
 *
 *
 *  Header only interface for BGLIB
 *
 *
 ****************************************************************************/

/*****************************************************************************
 *
 *  BGLIB usage:
 *      Define library, it should be defined globally to allow usage 
 *      from every function:
 *          BGLIB_DEFINE();
 *
 *      Declare and define output function, prototype is:
 *          void my_output(uint8 len1,uint8* data1,uint16 len2,uint8* data2);
 *
 *          function sends len1 amount of data from pointer data1 to device, and then sends
 *          len2 amount of data from pointer data2 to device
 *
 *      Initialize library,and provide output function:
 *          BGLIB_INITIALIZE(my_output);
 *
 *      Receiving messages:
 *          Allocate buffer of size BGLIB_MSG_MAXLEN for message, can be less but reader shall then drop too long packets from module
 *          Read BGLIB_MSG_HEADER_LEN amount of bytes from device to buffer
 *          Read BGLIB_MSG_LEN(buffer) amount of bytes from device to buffer after the header
 *          Get message ID by BGLIB_MSG_ID(buffer), use switch or if to compare to message_ids:
 *                  if(BGLIB_MSG_ID(buffer)==wifi_rsp_sme_wifi_on_id)
 *              Cast buffer to msg_packet structure by BGLIB_MSG(buffer) and use it to access message structure:
 *                  if(BGLIB_MSG(buffer)->rsp_sme_wifi_on.result != wifi_err_success)
 *
 *      Sending messages:
 *          Call macros which build message to temporary buffer and calls my_output to send message.
 *              wifi_cmd_sme_connect_bssid(&selected_bssid[0]);
 *          
 *
 ****************************************************************************/

/*****************************************************************************
 *
 *      Notes about SPI interface:
 *          Byte zero is sent by module when there is no valid data to send.
 *          Host shall drop bytes from module until first nonzero byte is received.
 *          Host shall send zero bytes to module when it is reading data from module
 *          and there is no valid data to send.
 *
 ****************************************************************************/

/*****************************************************************************
 *
 *      Support for compilers other than GCC and MSVC:
 *      
 *      To support specific compiler add compiler's structure packing directives to following macro:
 *          PACKSTRUCT( decl )
 *          
 *      BGLIB uses PACKSTRUCT macro to add packing information for structures:
 *      PACKSTRUCT(struct wifi_msg_dfu_reset_cmd_t
 *      {
 *          uint8	dfu;
 *      });
 *
 *
 ****************************************************************************/

/*lint -save --e{528,572,778,845,835,849}*/
#include "apitypes.h"
#ifdef __cplusplus
extern "C" {
#endif

/* Compability */
#ifndef PACKSTRUCT
/*Default packed configuration*/
#ifdef __GNUC__
#ifdef _WIN32
#define PACKSTRUCT( decl ) decl __attribute__((__packed__,gcc_struct))
#else
#define PACKSTRUCT( decl ) decl __attribute__((__packed__))
#endif
#define ALIGNED __attribute__((aligned(0x4)))
#elif __IAR_SYSTEMS_ICC__

#define PACKSTRUCT( decl ) __packed decl

#define ALIGNED
#elif _MSC_VER  //msvc

#define PACKSTRUCT( decl ) __pragma( pack(push, 1) ) decl __pragma( pack(pop) )
#define ALIGNED
#else 
#define PACKSTRUCT(a) a PACKED 
#endif
#endif


struct wifi_header
{
    uint8  type_hilen;
    uint8  lolen;
    uint8  cls;
    uint8  command;
};

#define BGLIB_DEFINE() struct wifi_cmd_packet bglib_temp_msg;\
void (*bglib_output)(uint8 len1,uint8* data1,uint16 len2,uint8* data2);
#define BGLIB_INITIALIZE(X) bglib_output=X;
#define BGLIB_MSG_ID(BUF) ((*((uint32*)BUF))&0xffff00f8)
#define BGLIB_MSG(X) ((struct wifi_cmd_packet*)X)
#define BGLIB_MSG_HEADER_LEN (4)
#define BGLIB_MSG_LEN(BUF) ((((*((uint32*)BUF))&0x7)<<8)|(((*((uint32*)BUF))&0xff00)>>8))
#define BGLIB_MSG_MAXLEN 2052
extern void (*bglib_output)(uint8 len1,uint8* data1,uint16 len2,uint8* data2);

enum system_main_state
{
	system_idle       = 1,
	system_powered    = 2,
	system_connecting = 4,
	system_connected  = 8,
	system_wps        = 16,
	system_ap         = 32,
	system_main_state_max= 33
};

enum endpoint_types
{
	endpoint_free       = 0x0,
	endpoint_uart       = 0x1,
	endpoint_usb        = 0x2,
	endpoint_tcp        = 0x4,
	endpoint_tcp_server = 0x8,
	endpoint_udp        = 0x10,
	endpoint_udp_server = 0x20,
	endpoint_script     = 0x40,
	endpoint_wait_close = 0x80,
	endpoint_spi        = 0x100,
	endpoint_i2c        = 0x200,
	endpoint_drop       = 0x400,
	endpoint_types_max  = 1025
};

enum hardware_alarm_repeat_mask
{
	hardware_alarm_every_half_second = 0,
	hardware_alarm_every_second      = 1,
	hardware_alarm_every_ten_seconds = 2,
	hardware_alarm_every_minute      = 3,
	hardware_alarm_every_ten_minutes = 4,
	hardware_alarm_every_hour        = 5,
	hardware_alarm_every_day         = 6,
	hardware_alarm_every_week        = 7,
	hardware_alarm_every_month       = 8,
	hardware_alarm_every_year        = 9,
	hardware_alarm_repeat_mask_max   = 10
};

#define FLASH_PS_KEY_MAC                   1
#define FLASH_PS_KEY_IPV4_SETTINGS         2
#define FLASH_PS_KEY_DNS0_SETTINGS         3
#define FLASH_PS_KEY_DNS1_SETTINGS         4
#define FLASH_PS_KEY_MODULE_SERVICE        5
#define FLASH_PS_KEY_APPL_NUM1             10
#define FLASH_PS_KEY_APPL_NUM2             11
#define FLASH_PS_KEY_APPL_NUM3             12
#define FLASH_PS_KEY_APPL_NUM4             13
#define FLASH_PS_KEY_APPL_STR1             14
#define FLASH_PS_KEY_APPL_STR2             15
#define FLASH_PS_KEY_APPL_STR3             16
#define FLASH_PS_KEY_APPL_STR4             17
#define FLASH_PS_KEY_APPL_TITLE            18
#define FLASH_PS_KEY_AP_SSID               20
#define FLASH_PS_KEY_AP_CHANNEL            21
#define FLASH_PS_KEY_AP_PW                 22
#define FLASH_PS_KEY_AP_WIFI_N             23
#define FLASH_PS_KEY_AP_SECURITY           24
#define FLASH_PS_KEY_CLIENT_SSID           25
#define FLASH_PS_KEY_CLIENT_PW             26
#define FLASH_PS_KEY_HTTP_PW               27
#define FLASH_PS_KEY_HTTP_LOGIN_LEVEL      28
#define FLASH_PS_KEY_DHCPS_ENABLE          30
#define FLASH_PS_KEY_DHCPS_SPACE           31
#define FLASH_PS_KEY_DHCPS_DISABLE_ROUTING 32
#define FLASH_PS_KEY_DNSS_ENABLE           35
#define FLASH_PS_KEY_DNSS_URL              36
#define FLASH_PS_KEY_DNSS_ANY_URL          37
#define FLASH_PS_KEY_AP_SCANLIST_ITEM_1    40
#define FLASH_PS_KEY_AP_SCANLIST_ITEM_2    41
#define FLASH_PS_KEY_AP_SCANLIST_ITEM_3    42
#define FLASH_PS_KEY_AP_SCANLIST_ITEM_4    43
#define FLASH_PS_KEY_AP_SCANLIST_ITEM_5    44
#define FLASH_PS_KEY_AP_SCANLIST_ITEM_6    45
#define FLASH_PS_KEY_AP_SCANLIST_ITEM_7    46
#define FLASH_PS_KEY_AP_SCANLIST_ITEM_8    47
#define FLASH_PS_KEY_AP_SCANLIST_ITEM_9    48
#define FLASH_PS_KEY_AP_SCANLIST_ITEM_10   49
#define FLASH_PS_KEY_AP_LABEL1             50
#define FLASH_PS_KEY_AP_LABEL2             51
#define FLASH_PS_KEY_AP_LABEL3             52
#define FLASH_PS_KEY_AP_LABEL4             53
#define FLASH_PS_KEY_AP_LABEL5             54
#define FLASH_PS_KEY_AP_LABEL6             55
#define FLASH_PS_KEY_AP_LABEL7             56
#define FLASH_PS_KEY_AP_LABEL8             57
#define FLASH_PS_KEY_AP_LABEL9             58
#define FLASH_PS_KEY_AP_LABEL10            59


enum wifi_parameter_types
{
    wifi_msg_parameter_uint8=2,
    wifi_msg_parameter_int8=3,
    wifi_msg_parameter_uint16=4,
    wifi_msg_parameter_int16=5,
    wifi_msg_parameter_uint32=6,
    wifi_msg_parameter_int32=7,
    wifi_msg_parameter_uint8array=8,
    wifi_msg_parameter_string1=9,
    wifi_msg_parameter_hwaddr=10,
    wifi_msg_parameter_uint16array=11
};

enum wifi_msg_types
{
    wifi_msg_type_cmd=0x00,
    wifi_msg_type_rsp=0x00,
    wifi_msg_type_evt=0x80
};
enum wifi_dev_types
{
    wifi_dev_type_ble    =0x00,
    wifi_dev_type_wifi   =0x08,
    wifi_dev_type_iwrap  =0x10,
    wifi_dev_type_dumo   =0x20,
    wifi_dev_type_test   =0x20,
    wifi_dev_type_gecko  =0x20
};

#define	wifi_cmd_dfu_reset_id                          	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x00000000)
#define	wifi_cmd_dfu_flash_set_address_id              	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x01000000)
#define	wifi_cmd_dfu_flash_upload_id                   	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x02000000)
#define	wifi_cmd_dfu_flash_upload_finish_id            	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x03000000)
#define	wifi_cmd_system_sync_id                        	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x00010000)
#define	wifi_cmd_system_reset_id                       	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x01010000)
#define	wifi_cmd_system_hello_id                       	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x02010000)
#define	wifi_cmd_system_set_max_power_saving_state_id  	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x03010000)
#define	wifi_cmd_config_get_mac_id                     	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x00020000)
#define	wifi_cmd_config_set_mac_id                     	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x01020000)
#define	wifi_cmd_sme_wifi_on_id                        	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x00030000)
#define	wifi_cmd_sme_wifi_off_id                       	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x01030000)
#define	wifi_cmd_sme_power_on_id                       	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x02030000)
#define	wifi_cmd_sme_start_scan_id                     	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x03030000)
#define	wifi_cmd_sme_stop_scan_id                      	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x04030000)
#define	wifi_cmd_sme_set_password_id                   	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x05030000)
#define	wifi_cmd_sme_connect_bssid_id                  	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x06030000)
#define	wifi_cmd_sme_connect_ssid_id                   	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x07030000)
#define	wifi_cmd_sme_disconnect_id                     	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x08030000)
#define	wifi_cmd_sme_set_scan_channels_id              	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x09030000)
#define	wifi_cmd_sme_set_operating_mode_id             	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x0A030000)
#define	wifi_cmd_sme_start_ap_mode_id                  	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x0B030000)
#define	wifi_cmd_sme_stop_ap_mode_id                   	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x0C030000)
#define	wifi_cmd_sme_scan_results_sort_rssi_id         	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x0D030000)
#define	wifi_cmd_sme_ap_client_disconnect_id           	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x0E030000)
#define	wifi_cmd_sme_set_ap_password_id                	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x0F030000)
#define	wifi_cmd_sme_set_ap_max_clients_id             	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x10030000)
#define	wifi_cmd_sme_start_wps_id                      	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x11030000)
#define	wifi_cmd_sme_stop_wps_id                       	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x12030000)
#define	wifi_cmd_sme_get_signal_quality_id             	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x13030000)
#define	wifi_cmd_sme_start_ssid_scan_id                	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x14030000)
#define	wifi_cmd_sme_set_ap_hidden_id                  	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x15030000)
#define	wifi_cmd_sme_set_11n_mode_id                   	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x16030000)
#define	wifi_cmd_tcpip_start_tcp_server_id             	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x00040000)
#define	wifi_cmd_tcpip_tcp_connect_id                  	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x01040000)
#define	wifi_cmd_tcpip_start_udp_server_id             	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x02040000)
#define	wifi_cmd_tcpip_udp_connect_id                  	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x03040000)
#define	wifi_cmd_tcpip_configure_id                    	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x04040000)
#define	wifi_cmd_tcpip_dns_configure_id                	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x05040000)
#define	wifi_cmd_tcpip_dns_gethostbyname_id            	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x06040000)
#define	wifi_cmd_tcpip_udp_bind_id                     	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x07040000)
#define	wifi_cmd_tcpip_dhcp_set_hostname_id            	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x08040000)
#define	wifi_cmd_tcpip_dhcp_enable_routing_id          	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x09040000)
#define	wifi_cmd_tcpip_mdns_set_hostname_id            	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x0A040000)
#define	wifi_cmd_tcpip_mdns_start_id                   	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x0B040000)
#define	wifi_cmd_tcpip_mdns_stop_id                    	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x0C040000)
#define	wifi_cmd_tcpip_dnssd_add_service_id            	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x0D040000)
#define	wifi_cmd_tcpip_dnssd_add_service_instance_id   	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x0E040000)
#define	wifi_cmd_tcpip_dnssd_add_service_attribute_id  	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x0F040000)
#define	wifi_cmd_tcpip_dnssd_remove_service_id         	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x10040000)
#define	wifi_cmd_tcpip_dnssd_start_service_id          	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x11040000)
#define	wifi_cmd_tcpip_dnssd_stop_service_id           	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x12040000)
#define	wifi_cmd_endpoint_send_id                      	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x00050000)
#define	wifi_cmd_endpoint_set_streaming_id             	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x01050000)
#define	wifi_cmd_endpoint_set_active_id                	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x02050000)
#define	wifi_cmd_endpoint_set_streaming_destination_id 	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x03050000)
#define	wifi_cmd_endpoint_close_id                     	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x04050000)
#define	wifi_cmd_endpoint_set_transmit_size_id         	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x05050000)
#define	wifi_cmd_endpoint_disable_id                   	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x06050000)
#define	wifi_cmd_hardware_set_soft_timer_id            	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x00060000)
#define	wifi_cmd_hardware_external_interrupt_config_id 	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x01060000)
#define	wifi_cmd_hardware_change_notification_config_id	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x02060000)
#define	wifi_cmd_hardware_change_notification_pullup_id	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x03060000)
#define	wifi_cmd_hardware_io_port_config_direction_id  	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x04060000)
#define	wifi_cmd_hardware_io_port_config_open_drain_id 	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x05060000)
#define	wifi_cmd_hardware_io_port_write_id             	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x06060000)
#define	wifi_cmd_hardware_io_port_read_id              	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x07060000)
#define	wifi_cmd_hardware_output_compare_id            	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x08060000)
#define	wifi_cmd_hardware_adc_read_id                  	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x09060000)
#define	wifi_cmd_hardware_rtc_init_id                  	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x0A060000)
#define	wifi_cmd_hardware_rtc_set_time_id              	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x0B060000)
#define	wifi_cmd_hardware_rtc_get_time_id              	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x0C060000)
#define	wifi_cmd_hardware_rtc_set_alarm_id             	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x0D060000)
#define	wifi_cmd_hardware_uart_conf_set_id             	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x0E060000)
#define	wifi_cmd_hardware_uart_conf_get_id             	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x0F060000)
#define	wifi_cmd_flash_ps_defrag_id                    	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x00070000)
#define	wifi_cmd_flash_ps_dump_id                      	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x01070000)
#define	wifi_cmd_flash_ps_erase_all_id                 	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x02070000)
#define	wifi_cmd_flash_ps_save_id                      	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x03070000)
#define	wifi_cmd_flash_ps_load_id                      	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x04070000)
#define	wifi_cmd_flash_ps_erase_id                     	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x05070000)
#define	wifi_cmd_i2c_start_read_id                     	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x00080000)
#define	wifi_cmd_i2c_start_write_id                    	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x01080000)
#define	wifi_cmd_i2c_stop_id                           	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x02080000)
#define	wifi_cmd_https_enable_id                       	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x00090000)
#define	wifi_cmd_https_add_path_id                     	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x01090000)
#define	wifi_cmd_https_api_response_id                 	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x02090000)
#define	wifi_cmd_https_api_response_finish_id          	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x03090000)
#define	wifi_cmd_ethernet_set_dataroute_id             	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x000A0000)
#define	wifi_cmd_ethernet_close_id                     	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x010A0000)
#define	wifi_cmd_ethernet_connected_id                 	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x020A0000)
#define	wifi_cmd_sdhc_fopen_id                         	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x000B0000)
#define	wifi_cmd_sdhc_fclose_id                        	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x010B0000)
#define	wifi_cmd_sdhc_fdir_id                          	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x020B0000)
#define	wifi_cmd_sdhc_fread_id                         	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x030B0000)
#define	wifi_cmd_sdhc_fwrite_id                        	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x040B0000)
#define	wifi_cmd_sdhc_fdelete_id                       	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x050B0000)
#define	wifi_cmd_sdhc_fmkdir_id                        	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x060B0000)
#define	wifi_cmd_sdhc_fchdir_id                        	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x070B0000)
#define	wifi_cmd_sdhc_frename_id                       	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x080B0000)
#define	wifi_cmd_sdhc_fchmode_id                       	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x090B0000)
#define	wifi_cmd_util_atoi_id                          	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x000C0000)
#define	wifi_cmd_util_itoa_id                          	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_cmd|0x010C0000)
#define	wifi_rsp_dfu_reset_id                          	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x00000000)
#define	wifi_rsp_dfu_flash_set_address_id              	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x01000000)
#define	wifi_rsp_dfu_flash_upload_id                   	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x02000000)
#define	wifi_rsp_dfu_flash_upload_finish_id            	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x03000000)
#define	wifi_rsp_system_sync_id                        	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x00010000)
#define	wifi_rsp_system_reset_id                       	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x01010000)
#define	wifi_rsp_system_hello_id                       	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x02010000)
#define	wifi_rsp_system_set_max_power_saving_state_id  	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x03010000)
#define	wifi_rsp_config_get_mac_id                     	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x00020000)
#define	wifi_rsp_config_set_mac_id                     	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x01020000)
#define	wifi_rsp_sme_wifi_on_id                        	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x00030000)
#define	wifi_rsp_sme_wifi_off_id                       	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x01030000)
#define	wifi_rsp_sme_power_on_id                       	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x02030000)
#define	wifi_rsp_sme_start_scan_id                     	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x03030000)
#define	wifi_rsp_sme_stop_scan_id                      	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x04030000)
#define	wifi_rsp_sme_set_password_id                   	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x05030000)
#define	wifi_rsp_sme_connect_bssid_id                  	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x06030000)
#define	wifi_rsp_sme_connect_ssid_id                   	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x07030000)
#define	wifi_rsp_sme_disconnect_id                     	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x08030000)
#define	wifi_rsp_sme_set_scan_channels_id              	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x09030000)
#define	wifi_rsp_sme_set_operating_mode_id             	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x0A030000)
#define	wifi_rsp_sme_start_ap_mode_id                  	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x0B030000)
#define	wifi_rsp_sme_stop_ap_mode_id                   	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x0C030000)
#define	wifi_rsp_sme_scan_results_sort_rssi_id         	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x0D030000)
#define	wifi_rsp_sme_ap_client_disconnect_id           	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x0E030000)
#define	wifi_rsp_sme_set_ap_password_id                	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x0F030000)
#define	wifi_rsp_sme_set_ap_max_clients_id             	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x10030000)
#define	wifi_rsp_sme_start_wps_id                      	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x11030000)
#define	wifi_rsp_sme_stop_wps_id                       	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x12030000)
#define	wifi_rsp_sme_get_signal_quality_id             	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x13030000)
#define	wifi_rsp_sme_start_ssid_scan_id                	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x14030000)
#define	wifi_rsp_sme_set_ap_hidden_id                  	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x15030000)
#define	wifi_rsp_sme_set_11n_mode_id                   	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x16030000)
#define	wifi_rsp_tcpip_start_tcp_server_id             	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x00040000)
#define	wifi_rsp_tcpip_tcp_connect_id                  	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x01040000)
#define	wifi_rsp_tcpip_start_udp_server_id             	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x02040000)
#define	wifi_rsp_tcpip_udp_connect_id                  	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x03040000)
#define	wifi_rsp_tcpip_configure_id                    	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x04040000)
#define	wifi_rsp_tcpip_dns_configure_id                	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x05040000)
#define	wifi_rsp_tcpip_dns_gethostbyname_id            	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x06040000)
#define	wifi_rsp_tcpip_udp_bind_id                     	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x07040000)
#define	wifi_rsp_tcpip_dhcp_set_hostname_id            	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x08040000)
#define	wifi_rsp_tcpip_dhcp_enable_routing_id          	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x09040000)
#define	wifi_rsp_tcpip_mdns_set_hostname_id            	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x0A040000)
#define	wifi_rsp_tcpip_mdns_start_id                   	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x0B040000)
#define	wifi_rsp_tcpip_mdns_stop_id                    	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x0C040000)
#define	wifi_rsp_tcpip_dnssd_add_service_id            	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x0D040000)
#define	wifi_rsp_tcpip_dnssd_add_service_instance_id   	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x0E040000)
#define	wifi_rsp_tcpip_dnssd_add_service_attribute_id  	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x0F040000)
#define	wifi_rsp_tcpip_dnssd_remove_service_id         	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x10040000)
#define	wifi_rsp_tcpip_dnssd_start_service_id          	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x11040000)
#define	wifi_rsp_tcpip_dnssd_stop_service_id           	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x12040000)
#define	wifi_rsp_endpoint_send_id                      	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x00050000)
#define	wifi_rsp_endpoint_set_streaming_id             	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x01050000)
#define	wifi_rsp_endpoint_set_active_id                	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x02050000)
#define	wifi_rsp_endpoint_set_streaming_destination_id 	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x03050000)
#define	wifi_rsp_endpoint_close_id                     	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x04050000)
#define	wifi_rsp_endpoint_set_transmit_size_id         	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x05050000)
#define	wifi_rsp_endpoint_disable_id                   	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x06050000)
#define	wifi_rsp_hardware_set_soft_timer_id            	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x00060000)
#define	wifi_rsp_hardware_external_interrupt_config_id 	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x01060000)
#define	wifi_rsp_hardware_change_notification_config_id	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x02060000)
#define	wifi_rsp_hardware_change_notification_pullup_id	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x03060000)
#define	wifi_rsp_hardware_io_port_config_direction_id  	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x04060000)
#define	wifi_rsp_hardware_io_port_config_open_drain_id 	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x05060000)
#define	wifi_rsp_hardware_io_port_write_id             	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x06060000)
#define	wifi_rsp_hardware_io_port_read_id              	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x07060000)
#define	wifi_rsp_hardware_output_compare_id            	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x08060000)
#define	wifi_rsp_hardware_adc_read_id                  	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x09060000)
#define	wifi_rsp_hardware_rtc_init_id                  	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x0A060000)
#define	wifi_rsp_hardware_rtc_set_time_id              	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x0B060000)
#define	wifi_rsp_hardware_rtc_get_time_id              	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x0C060000)
#define	wifi_rsp_hardware_rtc_set_alarm_id             	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x0D060000)
#define	wifi_rsp_hardware_uart_conf_set_id             	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x0E060000)
#define	wifi_rsp_hardware_uart_conf_get_id             	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x0F060000)
#define	wifi_rsp_flash_ps_defrag_id                    	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x00070000)
#define	wifi_rsp_flash_ps_dump_id                      	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x01070000)
#define	wifi_rsp_flash_ps_erase_all_id                 	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x02070000)
#define	wifi_rsp_flash_ps_save_id                      	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x03070000)
#define	wifi_rsp_flash_ps_load_id                      	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x04070000)
#define	wifi_rsp_flash_ps_erase_id                     	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x05070000)
#define	wifi_rsp_i2c_start_read_id                     	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x00080000)
#define	wifi_rsp_i2c_start_write_id                    	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x01080000)
#define	wifi_rsp_i2c_stop_id                           	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x02080000)
#define	wifi_rsp_https_enable_id                       	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x00090000)
#define	wifi_rsp_https_add_path_id                     	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x01090000)
#define	wifi_rsp_https_api_response_id                 	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x02090000)
#define	wifi_rsp_https_api_response_finish_id          	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x03090000)
#define	wifi_rsp_ethernet_set_dataroute_id             	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x000A0000)
#define	wifi_rsp_ethernet_close_id                     	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x010A0000)
#define	wifi_rsp_ethernet_connected_id                 	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x020A0000)
#define	wifi_rsp_sdhc_fopen_id                         	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x000B0000)
#define	wifi_rsp_sdhc_fclose_id                        	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x010B0000)
#define	wifi_rsp_sdhc_fdir_id                          	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x020B0000)
#define	wifi_rsp_sdhc_fread_id                         	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x030B0000)
#define	wifi_rsp_sdhc_fwrite_id                        	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x040B0000)
#define	wifi_rsp_sdhc_fdelete_id                       	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x050B0000)
#define	wifi_rsp_sdhc_fmkdir_id                        	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x060B0000)
#define	wifi_rsp_sdhc_fchdir_id                        	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x070B0000)
#define	wifi_rsp_sdhc_frename_id                       	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x080B0000)
#define	wifi_rsp_sdhc_fchmode_id                       	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x090B0000)
#define	wifi_rsp_util_atoi_id                          	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x000C0000)
#define	wifi_rsp_util_itoa_id                          	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_rsp|0x010C0000)
#define	wifi_evt_dfu_boot_id                           	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_evt|0x00000000)
#define	wifi_evt_system_boot_id                        	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_evt|0x00010000)
#define	wifi_evt_system_state_id                       	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_evt|0x01010000)
#define	wifi_evt_system_sw_exception_id                	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_evt|0x02010000)
#define	wifi_evt_system_power_saving_state_id          	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_evt|0x03010000)
#define	wifi_evt_config_mac_address_id                 	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_evt|0x00020000)
#define	wifi_evt_sme_wifi_is_on_id                     	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_evt|0x00030000)
#define	wifi_evt_sme_wifi_is_off_id                    	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_evt|0x01030000)
#define	wifi_evt_sme_scan_result_id                    	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_evt|0x02030000)
#define	wifi_evt_sme_scan_result_drop_id               	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_evt|0x03030000)
#define	wifi_evt_sme_scanned_id                        	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_evt|0x04030000)
#define	wifi_evt_sme_connected_id                      	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_evt|0x05030000)
#define	wifi_evt_sme_disconnected_id                   	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_evt|0x06030000)
#define	wifi_evt_sme_interface_status_id               	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_evt|0x07030000)
#define	wifi_evt_sme_connect_failed_id                 	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_evt|0x08030000)
#define	wifi_evt_sme_connect_retry_id                  	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_evt|0x09030000)
#define	wifi_evt_sme_ap_mode_started_id                	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_evt|0x0A030000)
#define	wifi_evt_sme_ap_mode_stopped_id                	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_evt|0x0B030000)
#define	wifi_evt_sme_ap_mode_failed_id                 	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_evt|0x0C030000)
#define	wifi_evt_sme_ap_client_joined_id               	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_evt|0x0D030000)
#define	wifi_evt_sme_ap_client_left_id                 	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_evt|0x0E030000)
#define	wifi_evt_sme_scan_sort_result_id               	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_evt|0x0F030000)
#define	wifi_evt_sme_scan_sort_finished_id             	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_evt|0x10030000)
#define	wifi_evt_sme_wps_stopped_id                    	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_evt|0x11030000)
#define	wifi_evt_sme_wps_completed_id                  	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_evt|0x12030000)
#define	wifi_evt_sme_wps_failed_id                     	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_evt|0x13030000)
#define	wifi_evt_sme_wps_credential_ssid_id            	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_evt|0x14030000)
#define	wifi_evt_sme_wps_credential_password_id        	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_evt|0x15030000)
#define	wifi_evt_sme_signal_quality_id                 	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_evt|0x16030000)
#define	wifi_evt_tcpip_configuration_id                	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_evt|0x00040000)
#define	wifi_evt_tcpip_dns_configuration_id            	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_evt|0x01040000)
#define	wifi_evt_tcpip_endpoint_status_id              	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_evt|0x02040000)
#define	wifi_evt_tcpip_dns_gethostbyname_result_id     	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_evt|0x03040000)
#define	wifi_evt_tcpip_udp_data_id                     	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_evt|0x04040000)
#define	wifi_evt_tcpip_mdns_started_id                 	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_evt|0x05040000)
#define	wifi_evt_tcpip_mdns_failed_id                  	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_evt|0x06040000)
#define	wifi_evt_tcpip_mdns_stopped_id                 	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_evt|0x07040000)
#define	wifi_evt_tcpip_dnssd_service_started_id        	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_evt|0x08040000)
#define	wifi_evt_tcpip_dnssd_service_failed_id         	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_evt|0x09040000)
#define	wifi_evt_tcpip_dnssd_service_stopped_id        	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_evt|0x0A040000)
#define	wifi_evt_endpoint_syntax_error_id              	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_evt|0x00050000)
#define	wifi_evt_endpoint_data_id                      	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_evt|0x01050000)
#define	wifi_evt_endpoint_status_id                    	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_evt|0x02050000)
#define	wifi_evt_endpoint_closing_id                   	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_evt|0x03050000)
#define	wifi_evt_endpoint_error_id                     	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_evt|0x04050000)
#define	wifi_evt_hardware_soft_timer_id                	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_evt|0x00060000)
#define	wifi_evt_hardware_change_notification_id       	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_evt|0x01060000)
#define	wifi_evt_hardware_external_interrupt_id        	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_evt|0x02060000)
#define	wifi_evt_hardware_rtc_alarm_id                 	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_evt|0x03060000)
#define	wifi_evt_hardware_uart_conf_id                 	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_evt|0x04060000)
#define	wifi_evt_flash_ps_key_id                       	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_evt|0x00070000)
#define	wifi_evt_flash_ps_key_changed_id               	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_evt|0x01070000)
#define	wifi_evt_flash_low_voltage_id                  	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_evt|0x02070000)
#define	wifi_evt_https_on_req_id                       	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_evt|0x00090000)
#define	wifi_evt_https_button_id                       	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_evt|0x01090000)
#define	wifi_evt_https_api_request_id                  	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_evt|0x02090000)
#define	wifi_evt_https_api_request_header_id           	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_evt|0x03090000)
#define	wifi_evt_https_api_request_data_id             	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_evt|0x04090000)
#define	wifi_evt_https_api_request_finished_id         	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_evt|0x05090000)
#define	wifi_evt_ethernet_link_status_id               	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_evt|0x000A0000)
#define	wifi_evt_sdhc_ready_id                         	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_evt|0x000B0000)
#define	wifi_evt_sdhc_fdata_id                         	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_evt|0x010B0000)
#define	wifi_evt_sdhc_ffile_id                         	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_evt|0x020B0000)
#define	wifi_evt_sdhc_fpwd_id                          	(((uint32)wifi_dev_type_wifi)|wifi_msg_type_evt|0x030B0000)
#ifndef BG_ERRORCODES
#define BG_ERRORCODES
enum wifi_error_spaces
{
	wifi_errspc_hardware=768,
	wifi_errspc_bg=256,
	wifi_errspc_tcpip=512,
};
typedef enum wifi_error
{
	wifi_err_hardware_ps_store_full            =wifi_errspc_hardware+1,//Flash reserved for PS store is full
	wifi_err_hardware_ps_key_not_found         =wifi_errspc_hardware+2,//PS key not found
	wifi_err_hardware_i2c_write_already_started=wifi_errspc_hardware+3,//Tried to start i2c write transmission, but it is already in progress.
	wifi_err_hardware_i2c_ack_missing          =wifi_errspc_hardware+4,//Acknowledge for i2c was not received.
	wifi_err_hardware_flash_failed             =wifi_errspc_hardware+8,//Writing to flash failed
	wifi_err_hardware_sdhc_not_opened          =wifi_errspc_hardware+5,//Access to unopened file
	wifi_err_hardware_sdhc_not_found           =wifi_errspc_hardware+6,//File not in SD card
	wifi_err_hardware_sdhc_disk_error          =wifi_errspc_hardware+7,//Disk error or disk full
	wifi_err_success                           =0,                     //No error
	wifi_err_invalid_param                     =wifi_errspc_bg+128,    //Command contained invalid parameter
	wifi_err_wrong_state                       =wifi_errspc_bg+129,    //Device is in wrong state to accept command
	wifi_err_out_of_memory                     =wifi_errspc_bg+130,    //Device has run out of memory
	wifi_err_not_implemented                   =wifi_errspc_bg+131,    //Feature is not implemented
	wifi_err_invalid_command                   =wifi_errspc_bg+132,    //Command was not recognized
	wifi_err_timeout                           =wifi_errspc_bg+133,    //Command or Procedure failed due to timeout
	wifi_err_unspecified                       =wifi_errspc_bg+134,    //Unspecified error
	wifi_err_hardware                          =wifi_errspc_bg+135,    //Hardware failure
	wifi_err_buffers_full                      =wifi_errspc_bg+136,    //Command not accepted, because internal buffers are full
	wifi_err_disconnected                      =wifi_errspc_bg+137,    //Command or Procedure failed due to disconnection
	wifi_err_too_many_requests                 =wifi_errspc_bg+138,    //Too many Simultaneous Requests
	wifi_err_ap_not_in_scanlist                =wifi_errspc_bg+139,    //Access Point not found from scanlist
	wifi_err_invalid_password                  =wifi_errspc_bg+140,    //Password is invalid or missing
	wifi_err_authentication_failure            =wifi_errspc_bg+141,    //WPA/WPA2 authentication has failed
	wifi_err_overflow                          =wifi_errspc_bg+142,    //Overflow detected
	wifi_err_multiple_pbc_sessions             =wifi_errspc_bg+143,    //Multiple PBC sessions detected
	wifi_err_eth_not_connected                 =wifi_errspc_bg+144,    //Ethernet cable not connected
	wifi_err_eth_route_not_set                 =wifi_errspc_bg+145,    //Ethernet route not set
	wifi_err_wrong_operating_mode              =wifi_errspc_bg+146,    //Wrong operating mode for this command
	wifi_err_not_found                         =wifi_errspc_bg+147,    //Requested resource not found
	wifi_err_already_exists                    =wifi_errspc_bg+148,    //Requested resource already exists
	wifi_err_invalid_configuration             =wifi_errspc_bg+149,    //Current configuration is invalid
	wifi_err_ap_lost                           =wifi_errspc_bg+150,    //Connection to Access Point lost
	wifi_err_tcpip_success                     =wifi_errspc_tcpip+0,   //No error
	wifi_err_tcpip_out_of_memory               =wifi_errspc_tcpip+1,   //Out of memory
	wifi_err_tcpip_buffer                      =wifi_errspc_tcpip+2,   //Buffer handling failed
	wifi_err_tcpip_timeout                     =wifi_errspc_tcpip+3,   //Timeout
	wifi_err_tcpip_routing                     =wifi_errspc_tcpip+4,   //Could not find route
	wifi_err_tcpip_in_progress                 =wifi_errspc_tcpip+5,   //Operation in progress
	wifi_err_tcpip_illegal_value               =wifi_errspc_tcpip+6,   //Illegal value
	wifi_err_tcpip_would_block                 =wifi_errspc_tcpip+7,   //Operation would block
	wifi_err_tcpip_in_use                      =wifi_errspc_tcpip+8,   //Address in use
	wifi_err_tcpip_already_connected           =wifi_errspc_tcpip+9,   //Already connected
	wifi_err_tcpip_abort                       =wifi_errspc_tcpip+10,  //Connection aborted
	wifi_err_tcpip_reset                       =wifi_errspc_tcpip+11,  //Connection reset
	wifi_err_tcpip_closed                      =wifi_errspc_tcpip+12,  //Connection closed
	wifi_err_tcpip_not_connected               =wifi_errspc_tcpip+13,  //Not connected
	wifi_err_tcpip_illegal_argument            =wifi_errspc_tcpip+14,  //Illegal argument
	wifi_err_tcpip_interface                   =wifi_errspc_tcpip+15,  //Interface error
	wifi_err_tcpip_service_not_running         =wifi_errspc_tcpip+16,  //Service not running
	wifi_err_tcpip_service_running             =wifi_errspc_tcpip+17,  //Service already running
	wifi_err_tcpip_hostname_not_set            =wifi_errspc_tcpip+18,  //Hostname not set
	wifi_err_tcpip_hostname_conflict           =wifi_errspc_tcpip+19,  //Hostname conflict detected
	wifi_err_tcpip_unknown_host                =wifi_errspc_tcpip+128, //Unknown host
	wifi_err_last
}errorcode_t;
#endif
PACKSTRUCT(struct wifi_msg_dfu_reset_cmd_t
{
	uint8	dfu;
});

PACKSTRUCT(struct wifi_msg_dfu_flash_set_address_cmd_t
{
	uint32	address;
});

PACKSTRUCT(struct wifi_msg_dfu_flash_set_address_rsp_t
{
	uint16	result;
});

PACKSTRUCT(struct wifi_msg_dfu_flash_upload_cmd_t
{
	uint8array	data;
});

PACKSTRUCT(struct wifi_msg_dfu_flash_upload_rsp_t
{
	uint16	result;
});

PACKSTRUCT(struct wifi_msg_dfu_flash_upload_finish_rsp_t
{
	uint16	result;
});

PACKSTRUCT(struct wifi_msg_dfu_boot_evt_t
{
	uint32	version;
});

PACKSTRUCT(struct wifi_msg_system_reset_cmd_t
{
	uint8	dfu;
});

PACKSTRUCT(struct wifi_msg_system_set_max_power_saving_state_cmd_t
{
	uint8	state;
});

PACKSTRUCT(struct wifi_msg_system_set_max_power_saving_state_rsp_t
{
	uint16	result;
});

PACKSTRUCT(struct wifi_msg_system_boot_evt_t
{
	uint16	major;
	uint16	minor;
	uint16	patch;
	uint16	build;
	uint16	bootloader_version;
	uint16	tcpip_version;
	uint16	hw;
});

PACKSTRUCT(struct wifi_msg_system_state_evt_t
{
	uint16	state;
});

PACKSTRUCT(struct wifi_msg_system_sw_exception_evt_t
{
	uint32	address;
	uint8	type;
});

PACKSTRUCT(struct wifi_msg_system_power_saving_state_evt_t
{
	uint8	state;
});

PACKSTRUCT(struct wifi_msg_config_get_mac_cmd_t
{
	uint8	hw_interface;
});

PACKSTRUCT(struct wifi_msg_config_get_mac_rsp_t
{
	uint16	result;
	uint8	hw_interface;
});

PACKSTRUCT(struct wifi_msg_config_set_mac_cmd_t
{
	uint8	hw_interface;
	hw_addr	mac;
});

PACKSTRUCT(struct wifi_msg_config_set_mac_rsp_t
{
	uint16	result;
	uint8	hw_interface;
});

PACKSTRUCT(struct wifi_msg_config_mac_address_evt_t
{
	uint8	hw_interface;
	hw_addr	mac;
});

PACKSTRUCT(struct wifi_msg_sme_wifi_on_rsp_t
{
	uint16	result;
});

PACKSTRUCT(struct wifi_msg_sme_wifi_off_rsp_t
{
	uint16	result;
});

PACKSTRUCT(struct wifi_msg_sme_power_on_cmd_t
{
	uint8	enable;
});

PACKSTRUCT(struct wifi_msg_sme_power_on_rsp_t
{
	uint16	result;
});

PACKSTRUCT(struct wifi_msg_sme_start_scan_cmd_t
{
	uint8	hw_interface;
	uint8array	chList;
});

PACKSTRUCT(struct wifi_msg_sme_start_scan_rsp_t
{
	uint16	result;
});

PACKSTRUCT(struct wifi_msg_sme_stop_scan_rsp_t
{
	uint16	result;
});

PACKSTRUCT(struct wifi_msg_sme_set_password_cmd_t
{
	uint8array	password;
});

PACKSTRUCT(struct wifi_msg_sme_set_password_rsp_t
{
	uint8	status;
});

PACKSTRUCT(struct wifi_msg_sme_connect_bssid_cmd_t
{
	hw_addr	bssid;
});

PACKSTRUCT(struct wifi_msg_sme_connect_bssid_rsp_t
{
	uint16	result;
	uint8	hw_interface;
	hw_addr	bssid;
});

PACKSTRUCT(struct wifi_msg_sme_connect_ssid_cmd_t
{
	uint8array	ssid;
});

PACKSTRUCT(struct wifi_msg_sme_connect_ssid_rsp_t
{
	uint16	result;
	uint8	hw_interface;
	hw_addr	bssid;
});

PACKSTRUCT(struct wifi_msg_sme_disconnect_rsp_t
{
	uint16	result;
	uint8	hw_interface;
});

PACKSTRUCT(struct wifi_msg_sme_set_scan_channels_cmd_t
{
	uint8	hw_interface;
	uint8array	chList;
});

PACKSTRUCT(struct wifi_msg_sme_set_scan_channels_rsp_t
{
	uint16	result;
});

PACKSTRUCT(struct wifi_msg_sme_set_operating_mode_cmd_t
{
	uint8	mode;
});

PACKSTRUCT(struct wifi_msg_sme_set_operating_mode_rsp_t
{
	uint16	result;
});

PACKSTRUCT(struct wifi_msg_sme_start_ap_mode_cmd_t
{
	uint8	channel;
	uint8	security;
	uint8array	ssid;
});

PACKSTRUCT(struct wifi_msg_sme_start_ap_mode_rsp_t
{
	uint16	result;
	uint8	hw_interface;
});

PACKSTRUCT(struct wifi_msg_sme_stop_ap_mode_rsp_t
{
	uint16	result;
	uint8	hw_interface;
});

PACKSTRUCT(struct wifi_msg_sme_scan_results_sort_rssi_cmd_t
{
	uint8	amount;
});

PACKSTRUCT(struct wifi_msg_sme_scan_results_sort_rssi_rsp_t
{
	uint16	result;
});

PACKSTRUCT(struct wifi_msg_sme_ap_client_disconnect_cmd_t
{
	hw_addr	mac_address;
});

PACKSTRUCT(struct wifi_msg_sme_ap_client_disconnect_rsp_t
{
	uint16	result;
	uint8	hw_interface;
});

PACKSTRUCT(struct wifi_msg_sme_set_ap_password_cmd_t
{
	uint8array	password;
});

PACKSTRUCT(struct wifi_msg_sme_set_ap_password_rsp_t
{
	uint8	status;
});

PACKSTRUCT(struct wifi_msg_sme_set_ap_max_clients_cmd_t
{
	uint8	max_clients;
});

PACKSTRUCT(struct wifi_msg_sme_set_ap_max_clients_rsp_t
{
	uint16	result;
	uint8	hw_interface;
});

PACKSTRUCT(struct wifi_msg_sme_start_wps_rsp_t
{
	uint16	result;
	uint8	hw_interface;
});

PACKSTRUCT(struct wifi_msg_sme_stop_wps_rsp_t
{
	uint16	result;
	uint8	hw_interface;
});

PACKSTRUCT(struct wifi_msg_sme_get_signal_quality_rsp_t
{
	uint16	result;
	uint8	hw_interface;
});

PACKSTRUCT(struct wifi_msg_sme_start_ssid_scan_cmd_t
{
	uint8array	ssid;
});

PACKSTRUCT(struct wifi_msg_sme_start_ssid_scan_rsp_t
{
	uint16	result;
});

PACKSTRUCT(struct wifi_msg_sme_set_ap_hidden_cmd_t
{
	uint8	hidden;
});

PACKSTRUCT(struct wifi_msg_sme_set_ap_hidden_rsp_t
{
	uint16	result;
	uint8	hw_interface;
});

PACKSTRUCT(struct wifi_msg_sme_set_11n_mode_cmd_t
{
	uint8	mode;
});

PACKSTRUCT(struct wifi_msg_sme_set_11n_mode_rsp_t
{
	uint16	result;
	uint8	hw_interface;
});

PACKSTRUCT(struct wifi_msg_sme_wifi_is_on_evt_t
{
	uint16	result;
});

PACKSTRUCT(struct wifi_msg_sme_wifi_is_off_evt_t
{
	uint16	result;
});

PACKSTRUCT(struct wifi_msg_sme_scan_result_evt_t
{
	hw_addr	bssid;
	int8	channel;
	int16	rssi;
	int8	snr;
	uint8	secure;
	uint8array	ssid;
});

PACKSTRUCT(struct wifi_msg_sme_scan_result_drop_evt_t
{
	hw_addr	bssid;
});

PACKSTRUCT(struct wifi_msg_sme_scanned_evt_t
{
	int8	status;
});

PACKSTRUCT(struct wifi_msg_sme_connected_evt_t
{
	int8	status;
	uint8	hw_interface;
	hw_addr	bssid;
});

PACKSTRUCT(struct wifi_msg_sme_disconnected_evt_t
{
	uint16	reason;
	uint8	hw_interface;
});

PACKSTRUCT(struct wifi_msg_sme_interface_status_evt_t
{
	uint8	hw_interface;
	uint8	status;
});

PACKSTRUCT(struct wifi_msg_sme_connect_failed_evt_t
{
	uint16	reason;
	uint8	hw_interface;
});

PACKSTRUCT(struct wifi_msg_sme_connect_retry_evt_t
{
	uint8	hw_interface;
});

PACKSTRUCT(struct wifi_msg_sme_ap_mode_started_evt_t
{
	uint8	hw_interface;
});

PACKSTRUCT(struct wifi_msg_sme_ap_mode_stopped_evt_t
{
	uint8	hw_interface;
});

PACKSTRUCT(struct wifi_msg_sme_ap_mode_failed_evt_t
{
	uint16	reason;
	uint8	hw_interface;
});

PACKSTRUCT(struct wifi_msg_sme_ap_client_joined_evt_t
{
	hw_addr	mac_address;
	uint8	hw_interface;
});

PACKSTRUCT(struct wifi_msg_sme_ap_client_left_evt_t
{
	hw_addr	mac_address;
	uint8	hw_interface;
});

PACKSTRUCT(struct wifi_msg_sme_scan_sort_result_evt_t
{
	hw_addr	bssid;
	int8	channel;
	int16	rssi;
	int8	snr;
	uint8	secure;
	uint8array	ssid;
});

PACKSTRUCT(struct wifi_msg_sme_wps_stopped_evt_t
{
	uint8	hw_interface;
});

PACKSTRUCT(struct wifi_msg_sme_wps_completed_evt_t
{
	uint8	hw_interface;
});

PACKSTRUCT(struct wifi_msg_sme_wps_failed_evt_t
{
	uint16	reason;
	uint8	hw_interface;
});

PACKSTRUCT(struct wifi_msg_sme_wps_credential_ssid_evt_t
{
	uint8	hw_interface;
	uint8array	ssid;
});

PACKSTRUCT(struct wifi_msg_sme_wps_credential_password_evt_t
{
	uint8	hw_interface;
	uint8array	password;
});

PACKSTRUCT(struct wifi_msg_sme_signal_quality_evt_t
{
	int8	rssi;
	uint8	hw_interface;
});

PACKSTRUCT(struct wifi_msg_tcpip_start_tcp_server_cmd_t
{
	uint16	port;
	int8	default_destination;
});

PACKSTRUCT(struct wifi_msg_tcpip_start_tcp_server_rsp_t
{
	uint16	result;
	uint8	endpoint;
});

PACKSTRUCT(struct wifi_msg_tcpip_tcp_connect_cmd_t
{
	ipv4	address;
	uint16	port;
	int8	routing;
});

PACKSTRUCT(struct wifi_msg_tcpip_tcp_connect_rsp_t
{
	uint16	result;
	uint8	endpoint;
});

PACKSTRUCT(struct wifi_msg_tcpip_start_udp_server_cmd_t
{
	uint16	port;
	int8	default_destination;
});

PACKSTRUCT(struct wifi_msg_tcpip_start_udp_server_rsp_t
{
	uint16	result;
	uint8	endpoint;
});

PACKSTRUCT(struct wifi_msg_tcpip_udp_connect_cmd_t
{
	ipv4	address;
	uint16	port;
	int8	routing;
});

PACKSTRUCT(struct wifi_msg_tcpip_udp_connect_rsp_t
{
	uint16	result;
	uint8	endpoint;
});

PACKSTRUCT(struct wifi_msg_tcpip_configure_cmd_t
{
	ipv4	address;
	ipv4	netmask;
	ipv4	gateway;
	uint8	use_dhcp;
});

PACKSTRUCT(struct wifi_msg_tcpip_configure_rsp_t
{
	uint16	result;
});

PACKSTRUCT(struct wifi_msg_tcpip_dns_configure_cmd_t
{
	uint8	index;
	ipv4	address;
});

PACKSTRUCT(struct wifi_msg_tcpip_dns_configure_rsp_t
{
	uint16	result;
});

PACKSTRUCT(struct wifi_msg_tcpip_dns_gethostbyname_cmd_t
{
	uint8array	name;
});

PACKSTRUCT(struct wifi_msg_tcpip_dns_gethostbyname_rsp_t
{
	uint16	result;
});

PACKSTRUCT(struct wifi_msg_tcpip_udp_bind_cmd_t
{
	uint8	endpoint;
	uint16	port;
});

PACKSTRUCT(struct wifi_msg_tcpip_udp_bind_rsp_t
{
	uint16	result;
});

PACKSTRUCT(struct wifi_msg_tcpip_dhcp_set_hostname_cmd_t
{
	uint8array	hostname;
});

PACKSTRUCT(struct wifi_msg_tcpip_dhcp_set_hostname_rsp_t
{
	uint16	result;
});

PACKSTRUCT(struct wifi_msg_tcpip_dhcp_enable_routing_cmd_t
{
	uint8	enable;
});

PACKSTRUCT(struct wifi_msg_tcpip_dhcp_enable_routing_rsp_t
{
	uint16	result;
});

PACKSTRUCT(struct wifi_msg_tcpip_mdns_set_hostname_cmd_t
{
	uint8array	hostname;
});

PACKSTRUCT(struct wifi_msg_tcpip_mdns_set_hostname_rsp_t
{
	uint16	result;
});

PACKSTRUCT(struct wifi_msg_tcpip_mdns_start_rsp_t
{
	uint16	result;
});

PACKSTRUCT(struct wifi_msg_tcpip_mdns_stop_rsp_t
{
	uint16	result;
});

PACKSTRUCT(struct wifi_msg_tcpip_dnssd_add_service_cmd_t
{
	uint16	port;
	uint8	protocol;
	uint8array	service;
});

PACKSTRUCT(struct wifi_msg_tcpip_dnssd_add_service_rsp_t
{
	uint16	result;
	uint8	index;
});

PACKSTRUCT(struct wifi_msg_tcpip_dnssd_add_service_instance_cmd_t
{
	uint8	index;
	uint8array	instance;
});

PACKSTRUCT(struct wifi_msg_tcpip_dnssd_add_service_instance_rsp_t
{
	uint16	result;
});

PACKSTRUCT(struct wifi_msg_tcpip_dnssd_add_service_attribute_cmd_t
{
	uint8	index;
	uint8array	attribute;
});

PACKSTRUCT(struct wifi_msg_tcpip_dnssd_add_service_attribute_rsp_t
{
	uint16	result;
});

PACKSTRUCT(struct wifi_msg_tcpip_dnssd_remove_service_cmd_t
{
	uint8	index;
});

PACKSTRUCT(struct wifi_msg_tcpip_dnssd_remove_service_rsp_t
{
	uint16	result;
});

PACKSTRUCT(struct wifi_msg_tcpip_dnssd_start_service_cmd_t
{
	uint8	index;
});

PACKSTRUCT(struct wifi_msg_tcpip_dnssd_start_service_rsp_t
{
	uint16	result;
});

PACKSTRUCT(struct wifi_msg_tcpip_dnssd_stop_service_cmd_t
{
	uint8	index;
});

PACKSTRUCT(struct wifi_msg_tcpip_dnssd_stop_service_rsp_t
{
	uint16	result;
});

PACKSTRUCT(struct wifi_msg_tcpip_configuration_evt_t
{
	ipv4	address;
	ipv4	netmask;
	ipv4	gateway;
	uint8	use_dhcp;
});

PACKSTRUCT(struct wifi_msg_tcpip_dns_configuration_evt_t
{
	uint8	index;
	ipv4	address;
});

PACKSTRUCT(struct wifi_msg_tcpip_endpoint_status_evt_t
{
	uint8	endpoint;
	ipv4	local_ip;
	uint16	local_port;
	ipv4	remote_ip;
	uint16	remote_port;
});

PACKSTRUCT(struct wifi_msg_tcpip_dns_gethostbyname_result_evt_t
{
	uint16	result;
	ipv4	address;
	uint8array	name;
});

PACKSTRUCT(struct wifi_msg_tcpip_udp_data_evt_t
{
	uint8	endpoint;
	ipv4	source_address;
	uint16	source_port;
	uint16array	data;
});

PACKSTRUCT(struct wifi_msg_tcpip_mdns_failed_evt_t
{
	uint16	reason;
});

PACKSTRUCT(struct wifi_msg_tcpip_mdns_stopped_evt_t
{
	uint16	reason;
});

PACKSTRUCT(struct wifi_msg_tcpip_dnssd_service_started_evt_t
{
	uint8	index;
});

PACKSTRUCT(struct wifi_msg_tcpip_dnssd_service_failed_evt_t
{
	uint16	reason;
	uint8	index;
});

PACKSTRUCT(struct wifi_msg_tcpip_dnssd_service_stopped_evt_t
{
	uint16	reason;
	uint8	index;
});

PACKSTRUCT(struct wifi_msg_endpoint_send_cmd_t
{
	uint8	endpoint;
	uint8array	data;
});

PACKSTRUCT(struct wifi_msg_endpoint_send_rsp_t
{
	uint16	result;
	uint8	endpoint;
});

PACKSTRUCT(struct wifi_msg_endpoint_set_streaming_cmd_t
{
	uint8	endpoint;
	uint8	streaming;
});

PACKSTRUCT(struct wifi_msg_endpoint_set_streaming_rsp_t
{
	uint16	result;
	uint8	endpoint;
});

PACKSTRUCT(struct wifi_msg_endpoint_set_active_cmd_t
{
	uint8	endpoint;
	uint8	active;
});

PACKSTRUCT(struct wifi_msg_endpoint_set_active_rsp_t
{
	uint16	result;
	uint8	endpoint;
});

PACKSTRUCT(struct wifi_msg_endpoint_set_streaming_destination_cmd_t
{
	uint8	endpoint;
	int8	streaming_destination;
});

PACKSTRUCT(struct wifi_msg_endpoint_set_streaming_destination_rsp_t
{
	uint16	result;
	uint8	endpoint;
});

PACKSTRUCT(struct wifi_msg_endpoint_close_cmd_t
{
	uint8	endpoint;
});

PACKSTRUCT(struct wifi_msg_endpoint_close_rsp_t
{
	uint16	result;
	uint8	endpoint;
});

PACKSTRUCT(struct wifi_msg_endpoint_set_transmit_size_cmd_t
{
	uint8	endpoint;
	uint16	size;
});

PACKSTRUCT(struct wifi_msg_endpoint_set_transmit_size_rsp_t
{
	uint16	result;
	uint8	endpoint;
});

PACKSTRUCT(struct wifi_msg_endpoint_disable_cmd_t
{
	uint8	endpoint;
});

PACKSTRUCT(struct wifi_msg_endpoint_disable_rsp_t
{
	uint16	result;
	uint8	endpoint;
});

PACKSTRUCT(struct wifi_msg_endpoint_syntax_error_evt_t
{
	uint16	result;
	uint8	endpoint;
});

PACKSTRUCT(struct wifi_msg_endpoint_data_evt_t
{
	uint8	endpoint;
	uint8array	data;
});

PACKSTRUCT(struct wifi_msg_endpoint_status_evt_t
{
	uint8	endpoint;
	uint32	type;
	uint8	streaming;
	int8	destination;
	uint8	active;
});

PACKSTRUCT(struct wifi_msg_endpoint_closing_evt_t
{
	uint16	reason;
	uint8	endpoint;
});

PACKSTRUCT(struct wifi_msg_endpoint_error_evt_t
{
	uint16	reason;
	uint8	endpoint;
});

PACKSTRUCT(struct wifi_msg_hardware_set_soft_timer_cmd_t
{
	uint32	time;
	uint8	handle;
	uint8	single_shot;
});

PACKSTRUCT(struct wifi_msg_hardware_set_soft_timer_rsp_t
{
	uint16	result;
});

PACKSTRUCT(struct wifi_msg_hardware_external_interrupt_config_cmd_t
{
	uint8	enable;
	uint8	polarity;
});

PACKSTRUCT(struct wifi_msg_hardware_external_interrupt_config_rsp_t
{
	uint16	result;
});

PACKSTRUCT(struct wifi_msg_hardware_change_notification_config_cmd_t
{
	uint32	enable;
});

PACKSTRUCT(struct wifi_msg_hardware_change_notification_config_rsp_t
{
	uint16	result;
});

PACKSTRUCT(struct wifi_msg_hardware_change_notification_pullup_cmd_t
{
	uint32	pullup;
});

PACKSTRUCT(struct wifi_msg_hardware_change_notification_pullup_rsp_t
{
	uint16	result;
});

PACKSTRUCT(struct wifi_msg_hardware_io_port_config_direction_cmd_t
{
	uint8	port;
	uint16	mask;
	uint16	direction;
});

PACKSTRUCT(struct wifi_msg_hardware_io_port_config_direction_rsp_t
{
	uint16	result;
});

PACKSTRUCT(struct wifi_msg_hardware_io_port_config_open_drain_cmd_t
{
	uint8	port;
	uint16	mask;
	uint16	open_drain;
});

PACKSTRUCT(struct wifi_msg_hardware_io_port_config_open_drain_rsp_t
{
	uint16	result;
});

PACKSTRUCT(struct wifi_msg_hardware_io_port_write_cmd_t
{
	uint8	port;
	uint16	mask;
	uint16	data;
});

PACKSTRUCT(struct wifi_msg_hardware_io_port_write_rsp_t
{
	uint16	result;
});

PACKSTRUCT(struct wifi_msg_hardware_io_port_read_cmd_t
{
	uint8	port;
	uint16	mask;
});

PACKSTRUCT(struct wifi_msg_hardware_io_port_read_rsp_t
{
	uint16	result;
	uint8	port;
	uint16	data;
});

PACKSTRUCT(struct wifi_msg_hardware_output_compare_cmd_t
{
	uint8	index;
	uint8	bit32;
	uint8	timer;
	uint8	mode;
	uint32	compare_value;
});

PACKSTRUCT(struct wifi_msg_hardware_output_compare_rsp_t
{
	uint16	result;
});

PACKSTRUCT(struct wifi_msg_hardware_adc_read_cmd_t
{
	uint8	input;
});

PACKSTRUCT(struct wifi_msg_hardware_adc_read_rsp_t
{
	uint16	result;
	uint8	input;
	uint16	value;
});

PACKSTRUCT(struct wifi_msg_hardware_rtc_init_cmd_t
{
	uint8	enable;
	int16	drift;
});

PACKSTRUCT(struct wifi_msg_hardware_rtc_init_rsp_t
{
	uint16	result;
});

PACKSTRUCT(struct wifi_msg_hardware_rtc_set_time_cmd_t
{
	int16	year;
	int8	month;
	int8	day;
	int8	weekday;
	int8	hour;
	int8	minute;
	int8	second;
});

PACKSTRUCT(struct wifi_msg_hardware_rtc_set_time_rsp_t
{
	uint16	result;
});

PACKSTRUCT(struct wifi_msg_hardware_rtc_get_time_rsp_t
{
	uint16	result;
	int16	year;
	int8	month;
	int8	day;
	int8	weekday;
	int8	hour;
	int8	minute;
	int8	second;
});

PACKSTRUCT(struct wifi_msg_hardware_rtc_set_alarm_cmd_t
{
	uint8	month;
	uint8	day;
	int8	weekday;
	uint8	hour;
	uint8	minute;
	uint8	second;
	uint8	repeat_mask;
	int16	repeat_count;
});

PACKSTRUCT(struct wifi_msg_hardware_rtc_set_alarm_rsp_t
{
	uint16	result;
});

PACKSTRUCT(struct wifi_msg_hardware_uart_conf_set_cmd_t
{
	uint8	id;
	uint32	rate;
	uint8	data_bits;
	uint8	stop_bits;
	uint8	parity;
	uint8	flow_ctrl;
});

PACKSTRUCT(struct wifi_msg_hardware_uart_conf_set_rsp_t
{
	uint16	result;
});

PACKSTRUCT(struct wifi_msg_hardware_uart_conf_get_cmd_t
{
	uint8	id;
});

PACKSTRUCT(struct wifi_msg_hardware_uart_conf_get_rsp_t
{
	uint16	result;
});

PACKSTRUCT(struct wifi_msg_hardware_soft_timer_evt_t
{
	uint8	handle;
});

PACKSTRUCT(struct wifi_msg_hardware_change_notification_evt_t
{
	uint32	timestamp;
});

PACKSTRUCT(struct wifi_msg_hardware_external_interrupt_evt_t
{
	uint8	irq;
	uint32	timestamp;
});

PACKSTRUCT(struct wifi_msg_hardware_uart_conf_evt_t
{
	uint8	id;
	uint32	rate;
	uint8	data_bits;
	uint8	stop_bits;
	uint8	parity;
	uint8	flow_ctrl;
});

PACKSTRUCT(struct wifi_msg_flash_ps_defrag_rsp_t
{
	uint16	result;
});

PACKSTRUCT(struct wifi_msg_flash_ps_dump_rsp_t
{
	uint16	result;
});

PACKSTRUCT(struct wifi_msg_flash_ps_erase_all_rsp_t
{
	uint16	result;
});

PACKSTRUCT(struct wifi_msg_flash_ps_save_cmd_t
{
	uint16	key;
	uint8array	value;
});

PACKSTRUCT(struct wifi_msg_flash_ps_save_rsp_t
{
	uint16	result;
});

PACKSTRUCT(struct wifi_msg_flash_ps_load_cmd_t
{
	uint16	key;
});

PACKSTRUCT(struct wifi_msg_flash_ps_load_rsp_t
{
	uint16	result;
	uint8array	value;
});

PACKSTRUCT(struct wifi_msg_flash_ps_erase_cmd_t
{
	uint16	key;
});

PACKSTRUCT(struct wifi_msg_flash_ps_erase_rsp_t
{
	uint16	result;
});

PACKSTRUCT(struct wifi_msg_flash_ps_key_evt_t
{
	uint16	key;
	uint8array	value;
});

PACKSTRUCT(struct wifi_msg_flash_ps_key_changed_evt_t
{
	uint16	key;
});

PACKSTRUCT(struct wifi_msg_i2c_start_read_cmd_t
{
	uint8	endpoint;
	uint16	slave_address;
	uint8	length;
});

PACKSTRUCT(struct wifi_msg_i2c_start_read_rsp_t
{
	uint16	result;
});

PACKSTRUCT(struct wifi_msg_i2c_start_write_cmd_t
{
	uint8	endpoint;
	uint16	slave_address;
});

PACKSTRUCT(struct wifi_msg_i2c_start_write_rsp_t
{
	uint16	result;
});

PACKSTRUCT(struct wifi_msg_i2c_stop_cmd_t
{
	uint8	endpoint;
});

PACKSTRUCT(struct wifi_msg_i2c_stop_rsp_t
{
	uint16	result;
});

PACKSTRUCT(struct wifi_msg_https_enable_cmd_t
{
	uint8	https;
	uint8	dhcps;
	uint8	dnss;
});

PACKSTRUCT(struct wifi_msg_https_enable_rsp_t
{
	uint16	result;
});

PACKSTRUCT(struct wifi_msg_https_add_path_cmd_t
{
	uint8	device;
	uint8array	path;
});

PACKSTRUCT(struct wifi_msg_https_add_path_rsp_t
{
	uint16	result;
});

PACKSTRUCT(struct wifi_msg_https_api_response_cmd_t
{
	uint32	request;
	uint8array	data;
});

PACKSTRUCT(struct wifi_msg_https_api_response_rsp_t
{
	uint16	result;
});

PACKSTRUCT(struct wifi_msg_https_api_response_finish_cmd_t
{
	uint32	request;
});

PACKSTRUCT(struct wifi_msg_https_api_response_finish_rsp_t
{
	uint16	result;
});

PACKSTRUCT(struct wifi_msg_https_on_req_evt_t
{
	uint8	service;
});

PACKSTRUCT(struct wifi_msg_https_button_evt_t
{
	uint8	number;
});

PACKSTRUCT(struct wifi_msg_https_api_request_evt_t
{
	uint32	request;
	uint8	method;
	uint8array	resource;
});

PACKSTRUCT(struct wifi_msg_https_api_request_header_evt_t
{
	uint32	request;
	uint8array	data;
});

PACKSTRUCT(struct wifi_msg_https_api_request_data_evt_t
{
	uint32	request;
	uint8array	data;
});

PACKSTRUCT(struct wifi_msg_https_api_request_finished_evt_t
{
	uint32	request;
});

PACKSTRUCT(struct wifi_msg_ethernet_set_dataroute_cmd_t
{
	uint8	route;
});

PACKSTRUCT(struct wifi_msg_ethernet_set_dataroute_rsp_t
{
	uint16	result;
});

PACKSTRUCT(struct wifi_msg_ethernet_connected_rsp_t
{
	uint8	state;
});

PACKSTRUCT(struct wifi_msg_ethernet_link_status_evt_t
{
	uint8	state;
});

PACKSTRUCT(struct wifi_msg_sdhc_fopen_cmd_t
{
	uint8	mode;
	uint8array	fname;
});

PACKSTRUCT(struct wifi_msg_sdhc_fopen_rsp_t
{
	uint16	result;
});

PACKSTRUCT(struct wifi_msg_sdhc_fclose_cmd_t
{
	uint8	fhandle;
});

PACKSTRUCT(struct wifi_msg_sdhc_fclose_rsp_t
{
	uint16	result;
});

PACKSTRUCT(struct wifi_msg_sdhc_fdir_cmd_t
{
	uint8	mode;
	uint8array	path;
});

PACKSTRUCT(struct wifi_msg_sdhc_fdir_rsp_t
{
	uint16	result;
});

PACKSTRUCT(struct wifi_msg_sdhc_fread_cmd_t
{
	uint8	fhandle;
	uint32	fsize;
});

PACKSTRUCT(struct wifi_msg_sdhc_fread_rsp_t
{
	uint16	result;
});

PACKSTRUCT(struct wifi_msg_sdhc_fwrite_cmd_t
{
	uint8	fhandle;
	uint16array	fdata;
});

PACKSTRUCT(struct wifi_msg_sdhc_fwrite_rsp_t
{
	uint16	result;
});

PACKSTRUCT(struct wifi_msg_sdhc_fdelete_cmd_t
{
	uint8array	fname;
});

PACKSTRUCT(struct wifi_msg_sdhc_fdelete_rsp_t
{
	uint16	result;
});

PACKSTRUCT(struct wifi_msg_sdhc_fmkdir_cmd_t
{
	uint8array	dir_name;
});

PACKSTRUCT(struct wifi_msg_sdhc_fmkdir_rsp_t
{
	uint16	result;
});

PACKSTRUCT(struct wifi_msg_sdhc_fchdir_cmd_t
{
	uint8array	dir_name;
});

PACKSTRUCT(struct wifi_msg_sdhc_fchdir_rsp_t
{
	uint16	result;
});

PACKSTRUCT(struct wifi_msg_sdhc_frename_cmd_t
{
	uint8	fhandle;
	uint8array	new_name;
});

PACKSTRUCT(struct wifi_msg_sdhc_frename_rsp_t
{
	uint16	result;
});

PACKSTRUCT(struct wifi_msg_sdhc_fchmode_cmd_t
{
	uint8	value;
	uint8array	fname;
});

PACKSTRUCT(struct wifi_msg_sdhc_fchmode_rsp_t
{
	uint16	result;
});

PACKSTRUCT(struct wifi_msg_sdhc_ready_evt_t
{
	uint8	fhandle;
	uint8	operation;
	uint16	result;
});

PACKSTRUCT(struct wifi_msg_sdhc_fdata_evt_t
{
	uint8	fhandle;
	uint16array	data;
});

PACKSTRUCT(struct wifi_msg_sdhc_ffile_evt_t
{
	uint8	fhandle;
	uint32	fsize;
	uint8	fattrib;
	uint8array	fname;
});

PACKSTRUCT(struct wifi_msg_sdhc_fpwd_evt_t
{
	uint8array	fdir;
});

PACKSTRUCT(struct wifi_msg_util_atoi_cmd_t
{
	uint8array	string1;
});

PACKSTRUCT(struct wifi_msg_util_atoi_rsp_t
{
	int32	value;
});

PACKSTRUCT(struct wifi_msg_util_itoa_cmd_t
{
	int32	value;
});

PACKSTRUCT(struct wifi_msg_util_itoa_rsp_t
{
	uint8array	string1;
});

PACKSTRUCT(
struct wifi_cmd_packet
{
	uint32   header;

union{
	uint8 handle;

	uint8 payload;

	struct wifi_msg_dfu_reset_cmd_t                           cmd_dfu_reset;
	struct wifi_msg_dfu_flash_set_address_cmd_t               cmd_dfu_flash_set_address;
	struct wifi_msg_dfu_flash_upload_cmd_t                    cmd_dfu_flash_upload;
	struct wifi_msg_dfu_boot_evt_t                            evt_dfu_boot;
	struct wifi_msg_dfu_flash_set_address_rsp_t               rsp_dfu_flash_set_address;
	struct wifi_msg_dfu_flash_upload_rsp_t                    rsp_dfu_flash_upload;
	struct wifi_msg_dfu_flash_upload_finish_rsp_t             rsp_dfu_flash_upload_finish;
	struct wifi_msg_system_reset_cmd_t                        cmd_system_reset;
	struct wifi_msg_system_set_max_power_saving_state_cmd_t   cmd_system_set_max_power_saving_state;
	struct wifi_msg_system_boot_evt_t                         evt_system_boot;
	struct wifi_msg_system_state_evt_t                        evt_system_state;
	struct wifi_msg_system_sw_exception_evt_t                 evt_system_sw_exception;
	struct wifi_msg_system_power_saving_state_evt_t           evt_system_power_saving_state;
	struct wifi_msg_system_set_max_power_saving_state_rsp_t   rsp_system_set_max_power_saving_state;
	struct wifi_msg_config_get_mac_cmd_t                      cmd_config_get_mac;
	struct wifi_msg_config_set_mac_cmd_t                      cmd_config_set_mac;
	struct wifi_msg_config_mac_address_evt_t                  evt_config_mac_address;
	struct wifi_msg_config_get_mac_rsp_t                      rsp_config_get_mac;
	struct wifi_msg_config_set_mac_rsp_t                      rsp_config_set_mac;
	struct wifi_msg_sme_power_on_cmd_t                        cmd_sme_power_on;
	struct wifi_msg_sme_start_scan_cmd_t                      cmd_sme_start_scan;
	struct wifi_msg_sme_set_password_cmd_t                    cmd_sme_set_password;
	struct wifi_msg_sme_connect_bssid_cmd_t                   cmd_sme_connect_bssid;
	struct wifi_msg_sme_connect_ssid_cmd_t                    cmd_sme_connect_ssid;
	struct wifi_msg_sme_set_scan_channels_cmd_t               cmd_sme_set_scan_channels;
	struct wifi_msg_sme_set_operating_mode_cmd_t              cmd_sme_set_operating_mode;
	struct wifi_msg_sme_start_ap_mode_cmd_t                   cmd_sme_start_ap_mode;
	struct wifi_msg_sme_scan_results_sort_rssi_cmd_t          cmd_sme_scan_results_sort_rssi;
	struct wifi_msg_sme_ap_client_disconnect_cmd_t            cmd_sme_ap_client_disconnect;
	struct wifi_msg_sme_set_ap_password_cmd_t                 cmd_sme_set_ap_password;
	struct wifi_msg_sme_set_ap_max_clients_cmd_t              cmd_sme_set_ap_max_clients;
	struct wifi_msg_sme_start_ssid_scan_cmd_t                 cmd_sme_start_ssid_scan;
	struct wifi_msg_sme_set_ap_hidden_cmd_t                   cmd_sme_set_ap_hidden;
	struct wifi_msg_sme_set_11n_mode_cmd_t                    cmd_sme_set_11n_mode;
	struct wifi_msg_sme_wifi_is_on_evt_t                      evt_sme_wifi_is_on;
	struct wifi_msg_sme_wifi_is_off_evt_t                     evt_sme_wifi_is_off;
	struct wifi_msg_sme_scan_result_evt_t                     evt_sme_scan_result;
	struct wifi_msg_sme_scan_result_drop_evt_t                evt_sme_scan_result_drop;
	struct wifi_msg_sme_scanned_evt_t                         evt_sme_scanned;
	struct wifi_msg_sme_connected_evt_t                       evt_sme_connected;
	struct wifi_msg_sme_disconnected_evt_t                    evt_sme_disconnected;
	struct wifi_msg_sme_interface_status_evt_t                evt_sme_interface_status;
	struct wifi_msg_sme_connect_failed_evt_t                  evt_sme_connect_failed;
	struct wifi_msg_sme_connect_retry_evt_t                   evt_sme_connect_retry;
	struct wifi_msg_sme_ap_mode_started_evt_t                 evt_sme_ap_mode_started;
	struct wifi_msg_sme_ap_mode_stopped_evt_t                 evt_sme_ap_mode_stopped;
	struct wifi_msg_sme_ap_mode_failed_evt_t                  evt_sme_ap_mode_failed;
	struct wifi_msg_sme_ap_client_joined_evt_t                evt_sme_ap_client_joined;
	struct wifi_msg_sme_ap_client_left_evt_t                  evt_sme_ap_client_left;
	struct wifi_msg_sme_scan_sort_result_evt_t                evt_sme_scan_sort_result;
	struct wifi_msg_sme_wps_stopped_evt_t                     evt_sme_wps_stopped;
	struct wifi_msg_sme_wps_completed_evt_t                   evt_sme_wps_completed;
	struct wifi_msg_sme_wps_failed_evt_t                      evt_sme_wps_failed;
	struct wifi_msg_sme_wps_credential_ssid_evt_t             evt_sme_wps_credential_ssid;
	struct wifi_msg_sme_wps_credential_password_evt_t         evt_sme_wps_credential_password;
	struct wifi_msg_sme_signal_quality_evt_t                  evt_sme_signal_quality;
	struct wifi_msg_sme_wifi_on_rsp_t                         rsp_sme_wifi_on;
	struct wifi_msg_sme_wifi_off_rsp_t                        rsp_sme_wifi_off;
	struct wifi_msg_sme_power_on_rsp_t                        rsp_sme_power_on;
	struct wifi_msg_sme_start_scan_rsp_t                      rsp_sme_start_scan;
	struct wifi_msg_sme_stop_scan_rsp_t                       rsp_sme_stop_scan;
	struct wifi_msg_sme_set_password_rsp_t                    rsp_sme_set_password;
	struct wifi_msg_sme_connect_bssid_rsp_t                   rsp_sme_connect_bssid;
	struct wifi_msg_sme_connect_ssid_rsp_t                    rsp_sme_connect_ssid;
	struct wifi_msg_sme_disconnect_rsp_t                      rsp_sme_disconnect;
	struct wifi_msg_sme_set_scan_channels_rsp_t               rsp_sme_set_scan_channels;
	struct wifi_msg_sme_set_operating_mode_rsp_t              rsp_sme_set_operating_mode;
	struct wifi_msg_sme_start_ap_mode_rsp_t                   rsp_sme_start_ap_mode;
	struct wifi_msg_sme_stop_ap_mode_rsp_t                    rsp_sme_stop_ap_mode;
	struct wifi_msg_sme_scan_results_sort_rssi_rsp_t          rsp_sme_scan_results_sort_rssi;
	struct wifi_msg_sme_ap_client_disconnect_rsp_t            rsp_sme_ap_client_disconnect;
	struct wifi_msg_sme_set_ap_password_rsp_t                 rsp_sme_set_ap_password;
	struct wifi_msg_sme_set_ap_max_clients_rsp_t              rsp_sme_set_ap_max_clients;
	struct wifi_msg_sme_start_wps_rsp_t                       rsp_sme_start_wps;
	struct wifi_msg_sme_stop_wps_rsp_t                        rsp_sme_stop_wps;
	struct wifi_msg_sme_get_signal_quality_rsp_t              rsp_sme_get_signal_quality;
	struct wifi_msg_sme_start_ssid_scan_rsp_t                 rsp_sme_start_ssid_scan;
	struct wifi_msg_sme_set_ap_hidden_rsp_t                   rsp_sme_set_ap_hidden;
	struct wifi_msg_sme_set_11n_mode_rsp_t                    rsp_sme_set_11n_mode;
	struct wifi_msg_tcpip_start_tcp_server_cmd_t              cmd_tcpip_start_tcp_server;
	struct wifi_msg_tcpip_tcp_connect_cmd_t                   cmd_tcpip_tcp_connect;
	struct wifi_msg_tcpip_start_udp_server_cmd_t              cmd_tcpip_start_udp_server;
	struct wifi_msg_tcpip_udp_connect_cmd_t                   cmd_tcpip_udp_connect;
	struct wifi_msg_tcpip_configure_cmd_t                     cmd_tcpip_configure;
	struct wifi_msg_tcpip_dns_configure_cmd_t                 cmd_tcpip_dns_configure;
	struct wifi_msg_tcpip_dns_gethostbyname_cmd_t             cmd_tcpip_dns_gethostbyname;
	struct wifi_msg_tcpip_udp_bind_cmd_t                      cmd_tcpip_udp_bind;
	struct wifi_msg_tcpip_dhcp_set_hostname_cmd_t             cmd_tcpip_dhcp_set_hostname;
	struct wifi_msg_tcpip_dhcp_enable_routing_cmd_t           cmd_tcpip_dhcp_enable_routing;
	struct wifi_msg_tcpip_mdns_set_hostname_cmd_t             cmd_tcpip_mdns_set_hostname;
	struct wifi_msg_tcpip_dnssd_add_service_cmd_t             cmd_tcpip_dnssd_add_service;
	struct wifi_msg_tcpip_dnssd_add_service_instance_cmd_t    cmd_tcpip_dnssd_add_service_instance;
	struct wifi_msg_tcpip_dnssd_add_service_attribute_cmd_t   cmd_tcpip_dnssd_add_service_attribute;
	struct wifi_msg_tcpip_dnssd_remove_service_cmd_t          cmd_tcpip_dnssd_remove_service;
	struct wifi_msg_tcpip_dnssd_start_service_cmd_t           cmd_tcpip_dnssd_start_service;
	struct wifi_msg_tcpip_dnssd_stop_service_cmd_t            cmd_tcpip_dnssd_stop_service;
	struct wifi_msg_tcpip_configuration_evt_t                 evt_tcpip_configuration;
	struct wifi_msg_tcpip_dns_configuration_evt_t             evt_tcpip_dns_configuration;
	struct wifi_msg_tcpip_endpoint_status_evt_t               evt_tcpip_endpoint_status;
	struct wifi_msg_tcpip_dns_gethostbyname_result_evt_t      evt_tcpip_dns_gethostbyname_result;
	struct wifi_msg_tcpip_udp_data_evt_t                      evt_tcpip_udp_data;
	struct wifi_msg_tcpip_mdns_failed_evt_t                   evt_tcpip_mdns_failed;
	struct wifi_msg_tcpip_mdns_stopped_evt_t                  evt_tcpip_mdns_stopped;
	struct wifi_msg_tcpip_dnssd_service_started_evt_t         evt_tcpip_dnssd_service_started;
	struct wifi_msg_tcpip_dnssd_service_failed_evt_t          evt_tcpip_dnssd_service_failed;
	struct wifi_msg_tcpip_dnssd_service_stopped_evt_t         evt_tcpip_dnssd_service_stopped;
	struct wifi_msg_tcpip_start_tcp_server_rsp_t              rsp_tcpip_start_tcp_server;
	struct wifi_msg_tcpip_tcp_connect_rsp_t                   rsp_tcpip_tcp_connect;
	struct wifi_msg_tcpip_start_udp_server_rsp_t              rsp_tcpip_start_udp_server;
	struct wifi_msg_tcpip_udp_connect_rsp_t                   rsp_tcpip_udp_connect;
	struct wifi_msg_tcpip_configure_rsp_t                     rsp_tcpip_configure;
	struct wifi_msg_tcpip_dns_configure_rsp_t                 rsp_tcpip_dns_configure;
	struct wifi_msg_tcpip_dns_gethostbyname_rsp_t             rsp_tcpip_dns_gethostbyname;
	struct wifi_msg_tcpip_udp_bind_rsp_t                      rsp_tcpip_udp_bind;
	struct wifi_msg_tcpip_dhcp_set_hostname_rsp_t             rsp_tcpip_dhcp_set_hostname;
	struct wifi_msg_tcpip_dhcp_enable_routing_rsp_t           rsp_tcpip_dhcp_enable_routing;
	struct wifi_msg_tcpip_mdns_set_hostname_rsp_t             rsp_tcpip_mdns_set_hostname;
	struct wifi_msg_tcpip_mdns_start_rsp_t                    rsp_tcpip_mdns_start;
	struct wifi_msg_tcpip_mdns_stop_rsp_t                     rsp_tcpip_mdns_stop;
	struct wifi_msg_tcpip_dnssd_add_service_rsp_t             rsp_tcpip_dnssd_add_service;
	struct wifi_msg_tcpip_dnssd_add_service_instance_rsp_t    rsp_tcpip_dnssd_add_service_instance;
	struct wifi_msg_tcpip_dnssd_add_service_attribute_rsp_t   rsp_tcpip_dnssd_add_service_attribute;
	struct wifi_msg_tcpip_dnssd_remove_service_rsp_t          rsp_tcpip_dnssd_remove_service;
	struct wifi_msg_tcpip_dnssd_start_service_rsp_t           rsp_tcpip_dnssd_start_service;
	struct wifi_msg_tcpip_dnssd_stop_service_rsp_t            rsp_tcpip_dnssd_stop_service;
	struct wifi_msg_endpoint_send_cmd_t                       cmd_endpoint_send;
	struct wifi_msg_endpoint_set_streaming_cmd_t              cmd_endpoint_set_streaming;
	struct wifi_msg_endpoint_set_active_cmd_t                 cmd_endpoint_set_active;
	struct wifi_msg_endpoint_set_streaming_destination_cmd_t  cmd_endpoint_set_streaming_destination;
	struct wifi_msg_endpoint_close_cmd_t                      cmd_endpoint_close;
	struct wifi_msg_endpoint_set_transmit_size_cmd_t          cmd_endpoint_set_transmit_size;
	struct wifi_msg_endpoint_disable_cmd_t                    cmd_endpoint_disable;
	struct wifi_msg_endpoint_syntax_error_evt_t               evt_endpoint_syntax_error;
	struct wifi_msg_endpoint_data_evt_t                       evt_endpoint_data;
	struct wifi_msg_endpoint_status_evt_t                     evt_endpoint_status;
	struct wifi_msg_endpoint_closing_evt_t                    evt_endpoint_closing;
	struct wifi_msg_endpoint_error_evt_t                      evt_endpoint_error;
	struct wifi_msg_endpoint_send_rsp_t                       rsp_endpoint_send;
	struct wifi_msg_endpoint_set_streaming_rsp_t              rsp_endpoint_set_streaming;
	struct wifi_msg_endpoint_set_active_rsp_t                 rsp_endpoint_set_active;
	struct wifi_msg_endpoint_set_streaming_destination_rsp_t  rsp_endpoint_set_streaming_destination;
	struct wifi_msg_endpoint_close_rsp_t                      rsp_endpoint_close;
	struct wifi_msg_endpoint_set_transmit_size_rsp_t          rsp_endpoint_set_transmit_size;
	struct wifi_msg_endpoint_disable_rsp_t                    rsp_endpoint_disable;
	struct wifi_msg_hardware_set_soft_timer_cmd_t             cmd_hardware_set_soft_timer;
	struct wifi_msg_hardware_external_interrupt_config_cmd_t  cmd_hardware_external_interrupt_config;
	struct wifi_msg_hardware_change_notification_config_cmd_t cmd_hardware_change_notification_config;
	struct wifi_msg_hardware_change_notification_pullup_cmd_t cmd_hardware_change_notification_pullup;
	struct wifi_msg_hardware_io_port_config_direction_cmd_t   cmd_hardware_io_port_config_direction;
	struct wifi_msg_hardware_io_port_config_open_drain_cmd_t  cmd_hardware_io_port_config_open_drain;
	struct wifi_msg_hardware_io_port_write_cmd_t              cmd_hardware_io_port_write;
	struct wifi_msg_hardware_io_port_read_cmd_t               cmd_hardware_io_port_read;
	struct wifi_msg_hardware_output_compare_cmd_t             cmd_hardware_output_compare;
	struct wifi_msg_hardware_adc_read_cmd_t                   cmd_hardware_adc_read;
	struct wifi_msg_hardware_rtc_init_cmd_t                   cmd_hardware_rtc_init;
	struct wifi_msg_hardware_rtc_set_time_cmd_t               cmd_hardware_rtc_set_time;
	struct wifi_msg_hardware_rtc_set_alarm_cmd_t              cmd_hardware_rtc_set_alarm;
	struct wifi_msg_hardware_uart_conf_set_cmd_t              cmd_hardware_uart_conf_set;
	struct wifi_msg_hardware_uart_conf_get_cmd_t              cmd_hardware_uart_conf_get;
	struct wifi_msg_hardware_soft_timer_evt_t                 evt_hardware_soft_timer;
	struct wifi_msg_hardware_change_notification_evt_t        evt_hardware_change_notification;
	struct wifi_msg_hardware_external_interrupt_evt_t         evt_hardware_external_interrupt;
	struct wifi_msg_hardware_uart_conf_evt_t                  evt_hardware_uart_conf;
	struct wifi_msg_hardware_set_soft_timer_rsp_t             rsp_hardware_set_soft_timer;
	struct wifi_msg_hardware_external_interrupt_config_rsp_t  rsp_hardware_external_interrupt_config;
	struct wifi_msg_hardware_change_notification_config_rsp_t rsp_hardware_change_notification_config;
	struct wifi_msg_hardware_change_notification_pullup_rsp_t rsp_hardware_change_notification_pullup;
	struct wifi_msg_hardware_io_port_config_direction_rsp_t   rsp_hardware_io_port_config_direction;
	struct wifi_msg_hardware_io_port_config_open_drain_rsp_t  rsp_hardware_io_port_config_open_drain;
	struct wifi_msg_hardware_io_port_write_rsp_t              rsp_hardware_io_port_write;
	struct wifi_msg_hardware_io_port_read_rsp_t               rsp_hardware_io_port_read;
	struct wifi_msg_hardware_output_compare_rsp_t             rsp_hardware_output_compare;
	struct wifi_msg_hardware_adc_read_rsp_t                   rsp_hardware_adc_read;
	struct wifi_msg_hardware_rtc_init_rsp_t                   rsp_hardware_rtc_init;
	struct wifi_msg_hardware_rtc_set_time_rsp_t               rsp_hardware_rtc_set_time;
	struct wifi_msg_hardware_rtc_get_time_rsp_t               rsp_hardware_rtc_get_time;
	struct wifi_msg_hardware_rtc_set_alarm_rsp_t              rsp_hardware_rtc_set_alarm;
	struct wifi_msg_hardware_uart_conf_set_rsp_t              rsp_hardware_uart_conf_set;
	struct wifi_msg_hardware_uart_conf_get_rsp_t              rsp_hardware_uart_conf_get;
	struct wifi_msg_flash_ps_save_cmd_t                       cmd_flash_ps_save;
	struct wifi_msg_flash_ps_load_cmd_t                       cmd_flash_ps_load;
	struct wifi_msg_flash_ps_erase_cmd_t                      cmd_flash_ps_erase;
	struct wifi_msg_flash_ps_key_evt_t                        evt_flash_ps_key;
	struct wifi_msg_flash_ps_key_changed_evt_t                evt_flash_ps_key_changed;
	struct wifi_msg_flash_ps_defrag_rsp_t                     rsp_flash_ps_defrag;
	struct wifi_msg_flash_ps_dump_rsp_t                       rsp_flash_ps_dump;
	struct wifi_msg_flash_ps_erase_all_rsp_t                  rsp_flash_ps_erase_all;
	struct wifi_msg_flash_ps_save_rsp_t                       rsp_flash_ps_save;
	struct wifi_msg_flash_ps_load_rsp_t                       rsp_flash_ps_load;
	struct wifi_msg_flash_ps_erase_rsp_t                      rsp_flash_ps_erase;
	struct wifi_msg_i2c_start_read_cmd_t                      cmd_i2c_start_read;
	struct wifi_msg_i2c_start_write_cmd_t                     cmd_i2c_start_write;
	struct wifi_msg_i2c_stop_cmd_t                            cmd_i2c_stop;
	struct wifi_msg_i2c_start_read_rsp_t                      rsp_i2c_start_read;
	struct wifi_msg_i2c_start_write_rsp_t                     rsp_i2c_start_write;
	struct wifi_msg_i2c_stop_rsp_t                            rsp_i2c_stop;
	struct wifi_msg_https_enable_cmd_t                        cmd_https_enable;
	struct wifi_msg_https_add_path_cmd_t                      cmd_https_add_path;
	struct wifi_msg_https_api_response_cmd_t                  cmd_https_api_response;
	struct wifi_msg_https_api_response_finish_cmd_t           cmd_https_api_response_finish;
	struct wifi_msg_https_on_req_evt_t                        evt_https_on_req;
	struct wifi_msg_https_button_evt_t                        evt_https_button;
	struct wifi_msg_https_api_request_evt_t                   evt_https_api_request;
	struct wifi_msg_https_api_request_header_evt_t            evt_https_api_request_header;
	struct wifi_msg_https_api_request_data_evt_t              evt_https_api_request_data;
	struct wifi_msg_https_api_request_finished_evt_t          evt_https_api_request_finished;
	struct wifi_msg_https_enable_rsp_t                        rsp_https_enable;
	struct wifi_msg_https_add_path_rsp_t                      rsp_https_add_path;
	struct wifi_msg_https_api_response_rsp_t                  rsp_https_api_response;
	struct wifi_msg_https_api_response_finish_rsp_t           rsp_https_api_response_finish;
	struct wifi_msg_ethernet_set_dataroute_cmd_t              cmd_ethernet_set_dataroute;
	struct wifi_msg_ethernet_link_status_evt_t                evt_ethernet_link_status;
	struct wifi_msg_ethernet_set_dataroute_rsp_t              rsp_ethernet_set_dataroute;
	struct wifi_msg_ethernet_connected_rsp_t                  rsp_ethernet_connected;
	struct wifi_msg_sdhc_fopen_cmd_t                          cmd_sdhc_fopen;
	struct wifi_msg_sdhc_fclose_cmd_t                         cmd_sdhc_fclose;
	struct wifi_msg_sdhc_fdir_cmd_t                           cmd_sdhc_fdir;
	struct wifi_msg_sdhc_fread_cmd_t                          cmd_sdhc_fread;
	struct wifi_msg_sdhc_fwrite_cmd_t                         cmd_sdhc_fwrite;
	struct wifi_msg_sdhc_fdelete_cmd_t                        cmd_sdhc_fdelete;
	struct wifi_msg_sdhc_fmkdir_cmd_t                         cmd_sdhc_fmkdir;
	struct wifi_msg_sdhc_fchdir_cmd_t                         cmd_sdhc_fchdir;
	struct wifi_msg_sdhc_frename_cmd_t                        cmd_sdhc_frename;
	struct wifi_msg_sdhc_fchmode_cmd_t                        cmd_sdhc_fchmode;
	struct wifi_msg_sdhc_ready_evt_t                          evt_sdhc_ready;
	struct wifi_msg_sdhc_fdata_evt_t                          evt_sdhc_fdata;
	struct wifi_msg_sdhc_ffile_evt_t                          evt_sdhc_ffile;
	struct wifi_msg_sdhc_fpwd_evt_t                           evt_sdhc_fpwd;
	struct wifi_msg_sdhc_fopen_rsp_t                          rsp_sdhc_fopen;
	struct wifi_msg_sdhc_fclose_rsp_t                         rsp_sdhc_fclose;
	struct wifi_msg_sdhc_fdir_rsp_t                           rsp_sdhc_fdir;
	struct wifi_msg_sdhc_fread_rsp_t                          rsp_sdhc_fread;
	struct wifi_msg_sdhc_fwrite_rsp_t                         rsp_sdhc_fwrite;
	struct wifi_msg_sdhc_fdelete_rsp_t                        rsp_sdhc_fdelete;
	struct wifi_msg_sdhc_fmkdir_rsp_t                         rsp_sdhc_fmkdir;
	struct wifi_msg_sdhc_fchdir_rsp_t                         rsp_sdhc_fchdir;
	struct wifi_msg_sdhc_frename_rsp_t                        rsp_sdhc_frename;
	struct wifi_msg_sdhc_fchmode_rsp_t                        rsp_sdhc_fchmode;
	struct wifi_msg_util_atoi_cmd_t                           cmd_util_atoi;
	struct wifi_msg_util_itoa_cmd_t                           cmd_util_itoa;
	struct wifi_msg_util_atoi_rsp_t                           rsp_util_atoi;
	struct wifi_msg_util_itoa_rsp_t                           rsp_util_itoa;
};

}ALIGNED);

extern struct wifi_cmd_packet bglib_temp_msg;

/**Reset system**/
#define wifi_cmd_dfu_reset(DFU) \
{\
bglib_temp_msg.cmd_dfu_reset.dfu=(DFU);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)1+0)>>8)))|((((uint32)1+0)&0xff)<<8)|((uint32)0x0<<16)|((uint32)0x0<<24);\
bglib_output (4+1,(uint8*)&bglib_temp_msg,0,(uint8*)NULL);\
}
/**This command set address on the flash memory for writing data.**/
#define wifi_cmd_dfu_flash_set_address(ADDRESS) \
{\
bglib_temp_msg.cmd_dfu_flash_set_address.address=(ADDRESS);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)4+0)>>8)))|((((uint32)4+0)&0xff)<<8)|((uint32)0x0<<16)|((uint32)0x1<<24);\
bglib_output (4+4,(uint8*)&bglib_temp_msg,0,(uint8*)NULL);\
}
/**Upload binary for flashing. Address will be updated automatically.**/
#define wifi_cmd_dfu_flash_upload(DATA_LEN,DATA_DATA) \
{\
bglib_temp_msg.cmd_dfu_flash_upload.data.len=(DATA_LEN);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)1+bglib_temp_msg.cmd_dfu_flash_upload.data.len)>>8)))|((((uint32)1+bglib_temp_msg.cmd_dfu_flash_upload.data.len)&0xff)<<8)|((uint32)0x0<<16)|((uint32)0x2<<24);\
bglib_output (4+1,(uint8*)&bglib_temp_msg,bglib_temp_msg.cmd_dfu_flash_upload.data.len,(uint8*)DATA_DATA);\
}
/**Uploading is finished.**/
#define wifi_cmd_dfu_flash_upload_finish() \
{\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)0+0)>>8)))|((((uint32)0+0)&0xff)<<8)|((uint32)0x0<<16)|((uint32)0x3<<24);\
bglib_output (4+0,(uint8*)&bglib_temp_msg,0,(uint8*)NULL);\
}
/**Sync system state**/
#define wifi_cmd_system_sync() \
{\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)0+0)>>8)))|((((uint32)0+0)&0xff)<<8)|((uint32)0x1<<16)|((uint32)0x0<<24);\
bglib_output (4+0,(uint8*)&bglib_temp_msg,0,(uint8*)NULL);\
}
/**Reset system**/
#define wifi_cmd_system_reset(DFU) \
{\
bglib_temp_msg.cmd_system_reset.dfu=(DFU);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)1+0)>>8)))|((((uint32)1+0)&0xff)<<8)|((uint32)0x1<<16)|((uint32)0x1<<24);\
bglib_output (4+1,(uint8*)&bglib_temp_msg,0,(uint8*)NULL);\
}
/**Hello (no-op)**/
#define wifi_cmd_system_hello() \
{\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)0+0)>>8)))|((((uint32)0+0)&0xff)<<8)|((uint32)0x1<<16)|((uint32)0x2<<24);\
bglib_output (4+0,(uint8*)&bglib_temp_msg,0,(uint8*)NULL);\
}
/**set maximum state for power saving**/
#define wifi_cmd_system_set_max_power_saving_state(STATE) \
{\
bglib_temp_msg.cmd_system_set_max_power_saving_state.state=(STATE);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)1+0)>>8)))|((((uint32)1+0)&0xff)<<8)|((uint32)0x1<<16)|((uint32)0x3<<24);\
bglib_output (4+1,(uint8*)&bglib_temp_msg,0,(uint8*)NULL);\
}
/**Get MAC address**/
#define wifi_cmd_config_get_mac(HW_INTERFACE) \
{\
bglib_temp_msg.cmd_config_get_mac.hw_interface=(HW_INTERFACE);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)1+0)>>8)))|((((uint32)1+0)&0xff)<<8)|((uint32)0x2<<16)|((uint32)0x0<<24);\
bglib_output (4+1,(uint8*)&bglib_temp_msg,0,(uint8*)NULL);\
}
/**Set MAC address**/
#define wifi_cmd_config_set_mac(HW_INTERFACE,MAC) \
{\
bglib_temp_msg.cmd_config_set_mac.hw_interface=(HW_INTERFACE);\
memcpy(&bglib_temp_msg.cmd_config_set_mac.mac,(MAC),sizeof(hw_addr));\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)7+0)>>8)))|((((uint32)7+0)&0xff)<<8)|((uint32)0x2<<16)|((uint32)0x1<<24);\
bglib_output (4+7,(uint8*)&bglib_temp_msg,0,(uint8*)NULL);\
}
/**Turn Wi-Fi on**/
#define wifi_cmd_sme_wifi_on() \
{\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)0+0)>>8)))|((((uint32)0+0)&0xff)<<8)|((uint32)0x3<<16)|((uint32)0x0<<24);\
bglib_output (4+0,(uint8*)&bglib_temp_msg,0,(uint8*)NULL);\
}
/**Turn wifi off**/
#define wifi_cmd_sme_wifi_off() \
{\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)0+0)>>8)))|((((uint32)0+0)&0xff)<<8)|((uint32)0x3<<16)|((uint32)0x1<<24);\
bglib_output (4+0,(uint8*)&bglib_temp_msg,0,(uint8*)NULL);\
}
/**Turn on or off the power to the Wi-Fi chip**/
#define wifi_cmd_sme_power_on(ENABLE) \
{\
bglib_temp_msg.cmd_sme_power_on.enable=(ENABLE);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)1+0)>>8)))|((((uint32)1+0)&0xff)<<8)|((uint32)0x3<<16)|((uint32)0x2<<24);\
bglib_output (4+1,(uint8*)&bglib_temp_msg,0,(uint8*)NULL);\
}
/**Initiate the scan procedure**/
#define wifi_cmd_sme_start_scan(HW_INTERFACE,CHLIST_LEN,CHLIST_DATA) \
{\
bglib_temp_msg.cmd_sme_start_scan.hw_interface=(HW_INTERFACE);\
bglib_temp_msg.cmd_sme_start_scan.chList.len=(CHLIST_LEN);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)2+bglib_temp_msg.cmd_sme_start_scan.chList.len)>>8)))|((((uint32)2+bglib_temp_msg.cmd_sme_start_scan.chList.len)&0xff)<<8)|((uint32)0x3<<16)|((uint32)0x3<<24);\
bglib_output (4+2,(uint8*)&bglib_temp_msg,bglib_temp_msg.cmd_sme_start_scan.chList.len,(uint8*)CHLIST_DATA);\
}
/**Terminate the scan procedure**/
#define wifi_cmd_sme_stop_scan() \
{\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)0+0)>>8)))|((((uint32)0+0)&0xff)<<8)|((uint32)0x3<<16)|((uint32)0x4<<24);\
bglib_output (4+0,(uint8*)&bglib_temp_msg,0,(uint8*)NULL);\
}
/**Set the Wi-Fi password**/
#define wifi_cmd_sme_set_password(PASSWORD_LEN,PASSWORD_DATA) \
{\
bglib_temp_msg.cmd_sme_set_password.password.len=(PASSWORD_LEN);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)1+bglib_temp_msg.cmd_sme_set_password.password.len)>>8)))|((((uint32)1+bglib_temp_msg.cmd_sme_set_password.password.len)&0xff)<<8)|((uint32)0x3<<16)|((uint32)0x5<<24);\
bglib_output (4+1,(uint8*)&bglib_temp_msg,bglib_temp_msg.cmd_sme_set_password.password.len,(uint8*)PASSWORD_DATA);\
}
/**Connect to a specific Access Point using its unique BSSID**/
#define wifi_cmd_sme_connect_bssid(BSSID) \
{\
memcpy(&bglib_temp_msg.cmd_sme_connect_bssid.bssid,(BSSID),sizeof(hw_addr));\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)6+0)>>8)))|((((uint32)6+0)&0xff)<<8)|((uint32)0x3<<16)|((uint32)0x6<<24);\
bglib_output (4+6,(uint8*)&bglib_temp_msg,0,(uint8*)NULL);\
}
/**Connect to an Access Point which belongs to the network identified with the given SSID**/
#define wifi_cmd_sme_connect_ssid(SSID_LEN,SSID_DATA) \
{\
bglib_temp_msg.cmd_sme_connect_ssid.ssid.len=(SSID_LEN);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)1+bglib_temp_msg.cmd_sme_connect_ssid.ssid.len)>>8)))|((((uint32)1+bglib_temp_msg.cmd_sme_connect_ssid.ssid.len)&0xff)<<8)|((uint32)0x3<<16)|((uint32)0x7<<24);\
bglib_output (4+1,(uint8*)&bglib_temp_msg,bglib_temp_msg.cmd_sme_connect_ssid.ssid.len,(uint8*)SSID_DATA);\
}
/**Disconnect from the Access Point**/
#define wifi_cmd_sme_disconnect() \
{\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)0+0)>>8)))|((((uint32)0+0)&0xff)<<8)|((uint32)0x3<<16)|((uint32)0x8<<24);\
bglib_output (4+0,(uint8*)&bglib_temp_msg,0,(uint8*)NULL);\
}
/**Set default scan channel list**/
#define wifi_cmd_sme_set_scan_channels(HW_INTERFACE,CHLIST_LEN,CHLIST_DATA) \
{\
bglib_temp_msg.cmd_sme_set_scan_channels.hw_interface=(HW_INTERFACE);\
bglib_temp_msg.cmd_sme_set_scan_channels.chList.len=(CHLIST_LEN);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)2+bglib_temp_msg.cmd_sme_set_scan_channels.chList.len)>>8)))|((((uint32)2+bglib_temp_msg.cmd_sme_set_scan_channels.chList.len)&0xff)<<8)|((uint32)0x3<<16)|((uint32)0x9<<24);\
bglib_output (4+2,(uint8*)&bglib_temp_msg,bglib_temp_msg.cmd_sme_set_scan_channels.chList.len,(uint8*)CHLIST_DATA);\
}
/**Set Wi-Fi operating mode**/
#define wifi_cmd_sme_set_operating_mode(MODE) \
{\
bglib_temp_msg.cmd_sme_set_operating_mode.mode=(MODE);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)1+0)>>8)))|((((uint32)1+0)&0xff)<<8)|((uint32)0x3<<16)|((uint32)0xa<<24);\
bglib_output (4+1,(uint8*)&bglib_temp_msg,0,(uint8*)NULL);\
}
/**Start Access Point mode**/
#define wifi_cmd_sme_start_ap_mode(CHANNEL,SECURITY,SSID_LEN,SSID_DATA) \
{\
bglib_temp_msg.cmd_sme_start_ap_mode.channel=(CHANNEL);\
bglib_temp_msg.cmd_sme_start_ap_mode.security=(SECURITY);\
bglib_temp_msg.cmd_sme_start_ap_mode.ssid.len=(SSID_LEN);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)3+bglib_temp_msg.cmd_sme_start_ap_mode.ssid.len)>>8)))|((((uint32)3+bglib_temp_msg.cmd_sme_start_ap_mode.ssid.len)&0xff)<<8)|((uint32)0x3<<16)|((uint32)0xb<<24);\
bglib_output (4+3,(uint8*)&bglib_temp_msg,bglib_temp_msg.cmd_sme_start_ap_mode.ssid.len,(uint8*)SSID_DATA);\
}
/**Stop Access Point mode**/
#define wifi_cmd_sme_stop_ap_mode() \
{\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)0+0)>>8)))|((((uint32)0+0)&0xff)<<8)|((uint32)0x3<<16)|((uint32)0xc<<24);\
bglib_output (4+0,(uint8*)&bglib_temp_msg,0,(uint8*)NULL);\
}
/**resend scan results, sorted by rssi**/
#define wifi_cmd_sme_scan_results_sort_rssi(AMOUNT) \
{\
bglib_temp_msg.cmd_sme_scan_results_sort_rssi.amount=(AMOUNT);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)1+0)>>8)))|((((uint32)1+0)&0xff)<<8)|((uint32)0x3<<16)|((uint32)0xd<<24);\
bglib_output (4+1,(uint8*)&bglib_temp_msg,0,(uint8*)NULL);\
}
/**Disconnect a station from the network.**/
#define wifi_cmd_sme_ap_client_disconnect(MAC_ADDRESS) \
{\
memcpy(&bglib_temp_msg.cmd_sme_ap_client_disconnect.mac_address,(MAC_ADDRESS),sizeof(hw_addr));\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)6+0)>>8)))|((((uint32)6+0)&0xff)<<8)|((uint32)0x3<<16)|((uint32)0xe<<24);\
bglib_output (4+6,(uint8*)&bglib_temp_msg,0,(uint8*)NULL);\
}
/**Set the Wi-Fi password for Access Point**/
#define wifi_cmd_sme_set_ap_password(PASSWORD_LEN,PASSWORD_DATA) \
{\
bglib_temp_msg.cmd_sme_set_ap_password.password.len=(PASSWORD_LEN);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)1+bglib_temp_msg.cmd_sme_set_ap_password.password.len)>>8)))|((((uint32)1+bglib_temp_msg.cmd_sme_set_ap_password.password.len)&0xff)<<8)|((uint32)0x3<<16)|((uint32)0xf<<24);\
bglib_output (4+1,(uint8*)&bglib_temp_msg,bglib_temp_msg.cmd_sme_set_ap_password.password.len,(uint8*)PASSWORD_DATA);\
}
/**Set the maximum amount of stations that can be associated to the Access Point at the same time.**/
#define wifi_cmd_sme_set_ap_max_clients(MAX_CLIENTS) \
{\
bglib_temp_msg.cmd_sme_set_ap_max_clients.max_clients=(MAX_CLIENTS);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)1+0)>>8)))|((((uint32)1+0)&0xff)<<8)|((uint32)0x3<<16)|((uint32)0x10<<24);\
bglib_output (4+1,(uint8*)&bglib_temp_msg,0,(uint8*)NULL);\
}
/**Start Wi-Fi Protected Setup session**/
#define wifi_cmd_sme_start_wps() \
{\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)0+0)>>8)))|((((uint32)0+0)&0xff)<<8)|((uint32)0x3<<16)|((uint32)0x11<<24);\
bglib_output (4+0,(uint8*)&bglib_temp_msg,0,(uint8*)NULL);\
}
/**Stop Wi-Fi Protected Setup session**/
#define wifi_cmd_sme_stop_wps() \
{\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)0+0)>>8)))|((((uint32)0+0)&0xff)<<8)|((uint32)0x3<<16)|((uint32)0x12<<24);\
bglib_output (4+0,(uint8*)&bglib_temp_msg,0,(uint8*)NULL);\
}
/**Get connection signal quality**/
#define wifi_cmd_sme_get_signal_quality() \
{\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)0+0)>>8)))|((((uint32)0+0)&0xff)<<8)|((uint32)0x3<<16)|((uint32)0x13<<24);\
bglib_output (4+0,(uint8*)&bglib_temp_msg,0,(uint8*)NULL);\
}
/**Initiate the SSID scan procedure**/
#define wifi_cmd_sme_start_ssid_scan(SSID_LEN,SSID_DATA) \
{\
bglib_temp_msg.cmd_sme_start_ssid_scan.ssid.len=(SSID_LEN);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)1+bglib_temp_msg.cmd_sme_start_ssid_scan.ssid.len)>>8)))|((((uint32)1+bglib_temp_msg.cmd_sme_start_ssid_scan.ssid.len)&0xff)<<8)|((uint32)0x3<<16)|((uint32)0x14<<24);\
bglib_output (4+1,(uint8*)&bglib_temp_msg,bglib_temp_msg.cmd_sme_start_ssid_scan.ssid.len,(uint8*)SSID_DATA);\
}
/**Set whether the Access Point is hidden or not.**/
#define wifi_cmd_sme_set_ap_hidden(HIDDEN) \
{\
bglib_temp_msg.cmd_sme_set_ap_hidden.hidden=(HIDDEN);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)1+0)>>8)))|((((uint32)1+0)&0xff)<<8)|((uint32)0x3<<16)|((uint32)0x15<<24);\
bglib_output (4+1,(uint8*)&bglib_temp_msg,0,(uint8*)NULL);\
}
/**Set whether 802.11n mode is enabled or not.**/
#define wifi_cmd_sme_set_11n_mode(MODE) \
{\
bglib_temp_msg.cmd_sme_set_11n_mode.mode=(MODE);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)1+0)>>8)))|((((uint32)1+0)&0xff)<<8)|((uint32)0x3<<16)|((uint32)0x16<<24);\
bglib_output (4+1,(uint8*)&bglib_temp_msg,0,(uint8*)NULL);\
}
/**Start TCP server**/
#define wifi_cmd_tcpip_start_tcp_server(PORT,DEFAULT_DESTINATION) \
{\
bglib_temp_msg.cmd_tcpip_start_tcp_server.port=(PORT);\
bglib_temp_msg.cmd_tcpip_start_tcp_server.default_destination=(DEFAULT_DESTINATION);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)3+0)>>8)))|((((uint32)3+0)&0xff)<<8)|((uint32)0x4<<16)|((uint32)0x0<<24);\
bglib_output (4+3,(uint8*)&bglib_temp_msg,0,(uint8*)NULL);\
}
/**Attempt to create a new TCP connection**/
#define wifi_cmd_tcpip_tcp_connect(ADDRESS,PORT,ROUTING) \
{\
bglib_temp_msg.cmd_tcpip_tcp_connect.address.u=(ADDRESS);\
bglib_temp_msg.cmd_tcpip_tcp_connect.port=(PORT);\
bglib_temp_msg.cmd_tcpip_tcp_connect.routing=(ROUTING);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)7+0)>>8)))|((((uint32)7+0)&0xff)<<8)|((uint32)0x4<<16)|((uint32)0x1<<24);\
bglib_output (4+7,(uint8*)&bglib_temp_msg,0,(uint8*)NULL);\
}
/**Create a new UDP Server**/
#define wifi_cmd_tcpip_start_udp_server(PORT,DEFAULT_DESTINATION) \
{\
bglib_temp_msg.cmd_tcpip_start_udp_server.port=(PORT);\
bglib_temp_msg.cmd_tcpip_start_udp_server.default_destination=(DEFAULT_DESTINATION);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)3+0)>>8)))|((((uint32)3+0)&0xff)<<8)|((uint32)0x4<<16)|((uint32)0x2<<24);\
bglib_output (4+3,(uint8*)&bglib_temp_msg,0,(uint8*)NULL);\
}
/**Create a new UDP connection**/
#define wifi_cmd_tcpip_udp_connect(ADDRESS,PORT,ROUTING) \
{\
bglib_temp_msg.cmd_tcpip_udp_connect.address.u=(ADDRESS);\
bglib_temp_msg.cmd_tcpip_udp_connect.port=(PORT);\
bglib_temp_msg.cmd_tcpip_udp_connect.routing=(ROUTING);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)7+0)>>8)))|((((uint32)7+0)&0xff)<<8)|((uint32)0x4<<16)|((uint32)0x3<<24);\
bglib_output (4+7,(uint8*)&bglib_temp_msg,0,(uint8*)NULL);\
}
/**Configure TCP/IP**/
#define wifi_cmd_tcpip_configure(ADDRESS,NETMASK,GATEWAY,USE_DHCP) \
{\
bglib_temp_msg.cmd_tcpip_configure.address.u=(ADDRESS);\
bglib_temp_msg.cmd_tcpip_configure.netmask.u=(NETMASK);\
bglib_temp_msg.cmd_tcpip_configure.gateway.u=(GATEWAY);\
bglib_temp_msg.cmd_tcpip_configure.use_dhcp=(USE_DHCP);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)13+0)>>8)))|((((uint32)13+0)&0xff)<<8)|((uint32)0x4<<16)|((uint32)0x4<<24);\
bglib_output (4+13,(uint8*)&bglib_temp_msg,0,(uint8*)NULL);\
}
/**Configure DNS**/
#define wifi_cmd_tcpip_dns_configure(INDEX,ADDRESS) \
{\
bglib_temp_msg.cmd_tcpip_dns_configure.index=(INDEX);\
bglib_temp_msg.cmd_tcpip_dns_configure.address.u=(ADDRESS);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)5+0)>>8)))|((((uint32)5+0)&0xff)<<8)|((uint32)0x4<<16)|((uint32)0x5<<24);\
bglib_output (4+5,(uint8*)&bglib_temp_msg,0,(uint8*)NULL);\
}
/**Start resolving a hostname into an IP address using the configured DNS server**/
#define wifi_cmd_tcpip_dns_gethostbyname(NAME_LEN,NAME_DATA) \
{\
bglib_temp_msg.cmd_tcpip_dns_gethostbyname.name.len=(NAME_LEN);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)1+bglib_temp_msg.cmd_tcpip_dns_gethostbyname.name.len)>>8)))|((((uint32)1+bglib_temp_msg.cmd_tcpip_dns_gethostbyname.name.len)&0xff)<<8)|((uint32)0x4<<16)|((uint32)0x6<<24);\
bglib_output (4+1,(uint8*)&bglib_temp_msg,bglib_temp_msg.cmd_tcpip_dns_gethostbyname.name.len,(uint8*)NAME_DATA);\
}
/**Bind existing UDP connection to specific source port**/
#define wifi_cmd_tcpip_udp_bind(ENDPOINT,PORT) \
{\
bglib_temp_msg.cmd_tcpip_udp_bind.endpoint=(ENDPOINT);\
bglib_temp_msg.cmd_tcpip_udp_bind.port=(PORT);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)3+0)>>8)))|((((uint32)3+0)&0xff)<<8)|((uint32)0x4<<16)|((uint32)0x7<<24);\
bglib_output (4+3,(uint8*)&bglib_temp_msg,0,(uint8*)NULL);\
}
/**Set DHCP hostname parameter**/
#define wifi_cmd_tcpip_dhcp_set_hostname(HOSTNAME_LEN,HOSTNAME_DATA) \
{\
bglib_temp_msg.cmd_tcpip_dhcp_set_hostname.hostname.len=(HOSTNAME_LEN);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)1+bglib_temp_msg.cmd_tcpip_dhcp_set_hostname.hostname.len)>>8)))|((((uint32)1+bglib_temp_msg.cmd_tcpip_dhcp_set_hostname.hostname.len)&0xff)<<8)|((uint32)0x4<<16)|((uint32)0x8<<24);\
bglib_output (4+1,(uint8*)&bglib_temp_msg,bglib_temp_msg.cmd_tcpip_dhcp_set_hostname.hostname.len,(uint8*)HOSTNAME_DATA);\
}
/**Enable/disable gateway and DNS router options in DHCP server offer and ack. Options enabled by default.**/
#define wifi_cmd_tcpip_dhcp_enable_routing(ENABLE) \
{\
bglib_temp_msg.cmd_tcpip_dhcp_enable_routing.enable=(ENABLE);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)1+0)>>8)))|((((uint32)1+0)&0xff)<<8)|((uint32)0x4<<16)|((uint32)0x9<<24);\
bglib_output (4+1,(uint8*)&bglib_temp_msg,0,(uint8*)NULL);\
}
/**Set mDNS hostname**/
#define wifi_cmd_tcpip_mdns_set_hostname(HOSTNAME_LEN,HOSTNAME_DATA) \
{\
bglib_temp_msg.cmd_tcpip_mdns_set_hostname.hostname.len=(HOSTNAME_LEN);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)1+bglib_temp_msg.cmd_tcpip_mdns_set_hostname.hostname.len)>>8)))|((((uint32)1+bglib_temp_msg.cmd_tcpip_mdns_set_hostname.hostname.len)&0xff)<<8)|((uint32)0x4<<16)|((uint32)0xa<<24);\
bglib_output (4+1,(uint8*)&bglib_temp_msg,bglib_temp_msg.cmd_tcpip_mdns_set_hostname.hostname.len,(uint8*)HOSTNAME_DATA);\
}
/**Start mDNS service**/
#define wifi_cmd_tcpip_mdns_start() \
{\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)0+0)>>8)))|((((uint32)0+0)&0xff)<<8)|((uint32)0x4<<16)|((uint32)0xb<<24);\
bglib_output (4+0,(uint8*)&bglib_temp_msg,0,(uint8*)NULL);\
}
/**Stop mDNS service**/
#define wifi_cmd_tcpip_mdns_stop() \
{\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)0+0)>>8)))|((((uint32)0+0)&0xff)<<8)|((uint32)0x4<<16)|((uint32)0xc<<24);\
bglib_output (4+0,(uint8*)&bglib_temp_msg,0,(uint8*)NULL);\
}
/**Add a new DNS-SD service**/
#define wifi_cmd_tcpip_dnssd_add_service(PORT,PROTOCOL,SERVICE_LEN,SERVICE_DATA) \
{\
bglib_temp_msg.cmd_tcpip_dnssd_add_service.port=(PORT);\
bglib_temp_msg.cmd_tcpip_dnssd_add_service.protocol=(PROTOCOL);\
bglib_temp_msg.cmd_tcpip_dnssd_add_service.service.len=(SERVICE_LEN);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)4+bglib_temp_msg.cmd_tcpip_dnssd_add_service.service.len)>>8)))|((((uint32)4+bglib_temp_msg.cmd_tcpip_dnssd_add_service.service.len)&0xff)<<8)|((uint32)0x4<<16)|((uint32)0xd<<24);\
bglib_output (4+4,(uint8*)&bglib_temp_msg,bglib_temp_msg.cmd_tcpip_dnssd_add_service.service.len,(uint8*)SERVICE_DATA);\
}
/**Add a DNS-SD service instance name**/
#define wifi_cmd_tcpip_dnssd_add_service_instance(INDEX,INSTANCE_LEN,INSTANCE_DATA) \
{\
bglib_temp_msg.cmd_tcpip_dnssd_add_service_instance.index=(INDEX);\
bglib_temp_msg.cmd_tcpip_dnssd_add_service_instance.instance.len=(INSTANCE_LEN);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)2+bglib_temp_msg.cmd_tcpip_dnssd_add_service_instance.instance.len)>>8)))|((((uint32)2+bglib_temp_msg.cmd_tcpip_dnssd_add_service_instance.instance.len)&0xff)<<8)|((uint32)0x4<<16)|((uint32)0xe<<24);\
bglib_output (4+2,(uint8*)&bglib_temp_msg,bglib_temp_msg.cmd_tcpip_dnssd_add_service_instance.instance.len,(uint8*)INSTANCE_DATA);\
}
/**Add a DNS-SD service attribute**/
#define wifi_cmd_tcpip_dnssd_add_service_attribute(INDEX,ATTRIBUTE_LEN,ATTRIBUTE_DATA) \
{\
bglib_temp_msg.cmd_tcpip_dnssd_add_service_attribute.index=(INDEX);\
bglib_temp_msg.cmd_tcpip_dnssd_add_service_attribute.attribute.len=(ATTRIBUTE_LEN);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)2+bglib_temp_msg.cmd_tcpip_dnssd_add_service_attribute.attribute.len)>>8)))|((((uint32)2+bglib_temp_msg.cmd_tcpip_dnssd_add_service_attribute.attribute.len)&0xff)<<8)|((uint32)0x4<<16)|((uint32)0xf<<24);\
bglib_output (4+2,(uint8*)&bglib_temp_msg,bglib_temp_msg.cmd_tcpip_dnssd_add_service_attribute.attribute.len,(uint8*)ATTRIBUTE_DATA);\
}
/**Remove a DNS-SD service**/
#define wifi_cmd_tcpip_dnssd_remove_service(INDEX) \
{\
bglib_temp_msg.cmd_tcpip_dnssd_remove_service.index=(INDEX);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)1+0)>>8)))|((((uint32)1+0)&0xff)<<8)|((uint32)0x4<<16)|((uint32)0x10<<24);\
bglib_output (4+1,(uint8*)&bglib_temp_msg,0,(uint8*)NULL);\
}
/**Start a DNS-SD service**/
#define wifi_cmd_tcpip_dnssd_start_service(INDEX) \
{\
bglib_temp_msg.cmd_tcpip_dnssd_start_service.index=(INDEX);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)1+0)>>8)))|((((uint32)1+0)&0xff)<<8)|((uint32)0x4<<16)|((uint32)0x11<<24);\
bglib_output (4+1,(uint8*)&bglib_temp_msg,0,(uint8*)NULL);\
}
/**Stop a DNS-SD service**/
#define wifi_cmd_tcpip_dnssd_stop_service(INDEX) \
{\
bglib_temp_msg.cmd_tcpip_dnssd_stop_service.index=(INDEX);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)1+0)>>8)))|((((uint32)1+0)&0xff)<<8)|((uint32)0x4<<16)|((uint32)0x12<<24);\
bglib_output (4+1,(uint8*)&bglib_temp_msg,0,(uint8*)NULL);\
}
/**Send data to a given endpoint**/
#define wifi_cmd_endpoint_send(ENDPOINT,DATA_LEN,DATA_DATA) \
{\
bglib_temp_msg.cmd_endpoint_send.endpoint=(ENDPOINT);\
bglib_temp_msg.cmd_endpoint_send.data.len=(DATA_LEN);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)2+bglib_temp_msg.cmd_endpoint_send.data.len)>>8)))|((((uint32)2+bglib_temp_msg.cmd_endpoint_send.data.len)&0xff)<<8)|((uint32)0x5<<16)|((uint32)0x0<<24);\
bglib_output (4+2,(uint8*)&bglib_temp_msg,bglib_temp_msg.cmd_endpoint_send.data.len,(uint8*)DATA_DATA);\
}
/**set streaming mode**/
#define wifi_cmd_endpoint_set_streaming(ENDPOINT,STREAMING) \
{\
bglib_temp_msg.cmd_endpoint_set_streaming.endpoint=(ENDPOINT);\
bglib_temp_msg.cmd_endpoint_set_streaming.streaming=(STREAMING);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)2+0)>>8)))|((((uint32)2+0)&0xff)<<8)|((uint32)0x5<<16)|((uint32)0x1<<24);\
bglib_output (4+2,(uint8*)&bglib_temp_msg,0,(uint8*)NULL);\
}
/**set active status**/
#define wifi_cmd_endpoint_set_active(ENDPOINT,ACTIVE) \
{\
bglib_temp_msg.cmd_endpoint_set_active.endpoint=(ENDPOINT);\
bglib_temp_msg.cmd_endpoint_set_active.active=(ACTIVE);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)2+0)>>8)))|((((uint32)2+0)&0xff)<<8)|((uint32)0x5<<16)|((uint32)0x2<<24);\
bglib_output (4+2,(uint8*)&bglib_temp_msg,0,(uint8*)NULL);\
}
/**set destination for streaming mode**/
#define wifi_cmd_endpoint_set_streaming_destination(ENDPOINT,STREAMING_DESTINATION) \
{\
bglib_temp_msg.cmd_endpoint_set_streaming_destination.endpoint=(ENDPOINT);\
bglib_temp_msg.cmd_endpoint_set_streaming_destination.streaming_destination=(STREAMING_DESTINATION);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)2+0)>>8)))|((((uint32)2+0)&0xff)<<8)|((uint32)0x5<<16)|((uint32)0x3<<24);\
bglib_output (4+2,(uint8*)&bglib_temp_msg,0,(uint8*)NULL);\
}
/**Close the endpoint**/
#define wifi_cmd_endpoint_close(ENDPOINT) \
{\
bglib_temp_msg.cmd_endpoint_close.endpoint=(ENDPOINT);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)1+0)>>8)))|((((uint32)1+0)&0xff)<<8)|((uint32)0x5<<16)|((uint32)0x4<<24);\
bglib_output (4+1,(uint8*)&bglib_temp_msg,0,(uint8*)NULL);\
}
/**Set transmit packet size. **/
#define wifi_cmd_endpoint_set_transmit_size(ENDPOINT,SIZE) \
{\
bglib_temp_msg.cmd_endpoint_set_transmit_size.endpoint=(ENDPOINT);\
bglib_temp_msg.cmd_endpoint_set_transmit_size.size=(SIZE);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)3+0)>>8)))|((((uint32)3+0)&0xff)<<8)|((uint32)0x5<<16)|((uint32)0x5<<24);\
bglib_output (4+3,(uint8*)&bglib_temp_msg,0,(uint8*)NULL);\
}
/**Disable endpoint**/
#define wifi_cmd_endpoint_disable(ENDPOINT) \
{\
bglib_temp_msg.cmd_endpoint_disable.endpoint=(ENDPOINT);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)1+0)>>8)))|((((uint32)1+0)&0xff)<<8)|((uint32)0x5<<16)|((uint32)0x6<<24);\
bglib_output (4+1,(uint8*)&bglib_temp_msg,0,(uint8*)NULL);\
}
/**Set soft timer to send events**/
#define wifi_cmd_hardware_set_soft_timer(TIME,HANDLE,SINGLE_SHOT) \
{\
bglib_temp_msg.cmd_hardware_set_soft_timer.time=(TIME);\
bglib_temp_msg.cmd_hardware_set_soft_timer.handle=(HANDLE);\
bglib_temp_msg.cmd_hardware_set_soft_timer.single_shot=(SINGLE_SHOT);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)6+0)>>8)))|((((uint32)6+0)&0xff)<<8)|((uint32)0x6<<16)|((uint32)0x0<<24);\
bglib_output (4+6,(uint8*)&bglib_temp_msg,0,(uint8*)NULL);\
}
/**Configure external interrupt**/
#define wifi_cmd_hardware_external_interrupt_config(ENABLE,POLARITY) \
{\
bglib_temp_msg.cmd_hardware_external_interrupt_config.enable=(ENABLE);\
bglib_temp_msg.cmd_hardware_external_interrupt_config.polarity=(POLARITY);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)2+0)>>8)))|((((uint32)2+0)&0xff)<<8)|((uint32)0x6<<16)|((uint32)0x1<<24);\
bglib_output (4+2,(uint8*)&bglib_temp_msg,0,(uint8*)NULL);\
}
/**Configure change notification**/
#define wifi_cmd_hardware_change_notification_config(ENABLE) \
{\
bglib_temp_msg.cmd_hardware_change_notification_config.enable=(ENABLE);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)4+0)>>8)))|((((uint32)4+0)&0xff)<<8)|((uint32)0x6<<16)|((uint32)0x2<<24);\
bglib_output (4+4,(uint8*)&bglib_temp_msg,0,(uint8*)NULL);\
}
/**Configure change notification pull-up**/
#define wifi_cmd_hardware_change_notification_pullup(PULLUP) \
{\
bglib_temp_msg.cmd_hardware_change_notification_pullup.pullup=(PULLUP);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)4+0)>>8)))|((((uint32)4+0)&0xff)<<8)|((uint32)0x6<<16)|((uint32)0x3<<24);\
bglib_output (4+4,(uint8*)&bglib_temp_msg,0,(uint8*)NULL);\
}
/**Configure I/O-port direction**/
#define wifi_cmd_hardware_io_port_config_direction(PORT,MASK,DIRECTION) \
{\
bglib_temp_msg.cmd_hardware_io_port_config_direction.port=(PORT);\
bglib_temp_msg.cmd_hardware_io_port_config_direction.mask=(MASK);\
bglib_temp_msg.cmd_hardware_io_port_config_direction.direction=(DIRECTION);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)5+0)>>8)))|((((uint32)5+0)&0xff)<<8)|((uint32)0x6<<16)|((uint32)0x4<<24);\
bglib_output (4+5,(uint8*)&bglib_temp_msg,0,(uint8*)NULL);\
}
/**Configure I/O-port open drain**/
#define wifi_cmd_hardware_io_port_config_open_drain(PORT,MASK,OPEN_DRAIN) \
{\
bglib_temp_msg.cmd_hardware_io_port_config_open_drain.port=(PORT);\
bglib_temp_msg.cmd_hardware_io_port_config_open_drain.mask=(MASK);\
bglib_temp_msg.cmd_hardware_io_port_config_open_drain.open_drain=(OPEN_DRAIN);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)5+0)>>8)))|((((uint32)5+0)&0xff)<<8)|((uint32)0x6<<16)|((uint32)0x5<<24);\
bglib_output (4+5,(uint8*)&bglib_temp_msg,0,(uint8*)NULL);\
}
/**Set the pins of an I/O-port**/
#define wifi_cmd_hardware_io_port_write(PORT,MASK,DATA) \
{\
bglib_temp_msg.cmd_hardware_io_port_write.port=(PORT);\
bglib_temp_msg.cmd_hardware_io_port_write.mask=(MASK);\
bglib_temp_msg.cmd_hardware_io_port_write.data=(DATA);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)5+0)>>8)))|((((uint32)5+0)&0xff)<<8)|((uint32)0x6<<16)|((uint32)0x6<<24);\
bglib_output (4+5,(uint8*)&bglib_temp_msg,0,(uint8*)NULL);\
}
/**Read the pins of an I/O-port**/
#define wifi_cmd_hardware_io_port_read(PORT,MASK) \
{\
bglib_temp_msg.cmd_hardware_io_port_read.port=(PORT);\
bglib_temp_msg.cmd_hardware_io_port_read.mask=(MASK);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)3+0)>>8)))|((((uint32)3+0)&0xff)<<8)|((uint32)0x6<<16)|((uint32)0x7<<24);\
bglib_output (4+3,(uint8*)&bglib_temp_msg,0,(uint8*)NULL);\
}
/**Set output compare settings**/
#define wifi_cmd_hardware_output_compare(INDEX,BIT32,TIMER,MODE,COMPARE_VALUE) \
{\
bglib_temp_msg.cmd_hardware_output_compare.index=(INDEX);\
bglib_temp_msg.cmd_hardware_output_compare.bit32=(BIT32);\
bglib_temp_msg.cmd_hardware_output_compare.timer=(TIMER);\
bglib_temp_msg.cmd_hardware_output_compare.mode=(MODE);\
bglib_temp_msg.cmd_hardware_output_compare.compare_value=(COMPARE_VALUE);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)8+0)>>8)))|((((uint32)8+0)&0xff)<<8)|((uint32)0x6<<16)|((uint32)0x8<<24);\
bglib_output (4+8,(uint8*)&bglib_temp_msg,0,(uint8*)NULL);\
}
/**Read devices A/D converter**/
#define wifi_cmd_hardware_adc_read(INPUT) \
{\
bglib_temp_msg.cmd_hardware_adc_read.input=(INPUT);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)1+0)>>8)))|((((uint32)1+0)&0xff)<<8)|((uint32)0x6<<16)|((uint32)0x9<<24);\
bglib_output (4+1,(uint8*)&bglib_temp_msg,0,(uint8*)NULL);\
}
/**Init Real Time Clock**/
#define wifi_cmd_hardware_rtc_init(ENABLE,DRIFT) \
{\
bglib_temp_msg.cmd_hardware_rtc_init.enable=(ENABLE);\
bglib_temp_msg.cmd_hardware_rtc_init.drift=(DRIFT);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)3+0)>>8)))|((((uint32)3+0)&0xff)<<8)|((uint32)0x6<<16)|((uint32)0xa<<24);\
bglib_output (4+3,(uint8*)&bglib_temp_msg,0,(uint8*)NULL);\
}
/**Set Real Time Clock time**/
#define wifi_cmd_hardware_rtc_set_time(YEAR,MONTH,DAY,WEEKDAY,HOUR,MINUTE,SECOND) \
{\
bglib_temp_msg.cmd_hardware_rtc_set_time.year=(YEAR);\
bglib_temp_msg.cmd_hardware_rtc_set_time.month=(MONTH);\
bglib_temp_msg.cmd_hardware_rtc_set_time.day=(DAY);\
bglib_temp_msg.cmd_hardware_rtc_set_time.weekday=(WEEKDAY);\
bglib_temp_msg.cmd_hardware_rtc_set_time.hour=(HOUR);\
bglib_temp_msg.cmd_hardware_rtc_set_time.minute=(MINUTE);\
bglib_temp_msg.cmd_hardware_rtc_set_time.second=(SECOND);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)8+0)>>8)))|((((uint32)8+0)&0xff)<<8)|((uint32)0x6<<16)|((uint32)0xb<<24);\
bglib_output (4+8,(uint8*)&bglib_temp_msg,0,(uint8*)NULL);\
}
/**Get Real Time Clock time**/
#define wifi_cmd_hardware_rtc_get_time() \
{\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)0+0)>>8)))|((((uint32)0+0)&0xff)<<8)|((uint32)0x6<<16)|((uint32)0xc<<24);\
bglib_output (4+0,(uint8*)&bglib_temp_msg,0,(uint8*)NULL);\
}
/**Set Real Time Clock alarm**/
#define wifi_cmd_hardware_rtc_set_alarm(MONTH,DAY,WEEKDAY,HOUR,MINUTE,SECOND,REPEAT_MASK,REPEAT_COUNT) \
{\
bglib_temp_msg.cmd_hardware_rtc_set_alarm.month=(MONTH);\
bglib_temp_msg.cmd_hardware_rtc_set_alarm.day=(DAY);\
bglib_temp_msg.cmd_hardware_rtc_set_alarm.weekday=(WEEKDAY);\
bglib_temp_msg.cmd_hardware_rtc_set_alarm.hour=(HOUR);\
bglib_temp_msg.cmd_hardware_rtc_set_alarm.minute=(MINUTE);\
bglib_temp_msg.cmd_hardware_rtc_set_alarm.second=(SECOND);\
bglib_temp_msg.cmd_hardware_rtc_set_alarm.repeat_mask=(REPEAT_MASK);\
bglib_temp_msg.cmd_hardware_rtc_set_alarm.repeat_count=(REPEAT_COUNT);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)9+0)>>8)))|((((uint32)9+0)&0xff)<<8)|((uint32)0x6<<16)|((uint32)0xd<<24);\
bglib_output (4+9,(uint8*)&bglib_temp_msg,0,(uint8*)NULL);\
}
/**Configure UART**/
#define wifi_cmd_hardware_uart_conf_set(ID,RATE,DATA_BITS,STOP_BITS,PARITY,FLOW_CTRL) \
{\
bglib_temp_msg.cmd_hardware_uart_conf_set.id=(ID);\
bglib_temp_msg.cmd_hardware_uart_conf_set.rate=(RATE);\
bglib_temp_msg.cmd_hardware_uart_conf_set.data_bits=(DATA_BITS);\
bglib_temp_msg.cmd_hardware_uart_conf_set.stop_bits=(STOP_BITS);\
bglib_temp_msg.cmd_hardware_uart_conf_set.parity=(PARITY);\
bglib_temp_msg.cmd_hardware_uart_conf_set.flow_ctrl=(FLOW_CTRL);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)9+0)>>8)))|((((uint32)9+0)&0xff)<<8)|((uint32)0x6<<16)|((uint32)0xe<<24);\
bglib_output (4+9,(uint8*)&bglib_temp_msg,0,(uint8*)NULL);\
}
/**Get uart configuration**/
#define wifi_cmd_hardware_uart_conf_get(ID) \
{\
bglib_temp_msg.cmd_hardware_uart_conf_get.id=(ID);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)1+0)>>8)))|((((uint32)1+0)&0xff)<<8)|((uint32)0x6<<16)|((uint32)0xf<<24);\
bglib_output (4+1,(uint8*)&bglib_temp_msg,0,(uint8*)NULL);\
}
/**Defragment persistent store**/
#define wifi_cmd_flash_ps_defrag() \
{\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)0+0)>>8)))|((((uint32)0+0)&0xff)<<8)|((uint32)0x7<<16)|((uint32)0x0<<24);\
bglib_output (4+0,(uint8*)&bglib_temp_msg,0,(uint8*)NULL);\
}
/**Dump all ps keys**/
#define wifi_cmd_flash_ps_dump() \
{\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)0+0)>>8)))|((((uint32)0+0)&0xff)<<8)|((uint32)0x7<<16)|((uint32)0x1<<24);\
bglib_output (4+0,(uint8*)&bglib_temp_msg,0,(uint8*)NULL);\
}
/**Erase all ps keys. Warning: This will erase the device's MAC address!**/
#define wifi_cmd_flash_ps_erase_all() \
{\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)0+0)>>8)))|((((uint32)0+0)&0xff)<<8)|((uint32)0x7<<16)|((uint32)0x2<<24);\
bglib_output (4+0,(uint8*)&bglib_temp_msg,0,(uint8*)NULL);\
}
/**Store a value to the given key in the persistent store**/
#define wifi_cmd_flash_ps_save(KEY,VALUE_LEN,VALUE_DATA) \
{\
bglib_temp_msg.cmd_flash_ps_save.key=(KEY);\
bglib_temp_msg.cmd_flash_ps_save.value.len=(VALUE_LEN);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)3+bglib_temp_msg.cmd_flash_ps_save.value.len)>>8)))|((((uint32)3+bglib_temp_msg.cmd_flash_ps_save.value.len)&0xff)<<8)|((uint32)0x7<<16)|((uint32)0x3<<24);\
bglib_output (4+3,(uint8*)&bglib_temp_msg,bglib_temp_msg.cmd_flash_ps_save.value.len,(uint8*)VALUE_DATA);\
}
/**Retrieve the value from the persistent store for the given key **/
#define wifi_cmd_flash_ps_load(KEY) \
{\
bglib_temp_msg.cmd_flash_ps_load.key=(KEY);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)2+0)>>8)))|((((uint32)2+0)&0xff)<<8)|((uint32)0x7<<16)|((uint32)0x4<<24);\
bglib_output (4+2,(uint8*)&bglib_temp_msg,0,(uint8*)NULL);\
}
/**Erase the key and value from the persisten store.**/
#define wifi_cmd_flash_ps_erase(KEY) \
{\
bglib_temp_msg.cmd_flash_ps_erase.key=(KEY);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)2+0)>>8)))|((((uint32)2+0)&0xff)<<8)|((uint32)0x7<<16)|((uint32)0x5<<24);\
bglib_output (4+2,(uint8*)&bglib_temp_msg,0,(uint8*)NULL);\
}
/**Start I2C transmission for reading**/
#define wifi_cmd_i2c_start_read(ENDPOINT,SLAVE_ADDRESS,LENGTH) \
{\
bglib_temp_msg.cmd_i2c_start_read.endpoint=(ENDPOINT);\
bglib_temp_msg.cmd_i2c_start_read.slave_address=(SLAVE_ADDRESS);\
bglib_temp_msg.cmd_i2c_start_read.length=(LENGTH);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)4+0)>>8)))|((((uint32)4+0)&0xff)<<8)|((uint32)0x8<<16)|((uint32)0x0<<24);\
bglib_output (4+4,(uint8*)&bglib_temp_msg,0,(uint8*)NULL);\
}
/**Start I2C transmission for writing**/
#define wifi_cmd_i2c_start_write(ENDPOINT,SLAVE_ADDRESS) \
{\
bglib_temp_msg.cmd_i2c_start_write.endpoint=(ENDPOINT);\
bglib_temp_msg.cmd_i2c_start_write.slave_address=(SLAVE_ADDRESS);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)3+0)>>8)))|((((uint32)3+0)&0xff)<<8)|((uint32)0x8<<16)|((uint32)0x1<<24);\
bglib_output (4+3,(uint8*)&bglib_temp_msg,0,(uint8*)NULL);\
}
/**Stop I2C transmission for writing**/
#define wifi_cmd_i2c_stop(ENDPOINT) \
{\
bglib_temp_msg.cmd_i2c_stop.endpoint=(ENDPOINT);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)1+0)>>8)))|((((uint32)1+0)&0xff)<<8)|((uint32)0x8<<16)|((uint32)0x2<<24);\
bglib_output (4+1,(uint8*)&bglib_temp_msg,0,(uint8*)NULL);\
}
/**Enable/disable http, dhcp and dns server**/
#define wifi_cmd_https_enable(HTTPS,DHCPS,DNSS) \
{\
bglib_temp_msg.cmd_https_enable.https=(HTTPS);\
bglib_temp_msg.cmd_https_enable.dhcps=(DHCPS);\
bglib_temp_msg.cmd_https_enable.dnss=(DNSS);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)3+0)>>8)))|((((uint32)3+0)&0xff)<<8)|((uint32)0x9<<16)|((uint32)0x0<<24);\
bglib_output (4+3,(uint8*)&bglib_temp_msg,0,(uint8*)NULL);\
}
/**Add a path to HTTP server**/
#define wifi_cmd_https_add_path(DEVICE,PATH_LEN,PATH_DATA) \
{\
bglib_temp_msg.cmd_https_add_path.device=(DEVICE);\
bglib_temp_msg.cmd_https_add_path.path.len=(PATH_LEN);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)2+bglib_temp_msg.cmd_https_add_path.path.len)>>8)))|((((uint32)2+bglib_temp_msg.cmd_https_add_path.path.len)&0xff)<<8)|((uint32)0x9<<16)|((uint32)0x1<<24);\
bglib_output (4+2,(uint8*)&bglib_temp_msg,bglib_temp_msg.cmd_https_add_path.path.len,(uint8*)PATH_DATA);\
}
/**REST API response**/
#define wifi_cmd_https_api_response(REQUEST,DATA_LEN,DATA_DATA) \
{\
bglib_temp_msg.cmd_https_api_response.request=(REQUEST);\
bglib_temp_msg.cmd_https_api_response.data.len=(DATA_LEN);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)5+bglib_temp_msg.cmd_https_api_response.data.len)>>8)))|((((uint32)5+bglib_temp_msg.cmd_https_api_response.data.len)&0xff)<<8)|((uint32)0x9<<16)|((uint32)0x2<<24);\
bglib_output (4+5,(uint8*)&bglib_temp_msg,bglib_temp_msg.cmd_https_api_response.data.len,(uint8*)DATA_DATA);\
}
/**REST API response finish**/
#define wifi_cmd_https_api_response_finish(REQUEST) \
{\
bglib_temp_msg.cmd_https_api_response_finish.request=(REQUEST);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)4+0)>>8)))|((((uint32)4+0)&0xff)<<8)|((uint32)0x9<<16)|((uint32)0x3<<24);\
bglib_output (4+4,(uint8*)&bglib_temp_msg,0,(uint8*)NULL);\
}
/**Set wired Ethernet data route**/
#define wifi_cmd_ethernet_set_dataroute(ROUTE) \
{\
bglib_temp_msg.cmd_ethernet_set_dataroute.route=(ROUTE);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)1+0)>>8)))|((((uint32)1+0)&0xff)<<8)|((uint32)0xa<<16)|((uint32)0x0<<24);\
bglib_output (4+1,(uint8*)&bglib_temp_msg,0,(uint8*)NULL);\
}
/**Close wired Ethernet connection**/
#define wifi_cmd_ethernet_close() \
{\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)0+0)>>8)))|((((uint32)0+0)&0xff)<<8)|((uint32)0xa<<16)|((uint32)0x1<<24);\
bglib_output (4+0,(uint8*)&bglib_temp_msg,0,(uint8*)NULL);\
}
/**Test wired Ethernet cable connection**/
#define wifi_cmd_ethernet_connected() \
{\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)0+0)>>8)))|((((uint32)0+0)&0xff)<<8)|((uint32)0xa<<16)|((uint32)0x2<<24);\
bglib_output (4+0,(uint8*)&bglib_temp_msg,0,(uint8*)NULL);\
}
/**Open file**/
#define wifi_cmd_sdhc_fopen(MODE,FNAME_LEN,FNAME_DATA) \
{\
bglib_temp_msg.cmd_sdhc_fopen.mode=(MODE);\
bglib_temp_msg.cmd_sdhc_fopen.fname.len=(FNAME_LEN);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)2+bglib_temp_msg.cmd_sdhc_fopen.fname.len)>>8)))|((((uint32)2+bglib_temp_msg.cmd_sdhc_fopen.fname.len)&0xff)<<8)|((uint32)0xb<<16)|((uint32)0x0<<24);\
bglib_output (4+2,(uint8*)&bglib_temp_msg,bglib_temp_msg.cmd_sdhc_fopen.fname.len,(uint8*)FNAME_DATA);\
}
/**Close file**/
#define wifi_cmd_sdhc_fclose(FHANDLE) \
{\
bglib_temp_msg.cmd_sdhc_fclose.fhandle=(FHANDLE);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)1+0)>>8)))|((((uint32)1+0)&0xff)<<8)|((uint32)0xb<<16)|((uint32)0x1<<24);\
bglib_output (4+1,(uint8*)&bglib_temp_msg,0,(uint8*)NULL);\
}
/**List files**/
#define wifi_cmd_sdhc_fdir(MODE,PATH_LEN,PATH_DATA) \
{\
bglib_temp_msg.cmd_sdhc_fdir.mode=(MODE);\
bglib_temp_msg.cmd_sdhc_fdir.path.len=(PATH_LEN);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)2+bglib_temp_msg.cmd_sdhc_fdir.path.len)>>8)))|((((uint32)2+bglib_temp_msg.cmd_sdhc_fdir.path.len)&0xff)<<8)|((uint32)0xb<<16)|((uint32)0x2<<24);\
bglib_output (4+2,(uint8*)&bglib_temp_msg,bglib_temp_msg.cmd_sdhc_fdir.path.len,(uint8*)PATH_DATA);\
}
/**Read data**/
#define wifi_cmd_sdhc_fread(FHANDLE,FSIZE) \
{\
bglib_temp_msg.cmd_sdhc_fread.fhandle=(FHANDLE);\
bglib_temp_msg.cmd_sdhc_fread.fsize=(FSIZE);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)5+0)>>8)))|((((uint32)5+0)&0xff)<<8)|((uint32)0xb<<16)|((uint32)0x3<<24);\
bglib_output (4+5,(uint8*)&bglib_temp_msg,0,(uint8*)NULL);\
}
/**Write file**/
#define wifi_cmd_sdhc_fwrite(FHANDLE,FDATA_LEN,FDATA_DATA) \
{\
bglib_temp_msg.cmd_sdhc_fwrite.fhandle=(FHANDLE);\
bglib_temp_msg.cmd_sdhc_fwrite.fdata.len=(FDATA_LEN);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)3+bglib_temp_msg.cmd_sdhc_fwrite.fdata.len)>>8)))|((((uint32)3+bglib_temp_msg.cmd_sdhc_fwrite.fdata.len)&0xff)<<8)|((uint32)0xb<<16)|((uint32)0x4<<24);\
bglib_output (4+3,(uint8*)&bglib_temp_msg,bglib_temp_msg.cmd_sdhc_fwrite.fdata.len,(uint8*)FDATA_DATA);\
}
/**Delete file**/
#define wifi_cmd_sdhc_fdelete(FNAME_LEN,FNAME_DATA) \
{\
bglib_temp_msg.cmd_sdhc_fdelete.fname.len=(FNAME_LEN);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)1+bglib_temp_msg.cmd_sdhc_fdelete.fname.len)>>8)))|((((uint32)1+bglib_temp_msg.cmd_sdhc_fdelete.fname.len)&0xff)<<8)|((uint32)0xb<<16)|((uint32)0x5<<24);\
bglib_output (4+1,(uint8*)&bglib_temp_msg,bglib_temp_msg.cmd_sdhc_fdelete.fname.len,(uint8*)FNAME_DATA);\
}
/**Create directory**/
#define wifi_cmd_sdhc_fmkdir(DIR_NAME_LEN,DIR_NAME_DATA) \
{\
bglib_temp_msg.cmd_sdhc_fmkdir.dir_name.len=(DIR_NAME_LEN);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)1+bglib_temp_msg.cmd_sdhc_fmkdir.dir_name.len)>>8)))|((((uint32)1+bglib_temp_msg.cmd_sdhc_fmkdir.dir_name.len)&0xff)<<8)|((uint32)0xb<<16)|((uint32)0x6<<24);\
bglib_output (4+1,(uint8*)&bglib_temp_msg,bglib_temp_msg.cmd_sdhc_fmkdir.dir_name.len,(uint8*)DIR_NAME_DATA);\
}
/**Change active directory**/
#define wifi_cmd_sdhc_fchdir(DIR_NAME_LEN,DIR_NAME_DATA) \
{\
bglib_temp_msg.cmd_sdhc_fchdir.dir_name.len=(DIR_NAME_LEN);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)1+bglib_temp_msg.cmd_sdhc_fchdir.dir_name.len)>>8)))|((((uint32)1+bglib_temp_msg.cmd_sdhc_fchdir.dir_name.len)&0xff)<<8)|((uint32)0xb<<16)|((uint32)0x7<<24);\
bglib_output (4+1,(uint8*)&bglib_temp_msg,bglib_temp_msg.cmd_sdhc_fchdir.dir_name.len,(uint8*)DIR_NAME_DATA);\
}
/**Rename file**/
#define wifi_cmd_sdhc_frename(FHANDLE,NEW_NAME_LEN,NEW_NAME_DATA) \
{\
bglib_temp_msg.cmd_sdhc_frename.fhandle=(FHANDLE);\
bglib_temp_msg.cmd_sdhc_frename.new_name.len=(NEW_NAME_LEN);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)2+bglib_temp_msg.cmd_sdhc_frename.new_name.len)>>8)))|((((uint32)2+bglib_temp_msg.cmd_sdhc_frename.new_name.len)&0xff)<<8)|((uint32)0xb<<16)|((uint32)0x8<<24);\
bglib_output (4+2,(uint8*)&bglib_temp_msg,bglib_temp_msg.cmd_sdhc_frename.new_name.len,(uint8*)NEW_NAME_DATA);\
}
/**Change file attributes**/
#define wifi_cmd_sdhc_fchmode(VALUE,FNAME_LEN,FNAME_DATA) \
{\
bglib_temp_msg.cmd_sdhc_fchmode.value=(VALUE);\
bglib_temp_msg.cmd_sdhc_fchmode.fname.len=(FNAME_LEN);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)2+bglib_temp_msg.cmd_sdhc_fchmode.fname.len)>>8)))|((((uint32)2+bglib_temp_msg.cmd_sdhc_fchmode.fname.len)&0xff)<<8)|((uint32)0xb<<16)|((uint32)0x9<<24);\
bglib_output (4+2,(uint8*)&bglib_temp_msg,bglib_temp_msg.cmd_sdhc_fchmode.fname.len,(uint8*)FNAME_DATA);\
}
/**
            Converts decimal value in ascii string1 to 32bit signed integer
            **/
#define wifi_cmd_util_atoi(STRING_LEN,STRING_DATA) \
{\
bglib_temp_msg.cmd_util_atoi.string1.len=(STRING_LEN);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)1+bglib_temp_msg.cmd_util_atoi.string1.len)>>8)))|((((uint32)1+bglib_temp_msg.cmd_util_atoi.string1.len)&0xff)<<8)|((uint32)0xc<<16)|((uint32)0x0<<24);\
bglib_output (4+1,(uint8*)&bglib_temp_msg,bglib_temp_msg.cmd_util_atoi.string1.len,(uint8*)STRING_DATA);\
}
/**
            Converts signed 32bit integer to decimal ascii value
            **/
#define wifi_cmd_util_itoa(VALUE) \
{\
bglib_temp_msg.cmd_util_itoa.value=(VALUE);\
bglib_temp_msg.header=(((uint32)wifi_msg_type_cmd|wifi_dev_type_wifi|(((uint32)4+0)>>8)))|((((uint32)4+0)&0xff)<<8)|((uint32)0xc<<16)|((uint32)0x1<<24);\
bglib_output (4+4,(uint8*)&bglib_temp_msg,0,(uint8*)NULL);\
}
 
#ifdef __cplusplus
}
#endif
/*lint -restore*/
#endif
