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
- **Required**: Yes
- **Security**: 2048-bit recommended for modern security

The server.pem and dh.pem form the websocketpp sample folder /websocketpp/examples/echo_server_tls/ have been copied into the src/mplex folder for easy testing.

## Setup Options

### Option 1: Copy Certificate Files (Recommended)
```bash
cd ~/diku/DikuMUD3/vme/bin
cp ../src/mplex/server.pem .
cp ../src/mplex/dh.pem .
```

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
- **Development**: Self-signed certificates are fine with `--no-check`
- **File Permissions**: Restrict access to private key files:
  ```bash
  chmod 600 server.pem dh.pem
  ```
- **Certificate Renewal**: Update certificates before expiration

## Troubleshooting

### Common Issues:

1. **"TLS init exception: use_certificate_chain_file: No such file or directory"**
   - **Cause**: Missing server.pem or dh.pem files
   - **Fix**: Copy/create certificate files in mplex execution directory

2. **"TLS handshake failed"**
   - **Cause**: Invalid certificate format or permissions
   - **Fix**: Verify PEM format and file permissions

3. **Connection refused**
   - **Cause**: mplex not running or wrong port
   - **Fix**: Ensure mplex is running with `-w -t` flags

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
