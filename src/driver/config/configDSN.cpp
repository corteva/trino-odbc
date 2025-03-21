#include "../../util/windowsLean.hpp"
#include <sql.h>
#include <sqlext.h>

#include <map>
#include <odbcinst.h>
#include <string>

#include "../../util/cryptUtils.hpp"
#include "../../util/delimKvpHelper.hpp"
#include "../../util/writeLog.hpp"
#include "dsnConfigForm.hpp"
#include "profileReader.hpp"

void writeToProfile(DriverConfig result,
                    std::string section,
                    std::string value) {
  std::string dsn = result.getDSN();
  if (section == "clientSecret") {
    SQLWritePrivateProfileString(dsn.c_str(),
                                 "encryptedClientSecret",
                                 encryptString(value).c_str(),
                                 "ODBC.INI");
  } else {
    SQLWritePrivateProfileString(
        dsn.c_str(), section.c_str(), value.c_str(), "ODBC.INI");
  }
}

BOOL APIENTRY ConfigDSN(HWND hwndParent,
                        WORD fRequest,
                        LPCSTR lpszDriver,
                        LPCSTR lpszAttributes) {

  WriteLog(LL_INFO, "Entering ConfigDSN");

  std::map<std::string, std::string> attributes =
      parseKVPsFromNullDelimChars(lpszAttributes);

  WriteLog(LL_TRACE, "  lpszAttributes are: ");
  WriteLog(LL_TRACE, attributes);

  DSNForm form = DSNForm(hwndParent, attributes);

  switch (fRequest) {
    case ODBC_ADD_DSN: {
      WriteLog(LL_TRACE, "  Request to add a DSN");
      form.ShowDSNForm();
      SQLWriteDSNToIni(form.getResult().getDSN().c_str(), lpszDriver);
      break;
    }
    case ODBC_CONFIG_DSN: {
      WriteLog(LL_TRACE, "  Request to configure a DSN");
      // For a config operation, we already have a DSN configured.
      // We want to load that profile and populate the form from
      // the values from that prior configuration.
      std::string dsn     = attributes.at("DSN");
      DriverConfig config = readDriverConfigFromProfile(dsn);
      form.setReadOnlyDSN();
      form.updateConfig(config);
      form.ShowDSNForm();
      break;
    }
    case ODBC_REMOVE_DSN: {
      WriteLog(LL_TRACE, "  Request to remove a DSN");
      SQLRemoveDSNFromIni(form.getResult().getDSN().c_str());
      break;
    }
    default: {
      WriteLog(LL_WARN, "  Unknown request");
      return false;
      break;
    }
  }

  WriteLog(LL_TRACE, "  Grabbing form result");
  DriverConfig result = form.getResult();

  if (result.getIsSaved() and SQLValidDSN(result.getDSN().c_str())) {
    std::map<std::string, std::string> kvps = driverConfigToKVPs(result);

    // Just write every attribute of the driver config to the profile.
    for (auto it = kvps.begin(); it != kvps.end(); it++) {
      writeToProfile(result, it->first, it->second);
    }
    return true;
  } else {
    return false;
  }
}
