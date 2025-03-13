#include "googleCalendar.h"

#include <HTTPClient.h>
#include <ArduinoJson.h>

static char LOG_TAG[] = "GoogleCalendar";

CalendarEvent* GoogleCalendarAPI::pollEvents(int* numEvents) {
    HTTPClient client;

    client.begin("http://192.168.137.1:8000/calendar/v3/calendars/" + calendarId + "/events");
    // client.addHeader("Authorization", "Bearer " + apiKey);

    int res = client.GET();
    if (res > 0) {
        auto out = client.getString();
        JsonDocument doc;
        deserializeJson(doc, out);
        
        JsonArray items = doc["items"];

        CalendarEvent* calendarEvents = new CalendarEvent[items.size()];

        for (int i = 0; i < items.size(); i++) {
            JsonVariant item = items[i];
            calendarEvents[i].name = item["summary"].as<String>();
            
            strptime(item["start"]["dateTime"].as<String>().c_str(), "%Y-%m-%dT%H:%M:%S.%f", &calendarEvents[i].startTime);
        }

        *numEvents = items.size();
        return calendarEvents;

    } else {
        ESP_LOGE(LOG_TAG, "Error fetching events from the google calendar API!");
    }

    return nullptr;
}