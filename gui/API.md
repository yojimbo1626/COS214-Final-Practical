# TIVIDY GUI: page ↔ C++ engine API

The web page (`gui/web/index.html`, `style.css`, `app.js`) has **no workflow logic**. The C++ program
`tividy_gui` (`app/gui_main.cpp`) runs the engine and a small local web server (cpp-httplib). It serves
the page at `http://localhost:8080` and answers the calls below by calling the engine through
`WorkflowSession` (`app/WorkflowSession.cpp`). This meets the spec rule that "a GUI must still call the
C++ engine".

POST bodies are form-encoded (`a=1&b=2`). Every reply is JSON:
`{"ok": true|false, "message": "...", "status": {...}}`. For an invalid action, `ok` is false and
`message` says why, e.g. *"Not allowed: cannot assign a worker while 'Party order' is Available. Start the
workflow first."*

`status` is the whole picture of the current workflow instance, and the page redraws from it:

```json
{
  "sessions": ["Donut shop: Saturday orders", "Hair salon: Wedding bookings"],
  "current": 0,
  "maxRequests": 5,
  "maxItems": 10,
  "workflow": {"id": "donut", "title": "Donut shop", "name": "Saturday orders",
               "rule": "Extra icing cannot go on a Glazed donut: it is already iced."},
  "items": [{"name": "Party order", "state": "Assigned"}, {"name": "Office treats", "state": "Available"}],
  "currentItem": 0,
  "workItem": {"name": "Party order", "description": "...", "client": "Thandi Mokoena",
               "worker": "Sipho (baker)", "strategy": "Role",
               "state": "Available|InProgress|Assigned|Completed|Blocked|Rejected",
               "blockedFrom": "Available (only while Blocked)",
               "requests": ["Output #1: Glazed donut"]},
  "outputs": ["Output #1: Glazed donut"],
  "notifications": ["[Client #3 (Thandi Mokoena)] received update: WorkItem 'Party order' is now Assigned"],
  "history": ["WorkItem 'Party order' is now InProgress", "..."],
  "features": ["Sprinkles", "Extra icing", "Gift box"]
}
```

| Method + path | Params | Engine call (pattern) |
|---|---|---|
| GET `/api/workflows` | – | The two demo industries, their rule and their test data |
| GET `/api/status` | – | Builds `status` |
| POST `/api/workflow` | `workflow` (donut/salon/custom), `name`; for custom also `products`, `features` (comma separated) | New independent `WorkflowSession` |
| POST `/api/select` | `index` | Switches to an existing workflow instance |
| POST `/api/item/select` | `index` | Switches to another work item of the current workflow |
| POST `/api/workitem` | `name, description, strategy (role/rule/workload), client, worker` | Creates the Worker (with its Strategy), the Client and the WorkItem. The worker fills the registry with the industry's products (Factory Method → Prototype). The client, a HistoryLogger and a SessionObserver subscribe (Observer). Adds a work item (at most 10 per workflow) |
| GET `/api/outputs/first` | – | `iterator->first()` (Iterator) → `{"output"}` |
| GET `/api/outputs/next` | – | `iterator->next()`, wraps round (Iterator) |
| POST `/api/requests` | `output`, `extra` (optional) | `Client::addRequest` / `addSpecialRequest` through the ClientProcessor (Proxy → Facade → Command; Decorator for extras). At most 5 |
| POST `/api/action` | `action` = `start-workflow`, `assign`, `complete`, `block`, `cancel`, `undo` | SystemProcessor (Facade) → WorkflowController (Command) → WorkItem state (State). `block` calls `WorkItem::blocked()` (State). `cancel` goes through the client's proxy once Assigned, through `SystemProcessor::cancelWork()` once started, and straight to `WorkItem::cancel()` before the start. `complete` runs the worker's Strategy, then the outputs are cloned and delivered. `undo` restores the last WorkItemSnapshot (Memento) |

Before a request is added, the industry rule is checked (donut shop: no Extra icing on a Glazed donut;
hair salon: at most one Hair colour per appointment). A broken rule is refused with `ok: false`.
| GET `/api/report` | – | `{"text"}`: the report for the current workflow, every work item |

## State transitions (from the engine's state classes)

Available --start--> InProgress --assign--> Assigned --complete--> Completed
Available --block--> Blocked --block--> Available
Available / InProgress / Assigned / Blocked --cancel--> Rejected (shown as CANCELLED)

Anything else is refused with `ok: false` and a reason. The buttons stay clickable, so a marker can see
the error handling.
