# Simulations

A collection of mathematical simulations including epidemiological disease models and financial market simulations, implemented in C++ with visualization capabilities for most.

## Features

- **Epidemiological Models**:
  - SIR (Susceptible-Infected-Recovered) compartmental model with Euler integration
  - SEIR (Susceptible-Exposed-Infected-Recovered) model with staged compartments
  - Agent-Based Model (ABM) for population-level disease dynamics
<img width="800" height="554" alt="abm usable" src="https://github.com/user-attachments/assets/05cd0662-e7dd-49d0-a175-c36433d6c9a6" />

- **Financial Simulations**:
  - VaR messurment
  - Monte Carlo based on GBM
  - black sholes for testing error in stochastic methods
  - cholesky for testing VaR based in returns
<img width="500" height="312" alt="Adobe Express - MonteCarlo" src="https://github.com/user-attachments/assets/c570ce70-49f1-472d-a7da-073e370b51ea" />

- **Visualization**: SFML-based graphics rendering for simulation results

## Tech stack

- **Language**: C++ (C++17)
- **Graphics**: SFML 3.1.0 (Simple and Fast Multimedia Library)
- **Compiler**: g++ (MinGW Strawberry C++)
- **Build System**: g++ with task configuration in VS Code

## Installation

1. **Install SFML**:
   - Download SFML 3.1.0 from https://www.sfml-dev.org/
   - Extract to a known location (e.g., `C:\Users\user\Downloads\SFML-3.1.0\`)

2. **Install Compiler**:
   - Install Strawberry Perl/MinGW with g++ compiler

3. **Clone/Download Repository**:
   ```
   https://github.com/PraxedisJRuv/simulations
   ```

## Usage

Currently working on SFML installation guide

### Building

Use the default build task in VS Code:
- Depending on your cpp configuration, just run the file (currently working in propper modularization).
- Or run: `g++.exe -fdiagnostics-color=always -g <file> -o <output.exe> -I<SFML_include> -L<SFML_lib> -lsfml-graphics -lsfml-window -lsfml-system`

### Running Simulations

Build and run each simulation from its source folder. Example commands:
```bash
# SIR model
g++.exe -fdiagnostics-color=always -g epidemiological/SIR/SIR_main.cpp epidemiological/SIR/SIR.cpp -I<C:/SFML-3.1.0/include> -L<C:/SFML-3.1.0/lib> -lsfml-graphics -lsfml-window -lsfml-system -o SIR.exe
./SIR.exe

# SEIR model with visualization
g++.exe -fdiagnostics-color=always -g epidemiological/SEIR/SEIR_main.cpp epidemiological/SEIR/SEIR.cpp -I<C:/SFML-3.1.0/include> -L<C:/SFML-3.1.0/lib> -lsfml-graphics -lsfml-window -lsfml-system -o SEIR.exe
./SEIR.exe

# Agent-Based Model
g++.exe -fdiagnostics-color=always -g epidemiological/ABM/ABM_main.cpp epidemiological/ABM/headers/ABM_functions.hpp -I<C:/SFML-3.1.0/include> -L<C:/SFML-3.1.0/lib> -lsfml-graphics -lsfml-window -lsfml-system -o ABM.exe
./ABM.exe

# Financial price container simulation
g++.exe -fdiagnostics-color=always -g financial/Price_Container/pc_main.cpp financial/Price_Container/price_container.cpp -o price_container.exe
./price_container.exe
```

## Project Structure

```
simulations/
├── README.md
├── LICENSE
├── epidemiological/
│   ├── ABM/
│   │   ├── ABM_main.cpp
│   │   └── headers/
│   │       ├── ABM_functions.hpp
│   │       ├── ABM_structures.hpp
│   │       ├── ABM_tests.hpp
│   │       └── ABM_visualization.hpp
│   ├── SEIR/
│   │   ├── SEIR_main.cpp
│   │   └── headers/
│   │       ├── SEIR_functions.hpp
│   │       ├── SEIR_structures.hpp
│   │       ├── SEIR_tests.hpp
│   │       └── SEIR_visualization.hpp
│   └── SIR/
│       ├── SIR_main.cpp
│       └── headers/     
│           ├── SIR_functions.hpp
│           ├── SIR_structures.hpp
│           └── SIR_tests.hpp
└── financial/
    ├── FABM/
    │   ├── strategy/
    │   │   ├── FAMB_strat_main.cpp
    │   │   └── headers/
    │   │       ├── FABM_strat_functions.hpp
    │   │       ├── FABM_strat_strucutres.hpp
    │   │       └── FABM_strat_tests.hpp
    │   └── systemmic_risk/
    │       └── FABM_sr.cpp
    ├── interest/
    │   └── interest.cpp
    └── Price_Container/
        ├── pc_main.cpp
        └── headers/
            ├── pc_functions.hpp
            ├── pc_tests.hpp
            ├── pc_visualization.hpp
            └── price_container.hpp
```

## Methodology

### Epidemiological Models

- **SIR Model**: Uses the Euler numerical integration method to solve differential equations tracking the movement of populations through Susceptible, Infected, Recovered states
- **SEIR Model**: Extends SIR with an Exposed compartment, using staged transitions to model disease incubation periods
- **Agent-Based Model (ABM)**: Simulates individual agents with stochastic state transitions based on infection probability and recovery rates

### Financial Models

- **Price Container**: Analyzes historical price data, computing statistical measures for technical analysis
- **Interest Model**: Models compound interest and investment growth calculations

All simulations use C++ STL for data management and `<random>` for stochastic processes.
