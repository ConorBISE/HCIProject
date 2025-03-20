To run:
- Install Deno
- Create a Google Cloud Project, and generate a OAuth client ID and secret. Put the client ID into integrationsAuth/auth.ts, and the client secret into proxy.ts
- Run deno -A run auth.ts. Navigate to localhost:8080, and authorize access to your Google account
- Change the value of CALENDAR_ID in main.cpp to either a calendar you wish to poll for events, or "primary" for your primary calendar
- Run deno -A run proxy.ts in the background. Configure main.cpp with the SSID and password of your mobile hotspot
- Upload the project to the firebeetle