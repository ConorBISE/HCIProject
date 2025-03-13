import { serve } from "https://deno.land/std@0.114.0/http/server.ts";

const targetUrl = "https://www.googleapis.com";

const tokenData = JSON.parse(await Deno.readTextFile("token.json"));

async function handler(req: Request): Promise<Response> {
    const url = new URL(req.url);
    const proxyUrl = `${targetUrl}${url.pathname}${url.search}`;

    console.log(`Received request: ${req.method} ${url.pathname}${url.search}`);

    const headers = new Headers(req.headers);
    headers.set("Authorization", `Bearer ${tokenData.access_token}`);

    const proxyReq = new Request(proxyUrl, {
        method: req.method,
        headers: headers,
        body: req.body,
    });

    const proxyRes = await fetch(proxyReq);
    const responseHeaders = new Headers(proxyRes.headers);

    return new Response(proxyRes.body, {
        status: proxyRes.status,
        statusText: proxyRes.statusText,
        headers: responseHeaders,
    });
}

console.log("Proxy server is running on http://0.0.0.0:8000");
await serve(handler, { addr: "0.0.0.0:8000" });