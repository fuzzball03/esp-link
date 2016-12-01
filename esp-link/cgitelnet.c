#include <esp8266.h>
#include "cgi.h"
#include "config.h"
#include "serbridge.h"

#define TELNET_DBG
#ifdef TELNET_DBG
#define DBG(format, ...)                                                       \
  do {                                                                         \
    os_printf(format, ##__VA_ARGS__);                                         \
  } while (0)
#else
#define DBG(format, ...)                                                       \
  do {                                                                         \
  } while (0)
#endif

// The three definitions below need to match with each other, and with the
// content of html/ui.js:ajaxSelectPresets
const static int nPortModes = 4;
static char *portMode[] = {"open", "disabled", "secure", "password"};
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
static int passCheck(char *s);
static char *safePassCheck(int i);

// Cgi to return undefined
int ICACHE_FLASH_ATTR cgiUndefined(HttpdConnData *connData) {
  httpdStartResponse(connData, 204);
  httpdEndHeaders(connData);
  return HTTPD_CGI_DONE;
}

// Cgi to return current Telnet bridge settings
int ICACHE_FLASH_ATTR cgiTelnetGet(HttpdConnData *connData) {
  char buff[160];

  if (connData->conn == NULL)
    return HTTPD_CGI_DONE; // Connection aborted

  int len;

  DBG("Current telnet ports: port0=%d (mode %d %s) port1=%d (mode %d %s)\n",
      flashConfig.telnet_port0, flashConfig.telnet_port0mode,
      portMode2string(flashConfig.telnet_port0mode), flashConfig.telnet_port1,
      flashConfig.telnet_port1mode,
      portMode2string(flashConfig.telnet_port1mode));
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
                  safePassCheck(0),
                  safePassCheck(1));
  // clang-format on

  jsonHeader(connData, 200);
  httpdSend(connData, buff, len);

  return HTTPD_CGI_DONE;
}

// Cgi to change choice of Telnet settings. Must receive all variables!
int ICACHE_FLASH_ATTR cgiTelnetSet(HttpdConnData *connData) {
  char buf[80];

  if (connData->conn == NULL) {
    return HTTPD_CGI_DONE; // Connection aborted
  }

  // Let's parse our url
  int8_t ok0, ok1, mok0, mok1, pok0, pok1;
  int8_t allOk =
      1; // Set this to 0 during any sanity check so we return an error
  uint16_t port0, port1;
  char port0mode[16], port1mode[16], port0pass[16], port1pass[16];

  ok0 = getUInt16Arg(connData, "port0", &port0);
  ok1 = getUInt16Arg(connData, "port1", &port1);
  mok0 = getStringArg(connData, "port0mode", port0mode, sizeof(port0mode));
  mok1 = getStringArg(connData, "port1mode", port1mode, sizeof(port1mode));
  pok0 = getStringArg(connData, "port0pass", port0pass, sizeof(port0pass));
  pok1 = getStringArg(connData, "port1pass", port1pass, sizeof(port1pass));

  DBG(buf, "CgiTelnetSet -> Failed to parse all values form url.\n ok0: %d "
           "ok1: %d mok0: %d "
           "mok1: %d pok0: %d pok1: %d",
      ok0, ok1, mok0, mok1, pok0, pok1);

  if (ok0 + ok1 + mok0 + mok1 + pok0 + pok1 != 6) {
    os_sprintf(buf, "CgiTelnetSet -> Failed to parse all values form url.\n "
                    "ok0: %d ok1: %d "
                    "mok0: %d mok1: %d pok0: %d pok1: %d",
               ok0, ok1, mok0, mok1, pok0, pok1);
    allOk = 0;
  }

  // ###### Various sanity checks
  // Port numbers
  if (port0 == port1 || port0 == 80 || port1 == 80 || port0 == 423 ||
      port1 == 423) {
    os_sprintf(buf, "Ports cannot be the same(or used reserved port).\n Tried "
                    "to set: port0=%d port1=%d\n",
               port0, port1);
    allOk = 0;
  }

  // Password management
  if (strlen(port0pass) > sizeof(flashConfig.telnet_port0pass)) {
    // FIXME Shouldnt this be printing max %d from the stored flash size not the
    // pass received via url?
    os_sprintf(buf, "Port 0 password too long (max %d)", strlen(port0pass));
    allOk = 0;
  }

  if (strlen(port1pass) > sizeof(flashConfig.telnet_port1pass)) {
    os_sprintf(buf, "Port 1 password too long (max %d)", strlen(port1pass));
    allOk = 0;
  }

  // FIXME
  // I Think we need more sanity checks for a password besides a valid string?
  // Check for invalid characters, ie TAB \n \r non ascii chars

  if (allOk) { // Let's store all values to flash
    flashConfig.telnet_port0 = port0;
    flashConfig.telnet_port1 = port1;
    flashConfig.telnet_port0mode = string2portMode(port0mode);
    flashConfig.telnet_port1mode = string2portMode(port1mode);
    if (passCheck(port1pass)) {
      strcpy(flashConfig.telnet_port0pass, port0pass);
      DBG("Password0 changed to \"%s\"\n", port0pass);
    } else {
      DBG("Password0 remains unchanged. Received: \"%s\"\n", port0pass);
    }
    if (passCheck(port1pass)) {
      strcpy(flashConfig.telnet_port1pass, port1pass);
      DBG("Password1 changed to \"%s\"\n", port1pass);
    } else {
      DBG("Password1 remains unchanged. Received: \"%s\"\n", port1pass);
    }
  } else {
    errorResponse(connData, 400, buf);
  }

  // save to flash
  if (configSave()) {
    httpdStartResponse(connData, 204);
    httpdEndHeaders(connData);
    // Let's inform the user that changes have been made
    os_printf("Telnet ports changed: port0=%d port1=%d\n",
              flashConfig.telnet_port0, flashConfig.telnet_port1);
    // Reset telnet bridge after good configSave
    serbridgeClose(); // Close existing connections
    serbridgeInit();
    serbridgeStart(0, flashConfig.telnet_port0, flashConfig.telnet_port0mode,
                   flashConfig.telnet_port0pass);
    serbridgeStart(1, flashConfig.telnet_port1, flashConfig.telnet_port1mode,
                   flashConfig.telnet_port1pass);
  } else {
    httpdStartResponse(connData, 500);
    httpdEndHeaders(connData);
    httpdSend(connData, "Failed to save config", -1);
  }

  return HTTPD_CGI_DONE;
}

