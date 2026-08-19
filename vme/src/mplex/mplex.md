# mplex TLS/SSL Setup Guide

## Overview
Websocket support is enabled with the `-w` flag, and TLS on top of it with `-t`:

- `mplex -w` listens for plain `ws://`. No certificates are read and none are
  needed. This is the one to use on localhost, where a browser is happy with
  `ws://` and getting a certificate it will accept is a nuisance.
- `mplex -w -t` listens for `wss://`, and needs the two certificate files
  below. This is what a public server should run: a page served over https
  cannot open a `ws://` connection at all.

The two are separate listeners, so a host that wants both runs two mplexers on
two ports.

> For a while `-t` set a flag that nothing read, and `-w` alone listened for
> TLS regardless. A browser pointed at `ws://` got "TLS handshake failed" while
> the log said `TLS = false` on the line above. Fixed; the log now names the
> scheme it is actually listening for.

## Prerequisites
- OpenSSL development libraries (only needed for `-t`)
- Compiled mplex with TLS support (default in current build)

## TLS Certificate Files
mplex requires two certificate files in the **same directory** where mplex is executed (typically `~/diku/DikuMUD3/vme/bin/`):

### 1. server.pem
- **Purpose**: Contains both the SSL certificate AND private key
- **Format**: PEM format
- **Required**: Yes

### 2. dh.pem
- **Purpose**: Diffie-Hellman parameters for key exchange
- **Format**: PEM format
- **Required**: No, in practice. TLS 1.3 does not use these parameters, and
  the exception thrown when the file is missing is caught and logged rather
  than being fatal. You will see `TLS init exception: use_tmp_dh_file: No such
  file or directory` in mplex.log and the connection will still complete at
  TLSv1.3. Supply it anyway if you need to support older clients.
- **Security**: 2048-bit recommended for modern security

The server.pem and dh.pem form the websocketpp sample folder /websocketpp/examples/echo_server_tls/ have been copied into the src/mplex folder for easy testing.

## Setup Options

### Option 1: Copy Certificate Files (Recommended)
```bash
cd ~/diku/DikuMUD3/vme/bin
cp ../src/mplex/server.pem .
cp ../src/mplex/dh.pem .
```

### Option 2: Make your own
```bash
cd ~/diku/DikuMUD3/vme/bin
openssl req -x509 -newkey rsa:2048 -keyout key.pem -out cert.pem \
  -days 365 -nodes -subj "/CN=localhost" \
  -addext "subjectAltName=DNS:localhost,IP:127.0.0.1"
cat key.pem cert.pem > server.pem     # one file holds both
openssl dhparam -out dh.pem 2048      # optional, see above
```

Two things that will bite otherwise:

- **server.pem must contain the key *and* the certificate.** mplex reads the
  same filename for both (`use_certificate_chain_file` then
  `use_private_key_file`), hence the `cat`.
- **Use `-nodes`, so the key is not encrypted.** The password callback in
  echo_server.cpp returns a hardcoded `"test"`, so an encrypted key only opens
  if that happens to be its passphrase.

### Option 3: A real certificate
For a server on the open internet, get one for the actual hostname (Let's
Encrypt or similar) and concatenate the private key and the full chain into
server.pem. This is the only option that a visiting browser will accept
without being told to.

## Running mplex

### Plain websocket, for local development:
```bash
cd ~/diku/DikuMUD3/vme/bin
./mplex -w -p 4280
```
Then point the client at it with `protocol=ws`:
`https://www.valhalla.com/v3/client/client-a.html?protocol=ws&host=localhost&port=4280&endpoint=%2Fecho&reader=false`

### Start TLS-enabled mplex:
```bash
cd ~/diku/DikuMUD3/vme/bin
./mplex -w -t
```

### Flags:
- `-w`: WebSocket mode. Without `-t` this is plain `ws://`
- `-t`: Enable TLS/SSL support, making it `wss://`. Requires `-w`

## Testing TLS Connection

### Test with wscat (ignoring certificate validation):
```bash
wscat -c wss://localhost:4242 --no-check
```

The --no-check flag means it won't test for the authority validity of the keys.

### Test with certificate validation (production):
```bash
wscat -c wss://localhost:4242
```

or connect with a webbrowser:

https://www.valhalla.com/v3/client/client-a.html?protocol=wss&host=localhost&port=4242&endpoint=%2Fecho&reader=false


## Security Considerations

- **Production**: Use certificates from a trusted CA
- **Development**: Self-signed certificates are fine with `--no-check` in
  wscat, but see the browser caveat below -- plain `ws://` is usually easier
- **File Permissions**: Restrict access to private key files:
  ```bash
  chmod 600 server.pem dh.pem
  ```
- **Certificate Renewal**: Update certificates before expiration

## The catch with self-signed certificates

**A browser will not prompt you about a self-signed certificate on a `wss://`
connection.** There is no clickthrough interstitial for websockets the way
there is for a page load: the connection simply fails, and the console rarely
says why. mplex is working correctly and there is nothing in mplex.log to find.

The workaround is to make the browser accept the certificate through a normal
page load first:

1. Visit `https://localhost:4280/` directly in the same browser.
2. Accept the security warning ("Advanced" -> "Proceed", or "Accept the Risk").
   mplex will not serve a page, so expect an error *after* the warning -- the
   warning is the part that matters.
3. Go back to the client. The `wss://` connection to that same host and port
   now inherits the exception.

It is per-browser and per-port, and it lapses whenever the certificate changes.

**This is why `-w` without `-t` exists.** On localhost, plain `ws://` avoids
the whole dance, and browsers are perfectly happy with it as long as the page
itself is not served over https. Save the certificates for the public server,
where a page on https leaves you no choice.

## Troubleshooting

### Common Issues:

1. **"TLS init exception: use_certificate_chain_file: No such file or directory"**
   - **Cause**: Missing server.pem or dh.pem files
   - **Fix**: Copy/create certificate files in mplex execution directory

2. **"TLS handshake failed"**
   - **Cause**: Most often the client is speaking plain `ws://` to an mplex
     started with `-t`. No certificate is looked at, so moving certificate
     files around will not help. Otherwise: invalid PEM format or permissions.
   - **Fix**: Match the two ends -- `ws://` to `mplex -w`, `wss://` to
     `mplex -w -t`. The first line of mplex.log now says which one is running.

3. **The browser connects to nothing and says nothing**
   - **Cause**: `wss://` with a self-signed certificate the browser has not
     been told to trust.
   - **Fix**: See "The catch with self-signed certificates" above, or use
     plain `ws://` locally.

4. **Connection refused**
   - **Cause**: mplex not running or wrong port
   - **Fix**: Check mplex is running and the port matches

### Log Monitoring:
```bash
cd ~/diku/DikuMUD3/vme/bin
tail -f mplex.log
```

Look for:
- `TLS context initialized successfully` (good)
- `TLS init exception` (bad)
- `IP connection from: 127.0.0.1` (connection established)

## File Locations Summary

```
~/diku/DikuMUD3/vme/src/mplex/    # Source directory
└── mplex.cpp     # Source code

~/diku/DikuMUD3/vme/bin/          # Execution directory  
├── mplex         # Compiled binary
├── server.pem    # REQUIRED: Certificate file (copy or symlink)
├── dh.pem        # REQUIRED: DH parameters (copy or symlink)
└── mplex.log     # Runtime log
```

## Default Configuration

- **TLS Port**: 4242
- **Protocol**: TLS 1.2+
- **Cipher Suites**: Mozilla Modern/Intermediate compatible
- **Disabled Protocols**: SSLv2, SSLv3

The TLS implementation is production-ready and follows modern security best practices.
