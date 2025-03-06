#ifndef GOOGLE_CALENDAR_H
#define GOOGLE_CALENDAR_H

#include <Arduino.h>

class GoogleCalendarAPI {
public:
    GoogleCalendarAPI(String apiKey, String calendarId = "primary") : apiKey(apiKey), calendarId(calendarId) {};

    CalendarEvent* pollEvents(int* numEvents);

private:
    String apiKey;
    String calendarId;
};

struct CalendarEvent {
    String name;
    tm startTime;
};

#endif // GOOGLE_CALENDAR_H