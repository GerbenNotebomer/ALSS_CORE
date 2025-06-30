#include "web_server_manager.h"
#include "routes/route_root.h"
#include "routes/route_dashboard.h"
#include "routes/route_calibratie.h"
#include "routes/route_debug_log.h"
#include "routes/route_sd_card.h"
#include "routes/route_ota.h"
#include "routes/route_meters.h"
#include "routes/route_reboot.h"
#include "routes/route_settings.h"
#include "routes/route_weersvoorspelling.h"
#include "routes/route_json.h"

AsyncWebServer WebServerManager::server(80);

//  --  begin  --  //
void WebServerManager::begin()
{
    RouteJson::registerRoutes(server);
    RouteRoot::registerRoutes(server);
    RouteDashboard::registerRoutes(server);
    RouteCalibration::registerRoutes(server);
    RouteDebugLog::registerRoutes(server);
    RouteOTA::registerRoutes(server);
    RouteSdCard::registerRoutes(server);
    RouteMeters::registerRoutes(server);
    RouteReboot::registerRoutes(server);
    RouteSettings::registerRoutes(server);
    RouteWeersvoorspelling::registerRoutes(server);
    server.serveStatic("/logo", LittleFS, "/logo");
    server.begin();
}