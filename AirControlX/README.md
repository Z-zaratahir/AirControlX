# ✈️ AirControlX – Automated Air Traffic Control System

> 🎓 **Operating Systems Project – Spring 2025**  
> 🔧 Developed by: Zara Tahir (i230802) & Taha Ahmed (i230532)  
> 🏫 FAST-NUCES Islamabad Campus

---

![Language](https://img.shields.io/badge/Language-C++-blue.svg)
![Status](https://img.shields.io/badge/Project-Phase%202%20Complete-green.svg)
![License](https://img.shields.io/badge/Use-Academic-orange.svg)
![OS Concepts](https://img.shields.io/badge/OS%20Concepts-Threads%2C%20IPC%2C%20Sync-critical)

---

## 📌 Project Summary

**AirControlX** is a simulation of an **Automated Air Traffic Control System (ATCS)** designed for an international airport with multiple runways. Built using **C/C++**, this project integrates core **Operating Systems concepts**, including:

- 🔄 Multithreading
- 🔐 Synchronization (mutexes, semaphores)
- ⚙️ Process Management
- 📊 Real-time Simulation
- 🎨 Graphics using SFML

---

## 🧠 Operating Systems Concepts Used

| Concept            | Application in Project                             |
| ------------------ | -------------------------------------------------- |
| Threads            | Managing flight flows, radar, scheduling           |
| Mutexes/Semaphores | Runway access control, data integrity              |
| Scheduling         | Flight queues based on priority & time             |
| User Input         | StripePay, AVN Generator, Airline Portal processes |
| Process Handling   | Separate processes for modules like ATC Controller |

---

<details>
<summary>📦 <strong>Modules Overview</strong></summary>

### 🧩 Module 1 – Rules & Restrictions

- Define airlines, aircraft types, and runways
- Assign runways based on direction & flight type
- Enforce speed rules during flight phases
- Detect and log Airspace Violations (AVNs)
- Ground fault handling during taxiing

---

### 🧩 Module 2 – Core ATCS Functionality

- Flight entry, scheduling & queueing
- Runway allocation via synchronization
- Manage aircraft status across phases
- Real-time violation tracking & delay estimation

---

### 🧩 Module 3 – Subsystems & Integration

- **ATC Controller**: Monitors flight violations
- **AVN Generator**: Issues airspace violation notices
- **Airline Portal**: View/pay violation fines
- **StripePay**: Simulated payment processor
- **Graphical Simulation**: Aircraft animation using SFML/OpenGL

</details>

---

## 🛠️ Technologies Used

- **Language**: C / C++
- **Graphics**: SFML
- **Concurrency**: pthreads / std::thread
- **Build Tools**: G++, Makefile

---

## 🖥️ How to Run

### 🔧 Requirements

- Linux OS (POSIX compliant)
- g++
- SFML or OpenGL library

### 🚀 Build & Execute

```bash
g++ main.cpp Flight.cpp Runway.cpp FlightController.cpp FlightScheduler.cpp RunwayManager.cpp AVNManager.cpp -o aircontrolx -lsfml-graphics -lsfml-window -lsfml-system -pthread && ./aircontrolx

```
