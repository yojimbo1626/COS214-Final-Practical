/**
 * @file gui_main.cpp
 * @brief TIVIDY GUI: a small local web server around the C++ engine.
 *
 * Run it, then open http://localhost:8080. The web page (gui/web) holds no workflow logic:
 * every button sends a request here, and this program calls the engine through
 * WorkflowSession and answers with JSON. See gui/API.md for the endpoints.
 *
 * Usage: tividy_gui [port] [web-folder]   (defaults: 8080, gui/web)
 */
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>
#include <vector>

#include "httplib.h"

#include "IndustryCatalog.h"
#include "WorkflowSession.h"

namespace {

std::vector<std::unique_ptr<WorkflowSession> > sessions;   ///< Every workflow instance started so far.
int currentSession = -1;                                   ///< Index of the instance the page is showing.
std::mutex engineMutex;                                    ///< One request at a time touches the engine.

// ------------------------------------------------------------------ JSON helpers

std::string esc(const std::string& s) {
    std::string out = "\"";
    for (size_t i = 0; i < s.size(); ++i) {
        unsigned char c = static_cast<unsigned char>(s[i]);
        switch (c) {
            case '"':  out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\n': out += "\\n"; break;
            case '\r': out += "\\r"; break;
            case '\t': out += "\\t"; break;
            default:
                if (c < 0x20) { char buf[8]; std::snprintf(buf, sizeof buf, "\\u%04x", c); out += buf; }
                else out += static_cast<char>(c);
        }
    }
    return out + "\"";
}

std::string list(const std::vector<std::string>& items) {
    std::string out = "[";
    for (size_t i = 0; i < items.size(); ++i) { if (i) out += ","; out += esc(items[i]); }
    return out + "]";
}

WorkflowSession* current() {
    if (currentSession < 0 || currentSession >= static_cast<int>(sessions.size())) return nullptr;
    return sessions[currentSession].get();
}

std::string statusJson() {
    WorkflowSession* s = current();
    std::ostringstream j;
    j << "{";
    std::vector<std::string> names;
    for (size_t i = 0; i < sessions.size(); ++i)
        names.push_back(sessions[i]->industry().title + ": " + sessions[i]->name());
    j << "\"sessions\":" << list(names) << ",\"current\":" << currentSession << ",";
    j << "\"maxRequests\":" << WorkflowSession::MAX_REQUESTS << ",";
    if (s == nullptr) {
        j << "\"workflow\":null,\"workItem\":null,\"outputs\":[],\"notifications\":[],\"history\":[],\"features\":[]}";
        return j.str();
    }
    j << "\"workflow\":{\"id\":" << esc(s->industry().id) << ",\"title\":" << esc(s->industry().title)
      << ",\"name\":" << esc(s->name()) << "},";
    if (s->hasWorkItem()) {
        j << "\"workItem\":{\"name\":" << esc(s->itemName()) << ",\"description\":" << esc(s->itemDescription())
          << ",\"client\":" << esc(s->clientName()) << ",\"worker\":" << esc(s->workerName())
          << ",\"strategy\":" << esc(s->strategyName()) << ",\"state\":" << esc(s->state())
          << ",\"requests\":" << list(s->requests()) << "},";
    } else {
        j << "\"workItem\":null,";
    }
    j << "\"outputs\":" << list(s->outputs()) << ",\"notifications\":" << list(s->notifications())
      << ",\"history\":" << list(s->history()) << ",\"features\":" << list(s->features()) << "}";
    return j.str();
}

void reply(httplib::Response& res, bool ok, const std::string& message, const std::string& extra = "") {
    std::string body = "{\"ok\":" + std::string(ok ? "true" : "false") + ",\"message\":" + esc(message);
    if (!extra.empty()) body += "," + extra;
    body += ",\"status\":" + statusJson() + "}";
    res.set_content(body, "application/json");
}

std::string param(const httplib::Request& req, const char* key) {
    return req.has_param(key) ? req.get_param_value(key) : std::string();
}

}  // namespace

