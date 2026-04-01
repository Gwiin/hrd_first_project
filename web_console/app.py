from __future__ import annotations

import os
import threading
from collections import deque
from copy import deepcopy
from datetime import datetime
from typing import Any

from flask import Flask, jsonify, render_template, request
import paho.mqtt.client as mqtt


BROKER_HOST = os.getenv("MQTT_BROKER_HOST", "163.152.213.111")
BROKER_PORT = int(os.getenv("MQTT_BROKER_PORT", "1883"))
MQTT_USERNAME = os.getenv("MQTT_USERNAME", "")
MQTT_PASSWORD = os.getenv("MQTT_PASSWORD", "")
MQTT_SUBSCRIBE_TOPIC = os.getenv("MQTT_SUBSCRIBE_TOPIC", "house/#")
HEARTBEAT_TIMEOUT_S = int(os.getenv("HEARTBEAT_TIMEOUT_S", "6"))
WEB_CONSOLE_PORT = int(os.getenv("WEB_CONSOLE_PORT", "5000"))
LOG_LIMIT = int(os.getenv("WEB_CONSOLE_LOG_LIMIT", "200"))

COMMAND_TOPICS = {
    "mode_auto": ("house/mode", "AUTO"),
    "mode_manual": ("house/mode", "MANUAL"),
    "light_on": ("house/cmd/light", "ON"),
    "light_off": ("house/cmd/light", "OFF"),
    "window_open": ("house/cmd/window", "OPEN"),
    "window_close": ("house/cmd/window", "CLOSE"),
}


def utc_timestamp() -> str:
    return datetime.now().strftime("%Y-%m-%d %H:%M:%S")


def parse_key_value_payload(payload: str) -> dict[str, str]:
    parsed: dict[str, str] = {}

    for part in payload.split(","):
        if "=" not in part:
            continue
        key, value = part.split("=", 1)
        parsed[key.strip()] = value.strip()

    return parsed


def normalize_numeric_fields(parsed: dict[str, str]) -> dict[str, Any]:
    normalized: dict[str, Any] = {}

    for key, value in parsed.items():
        if key == "light":
            try:
                normalized[key] = int(float(value))
                continue
            except ValueError:
                pass

        if key in {"temp", "humidity"}:
            try:
                normalized[key] = float(value)
                continue
            except ValueError:
                pass

        normalized[key] = value

    return normalized


class ConsoleState:
    def __init__(self) -> None:
        self._lock = threading.Lock()
        self._logs: deque[dict[str, Any]] = deque(maxlen=LOG_LIMIT)
        self._state: dict[str, Any] = {
            "broker": {
                "host": BROKER_HOST,
                "port": BROKER_PORT,
                "subscribe_topic": MQTT_SUBSCRIBE_TOPIC,
                "connected": False,
                "last_error": "",
                "last_connect_at": None,
            },
            "env": {
                "light": None,
                "temp": None,
                "humidity": None,
                "raw": "",
                "last_updated": None,
            },
            "nodeB": {
                "lamp": None,
                "window": None,
                "result": None,
                "raw": "",
                "last_updated": None,
            },
            "nodeC": {
                "mode": None,
                "light": None,
                "temp": None,
                "humidity": None,
                "lamp": None,
                "window": None,
                "raw": "",
                "last_updated": None,
            },
            "heartbeat": {
                "nodeA": {"payload": None, "last_seen": None, "online": False},
                "nodeB": {"payload": None, "last_seen": None, "online": False},
                "nodeC": {"payload": None, "last_seen": None, "online": False},
            },
            "last_topics": {},
        }

    def add_log(self, direction: str, topic: str, payload: str) -> None:
        entry = {
            "time": utc_timestamp(),
            "direction": direction,
            "topic": topic,
            "payload": payload,
        }
        with self._lock:
            self._logs.appendleft(entry)
            self._state["last_topics"][topic] = entry

    def set_broker_connected(self, connected: bool, error: str = "") -> None:
        with self._lock:
            broker = self._state["broker"]
            broker["connected"] = connected
            broker["last_error"] = error
            if connected:
                broker["last_connect_at"] = utc_timestamp()

    def update_from_message(self, topic: str, payload: str) -> None:
        now = utc_timestamp()
        with self._lock:
            if topic == "house/env":
                parsed = normalize_numeric_fields(parse_key_value_payload(payload))
                self._state["env"].update(
                    {
                        "light": parsed.get("light"),
                        "temp": parsed.get("temp"),
                        "humidity": parsed.get("humidity"),
                        "raw": payload,
                        "last_updated": now,
                    }
                )
                return

            if topic == "house/status/nodeB":
                parsed = normalize_numeric_fields(parse_key_value_payload(payload))
                self._state["nodeB"].update(
                    {
                        "lamp": parsed.get("lamp"),
                        "window": parsed.get("window"),
                        "result": parsed.get("result"),
                        "raw": payload,
                        "last_updated": now,
                    }
                )
                return

            if topic == "house/status/nodeC":
                parsed = normalize_numeric_fields(parse_key_value_payload(payload))
                self._state["nodeC"].update(
                    {
                        "mode": parsed.get("mode"),
                        "light": parsed.get("light"),
                        "temp": parsed.get("temp"),
                        "humidity": parsed.get("humidity"),
                        "lamp": parsed.get("lamp"),
                        "window": parsed.get("window"),
                        "raw": payload,
                        "last_updated": now,
                    }
                )
                return

            if topic.startswith("house/heartbeat/"):
                node_name = topic.rsplit("/", 1)[-1]
                if node_name in self._state["heartbeat"]:
                    self._state["heartbeat"][node_name].update(
                        {
                            "payload": payload,
                            "last_seen": now,
                            "online": True,
                        }
                    )

    def snapshot(self) -> dict[str, Any]:
        with self._lock:
            snapshot = deepcopy(self._state)
            logs = list(self._logs)

        for item in snapshot["heartbeat"].values():
            item["online"] = self._is_online(item["last_seen"])

        snapshot["logs"] = logs
        snapshot["generated_at"] = utc_timestamp()
        return snapshot

    def logs_snapshot(self) -> list[dict[str, Any]]:
        with self._lock:
            return list(self._logs)

    @staticmethod
    def _is_online(last_seen: str | None) -> bool:
        if not last_seen:
            return False

        try:
            last_dt = datetime.strptime(last_seen, "%Y-%m-%d %H:%M:%S")
        except ValueError:
            return False

        return (datetime.now() - last_dt).total_seconds() <= HEARTBEAT_TIMEOUT_S


