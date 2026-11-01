## 📖 Description

**TIVIDY** is a generic workflow management system written in **C++11** for COS 214. One reusable workflow engine runs two workflows from two different industries:

- 🍩 **Donut shop**: a client orders donuts, a baker takes the order, prepares it and hands over the finished products.
- 💇 **Hair salon**: a client books salon services, a stylist takes the booking, performs the services and completes the appointment.

Both industries use the **same engine classes without modification**. Only the industry data changes: the output types each industry offers, its participants and its test data.

The engine is built from **11 Gang of Four design patterns** (see [Design patterns](#-design-patterns)), and you drive it through an interactive local web GUI that calls the C++ engine directly.

## System report

- 📄 Final report (PDF): [`report/`](report/) **============================== Link?**
- 📝 Working document: https://docs.google.com/document/d/1N_PeSxPsGrc7koyDV58-MfS9dqtkm5GRdOBVOaCl76U/edit?tab=t.0
- 🧩 UML exports and Visual Paradigm project: [`docs/`](docs/) **============================== Link?**

## 🔁 How a workflow runs

Each workflow instance has its own work item, participants, requests, state and history, so two instances never affect each other.

```
             start                assign             complete
 Available ────────▶ InProgress ────────▶ Assigned ────────▶ Completed
     │  ▲                │                    │
block│  │unblock         │cancel              │cancel
     ▼  │                ▼                    ▼
   Blocked ──cancel──▶ Rejected ◀────────────┘
```

1. **Start a workflow:** create a new one (choose an industry and give it a name), or continue an existing one.
2. **Create the work item:** say what it is, choose an execution strategy (role, rule or workload) and enter the client and worker.
3. **Add requests:** browse the industry's available outputs and add up to **5** requests. You can add an extra feature to any of them, such as *sprinkles* or *deep-conditioning treatment*.
4. **Move the work item through its states** with the action buttons. Invalid actions, such as completing an item that was never assigned, are refused with a message, and the state does not change.
5. **Watch the results:** the progress bar shows the current state, participants receive notifications, the history records every change, and **Undo** restores the previous snapshot.
6. **Complete the work item:** the requested outputs are produced, and the report summarises the instance.

**Exception paths:** *cancel* (Rejected, allowed from Available, InProgress, Assigned and Blocked), *block / unblock* (for example, out of stock or stylist unavailable), invalid actions refused, and *undo*.

## 🚀 Getting Started

### Prerequisites

Before you begin, make sure you have the following installed:
- **g++** (any C++11-compatible compiler, e.g. g++ 4.8.1 or newer)
- **cmake** 3.14 or newer
- **git**
- An internet connection the **first** time you configure (CMake downloads doctest v2.4.11)
- A web browser (for the GUI)
- **doxygen** (optional, to generate the code documentation)
- **docker** (optional)

#### Install on Ubuntu/Debian:
```bash
sudo apt-get install g++ cmake git build-essential doxygen
```

### Installation

1. **Clone the repository:**
   ```bash
   git clone https://github.com/yojimbo1626/COS214-Final-Practical.git
   cd COS214-Final-Practical
   ```

2. **Configure and build:**
   ```bash
   cmake -S . -B build
   cmake --build build
   ```

### Running the Application

- **GUI (local web interface):**
  ```bash
  ./build/tividy_gui
  ```
  Then open **http://localhost:8080** in your browser. The page holds no workflow logic. Every button calls the C++ engine running in `tividy_gui`.

  On the welcome page you can:
  - **Create a new workflow:** choose an industry (**Donut shop** or **Hair salon**), give your workflow a name and press **GET STARTED**. Each new workflow is an independent instance with its own work item, participants, requests and history. Realistic test data for the industry is filled in for you, and you can change it.
  - **Use an existing workflow:** press **USE EXISTING WORKFLOW** to go back to the workflow that is already running.

### Running the Tests

Unit tests use **doctest**:
```bash
ctest --test-dir build --output-on-failure
```
Or run the test program directly (shows every test case):
```bash
./build/unit_tests
```

The same tests run automatically on every push to `branches` through **GitHub Actions** (see the *Actions* tab).

### Additional Commands

**Documentation:**
- `doxygen Doxyfile`: generates the code documentation in `docs/doxygen/`

**Docker:**
- `docker build -t tividy .`: build the image
- `docker run --rm -p 8080:8080 tividy`: run TIVIDY in a container, then open http://localhost:8080

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

**Generic engine classes reused by any industry:** all of the classes above. The industries differ only in the outputs registered in `OutputRegistry` and in their test data.

## 📁 Repository structure

```
include/      C++ headers
src/          C++ implementation files
tests/        doctest unit tests (*Tests.cpp) and shared test helpers
gui/web/      GUI page (index.html, style.css, app.js) **============================== change?**
docs/         UML exports, Visual Paradigm project, Doxygen output **============================== change?**
report/       Final PDF report **============================== change?**
.github/      GitHub Actions CI workflow
```

## 🌿 Branching and review process

- `main` is locked. All work is merged into `dev` through pull requests.
- Each group branches from `dev` into `groupX/<pattern_name>`, with sub-branches `feature/<name>`.
- Every pull request needs **2 approvals**.
- Unit tests are written **before** merging into `dev`. CI builds and runs them on `dev`.
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

- [doctest](https://github.com/doctest/doctest) v2.4.11 (MIT): unit-test framework, downloaded by CMake
- [cpp-httplib](https://github.com/yhirose/cpp-httplib) (MIT): single-header HTTP server used by the GUI
**============================== link??**
---

<div align="center">
  <strong>🍩 💇 Thanks for checking out TIVIDY! 💇 🍩</strong>
</div>
