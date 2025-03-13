#ifndef GOOGLE_CALENDAR_H
#define GOOGLE_CALENDAR_H

#include <Arduino.h>

struct CalendarEvent {
    String name;
    tm startTime;
};

class GoogleCalendarAPI {
public:
    GoogleCalendarAPI(String calendarId = "primary") : calendarId(calendarId) {};

    CalendarEvent* pollEvents(int* numEvents);

private:
    String calendarId;
};

#endif // GOOGLE_CALENDAR_H