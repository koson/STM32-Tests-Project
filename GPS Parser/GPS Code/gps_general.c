

#include "includes.h"
#include "gps_general.h"
//#define _VIRTUAL_GPS_

void gps_parser(const char* pBuf, const int size);

const char ublox_msg_led_gps_settings[] = { 0xB5, 0x62, 0x06, 0x31, 0x20, 0x00, 0x00, 0x01, 0x00, 0x00, 0x32, 0x00, 0x00, 0x00, 0x40, 0x42, 0x0F, 0x00, 0x40, 0x42, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x42, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x57, 0x00, 0x00, 0x00, 0x94, 0x56, 0xB5, 0x62, 0x06, 0x31, 0x01, 0x00, 0x00, 0x38, 0xE5 };    // led settings

const char ublox_msg_navi_auto_2d[] = {
    0xB5,
    0x62,
    0x06,
    0x24,
    0x24,
    0x00,
    0xFF,
    0xFF,
    0x04,
    0x01,
    0x00,
    0x00,
    0x00,
    0x00,
    0x10,
    0x27,
    0x00,
    0x00,
    0x05,
    0x00,
    0xFA,
    0x00,
    0xFA,
    0x00,
    0x64,
    0x00,
    0x2C,
    0x01,
    0x00,
    0x3C,
    0x00,
    0x00,
    0x00,
    0x00,
    0xC8,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x16,
    0xA2
};    // navi-auto+2d

const char ublox_msg_gll_disabled[] = {
    0xB5,
    0x62,
    0x06,
    0x01,
    0x03,
    0x00,
    0xF0,
    0x01,
    0x00,
    0xFB,
    0x11
};    // gll-disabled
const char ublox_msg_vtg_disabled[] = {
    0xB5,
    0x62,
    0x06,
    0x01,
    0x03,
    0x00,
    0xF0,
    0x05,
    0x00,
    0xFF,
    0x19
};    // vtg-disabled
const char ublox_msg_gsv_disabled[] = {
    0xB5,
    0x62,
    0x06,
    0x01,
    0x03,
    0x00,
    0xF0,
    0x03,
    0x00,
    0xFD,
    0x15
};    // gsv-disalbed
const char ublox_msg_gsa_disabled[] = {
    0xB5,
    0x62,
    0x06,
    0x01,
    0x03,
    0x00,
    0xF0,
    0x02,
    0x00,
    0xFC,
    0x13
};    // gsa-disalbed

const char ublox_msg_config_filt_stop[] = { 0xB5, 0x62, 0x06, 0x24, 0x24, 0x00, 0xFF, 0xFF, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x10, 0x27, 0x00, 0x00, 0x05, 0x00, 0xFA, 0x00, 0xFA, 0x00, 0x64, 0x00, 0x2C, 0x01, 0x68, 0x3C, 0x00, 0x00, 0x00, 0x00, 0xC8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7C, 0x0C };

const char sim_msg_disable[] = "$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0*34\r\n";

char msg_rmc[] = "$GNRMC,165835.000,A,6001.8859,N,03024.1946,E,0.02,26.13,151016,,,D*4E\r\n";
char msg_gga[] = "$GPGGA,111812.00,6001.89706,N,03024.19850,E,1,05,5.67,70.1,M,15.8,M,,*6A\r\n";

#if UART_GPS == 1
char* pRxBufferGPS = (char*)(g_aucRxBufferUSART1);
#endif

#if UART_GPS == 2
char* pRxBufferGPS = (char*)(g_aucRxBufferUSART2);
#endif

#if UART_GPS == 3
char* pRxBufferGPS = (char*)(g_aucRxBufferUSART3);
#endif

void vGpsHandler(void* pvParameters)
{
    // char *nmea =
    // "$GNRMC,165835.000,A,6001.8859,N,03024.1946,E,0.02,26.13,151016,,,D*4B\r\n$GPGGA,091719.714,3654.822,N,07502.393,W,0,00,,,M,,M,,*51\r\n";
    while(1) {
        if(osMutexWait(mGPS_DATA_ARRIVAL, 1000) == osOK) {
            if(!(GpsBufNumber())) {
                // gps_parser(nmea, strlen(nmea));
                gps_parser(pRxBufferGPS, RX_BUFFER_SIZE1 / 2);
                // USART_Write(UART_DBG, pRxBufferGPS, RX_BUFFER_SIZE1/2);
            }
            else {
                gps_parser(&pRxBufferGPS[RX_BUFFER_SIZE1 / 2], RX_BUFFER_SIZE1 / 2);
                // USART_Write(UART_DBG, &pRxBufferGPS[RX_BUFFER_SIZE1/2], RX_BUFFER_SIZE1/2);
            }
        }
    }
}