int ICACHE_FLASH_ATTR cgiTelnet(HttpdConnData *connData) {
  if (connData->conn == NULL)
    return HTTPD_CGI_DONE; // Connection aborted. Clean up.
  if (connData->requestType == HTTPD_METHOD_GET) {
    return cgiTelnetGet(connData);
  } else if (connData->requestType == HTTPD_METHOD_POST) {
    return cgiTelnetSet(connData);
  } else {
    jsonHeader(connData, 404);
    return HTTPD_CGI_DONE;
  }
}

/*
HELPER AND UTILITIE FUNCTIONS
*/
static ICACHE_FLASH_ATTR char *portMode2string(int8_t m) {
  // Should we put this into flash?
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

// Place any invalid password masks here
static char *passBadMasks[] = {"********", "", "INVALID", "password"};

static ICACHE_FLASH_ATTR int passCheck(char *s) {
  DBG("Checking pass %s against bad masks", s);
  for (int i = 0; i < sizeof(passBadMasks); i++) {
    // Do I need to use 'sizeof(x)/sizeof(x[1])' or is it better to just assign
    // a static int?
    if (strcmp(s, passBadMasks[i]) == 0) {
      DBG("Pass matches bad mask # %d.\n %s matches %s \n", i, passBadMasks[i],
          s);
      return 0;
    }
  }
  return 1;
}

static ICACHE_FLASH_ATTR char *safePassCheck(int i) {
  if (i == 0) {
    if (strcmp(flashConfig.telnet_port0pass, flashDefault.telnet_port0pass) ==
        0) {
      return flashConfig.telnet_port0pass;
    } else {
      return "********";
    }
  }
  if (i == 1) {
    if (strcmp(flashConfig.telnet_port1pass, flashDefault.telnet_port1pass) ==
        0) {
      return flashConfig.telnet_port1pass;
    } else {
      return "********";
    }
  }
  return "INVALID"; // No valid value given
}
