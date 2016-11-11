#include <esp8266.h>
#include "cgi.h"
#include "config.h"
#include "serbridge.h"

static char *portMode[] = { "open", "disabled", "secure" };

// Cgi to return choice of Telnet ports
int ICACHE_FLASH_ATTR cgiTelnetGet(HttpdConnData *connData) {
  char buff[80];

  if (connData->conn==NULL) return HTTPD_CGI_DONE; // Connection aborted

  int len;
  
  os_printf("Current telnet ports: port0=%d port1=%d\n",
	flashConfig.telnet_port0, flashConfig.telnet_port1);
	
  len = os_sprintf(buff,
      "{ \"port0\": \"%d\", \"port1\": \"%d\" }",
      flashConfig.telnet_port0, flashConfig.telnet_port1);

  jsonHeader(connData, 200);
  httpdSend(connData, buff, len);

  return HTTPD_CGI_DONE;
}

// Cgi to change choice of Telnet ports
int ICACHE_FLASH_ATTR cgiTelnetSet(HttpdConnData *connData) {
  char buf[80];

  if (connData->conn==NULL) {
    return HTTPD_CGI_DONE; // Connection aborted
  }

  int8_t ok0, ok1;
  uint16_t port0, port1;
  ok0 = getUInt16Arg(connData, "port0", &port0);
  ok1 = getUInt16Arg(connData, "port1", &port1);
  os_printf("cgiTelnetSet ok0 %d ok1 %d port0 %d port1 %d\n", ok0, ok1, port0, port1);

  if (ok0 <= 0 && ok1 <= 0) { //If we get at least one good value, this should be >= 1
    ets_sprintf(buf, "Unable to fetch telnet ports. Previous %d %d  Received: port0=%d port1=%d\n",
	  flashConfig.telnet_port0, flashConfig.telnet_port1, port0, port1);
    os_printf(buf);
    errorResponse(connData, 400, buf);
    return HTTPD_CGI_DONE;
  }

  if (ok0 == 1) flashConfig.telnet_port0 = port0;
  if (ok1 == 1) flashConfig.telnet_port1 = port1;

  // check whether ports are different
  if (flashConfig.telnet_port0 == flashConfig.telnet_port1) {
      os_sprintf(buf, "Ports cannot be the same.\n Tried to set: port0=%d port1=%d\n",
        flashConfig.telnet_port0, flashConfig.telnet_port1);
      os_printf(buf);
      errorResponse(connData, 400, buf);
      return HTTPD_CGI_DONE;
  }

  os_printf("Telnet ports changed: port0=%d port1=%d\n",
    flashConfig.telnet_port0, flashConfig.telnet_port1);

  // save to flash
  if (configSave()) {
    httpdStartResponse(connData, 204);
    httpdEndHeaders(connData);
  } else {
    httpdStartResponse(connData, 500);
    httpdEndHeaders(connData);
    httpdSend(connData, "Failed to save config", -1);
  }
    
  // apply the changes
  serbridgeInit();
  serbridgeStart(0, flashConfig.telnet_port0, flashDefault.telnet_port0mode);
  serbridgeStart(1, flashConfig.telnet_port1, flashDefault.telnet_port1mode);

  return HTTPD_CGI_DONE;
}

int ICACHE_FLASH_ATTR cgiTelnet(HttpdConnData *connData) {
  if (connData->conn==NULL) return HTTPD_CGI_DONE; // Connection aborted. Clean up.
  if (connData->requestType == HTTPD_METHOD_GET) {
    return cgiTelnetGet(connData);
  } else if (connData->requestType == HTTPD_METHOD_POST) {
    return cgiTelnetSet(connData);
  } else {
    jsonHeader(connData, 404);
    return HTTPD_CGI_DONE;
  }
}

static char *portMode2string(int8_t m) { //Should we put this into flash?
   if (m < 0 || m > 2) return "?";
   return portMode[m];
 }
 
// print various Telnet information into json buffer
int ICACHE_FLASH_ATTR printTelnetSecurity(char *buff) {
  int len;
 
  len = os_sprintf(buff,
    "{ \"port0mode\": \"%s\", \"port0portnumber\": \"%d\", \"port0pwd\": \"%s\", "
    "\"port1mode\": \"%s\", \"port1portnumber\": \"%d\", \"port1pwd\": \"%s\" }",
    portMode2string(flashConfig.telnet_port0mode), flashConfig.telnet_port0, flashConfig.telnet_port0pass,
    portMode2string(flashConfig.telnet_port1mode), flashConfig.telnet_port1, flashConfig.telnet_port1pass
  );
 
  return len;
}
