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

  var current = { view: "welcome", status: null, workflows: [], lastView: "welcome", existingMode: false };

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
    if (view !== "report") { current.lastView = view; }
    current.view = view;
    if (view === "report") { loadReport(); }
  }

  var BACK = { questions: "welcome", create: "questions", board: "welcome" };

  function goBack() {
    if (current.view === "report") { show(current.lastView); return; }
    show(BACK[current.view] || "welcome");
  }

  function goDashboard() {
    var st = current.status;
    if (!st || !st.workflow) { toast("Start a workflow first.", "bad"); return; }
    if (!st.workItem) { toast("Create a work item first.", "bad"); return; }
    show("board");
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

  var STATE_LABEL = { Available: "Available", InProgress: "In-progress", Assigned: "Assigned",
                      Completed: "Completed", Blocked: "Blocked", Rejected: "Cancelled" };

  /** Draws everything from one status object sent by the engine. */
  function render(status) {
    current.status = status;
    var wf = status.workflow || {};
    var item = status.workItem || null;

    setText("topbar-title", wf.title ? (wf.title === wf.name ? wf.name : wf.title + "  |  " + (wf.name || "")) : "");

    var slotText = item && item.name ? item.name.toUpperCase() : "WORKITEM";
    Array.prototype.forEach.call(document.querySelectorAll(".item-name-slot"), function (el) {
      el.textContent = slotText;
    });

    // Work items of this workflow
    var tabs = $("item-tabs");
    tabs.innerHTML = "";
    (status.items || []).forEach(function (it, i) {
      var b = document.createElement("button");
      b.className = "item-tab" + (i === status.currentItem ? " active" : "");
      b.innerHTML = "<strong></strong><small></small>";
      b.firstChild.textContent = it.name;
      b.lastChild.textContent = STATE_LABEL[it.state] || it.state;
      b.addEventListener("click", function () {
        api("POST", "/api/item/select", { index: i }).then(function () { clearSelectedAction(); browseFirst(); });
      });
      tabs.appendChild(b);
    });
    var count = (status.items || []).length;
    setText("item-count", count);
    setText("item-max", status.maxItems || 10);
    $("btn-add-item").disabled = count >= (status.maxItems || 10);

    // Industry rule
    $("rule-text").hidden = !wf.rule;
    if (wf.rule) { setText("rule-text", "Rule: " + wf.rule); }

    // Work item details
    var dl = $("item-details");
    dl.innerHTML = "";
    var rows = item ? [
      ["Name", item.name], ["About", item.description], ["Client", item.client],
      ["Worker", item.worker || "(not assigned)"], ["Strategy", item.strategy],
      ["State", STATE_LABEL[item.state] || item.state]
    ] : [["", "No work item yet"]];
    rows.forEach(function (r) {
      var dt = document.createElement("dt"); dt.textContent = r[0];
      var dd = document.createElement("dd"); dd.textContent = r[1] || "-";
      dl.appendChild(dt); dl.appendChild(dd);
    });

    // State pattern: progress bar. A Blocked item shows where it was blocked from.
    var state = item ? item.state : "";
    var shown = state === "Blocked" ? item.blockedFrom : state;
    var idx = STATE_ORDER.indexOf(shown);
    $("progress").classList.toggle("halted", state === "Rejected");
    Array.prototype.forEach.call(document.querySelectorAll("#progress li"), function (li, i) {
      li.classList.toggle("current", i === idx && state !== "Blocked");
      li.classList.toggle("paused", i === idx && state === "Blocked");
      li.classList.toggle("done", idx >= 0 && i < idx);
    });
    Array.prototype.forEach.call(document.querySelectorAll(".block-arrow"), function (el) {
      el.classList.toggle("active", state === "Blocked" && el.getAttribute("data-from") === item.blockedFrom);
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

    fillExisting(status.sessions || []);

    fillList("notification-list", status.notifications, "No notifications yet.", true);
    fillList("history-list", status.history, "Nothing has happened yet.", true);
    fillList("output-list", status.outputs, "No outputs yet. Complete the work item to create them.");
  }

  /** The "use existing workflow" choices: the two demo workflows, then any workflow already running. */
  function fillExisting(sessions) {
    var sel = $("in-existing");
    var keep = sel.value;
    sel.innerHTML = "";
    var demo = document.createElement("optgroup");
    demo.label = "Demo workflows";
    current.workflows.forEach(function (w) {
      var o = document.createElement("option"); o.value = "preset:" + w.id; o.textContent = w.title; demo.appendChild(o);
    });
    sel.appendChild(demo);
    if (sessions.length) {
      var running = document.createElement("optgroup");
      running.label = "Continue a running workflow";
      sessions.forEach(function (name, i) {
        var o = document.createElement("option"); o.value = "session:" + i; o.textContent = name; running.appendChild(o);
      });
      sel.appendChild(running);
    }
    if (keep) { sel.value = keep; }
    if (!sel.value && sel.options.length) { sel.selectedIndex = 0; }
    showExistingRule();
  }

  function showExistingRule() {
    var v = $("in-existing").value || "";
    var w = v.indexOf("preset:") === 0 ? presetById(v.substring(7)) : null;
    setText("existing-rule", w && w.rule ? "Rule: " + w.rule : "");
  }

  function presetById(id) {
    return current.workflows.filter(function (x) { return x.id === id; })[0];
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
  function browseFirst() { return api("GET", "/api/outputs/first").then(showBrowse); }

  // ---------------------------------------------------------------- forms
  /** Pre-fills the work item forms with a demo workflow's test data (blank for a custom workflow). */
  function applyDefaults(w) {
    var d = (w && w.defaults) || {};
    $("in-item-name").value = d.itemName || "";
    $("in-item-desc").value = d.itemDescription || "";
    $("in-client").value = d.client || "";
    $("in-worker").value = d.worker || "";
  }

  function setExistingMode(on) {
    current.existingMode = on;
    $("custom-fields").hidden = on;
    $("existing-fields").hidden = !on;
    $("btn-toggle-existing").innerHTML = on ? "&lt; CREATE A NEW WORKFLOW" : "USE EXISTING WORKFLOW &gt;";
    $("btn-get-started").textContent = on ? "USE THIS WORKFLOW" : "GET STARTED";
  }

  function clearSelectedAction() {
    Array.prototype.forEach.call(document.querySelectorAll(".chev-wrap"), function (w) { w.classList.remove("selected"); });
  }

  function loadWorkflows() {
    api("GET", "/api/workflows").then(function (data) {
      if (!data.ok) { return; }
      current.workflows = data.workflows || [];
      fillExisting((current.status && current.status.sessions) || []);
    });
  }

  function wire() {
    $("btn-back").addEventListener("click", goBack);
    $("btn-report").addEventListener("click", function () { show("report"); });
    $("btn-dashboard").addEventListener("click", goDashboard);
    $("btn-toggle-existing").addEventListener("click", function () { setExistingMode(!current.existingMode); });
    $("in-existing").addEventListener("change", showExistingRule);

    $("btn-get-started").addEventListener("click", function () {
      if (current.existingMode) {
        var v = $("in-existing").value || "";
        if (v.indexOf("session:") === 0) {
          api("POST", "/api/select", { index: v.substring(8) }).then(function (data) {
            if (!data.ok) { toast(data.message, "bad"); return; }
            clearSelectedAction();
            if (data.status && data.status.workItem) { show("board"); browseFirst(); } else { show("questions"); }
          });
          return;
        }
        var w = presetById(v.substring(7));
        if (!w) { toast("Choose a workflow.", "bad"); return; }
        api("POST", "/api/workflow", { workflow: w.id, name: w.defaults.workflowName }).then(function (data) {
          if (data.ok) { applyDefaults(w); clearSelectedAction(); show("questions"); } else { toast(data.message, "bad"); }
        });
        return;
      }
      var name = $("in-workflow-name").value.trim();
      if (!name) { toast("Type a name for your workflow, or use an existing one.", "bad"); return; }
      if (!$("in-products").value.trim()) { toast("List at least one thing this workflow produces.", "bad"); return; }
      api("POST", "/api/workflow", {
        workflow: "custom", name: name, products: $("in-products").value, features: $("in-features").value
      }).then(function (data) {
        if (data.ok) { applyDefaults(null); clearSelectedAction(); show("questions"); } else { toast(data.message, "bad"); }
      });
    });

    $("btn-questions-next").addEventListener("click", function () {
      if (!$("in-item-name").value.trim()) { toast("Please name your work item.", "bad"); return; }
      setText("create-item-name", $("in-item-name").value.trim().toUpperCase());
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
          clearSelectedAction();
          show("board");
          browseFirst();
          toast(data.message, "ok");
        } else { toast(data.message, "bad"); }
      });
    });

    $("btn-add-item").addEventListener("click", function () {
      $("in-item-name").value = "";
      $("in-item-desc").value = "";
      show("questions");
      $("in-item-name").focus();
    });

    // Command pattern: every chevron sends one action to the engine. The last one clicked is outlined in white.
    Array.prototype.forEach.call(document.querySelectorAll(".chev"), function (btn) {
      btn.addEventListener("click", function () {
        clearSelectedAction();
        btn.parentNode.classList.add("selected");
        var action = btn.getAttribute("data-action");
        api("POST", "/api/action", { action: action }).then(function (data) {
          btn.classList.remove("flash-bad");
          void btn.offsetWidth; // restart the animation
          if (!data.ok) { btn.classList.add("flash-bad"); }
          toast(data.message || (data.ok ? "Done." : "Not allowed."), data.ok ? "ok" : "bad");
        });
      });
    });

    // Iterator pattern
    $("btn-first").addEventListener("click", browseFirst);
    $("btn-next").addEventListener("click", function () { api("GET", "/api/outputs/next").then(showBrowse); });

    $("btn-add-request").addEventListener("click", function () {
      var output = $("browse-current").textContent;
      if (!output || output === "-") { toast("Press FIRST to browse the outputs, then pick one.", "bad"); return; }
      var extra = $("in-extra").value;
      api("POST", "/api/requests", { output: output, extra: extra }).then(function (data) {
        if (data.ok && extra) { browseFirst(); }
        toast(data.message || (data.ok ? "Request added." : "Request not added."), data.ok ? "ok" : "bad");
      });
    });
  }

  wire();
  setExistingMode(false);
  show("welcome");
  loadWorkflows();
  api("GET", "/api/status");
})();
