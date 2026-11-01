/**
 * TIVIDY workflow manager front-end.
 *
 * This page holds NO workflow logic. Every action is sent to the C++ engine
 * (the local server started by the tividy_gui program), and the page only
 * draws the status the engine sends back. See API.md for the endpoints.
 */
(function () {
  "use strict";

  var MAX_REQUESTS_FALLBACK = 5; // the engine sends the real limit in status.maxRequests
  var STATE_ORDER = ["Available", "InProgress", "Assigned", "Completed"];

  var current = { view: "welcome", status: null, workflows: [], lastViewBeforeReport: "welcome" };

  function $(id) { return document.getElementById(id); }

  // ---------------------------------------------------------------- API
  /** Sends a request to the C++ engine and returns the parsed JSON. */
  function api(method, path, params) {
    var opts = { method: method, headers: {} };
    if (params) {
      opts.headers["Content-Type"] = "application/x-www-form-urlencoded";
      opts.body = new URLSearchParams(params).toString();
    }
    return fetch(path, opts).then(function (res) {
      return res.json().catch(function () {
        return { ok: false, message: "The engine sent a reply that is not JSON (HTTP " + res.status + ")." };
      });
    }).then(function (data) {
      setEngineStatus(true);
      if (data && data.status) { render(data.status); }
      return data;
    }, function () {
      setEngineStatus(false);
      var msg = "Cannot reach the C++ engine. Start it with ./build/tividy_gui and open http://localhost:8080";
      toast(msg, "bad");
      return { ok: false, message: msg };
    });
  }

  function setEngineStatus(up) {
    var el = $("engine-status");
    el.textContent = up ? "engine: connected" : "engine: not running";
    el.classList.toggle("down", !up);
  }

  // ---------------------------------------------------------------- views
  function show(view) {
    ["welcome", "questions", "create", "board", "report"].forEach(function (v) {
      $("view-" + v).hidden = v !== view;
    });
    $("topbar").hidden = view === "welcome";
    if (view !== "report") { current.lastViewBeforeReport = view; }
    current.view = view;
    if (view === "report") { loadReport(); }
  }

  var BACK = { questions: "welcome", create: "questions", board: "create" };

  function goBack() {
    if (current.view === "report") { show(current.lastViewBeforeReport); return; }
    show(BACK[current.view] || "welcome");
  }

  // ---------------------------------------------------------------- toast
  var toastTimer = null;
  function toast(text, kind) {
    var el = $("toast");
    el.textContent = text;
    el.className = "toast " + (kind || "");
    el.hidden = false;
    clearTimeout(toastTimer);
    toastTimer = setTimeout(function () { el.hidden = true; }, 3800);
  }

  // ---------------------------------------------------------------- render
  function setText(id, value) { $(id).textContent = value; }

  function fillList(id, items, emptyText, newestFirst) {
    var ul = $(id);
    ul.innerHTML = "";
    var list = (items || []).slice();
    if (newestFirst) { list.reverse(); }
    if (list.length === 0) {
      var li = document.createElement("li");
      li.className = "empty";
      li.textContent = emptyText;
      ul.appendChild(li);
      return;
    }
    list.forEach(function (text) {
      var item = document.createElement("li");
      item.textContent = text;
      ul.appendChild(item);
    });
  }

  /** Draws everything from one status object sent by the engine. */
  function render(status) {
    current.status = status;
    var wf = status.workflow || {};
    var item = status.workItem || null;

    setText("topbar-title", wf.title ? (wf.title + "  |  " + (wf.name || "")) : "");

    var slotText = item && item.name ? item.name.toUpperCase() : "WORKITEM";
    Array.prototype.forEach.call(document.querySelectorAll(".item-name-slot"), function (el) {
      el.textContent = slotText;
    });

    // Work item details
    var dl = $("item-details");
    dl.innerHTML = "";
    var rows = item ? [
      ["Name", item.name], ["About", item.description], ["Client", item.client],
      ["Worker", item.worker || "(not assigned)"], ["Strategy", item.strategy],
      ["State", item.state]
    ] : [["", "No work item yet"]];
    rows.forEach(function (r) {
      var dt = document.createElement("dt"); dt.textContent = r[0];
      var dd = document.createElement("dd"); dd.textContent = r[1] || "-";
      dl.appendChild(dt); dl.appendChild(dd);
    });

    // State pattern: progress bar
    var state = item ? item.state : "";
    var idx = STATE_ORDER.indexOf(state);
    var halted = state === "Blocked" || state === "Rejected";
    $("progress").classList.toggle("halted", halted);
    Array.prototype.forEach.call(document.querySelectorAll("#progress li"), function (li, i) {
      li.classList.toggle("current", i === idx);
      li.classList.toggle("done", idx >= 0 && i < idx);
    });
    Array.prototype.forEach.call(document.querySelectorAll(".side-state"), function (el) {
      el.classList.toggle("current", el.getAttribute("data-state") === state);
    });

    // Requests (max 5)
    var max = status.maxRequests || MAX_REQUESTS_FALLBACK;
    var reqs = item ? (item.requests || []) : [];
    setText("req-count", reqs.length);
    setText("req-max", max);
    $("req-count").parentNode.classList.toggle("full", reqs.length >= max);
    $("btn-add-request").disabled = !item || reqs.length >= max;
    fillList("request-list", reqs, "No requests yet. Browse the outputs and add up to " + max + ".");

    // Decorator extras offered by this industry
    var extra = $("in-extra");
    var features = status.features || [];
    if (extra.getAttribute("data-for") !== features.join("|")) {
      extra.innerHTML = '<option value="">none</option>';
      features.forEach(function (f) {
        var o = document.createElement("option"); o.value = f; o.textContent = f; extra.appendChild(o);
      });
      extra.setAttribute("data-for", features.join("|"));
    }

    // Existing workflow instances
    var sessions = status.sessions || [];
    var ex = $("in-existing");
    ex.innerHTML = "";
    sessions.forEach(function (name, i) {
      var o = document.createElement("option"); o.value = i; o.textContent = name; ex.appendChild(o);
    });
    if (status.current >= 0) { ex.value = status.current; }
    $("existing-field").hidden = sessions.length === 0;
    $("btn-existing").disabled = sessions.length === 0;

    fillList("notification-list", status.notifications, "No notifications yet.", true);
    fillList("history-list", status.history, "Nothing has happened yet.", true);
    fillList("output-list", status.outputs, "No outputs yet. Complete the work item to create them.");
  }

  // ---------------------------------------------------------------- report
  function loadReport() {
    setText("report-text", "Loading...");
    api("GET", "/api/report").then(function (data) {
      setText("report-text", data.ok ? data.text : (data.message || "No report available."));
    });
  }

  // ---------------------------------------------------------------- iterator
  function showBrowse(data) {
    if (data.ok) { setText("browse-current", data.output); }
    else { setText("browse-current", "-"); toast(data.message, "bad"); }
  }

  // ---------------------------------------------------------------- start-up
  function loadWorkflows() {
    api("GET", "/api/workflows").then(function (data) {
      if (!data.ok) { return; }
      current.workflows = data.workflows || [];
      var sel = $("in-workflow");
      sel.innerHTML = "";
      current.workflows.forEach(function (w) {
        var opt = document.createElement("option");
        opt.value = w.id;
        opt.textContent = w.title;
        sel.appendChild(opt);
      });
      applyDefaults();
    });
  }

  /** Pre-fills the forms with the realistic test data the engine supplies for the chosen industry. */
  function applyDefaults() {
    var id = $("in-workflow").value;
    var w = current.workflows.filter(function (x) { return x.id === id; })[0];
    if (!w || !w.defaults) { return; }
    $("in-workflow-name").placeholder = w.defaults.workflowName || "";
    $("in-item-name").value = w.defaults.itemName || "";
    $("in-item-desc").value = w.defaults.itemDescription || "";
    $("in-client").value = w.defaults.client || "";
    $("in-worker").value = w.defaults.worker || "";
  }

  function wire() {
    $("in-workflow").addEventListener("change", applyDefaults);
    $("btn-back").addEventListener("click", goBack);
    $("btn-report").addEventListener("click", function () { show("report"); });

    $("btn-existing").addEventListener("click", function () {
      api("POST", "/api/select", { index: $("in-existing").value }).then(function (data) {
        if (!data.ok) { toast(data.message || "There is no running workflow yet.", "bad"); return; }
        if (data.status && data.status.workItem) { show("board"); api("GET", "/api/outputs/first").then(showBrowse); }
        else { show("questions"); }
      });
    });

    $("btn-get-started").addEventListener("click", function () {
      var name = $("in-workflow-name").value.trim() || $("in-workflow-name").placeholder;
      api("POST", "/api/workflow", { workflow: $("in-workflow").value, name: name }).then(function (data) {
        if (data.ok) { show("questions"); } else { toast(data.message, "bad"); }
      });
    });

    $("btn-questions-next").addEventListener("click", function () {
      if (!$("in-item-name").value.trim()) { toast("Please name your work item.", "bad"); return; }
      show("create");
    });

    $("btn-create").addEventListener("click", function () {
      var strategy = document.querySelector("input[name=strategy]:checked").value;
      api("POST", "/api/workitem", {
        name: $("in-item-name").value.trim(),
        description: $("in-item-desc").value.trim(),
        strategy: strategy,
        client: $("in-client").value.trim(),
        worker: $("in-worker").value.trim()
      }).then(function (data) {
        if (data.ok) {
          show("board");
          api("GET", "/api/outputs/first").then(showBrowse);
        } else { toast(data.message, "bad"); }
      });
    });

    // Command pattern: every chevron sends one action to the engine.
    Array.prototype.forEach.call(document.querySelectorAll(".chev"), function (btn) {
      btn.addEventListener("click", function () {
        var action = btn.getAttribute("data-action");
        api("POST", "/api/action", { action: action }).then(function (data) {
          btn.classList.remove("flash-ok", "flash-bad");
          void btn.offsetWidth; // restart the animation
          btn.classList.add(data.ok ? "flash-ok" : "flash-bad");
          toast(data.message || (data.ok ? "Done." : "Not allowed."), data.ok ? "ok" : "bad");
        });
      });
    });

    // Iterator pattern
    $("btn-first").addEventListener("click", function () { api("GET", "/api/outputs/first").then(showBrowse); });
    $("btn-next").addEventListener("click", function () { api("GET", "/api/outputs/next").then(showBrowse); });

    $("btn-add-request").addEventListener("click", function () {
      var output = $("browse-current").textContent;
      if (!output || output === "-") { toast("Press FIRST to browse the outputs, then pick one.", "bad"); return; }
      var extra = $("in-extra").value;
      api("POST", "/api/requests", { output: output, extra: extra }).then(function (data) {
        if (data.ok && extra) { api("GET", "/api/outputs/first").then(showBrowse); }
        toast(data.message || (data.ok ? "Request added." : "Request not added."), data.ok ? "ok" : "bad");
      });
    });
  }

  wire();
  show("welcome");
  loadWorkflows();
  api("GET", "/api/status");
})();
