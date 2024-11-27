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