app = Flask(__name__)
state = ConsoleState()
mqtt_client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2, client_id="web-console")


def on_connect(client: mqtt.Client, _userdata: Any, _flags: Any, reason_code: Any, _properties: Any) -> None:
    if reason_code == 0:
        state.set_broker_connected(True)
        client.subscribe(MQTT_SUBSCRIBE_TOPIC)
        state.add_log("system", "broker", f"connected to {BROKER_HOST}:{BROKER_PORT}")
    else:
        state.set_broker_connected(False, str(reason_code))
        state.add_log("system", "broker", f"connect failed: {reason_code}")


def on_disconnect(_client: mqtt.Client, _userdata: Any, _flags: Any, reason_code: Any, _properties: Any) -> None:
    state.set_broker_connected(False, f"disconnect: {reason_code}")
    state.add_log("system", "broker", f"disconnected: {reason_code}")


def on_message(_client: mqtt.Client, _userdata: Any, msg: mqtt.MQTTMessage) -> None:
    payload = msg.payload.decode("utf-8", errors="replace")
    state.add_log("rx", msg.topic, payload)
    state.update_from_message(msg.topic, payload)


def configure_mqtt() -> None:
    if MQTT_USERNAME:
        mqtt_client.username_pw_set(MQTT_USERNAME, MQTT_PASSWORD or None)

    mqtt_client.on_connect = on_connect
    mqtt_client.on_disconnect = on_disconnect
    mqtt_client.on_message = on_message
    mqtt_client.connect_async(BROKER_HOST, BROKER_PORT, keepalive=60)
    mqtt_client.loop_start()


@app.get("/")
def index() -> str:
    return render_template("index.html", broker_host=BROKER_HOST, broker_port=BROKER_PORT)


@app.get("/api/state")
def api_state() -> Any:
    return jsonify(state.snapshot())


@app.get("/api/logs")
def api_logs() -> Any:
    return jsonify({"logs": state.logs_snapshot()})


@app.post("/api/command")
def api_command() -> Any:
    payload = request.get_json(silent=True) or {}
    command = payload.get("command", "")

    if command not in COMMAND_TOPICS:
        return jsonify({"ok": False, "error": "unsupported command"}), 400

    topic, message = COMMAND_TOPICS[command]
    info = mqtt_client.publish(topic, message, qos=1)
    if info.rc != mqtt.MQTT_ERR_SUCCESS:
        return jsonify({"ok": False, "error": "mqtt publish failed"}), 503

    state.add_log("tx", topic, message)
    return jsonify({"ok": True, "topic": topic, "payload": message})


if __name__ == "__main__":
    configure_mqtt()
    app.run(host="0.0.0.0", port=WEB_CONSOLE_PORT, debug=False)
