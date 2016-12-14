/*
* ----------------------------------------------------------------------------
* "THE BEER-WARE LICENSE" (Revision 42):
* Jeroen Domburg <jeroen@spritesmods.com> wrote this file. As long as you retain
* this notice you can do whatever you want with this stuff. If we meet some day,
* and you think this stuff is worth it, you can buy me a beer in return.
* ----------------------------------------------------------------------------
* Heavily modified and enhanced by Thorsten von Eicken in 2015
* ----------------------------------------------------------------------------
*/

#include <esp8266.h>
#include "httpd.h"
#include "httpdespfs.h"
#include "cgi.h"
#include "cgiwifi.h"
#include "cgipins.h"
#include "cgitcp.h"
#include "cgimqtt.h"
#include "cgiflash.h"
#include "cgioptiboot.h"
#include "cgiwebserversetup.h"
#include "auth.h"
#include "espfs.h"
#include "uart.h"
#include "serbridge.h"
#include "status.h"
#include "serled.h"
#include "console.h"
#include "config.h"
#include "log.h"
#include "gpio.h"
#include "cgiservices.h"
#include "web-server.h"
#include "cgitelnet.h"

#ifdef SYSLOG
#include "syslog.h"
#define NOTICE(format, ...) do {                                            \
  LOG_NOTICE(format, ## __VA_ARGS__ );                                      \
  os_printf(format "\n", ## __VA_ARGS__);                                   \
} while ( 0 )
#else
#define NOTICE(format, ...) do {                                            \
  os_printf(format "\n", ## __VA_ARGS__);                                   \
} while ( 0 )
#endif

#ifdef MEMLEAK_DEBUG
#include "mem.h"
bool ICACHE_FLASH_ATTR check_memleak_debug_enable(void)
{
    return MEMLEAK_DEBUG_ENABLE;
}
#endif

static int pwdLen = -1;
static char buffer[256];
static char *user[9], *pass[9];
static int maxuser = -1;

static void initPasswordData() {
  if (espFsIsValid(userPageCtx)) {
    // os_printf("initPasswordData : valid fs\n");
    EspFsFile *fp = espFsOpen(userPageCtx, "webaccess.txt");
    // os_printf("initPasswordData : file open\n");
    if (fp != NULL) {
      // os_printf("initPasswordData : about to read ...\n");
      pwdLen = espFsRead(fp, buffer, sizeof(buffer));
      // os_printf("initPasswordData : file read\n");
      espFsClose(fp);
      // os_printf("initPasswordData : file closed\n");
    }
  }
  os_printf("initPasswordData : read %d bytes\n", pwdLen);
  int i, ix, lastuser = 0, lastpass = -1;
  for (ix=i=0; i<pwdLen && ix < 8; i++) {
    if (buffer[i] == ',') {
      lastpass = i+1;
      buffer[i] = 0;
    } else if (buffer[i] == '\n') {
      buffer[i] = 0;
      if (lastpass < 0)
        continue;
      user[ix] = &buffer[lastuser];
      if (buffer[i+1] == '\r')
        lastuser = i+2;
      else
        lastuser = i+1;
      pass[ix] = &buffer[lastpass];
      lastpass = -1;

      os_printf("### user {%s} pass {%s}\n", user[ix], pass[ix]);

      ix++;
      maxuser = ix;
      user[ix] = pass[ix] = NULL;
    }
  }
}

int myPassFn(HttpdConnData *connData, int no, char *puser, int userLen, char *ppass, int passLen) {
  if (pwdLen < 0)
    initPasswordData();

  if (pwdLen < 0) { // Fallback
    if (no==0) {
      os_strcpy(puser, "admin");
      os_strcpy(ppass, "s3cr3t");
      return 1;
    } else
      return 0;
  }

#if 0
  if (no==0) {
    os_strcpy(puser, "admin");
    os_strcpy(ppass, "s3cr3t");
    return 1;
// Add more users this way. Check against incrementing no for each user added.
//  } else if (no==1) {
//    os_strcpy(puser, "user1");
//    os_strcpy(ppass, "something");
//    return 1;
  }
#else
  if (no < 0 || no >maxuser)
    return 0;
  os_strcpy(puser, user[no]);
  os_strcpy(ppass, pass[no]);
  return 1;
#endif
  return 0;
}

/*
This is the main url->function dispatching data struct.
In short, it's a struct with various URLs plus their handlers. The handlers can
be 'standard' CGI functions you wrote, or 'special' CGIs requiring an argument.
They can also be auth-functions. An asterisk will match any url starting with
everything before the asterisks; "*" matches everything. The list will be
handled top-down, so make sure to put more specific rules above the more
general ones. Authorization things (like authBasic) act as a 'barrier' and
should be placed above the URLs they protect.
*/
HttpdBuiltInUrl builtInUrls[] = {
  { "/", cgiRedirect, "/home.html" },
  { "/menu", cgiMenu, NULL },
  { "/flash/next", cgiGetFirmwareNext, NULL },
  { "/flash/upload", cgiUploadFirmware, NULL },
  { "/flash/reboot", cgiRebootFirmware, NULL },
  { "/pgm/sync", cgiOptibootSync, NULL },
  { "/pgm/upload", cgiOptibootData, NULL },
  { "/log/text", ajaxLog, NULL },
  { "/log/dbg", ajaxLogDbg, NULL },
  { "/log/reset", cgiReset, NULL },
  { "/console/reset", ajaxConsoleReset, NULL },
  { "/console/baud", ajaxConsoleBaud, NULL },
  { "/console/fmt", ajaxConsoleFormat, NULL },
  { "/console/text", ajaxConsole, NULL },
  { "/console/send", ajaxConsoleSend, NULL },
  //Enable the line below to protect the WiFi configuration with an username/password combo.
  { "/wifi/*", authBasic, myPassFn},
  { "/wifi", cgiRedirect, "/wifi/wifi.html" },
  { "/wifi/", cgiRedirect, "/wifi/wifi.html" },
  { "/wifi/info", cgiWifiInfo, NULL },
  { "/wifi/scan", cgiWiFiScan, NULL },
  { "/wifi/connect", cgiWiFiConnect, NULL },
  { "/wifi/connstatus", cgiWiFiConnStatus, NULL },
  { "/wifi/setmode", cgiWiFiSetMode, NULL },
  { "/wifi/special", cgiWiFiSpecial, NULL },
  { "/wifi/apinfo", cgiApSettingsInfo, NULL },
  { "/wifi/apchange", cgiApSettingsChange, NULL },
  { "/system/*", authBasic, myPassFn},
  { "/system/info", cgiSystemInfo, NULL },
  { "/system/update", cgiSystemSet, NULL },
  { "/services/info", cgiServicesInfo, NULL },
  { "/services/update", cgiServicesSet, NULL },
  { "/pins", cgiPins, NULL },
  { "/telnet", cgiTelnet, NULL},
#ifdef MQTT
  { "/mqtt", cgiMqtt, NULL },
#endif
  { "/web-server/upload", cgiWebServerSetupUpload, NULL },
  { "*.json", WEB_CgiJsonHook, NULL }, //Catch-all cgi JSON queries
  { "*", cgiEspFsHook, NULL }, //Catch-all cgi function for the filesystem
  { NULL, NULL, NULL }
};

#ifdef SHOW_HEAP_USE
static ETSTimer prHeapTimer;
static void ICACHE_FLASH_ATTR prHeapTimerCb(void *arg) {
  os_printf("Heap: %ld\n", (unsigned long)system_get_free_heap_size());
}
#endif

# define VERS_STR_STR(V) #V
# define VERS_STR(V) VERS_STR_STR(V)
char* esp_link_version = VERS_STR(VERSION);

// address of espfs binary blob
extern uint32_t _binary_espfs_img_start;

extern void app_init(void);
extern void mqtt_client_init(void);

void ICACHE_FLASH_ATTR
user_rf_pre_init(void) {
  //default is enabled
  system_set_os_print(DEBUG_SDK);
}

/* user_rf_cal_sector_set is a required function that is called by the SDK to get a flash
 * sector number where it can store RF calibration data. This was introduced with SDK 1.5.4.1
 * and is necessary because Espressif ran out of pre-reserved flash sectors. Ooops...  */
uint32 ICACHE_FLASH_ATTR
user_rf_cal_sector_set(void) {
  uint32_t sect = 0;
  switch (system_get_flash_size_map()) {
  case FLASH_SIZE_4M_MAP_256_256: // 512KB
    sect = 128 - 10; // 0x76000
  default:
    sect = 128; // 0x80000
  }
  return sect;
}

// Main routine to initialize esp-link.
void ICACHE_FLASH_ATTR
user_init(void) {
  // uncomment the following three lines to see flash config messages for troubleshooting
  //uart_init(115200, 115200);
  //logInit();
  //os_delay_us(100000L);
  // get the flash config so we know how to init things
  //configWipe(); // uncomment to reset the config for testing purposes
  bool restoreOk = configRestore();
  // Init gpio pin registers
  gpio_init();
  gpio_output_set(0, 0, 0, (1<<15)); // some people tie it to GND, gotta ensure it's disabled
  // init UART
  uart_init(CALC_UARTMODE(flashConfig.data_bits, flashConfig.parity, flashConfig.stop_bits),
            flashConfig.baud_rate, 115200);
  logInit(); // must come after init of uart
  // Say hello (leave some time to cause break in TX after boot loader's msg
  os_delay_us(10000L);
  os_printf("\n\n** %s\n", esp_link_version);
  os_printf("Flash config restore %s\n", restoreOk ? "ok" : "*FAILED*");
  // Status LEDs
  statusInit();
  serledInit();
  // Wifi
  wifiInit();
  // init the flash filesystem with the html stuff
  espFsInit(espLinkCtx, &_binary_espfs_img_start, ESPFS_MEMORY);

  //EspFsInitResult res = espFsInit(&_binary_espfs_img_start);
  //os_printf("espFsInit %s\n", res?"ERR":"ok");
  // mount the http handlers
  httpdInit(builtInUrls, 80);
  WEB_Init();

  // init the wifi-serial configurable transparent bridge (port defaults 23&2323)
  serbridgeInit();
  serbridgeStart(0, flashConfig.telnet_port0, flashConfig.telnet_port0mode, flashConfig.telnet_port0pass);
  serbridgeStart(1, flashConfig.telnet_port1, flashConfig.telnet_port1mode, flashConfig.telnet_port1pass);
  
  uart_add_recv_cb(&serbridgeUartCb);
#ifdef SHOW_HEAP_USE
  os_timer_disarm(&prHeapTimer);
  os_timer_setfn(&prHeapTimer, prHeapTimerCb, NULL);
  os_timer_arm(&prHeapTimer, 10000, 1);
#endif

  struct rst_info *rst_info = system_get_rst_info();
  NOTICE("Reset cause: %d=%s", rst_info->reason, rst_codes[rst_info->reason]);
  NOTICE("exccause=%d epc1=0x%x epc2=0x%x epc3=0x%x excvaddr=0x%x depc=0x%x",
    rst_info->exccause, rst_info->epc1, rst_info->epc2, rst_info->epc3,
    rst_info->excvaddr, rst_info->depc);
  uint32_t fid = spi_flash_get_id();
  NOTICE("Flash map %s, manuf 0x%02X chip 0x%04X", flash_maps[system_get_flash_size_map()],
      fid & 0xff, (fid&0xff00)|((fid>>16)&0xff));
  NOTICE("** %s: ready, heap=%ld", esp_link_version, (unsigned long)system_get_free_heap_size());

  // Init SNTP service
  cgiServicesSNTPInit();
#ifdef MQTT
  if (flashConfig.mqtt_enable) {
    NOTICE("initializing MQTT");
    mqtt_client_init();
  }
#endif
  NOTICE("initializing user application");
  app_init();
  NOTICE("Waiting for work to do...");
#ifdef MEMLEAK_DEBUG
  system_show_malloc();
#endif
}
