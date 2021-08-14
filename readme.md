# Sequence Animator Controller

 - ESP8266 (ESP12F)
 - Arduino Framework

# Requests

The following types of requests are supported by the Controller's web API.

```sh
curl \
  --request POST \
  --data '{"keys": [ {"type": "light", "index": 1, "color": "#123456"} ]}' \
  http://192.168.1.219/api/state
```

```sh
curl \
  --request POST \
  --data '{"keys": [ {"type": "audio", "index": 1, "volume": 10} ]}' \
  http://192.168.1.219/api/state

```