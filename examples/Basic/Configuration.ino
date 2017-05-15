#include "HookAction.cpp"

int tryParseByte(String value)
{
  if (value == "")
    return 0;
  return value.toInt();
}

void initConfiguration()
{
  HookAction hookAction("configure", [](NameValueCollection parameters) { //?temperatureInterval=[seconds]&pingInterval=[seconds]
    int temperatureInterval = tryParseByte(parameters["temperatureInterval"]);
    if (temperatureInterval != 0)
      cfg.temperatureInterval = temperatureInterval;

    int pingInterval = tryParseByte(parameters["pingInterval"]);
    if (pingInterval != 0)
      cfg.pingInterval = pingInterval;

    configureTimers();

    return 200;
  });

  hooks.registerAction(hookAction);
}
