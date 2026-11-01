## 📖 Description

**TIVIDY** is a generic workflow management system written in **C++11** for COS 214. One reusable workflow engine runs two workflows from two different industries:

- 🍩 **Donut shop**: a client orders donuts, a baker takes the order, prepares it and hands over the finished products.
- 💇 **Hair salon**: a client books salon services, a stylist takes the booking, performs the services and completes the appointment.

Both industries use the **same engine classes without modification**. Only the industry data changes: the output types each industry offers, its extras, its one business rule and its test data. You can also create your **own custom workflow** by typing a name and the outputs it produces.

The engine is built from **11 Gang of Four design patterns** (see [Design patterns](#-design-patterns)), and you drive it through an interactive local web GUI that calls the C++ engine directly.

## System report

- 📄 Final report (PDF): [[`report/`](report/)](https://docs.google.com/document/d/1N_PeSxPsGrc7koyDV58-MfS9dqtkm5GRdOBVOaCl76U/edit?tab=t.0)
- 📝 Working document: https://docs.google.com/document/d/1N_PeSxPsGrc7koyDV58-MfS9dqtkm5GRdOBVOaCl76U/edit?tab=t.0
- 🧩 UML class diagram: [`UML/TIVIDY Class Diagram.jpg`](UML/TIVIDY%20Class%20Diagram.jpg)
- 📚 Code documentation (Doxygen): [`docs/html/index.html`](docs/html/index.html)

## 🔁 How a workflow runs

Each workflow instance has its own work items, participants, requests and history, so two instances never affect each other. A workflow can hold up to **10 work items**, and each work item has its own client, worker, strategy and state.

```
             start                assign             complete
 Available ────────▶ InProgress ────────▶ Assigned ────────▶ Completed
     │  ▲                │                    │
block│  │unblock         │cancel              │cancel
     ▼  │                ▼                    ▼
   Blocked ──cancel──▶ Rejected ◀────────────┘
```

1. **Choose a workflow:** type a name for a new custom workflow (and what it produces), or use one of the existing demo workflows: **Donut shop** or **Hair salon**.
2. **Create a work item:** the worker says what the work item is and which execution strategy is used (role, rule or workload), then enters the client and worker names.
3. **Add more work items** at any time with **+ ADD WORK ITEM** on the dashboard, and switch between them with their tabs.
4. **Add requests:** browse the available outputs (**FIRST / NEXT**) and add up to **5** requests per work item. You can add an extra feature to any of them, such as *Sprinkles* or *Deep-conditioning treatment*.
5. **Move the work item through its states** with the action buttons. The last button you pressed is outlined in white. Invalid actions, such as completing an item that was never assigned, are refused with a message, and the state does not change.
6. **Watch the results:** the progress bar shows the current state (with a **BLOCKED** arrow under Available while the item is blocked), the notifications and history panels record every change, and **Undo** restores the previous snapshot.
7. **Complete the work item:** the requested outputs are produced, and the **REPORT** page summarises every work item in the workflow.

**Exception paths:** *cancel* (Rejected, allowed from Available, InProgress, Assigned and Blocked), *block / unblock* (Available ↔ Blocked, for example waiting for stock or a free chair), invalid actions refused, industry rules refused, and *undo*.

**Industry rules** (checked before a request is added):
- 🍩 Donut shop: *Extra icing* cannot go on a *Glazed donut*, because it is already iced.
- 💇 Hair salon: only one *Hair colour* per appointment, because the colour needs the full processing time.

## 🚀 Getting Started

### Prerequisites

Before you begin, make sure you have the following installed:
- **g++** (any C++11-compatible compiler, e.g. g++ 4.8.1 or newer)
- **cmake** 3.14 or newer
- **git**
- An internet connection the **first** time you configure (CMake downloads doctest and cpp-httplib)
- A web browser (for the GUI)
- **doxygen** (optional, to generate the code documentation)
- **docker** (optional)

#### Install on Ubuntu/Debian:
```bash
sudo apt-get install g++ cmake git build-essential doxygen
```

#### Install on macOS (Homebrew):
```bash
brew install cmake doxygen
```

### Installation

1. **Clone the repository:**
   ```bash
   git clone https://github.com/yojimbo1626/COS214-Final-Practical.git
   cd COS214-Final-Practical
   ```

2. **Configure and build:**
   ```bash
   cmake -S . -B build -DCMAKE_POLICY_VERSION_MINIMUM=3.5
   cmake --build build
   ```
   (`-DCMAKE_POLICY_VERSION_MINIMUM=3.5` is only needed with CMake 4, and is harmless otherwise.)

### Running the Application

- **GUI (local web interface):** run this from the repository root, so it can find `gui/web`:
  ```bash
  ./build/tividy_gui
  ```
  Then open **http://localhost:8080** in your browser. Press **Ctrl+C** in the terminal to stop it. The page holds no workflow logic: every button calls the C++ engine running in `tividy_gui`.

  On the welcome page you can:
  - **Create a custom workflow:** type a name in **CHOOSE A WORKFLOW**, list what it produces (comma separated) and, optionally, the extras a client can add. Then press **GET STARTED**.
  - **Use an existing workflow:** press **USE EXISTING WORKFLOW**, choose **Donut shop** or **Hair salon** (or a workflow that is already running) and press **USE THIS WORKFLOW**. Realistic test data for the industry is filled in for you, and you can change it.

  The top bar has **REPORT** (the workflow report) and **DASHBOARD** (the work items moving through the workflow).

### Running the Tests

Unit tests use **doctest**:
```bash
ctest --test-dir build --output-on-failure
```
Or run the test program directly (shows every test case):
```bash
./build/unit_tests
```

The same tests run automatically through **GitHub Actions** on every push, and on pull requests into `dev` and `main` (see the *Actions* tab).

### Additional Commands

**Documentation:**
- `doxygen Doxyfile`: generates the code documentation in `docs/html/` (open `docs/html/index.html`)

**Docker:**
- `docker build -f dockerfile -t tividy .`: build the image (this also runs all the tests)
- `docker run --rm -it --init -p 8080:8080 tividy`: run TIVIDY in a container, then open http://localhost:8080 (Ctrl+C stops it)

**Cleanup:**
- `rm -rf build`: remove all build files

## 🧩 Design patterns

| Pattern | Category | Main classes | What it does in TIVIDY |
|---|---|---|---|
| Factory Method | Creational | `Producer`, `OutputProducer` | Creates the `WorkOutput` a client asked for |
| Prototype | Creational | `OutputComponent::clone()`, `OutputRegistry` | Stores one prototype per output type and clones it for each request |
| Decorator | Structural | `OutputFeature`, `WorkOutputFeature` | Adds extra features to an output (e.g. sprinkles) |
| Proxy | Structural | `ClientProcessor` | Protection proxy: the client can only add requests, see outputs and cancel |
| Facade | Structural | `SystemProcessor` | One simple entry point to the whole workflow subsystem |
| State | Behavioural | `WorkItemState`, `Available`, `InProgress`, `Assigned`, `Completed`, `Blocked`, `Rejected` | The work item's lifecycle and the transitions allowed from each state |
| Strategy | Behavioural | `TaskExecutionStrategy`, `RoleExecution`, `RuleExecution`, `WorkloadExecution` | How a worker carries out a task |
| Observer | Behavioural | `WorkUnit`, `Participant`, `Client`, `Worker`, `HistoryLogger` | Pushes every state change to the client, the worker and the history log |
| Command | Behavioural | `WorkflowController`, `Command`, `AddRequest`, `StartWorkflow`, `AssignWorkItem`, `CompleteWorkItem`, `CancelWorkItem` | Each workflow action is an object the controller executes |
| Memento | Behavioural | `WorkItemSnapshot`, `WorkItemHistory` | Saves a snapshot before each action so it can be undone |
| Iterator | Behavioural | `Iterator<Item>`, `Registry<Item>`, `OutputRegistryIterator` | Browses the available outputs (first/next, wrapping around) |

**Generic engine classes reused by any industry:** all of the classes above. The industries differ only in the outputs registered in `OutputRegistry`, their extras, their rule and their test data.

**GUI application classes (`app/`):** `WorkflowSession` (one workflow instance and its work items), `IndustryCatalog` (industry data and rules), `SessionObserver` (an extra Observer that feeds the GUI's notifications panel) and `gui_main.cpp` (the local web server).

## 📁 Repository structure

```
include/      C++ headers of the engine
src/          C++ implementation files of the engine
tests/        doctest unit and integration tests (*Tests.cpp) and shared test helpers
app/          GUI application layer: WorkflowSession, IndustryCatalog, SessionObserver, web server (gui_main.cpp)
gui/web/      GUI page (index.html, style.css, app.js)
gui/API.md    The page-to-engine API (every endpoint and the engine call behind it)
UML/          UML class diagram
docs/         Doxygen code documentation (docs/html)
report/       Final PDF report
.github/      GitHub Actions CI workflow
```

## 🌿 Branching and review process

- `main` is locked. All work is merged into `dev` through pull requests.
- Each group branches from `dev` into `groupX/<pattern_name>`, with sub-branches `feature/<name>`.
- Every pull request needs **2 approvals**.
- Unit tests are written **before** merging into `dev`. CI builds and runs them.
- Code style: C++11, headers in `include/`, implementation in `src/`, Doxygen comments on every class and method, and no `using namespace std;` (always `std::`).

## The Team

| Name | Student number |
|------|------|
| Junayd Rafudeen | u25011503 |
| Lillian Muller | u25253990 |
| Bianca Kritzman | u25144465 |
| Dian le Roux | u25147065 |
| Conrad Botha | u25017749 |
| Simnikiwe Dlamini | u25141300 |
| Marko de Swardt | u24658562 |

## 📚 Third-party dependencies

Both are downloaded automatically by CMake (`FetchContent`) the first time you configure:

- [doctest](https://github.com/doctest/doctest) v2.4.11 (MIT): unit-test framework
- [cpp-httplib](https://github.com/yhirose/cpp-httplib) v0.14.3 (MIT): single-header HTTP server used by the GUI

---

<div align="center">
  <strong>🍩 💇 Thanks for checking out TIVIDY! 💇 🍩</strong>
</div>
