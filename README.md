# COMP3004 Team 7: Neureset Direct Neurofeedback EEG Device Simulator

## Project Team

-   Anjish Pradhananga
    -   MainWindow Use case diagram
    -   State Diagram for device
    -   Use cases: 4,6,7
    -   Discussions for design and implementations.
    -   Came up with the cool name B| LOL
    -   MainWindow UI and implementation:
        -   The Ui components
        -   Power on/off
        -   Charging implementation
        -   Connection Lost + Connection Reconnected
        -   Stop/pause/play implementation
        -   Elapsed time and connection lost time timers
        -   Viewing Device Session history and selecting for upload
        -   Viewing and Setting device Date/time
        -   Power Low use case implementation
        -   Progress bar implementation
    -   Backend Session Data management and handling fixes
    -   Backend Stopping/pauses mechanism fixes
    -   Bug fixes here and there
-   Aria Zhang
    - Use case 6
    - Contributed to creating UML and Sequence Diagrams.
    - Contributed to completing the Use Case Diagram.
    - Implemented SessionManager Class.
    - Contributed to ensure data is properly transferred from EEG to Device.
    - Fixed bugs for Device Class, SignalProcessor Class and MainUI.

-   Ryan Wang
    - Use case diagram: 5
    - Sequence diagram: 1, 5
    - Implemented EEG and SignalProcessor class
    - Modified WaveGenrator class to retrieve required data
    - Added testing code for SessionManager class to verify correct data has been collected
    - Small fixes for MainUI and TestUI
-   Seyedsajad Hosseini
    - Main use case 
    - UML class diagram
    - Design patterns report
    - Implemented TestUI window
	- Implemented TestUI class
	- Implemented Device class
	- Implemented WaveGenerator class
	- Modified EEG class
	- Modified timer and progress bar in MainWindow
	- Critical fixes in MainWindow and SessionManager
-   Yan Tang
    -   Use Case 2 and Use Case 3.
    -   Contributed to creating UML and Sequence Diagrams.
    -   Contributed to completing the traceability matrix.
    -   Contributed to completing the Use Case Diagram.
    -   Implemented PC UI.
    -   Implemented PC functionalities.
    -   Implemented database functionality.
    -   Set up the Qt Custom Plot library.
    -   Participated in discussions regarding design and implementations.

## Project Overview

This project simulates the software for a hypothetical consumer EEG direct neurofeedback device called Neureset, based on the LENS Neurofeedback system. It aims to provide a software-based prototype for treating various conditions such as ADHD, PTSD, TBI, anxiety, and more, as well as optimizing brain function and cognitive performance.

Neureset is intended as a standalone consumer device for direct neurofeedback, requiring no therapist intervention. It consists of an EEG headset connected to a handheld device, functioning both as a signal processor and user interface. The device calculates dominant frequencies, delivers treatments, and stores session data.

## Getting Started

### Prerequisites

-   Qt framework installed on your system.
-   A C++ development environment compatible with the Qt framework.
-   Access to the course VM (COMP3004-F23) provided [here](https://carleton.ca/scs/tech-support/virtual-machines/).
-   [Mermaid Markdown](https://marketplace.visualstudio.com/items?itemName=bierner.markdown-mermaid) - Vscode extension to run and view diagrams.

### Installation

1. Clone the repository to your local machine.
2. Open the project in Qt Creator.
3. Configure the project using the Qt version installed on your system.
4. Build the project to generate the executable.

## Usage

1. **Turn the Device On**
2. **Start a New Session**: Select the 'New Session' option from the menu and press the play button. Ensure the EEG headset is properly connected.
3. **Session Progress**: Monitor the session progress through the timer and progress bar displayed on the device UI.
4. **Pause/Stop Session**: Sessions can be paused or stopped using the corresponding buttons. Paused sessions can be resumed unless stopped or terminated due to loss of contact.
5. **View Session Log**: Access the session log to review past sessions, including time and date.
6. **Upload Session Data**: Connect the device to a PC to upload and view detailed session data, including before and after dominant frequencies.

## Testing Guidelines

A waveform generator simulates realistic EEG waveforms for testing, covering four brainwave bands. The testing process involves:

-   Connecting to 21 EEG sites (7 for testing purposes).
-   Processing input waveforms, calculating dominant frequencies, and delivering treatments.
-   Adjusting therapy timing for quicker testing cycles.
-   Simulating connection losses and battery depletion scenarios.

## Deliverables

-   Source code of the Qt C++ project.
-   UML diagrams and design documentation: 
    - **Run Ctrl + Shift + V to run the markdown to view the design documents using the extension**
-   Requirements traceability matrix.
-   Video demonstration of the simulation through specified scenarios.

## Additional Information

For more background on the LENS Neurofeedback system and the theoretical foundation of direct neurofeedback, visit the [Ochs Labs website](https://main.ochslabs.com/about/) or refer to the provided links and reading materials.

## Acknowledgments