int main(int argc, char** argv) {
    int port = argc > 1 ? std::atoi(argv[1]) : 8080;
    std::string web = argc > 2 ? argv[2] : "gui/web";
    if (!std::ifstream((web + "/index.html").c_str())) {
        if (std::ifstream("../gui/web/index.html")) web = "../gui/web";
    }

    httplib::Server server;
    if (!server.set_mount_point("/", web)) {
        std::cerr << "Cannot find the web page folder '" << web << "'. Run from the repository root, "
                  << "or pass the folder: tividy_gui 8080 path/to/gui/web" << std::endl;
        return 1;
    }

    server.Get("/api/workflows", [](const httplib::Request&, httplib::Response& res) {
        std::lock_guard<std::mutex> lock(engineMutex);
        std::vector<Industry> all = industryCatalog();
        std::ostringstream w;
        w << "\"workflows\":[";
        for (size_t i = 0; i < all.size(); ++i) {
            if (i) w << ",";
            w << "{\"id\":" << esc(all[i].id) << ",\"title\":" << esc(all[i].title)
              << ",\"defaults\":{\"workflowName\":" << esc(all[i].defaultWorkflowName)
              << ",\"itemName\":" << esc(all[i].defaultItemName)
              << ",\"itemDescription\":" << esc(all[i].defaultItemDescription)
              << ",\"client\":" << esc(all[i].defaultClient)
              << ",\"worker\":" << esc(all[i].defaultWorker) << "}}";
        }
        w << "],\"active\":" << (sessions.empty() ? "false" : "true");
        reply(res, true, "", w.str());
    });

    server.Get("/api/status", [](const httplib::Request&, httplib::Response& res) {
        std::lock_guard<std::mutex> lock(engineMutex);
        reply(res, true, "");
    });

    server.Post("/api/workflow", [](const httplib::Request& req, httplib::Response& res) {
        std::lock_guard<std::mutex> lock(engineMutex);
        std::vector<Industry> all = industryCatalog();
        std::string id = param(req, "workflow");
        for (size_t i = 0; i < all.size(); ++i) {
            if (all[i].id == id) {
                std::string name = param(req, "name");
                if (name.empty()) name = all[i].defaultWorkflowName;
                sessions.push_back(std::unique_ptr<WorkflowSession>(new WorkflowSession(all[i], name)));
                currentSession = static_cast<int>(sessions.size()) - 1;
                return reply(res, true, "New " + all[i].title + " workflow '" + name + "' created.");
            }
        }
        reply(res, false, "Unknown industry '" + id + "'.");
    });

    server.Post("/api/select", [](const httplib::Request& req, httplib::Response& res) {
        std::lock_guard<std::mutex> lock(engineMutex);
        int index = std::atoi(param(req, "index").c_str());
        if (index < 0 || index >= static_cast<int>(sessions.size())) return reply(res, false, "No such workflow.");
        currentSession = index;
        reply(res, true, "Switched to " + sessions[index]->name() + ".");
    });

    server.Post("/api/workitem", [](const httplib::Request& req, httplib::Response& res) {
        std::lock_guard<std::mutex> lock(engineMutex);
        WorkflowSession* s = current();
        if (s == nullptr) return reply(res, false, "Start a workflow first.");
        ActionResult r = s->createWorkItem(param(req, "name"), param(req, "description"), param(req, "strategy"),
                                           param(req, "client"), param(req, "worker"));
        reply(res, r.ok, r.message);
    });

    server.Get("/api/outputs/first", [](const httplib::Request&, httplib::Response& res) {
        std::lock_guard<std::mutex> lock(engineMutex);
        WorkflowSession* s = current();
        if (s == nullptr) return reply(res, false, "Start a workflow first.");
        std::string out;
        ActionResult r = s->browseFirst(out);
        reply(res, r.ok, r.message, "\"output\":" + esc(out));
    });

    server.Get("/api/outputs/next", [](const httplib::Request&, httplib::Response& res) {
        std::lock_guard<std::mutex> lock(engineMutex);
        WorkflowSession* s = current();
        if (s == nullptr) return reply(res, false, "Start a workflow first.");
        std::string out;
        ActionResult r = s->browseNext(out);
        reply(res, r.ok, r.message, "\"output\":" + esc(out));
    });

    server.Post("/api/requests", [](const httplib::Request& req, httplib::Response& res) {
        std::lock_guard<std::mutex> lock(engineMutex);
        WorkflowSession* s = current();
        if (s == nullptr) return reply(res, false, "Start a workflow first.");
        ActionResult r = s->addRequest(param(req, "output"), param(req, "extra"));
        reply(res, r.ok, r.message);
    });

    server.Post("/api/action", [](const httplib::Request& req, httplib::Response& res) {
        std::lock_guard<std::mutex> lock(engineMutex);
        WorkflowSession* s = current();
        if (s == nullptr) return reply(res, false, "Start a workflow first.");
        std::string a = param(req, "action");
        ActionResult r(false, "Unknown action '" + a + "'.");
        if (a == "start-workflow") r = s->startWorkflow();
        else if (a == "assign") r = s->assign();
        else if (a == "complete") r = s->complete();
        else if (a == "cancel") r = s->cancel();
        else if (a == "block") r = s->toggleBlock();
        else if (a == "undo") r = s->undo();
        reply(res, r.ok, r.message);
    });

    server.Get("/api/report", [](const httplib::Request&, httplib::Response& res) {
        std::lock_guard<std::mutex> lock(engineMutex);
        WorkflowSession* s = current();
        if (s == nullptr) return reply(res, false, "No workflow has been started yet.");
        reply(res, true, "", "\"text\":" + esc(s->report()));
    });

    std::cout << "TIVIDY is running. Open http://localhost:" << port << " in your browser.\n"
              << "(Serving the page from '" << web << "'. Press Ctrl+C to stop.)" << std::endl;
    if (!server.listen("0.0.0.0", port)) {
        std::cerr << "Could not listen on port " << port << ". Is another program using it?" << std::endl;
        return 1;
    }
    return 0;
}
