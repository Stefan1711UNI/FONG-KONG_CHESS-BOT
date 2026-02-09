# SCARA Chess Robot - Kinematics Visualization

This tool simulates the workspace, reachability, and safety constraints of a 2-Link Planar (SCARA) robotic arm designed for an automated chess board.

It visualizes the Inverse Kinematics (IK) logic to verify that a **15cm + 15cm** arm configuration can successfully reach all 64 squares of a **32x32cm** chess board while maintaining a safe "Graveyard" zone for captured pieces.

## Project Overview

* **Robot Type:** 2-Link Planar Arm (SCARA configuration).
* **Arm Lengths:** L1 = 150mm, L2 = 150mm.
* **Board Dimensions:** 320mm x 320mm (40mm squares).
* **Machine Footprint:** 400mm x 400mm.
* **Key Feature:** The board is offset to the left to create a "Graveyard" and Control Panel zone on the right side.

## Features

* **Inverse Kinematics Solver:** Calculates the shoulder ($\theta_1$) and elbow ($\theta_2$) angles for any given coordinate $(x, y)$.
* **Reachability Check:** visually flags unreachable areas in **RED**.
* **Safety Geofencing:** Simulates the physical boundaries of the table to ensure the robot never attempts to move outside the machine limits.
* **Dynamic Interaction:** Move your mouse over the plot to see how the arm behaves in real-time.

## AI Disclaimer

This visualization tool was developed with the assistance of Artificial Intelligence (AI). It is intended solely as a **conceptual aid** to visualize the kinematic behavior and workspace constraints of the robot. While the mathematical models are accurate representations of standard planar kinematics, actual mechanical performance (friction, backlash, belt tension, etc.) may vary in the physical build.

## Installation

1.  **Clone the repository:**
    ```bash
    git clone <https://github.com/Stefan1711UNI/FONG-KONG_CHESS-BOT/tree/inverse_kinematics_visual>
    cd <inverse_kinematics_visual>
    ```

2.  **Install dependencies:**
    ```bash
    pip install -r requirements.txt
    ```

## Usage

Run the simulation script:

```bash

python ik_visual.py
