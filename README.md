# Learn-Cpp Short Project 1
## Authors

- **Jake Sakamoto** — Architect & C++ lead  
- **Oleg Nikitashin** — WebAssembly & GUI integration  

The elevator project has numerous capabilities such as prioritizing user requests based on the floor they are on, easy to understand and visual GUI developed and hosted on AWS, and configurable elevator numbers and floors. The project utilizes two classes; the elevator class and building class. The elevator class allows for the capability to support multiple elevators if need be. User requests are stored in vectors, which are then sorted and utilized to calculate the optimized floor the elevator needs to head to. The building class uses the number of floors and elevators to create the building (with a default of 10 floors and 2 elevators). Configuring the number of elevators, as well as the number of floors is as easy as going to the Web Assembly Wrappers portion and changing the parameters in the global class on line 238 to the number of floors, followed by number of elevators: "static Building globalBuilding(10, 2)". To setup and run the actual project itself, you'll need to setup and be able to run AWS (instructions that we used are linked below). After hosting successfully, you will be able to open an external tab that shows you the basic interface that allows you to input the desired floor, with the elevator visually going to the respective floor. If there are more than one requests, than any idle elevator will fill in to take the request -- otherwise the nearest operating elevator will take a request.

CIS 278 Class
Due Monday by 11:59pm Points 100 Submitting a file upload Available until May 5 at 11:59pm

Project Title: Elevator Simulation in C++

Course: Intermediate C++ Programming University Inspiration: University of Waterloo Project Type: Team Project (up to 3 students)

Project Overview:

In this project, your team will design and implement a simulation of an elevator system using C++. The simulation should model the behavior of a multi-floor building with one or more elevators, handling user requests, managing elevator movement, and simulating real-time actions. The main goal is to practice object-oriented programming (OOP), system design, and interactive console input/output.

## Elevator Simulation: Step‑by‑Step Guide

This document explains how to set up, build, and run our C++ elevator simulation in the browser using WebAssembly (Wasm). Follow these steps to get a working demo on your local machine or deploy it to AWS S3 for static‑site hosting.

---

## ✨ 1. Prerequisites

Before you begin, make sure you have:

1. **Git** (to clone the repo)  
2. **Emscripten SDK** (for compiling C++ to Wasm)

   ```bash
   # install emsdk
   git clone https://github.com/emscripten-core/emsdk.git
   cd emsdk
   ./emsdk install latest
   ./emsdk activate latest
   ```

   - Instructions: https://emscripten.org/docs/getting_started/downloads.html  

3. A modern **web browser** (Chrome, Firefox, Edge, or Safari)  
4. (Optional) An **AWS S3** bucket configured for static‑site hosting  

---

## 🛠 2. Clone & Inspect the Project

1. Clone our elevator-simulation repository

```
git clone https://github.com/Oleg-Niki/Learn-Cpp.git
cd Learn-Cpp
```

2. Examine the source:
    - CIS278_Lab.cpp         → C++ simulation code
    - index.html              → simple page to load Wasm
    - README.md               → this guide

## ⚙️ 3. Build the WebAssembly Module
Activate Emscripten

source /path/to/emsdk/emsdk_env.sh
Compile in cmd (copy paste below:)

```
emcc CIS278_Lab.cpp -O3 -s WASM=1 -s "EXPORTED_FUNCTIONS=['_addRequest','_stepSimulation','_getStatus']" -s "EXPORTED_RUNTIME_METHODS=['UTF8ToString']" -o elevator_sim.js

```

This produces:

elevator_sim.wasm

elevator_sim.js

A glue loader for the browser.

## 🌐 4. Frontend Integration
Open index.html (provided) download one from this rep to create your own:

```
<!DOCTYPE html>
<html>
<head>
  <meta charset="utf-8">
  <title>Elevator Simulator</title>
</head>
<body>
  <h1>Elevator Simulation</h1>
  <div id="status">Loading…</div>
  <input id="floor" type="number" min="1" max="10" placeholder="Floor #">
  <button id="call">Call Elevator</button>

  <script src="elevator_sim.js"></script>
  <script>
    Module.onRuntimeInitialized = () => {
      const statusEl = document.getElementById('status');
      document.getElementById('call').addEventListener('click', () => {
        const floor = Number(document.getElementById('floor').value);
        Module._addRequest(floor);
      });

      // Step & render loop
      setInterval(() => {
        Module._stepSimulation();
        const ptr = Module._getStatus();
        statusEl.textContent = UTF8ToString(ptr);
      }, 500);
    };
  </script>
</body>
</html>

```

_addRequest(floor): send a new floor request

_stepSimulation(): advance one simulation step

_getStatus(): returns a string with each elevator’s floor/direction


## 🚀 5. Local Testing
Start a simple HTTP server (to avoid Wasm CORS issues):

```
# Python 3
python3 -m http.server 8000
Open your browser at http://localhost:8000.

Enter floor numbers and watch the status update every 500 ms.
```

## ☁️ 6. Deploy to AWS S3 (Optional)

Build the Wasm & HTML as above.

Upload these files to your S3 bucket’s public/ folder:

```
index.html

elevator_sim.js

elevator_sim.wasm
```

Enable static‑site hosting in the S3 bucket settings.

Set the bucket policy & CORS to allow public reads.

Browse to your S3 URL (e.g. https://my-bucket.s3.amazonaws.com/index.html).

## 🔍 7. How It Works

C++ Core

Simulates multiple elevators, each with current floor, target floor, direction, and door state.

addRequest(): assigns a new floor call to the nearest idle elevator.

stepSimulation(): moves each elevator one step toward its target, opens/closes doors on arrival.

WebAssembly Bridge

Emscripten compiles C++ into elevator_sim.wasm + JS glue.

JS calls exported functions directly in the browser.

Frontend UI

Minimal HTML/JS that:

Sends requests (_addRequest)

Ticks the simulation (_stepSimulation)

Renders status (_getStatus)

## ✅ 8. Assignment Checklist

 Install & activate Emscripten SDK

 Clone & compile the C++ code to Wasm

 Verify elevator_sim.wasm & elevator_sim.js are generated

 Run index.html locally and confirm simulation works
