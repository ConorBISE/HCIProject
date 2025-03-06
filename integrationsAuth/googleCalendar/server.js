const CLIENT_ID = "953293647208-13trpjaab44iuj5ofllik4mjh7ulbo6r.apps.googleusercontent.com"
const CLIENT_SECRET = new TextDecoder().decode(Deno.readFileSync("secretKey"));
const REDIRECT_URI = "http://localhost:8080/redirect"
const SCOPE = "https://www.googleapis.com/auth/calendar.events.readonly"
const AUTH_URL = `https://accounts.google.com/o/oauth2/v2/auth?client_id=${CLIENT_ID}&redirect_uri=${REDIRECT_URI}&response_type=code&scope=${SCOPE}`

Deno.serve({port: 8080}, async (req) => {
    if (req.url.indexOf("/redirect") != -1) {
        const code = new URL(req.url).searchParams.get("code");
        const CODE_URL = `https://oauth2.googleapis.com/token?client_id=${CLIENT_ID}&client_secret=${CLIENT_SECRET}&code=${code}&grant_type=authorization_code&redirect_uri=${REDIRECT_URI}`

        const res = await (await fetch(CODE_URL, { method: "POST" })).text()
        return new Response(res)
    } else {
        return Response.redirect(AUTH_URL);
    }

});