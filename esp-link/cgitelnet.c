#include <esp8266.h>
#include "cgi.h"
#include "config.h"
#include "serbridge.h"

// The three definitions below need to match with each other, and with the content of html/ui.js:ajaxSelectPresets
const static int nPortModes = 4;
static char *portMode[] = {
    "open",
    "disabled",
    "secure",
    "password"};
#if 0
static int portModeBits[] = {
  /* open */		SER_BRIDGE_MODE_NONE,
  /* disabled */	SER_BRIDGE_MODE_DISABLED,
  /* secure */		SER_BRIDGE_MODE_SECURE,
  /* password */	SER_BRIDGE_MODE_PASSWORD
};
#endif

static int string2portMode(char *s);
static char *portMode2string(int8_t m);

// Cgi to return undefined
int ICACHE_FLASH_ATTR cgiUndefined(HttpdConnData *connData) {
  httpdStartResponse(connData, 204);
  httpdEndHeaders(connData);
  return HTTPD_CGI_DONE;
}

// Cgi to return choice of Telnet ports
int ICACHE_FLASH_ATTR cgiTelnetGet(HttpdConnData *connData) {
  char buff[160];

  if (connData->conn == NULL) return HTTPD_CGI_DONE;  // Connection aborted

  int len;

  // FIXME should only print when debug
  os_printf("Current telnet ports: port0=%d (mode %d %s) port1=%d (mode %d %s)\n",
            flashConfig.telnet_port0, flashConfig.telnet_port0mode, portMode2string(flashConfig.telnet_port0mode),
            flashConfig.telnet_port1, flashConfig.telnet_port1mode, portMode2string(flashConfig.telnet_port1mode));
  // clang-format off
  len = os_sprintf(buff,
                  "{"
                    "\"port0\": \"%d\","
                    "\"port1\": \"%d\","
                    "\"port0mode\": \"%s\","
                    "\"port1mode\": \"%s\","
                    "\"port0pass\": \"%s\","
                    "\"port1pass\": \"%s\"" 
                  "}",
                  flashConfig.telnet_port0,
                  flashConfig.telnet_port1,
                  portMode2string(flashConfig.telnet_port0mode), 
                  portMode2string(flashConfig.telnet_port1mode),
                  function () {if (flashConfig.telnet_port0pass !== "") return "SET"; else return "UNSET"; },
                  function () {if (flashConfig.telnet_port1pass !== "") return "SET"; else return "UNSET"; };
// clang-format on

jsonHeader(connData, 200);
httpdSend(connData, buff, len);

return HTTPD_CGI_DONE;
}

/*
 * Cgi to change choice of Telnet ports
 *
 * Need to discuss how this is being called.
 *
 * Current situation : can be called with several URLs :
 *	PUT http://esp-link/telnet?port1=35
 *	PUT http://esp-link/telnet?port1pass=qwerty
 *	PUT http://esp-link/telnet?port0mode=open&port1mode=secure
 *
 * Neither variable flashConfig nor the flash storage (copied from flashConfig by
 * our call to configSave()) should ever be left in an inconsistent state. A later
 * call of this function might pick up the inconsistency and save to flash.
 *
 * FIXME implementation not ok yet, awaiting discussion.
 * Alex: I believe saving all parameters in one call is best. Less dynamic, but
 * this is a rather simple function. Should we need something more complicated
 * (say pwd field), we can just call a put to a different URL. ie 'telnet/pwd''
 */
int ICACHE_FLASH_ATTR cgiTelnetSet(HttpdConnData *connData) {
  char buf[80];

  if (connData->conn == NULL) {
    return HTTPD_CGI_DONE;  // Connection aborted
  }

  int8_t ok0, ok1;
  uint16_t port0, port1;
  ok0 = getUInt16Arg(connData, "port0", &port0);
  ok1 = getUInt16Arg(connData, "port1", &port1);

  os_printf("cgiTelnetSet ok0 %d ok1 %d port0 %d port1 %d\n", ok0, ok1, port0,
            port1);

  if (ok0 == 1) flashConfig.telnet_port0 = port0;
  if (ok1 == 1) flashConfig.telnet_port1 = port1;

  // Change port mode
  int mok0, mok1;
  char port0mode[16], port1mode[16];
  mok0 = getStringArg(connData, "port0mode", port0mode, sizeof(port0mode));
  mok1 = getStringArg(connData, "port1mode", port1mode, sizeof(port1mode));

  os_printf("cgiTelnetSet mok0 %d mok1 %d port0 %s port1 %s\n", mok0, mok1, port0mode, port1mode);
  int mode0 = string2portMode(port0mode);
  int mode1 = string2portMode(port1mode);
  if (mok0 == 1) flashConfig.telnet_port0mode = mode0;
  if (mok1 == 1) flashConfig.telnet_port1mode = mode1;

  // Check whether ports are different
  if (flashConfig.telnet_port0 == flashConfig.telnet_port1) {
    os_sprintf(buf,
               "Ports cannot be the same.\n Tried to set: port0=%d port1=%d\n",
               flashConfig.telnet_port0, flashConfig.telnet_port1);
    os_printf(buf);
    errorResponse(connData, 400, buf);
    return HTTPD_CGI_DONE;
  }

  os_printf("Telnet ports changed: port0=%d port1=%d\n",
            flashConfig.telnet_port0, flashConfig.telnet_port1);

  // Password management
  int pok0, pok1;
  char port0pass[16], port1pass[16];
  pok0 = getStringArg(connData, "port0pass", port0pass, sizeof(port0pass));
  pok1 = getStringArg(connData, "port1pass", port1pass, sizeof(port1pass));

  if (pok0 == 1 && strlen(port0pass) > sizeof(flashConfig.telnet_port0pass)) {
    os_sprintf(buf, "Port 0 password too long (max %d)", strlen(port0pass));
    errorResponse(connData, 400, buf);
    return HTTPD_CGI_DONE;
  }

  if (pok1 == 1 && strlen(port1pass) > sizeof(flashConfig.telnet_port1pass)) {
    os_sprintf(buf, "Port 1 password too long (max %d)", strlen(port1pass));
    errorResponse(connData, 400, buf);
    return HTTPD_CGI_DONE;
  }

  if (pok0 == 1) {
    strcpy(flashConfig.telnet_port0pass, port0pass);
    os_printf("Changed port0 password to \"%s\"\n", port0pass);
  }
  if (pok1 == 1) {
    strcpy(flashConfig.telnet_port1pass, port1pass);
    os_printf("Changed port1 password to \"%s\"\n", port1pass);
  }

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
  serbridgeClose();  // Close existing connections
  serbridgeInit();
  serbridgeStart(0, flashConfig.telnet_port0, flashConfig.telnet_port0mode, flashConfig.telnet_port0pass);
  serbridgeStart(1, flashConfig.telnet_port1, flashConfig.telnet_port1mode, flashConfig.telnet_port1pass);

  return HTTPD_CGI_DONE;
}

int ICACHE_FLASH_ATTR cgiTelnet(HttpdConnData *connData) {
  if (connData->conn == NULL)
    return HTTPD_CGI_DONE;  // Connection aborted. Clean up.
  if (connData->requestType == HTTPD_METHOD_GET) {
    return cgiTelnetGet(connData);
  } else if (connData->requestType == HTTPD_METHOD_POST) {
    return cgiTelnetSet(connData);
  } else {
    jsonHeader(connData, 404);
    return HTTPD_CGI_DONE;
  }
}

static ICACHE_FLASH_ATTR char *portMode2string(int8_t m) {  //Should we put this into flash?
  if (m < 0 || m >= nPortModes)
    return "?";
  return portMode[m];
}

static ICACHE_FLASH_ATTR int string2portMode(char *s) {
  for (int i = 0; i < nPortModes; i++)
    if (strcmp(s, portMode[i]) == 0) {
      return i;
    }
  return -1;
}

// Randy - Not sure why this function is here? I do not see it called anywhere. Delete?
// FIXME Randy - any answer to the question above?
// print various Telnet information into json buffer
int ICACHE_FLASH_ATTR printTelnetSecurity(char *buff) {
  int len; //

// clang-format off
  len = os_sprintf(buff,
                  "{"
                    "\"port0\": \"%d\","
                    "\"port1\": \"%d\","
                    "\"port0mode\": \"%s\","
                    "\"port1mode\": \"%s\","
                    "\"port0pass\": \"%s\","
                    "\"port1pass\": \"%s\"" 
                  "}",
                  flashConfig.telnet_port0,
                  flashConfig.telnet_port1,
                  portMode2string(flashConfig.telnet_port0mode), 
                  portMode2string(flashConfig.telnet_port1mode),
                  flashConfig.telnet_port0pass,
                  flashConfig.telnet_port1pass);
// clang-format on

  return len;
}