const ids = {
  envLight: document.getElementById("env-light"),
  envTemp: document.getElementById("env-temp"),
  envHumidity: document.getElementById("env-humidity"),
  envRaw: document.getElementById("env-raw"),
  envUpdated: document.getElementById("env-updated"),
  nodecMode: document.getElementById("nodec-mode"),
  nodecLamp: document.getElementById("nodec-lamp"),
  nodecWindow: document.getElementById("nodec-window"),
  nodecRaw: document.getElementById("nodec-raw"),
  nodecUpdated: document.getElementById("nodec-updated"),
  nodebLamp: document.getElementById("nodeb-lamp"),
  nodebWindow: document.getElementById("nodeb-window"),
  nodebResult: document.getElementById("nodeb-result"),
  nodebRaw: document.getElementById("nodeb-raw"),
  nodebUpdated: document.getElementById("nodeb-updated"),
  brokerBadge: document.getElementById("broker-badge"),
  generatedAt: document.getElementById("generated-at"),
  commandStatus: document.getElementById("command-status"),
  logList: document.getElementById("log-list"),
};

function text(value, fallback = "-") {
  return value === null || value === undefined || value === "" ? fallback : String(value);
}

function updateHeartbeat(name, data) {
  const statusEl = document.getElementById(`hb-${name}-status`);
  const timeEl = document.getElementById(`hb-${name}-time`);
  statusEl.textContent = data.online ? "ONLINE" : "OFFLINE";
  timeEl.textContent = text(data.last_seen, "no heartbeat");
}

function renderLogs(logs) {
  if (!logs.length) {
    ids.logList.innerHTML = `<div class="log-entry"><div class="log-dir system">INFO</div><div class="log-topic">console</div><div class="log-payload">아직 수신된 로그가 없습니다.</div></div>`;
    return;
  }

  ids.logList.innerHTML = logs
    .slice(0, 40)
    .map(
      (log) => `
        <div class="log-entry">
          <div class="log-dir ${log.direction}">${text(log.direction).toUpperCase()}</div>
          <div>
            <div class="log-topic">${text(log.topic)}</div>
            <div class="log-meta">${text(log.time)}</div>
          </div>
          <div class="log-payload">${text(log.payload)}</div>
        </div>
      `
    )
    .join("");
}

async function refreshState() {
  try {
    const response = await fetch("/api/state");
    const data = await response.json();

    ids.envLight.textContent = text(data.env.light);
    ids.envTemp.textContent = data.env.temp === null || data.env.temp === undefined ? "-" : `${data.env.temp} C`;
    ids.envHumidity.textContent = data.env.humidity === null || data.env.humidity === undefined ? "-" : `${data.env.humidity} %`;
    ids.envRaw.textContent = text(data.env.raw, "No data");
    ids.envUpdated.textContent = text(data.env.last_updated, "waiting");

    ids.nodecMode.textContent = text(data.nodeC.mode);
    ids.nodecLamp.textContent = text(data.nodeC.lamp);
    ids.nodecWindow.textContent = text(data.nodeC.window);
    ids.nodecRaw.textContent = text(data.nodeC.raw, "No data");
    ids.nodecUpdated.textContent = text(data.nodeC.last_updated, "waiting");

    ids.nodebLamp.textContent = text(data.nodeB.lamp);
    ids.nodebWindow.textContent = text(data.nodeB.window);
    ids.nodebResult.textContent = text(data.nodeB.result);
    ids.nodebRaw.textContent = text(data.nodeB.raw, "No data");
    ids.nodebUpdated.textContent = text(data.nodeB.last_updated, "waiting");

    updateHeartbeat("nodeA", data.heartbeat.nodeA);
    updateHeartbeat("nodeB", data.heartbeat.nodeB);
    updateHeartbeat("nodeC", data.heartbeat.nodeC);

    ids.brokerBadge.textContent = data.broker.connected ? "broker online" : "broker offline";
    ids.brokerBadge.className = `badge ${data.broker.connected ? "online" : "offline"}`;
    ids.generatedAt.textContent = text(data.generated_at);

    renderLogs(data.logs || []);
  } catch (error) {
    ids.commandStatus.textContent = `refresh failed: ${error}`;
  }
}

async function sendCommand(command) {
  ids.commandStatus.textContent = `sending ${command}...`;
  try {
    const response = await fetch("/api/command", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ command }),
    });
    const data = await response.json();
    if (!response.ok) {
      throw new Error(data.error || "request failed");
    }
    ids.commandStatus.textContent = `sent ${data.topic} ${data.payload}`;
    await refreshState();
  } catch (error) {
    ids.commandStatus.textContent = `command failed: ${error.message}`;
  }
}

document.querySelectorAll("button[data-command]").forEach((button) => {
  button.addEventListener("click", () => sendCommand(button.dataset.command));
});

refreshState();
setInterval(refreshState, 1000);
