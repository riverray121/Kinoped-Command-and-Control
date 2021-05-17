# Kinoped-Command-and-Control
Project Created by Innovations Health Systems LLC

Contributors: Elijah Retzlaff

Establish command and control of the robotics of the Kinoped; capture, record, and store all relevant data produced during the use of the Kinoped; and create the foundational Unity VR Environment that allows for user command of the Kinoped’s robotic systems


Kinoped Command and Control Project Description
By Elijah Retzlaff – elijah@innovationshealth.com
May 1, 2021 – Draft 1


General Project Overview
Project Goal: To establish command and control of the robotics of the Kinoped; capture, record, and store all relevant data produced during the use of the Kinoped; and create the foundational Unity VR Environment that allows for user command of the Kinoped’s robotic systems.

General Project Overview Flowchart

 
General Project Definitions

Kinoped Legs (Left/Right): The legs of the Kinoped are viewed in relation to the user on the machine. Thus, the Left Kinoped Leg is the leg associated with the left leg of the individual looking down from their seat within the machine and the Right Kinoped Leg is the leg associated with the right leg of the individual looking down from their seat within the machine.
Kinoped Leg Segment (Upper/Lower): Each Kinoped Leg is composed of two Leg Segments (Upper and Lower) and a Footplate. The Upper Leg Segment is the parallelogram structure of the leg attached to the main body of the machine. The Lower Leg Segment is the parallelogram structure of the leg attached to Kinoped Footplate.
Kinoped Footplate: The system that includes the plate which the user's foot makes contact, the joint which allows it to rotate parallel to the transverse plane, the joint which allows it to rotate parallel to the frontal plane, and the piece of the device connecting the footplate to the parallelogram structure of the Lower Kinoped Leg Segment which allows it to rotate parallel to the sagittal plane. 
User Leg: Legs of the person currently using the Kinoped. Their motion is tracked using six Xsens sensors. 
Kinoped Joint Angles: Broadly used term to describe the angles of the Kinoped at each of the non-telescopic joints on the robot. 
Ingress/Egress Position: The position defined by positioning the Kinoped Legs such that the user is in a bent-legged squatting position from which they can freely enter and exit the Kinoped device.
Standing Position: The position defined by positioning the Kinoped Legs such that the user is in a straight-legged standing position parallel to the surface the Kinoped is stationed upon. 

General Project Description

	The command-and-control project as a whole consists of three primary elements that need to be established: complete control of the movement of the Kinoped Legs, collection of all relevant movement data for each Kinoped user, and an interactive VR environment for the user to interface with the machine. Six primary subprojects must also be completed, and all primary and subproject elements, along with elements already completed, need to be compiled. 
1.	PLC to PC Connection and Communications
2.	Movement Management System
3.	ROS Controller Hub
4.	Unity Environments
5.	Online Database Integration 
6.	Visual Controller Interface 
Each subproject is described and detailed in the following pages of the document. As each element is completed, they will be brought together and compiled into one complete command-and-control system for the Kinoped device (along with the elements of the system that have already been completed by USA Firmware). 
In the present design, there are three hardware components and one cloud component in the system. The Kinoped-PLC component controls all sensor, motor, and cylinder input and output. The PC runs Linux Debian 10 (Buster), is connected to the PLC via ethernet, and runs all the software for the command-and-control system (excluding the online master database which will be stored on a Cloud Database). The Valve Index VR Headset (and controllers) are connected to the PC via DisplayPort and USB and serve for visualizing the VR environment and simple selection interactions (using the controllers) in the environment.
Using the general project overview flowchart included above as a guide, we can see the general structure of the project and the interactions between different sub-elements of the project. At the present stage of the project, the fully or partially completed elements include XSENS sensor-ROS node communications, the PostgreSQL database and its ROS node communications, the WebUI interface and its ROS node communications, and the ROS controller hub.

General Project Questions

•	Will the creation of this system be possible with a non-Realtime operating system?
o	Will ROS2 (designed for real-time control) be required for this project? 
	Note: If ROS2 is used Unity Robotics Hub for ROS2 and MoveIt!2 must also be used for the project
•	Provided that the current PC purchased for the project may not be able to support VR (due to not being able to fit a larger (VR capable) GPU in its casing), do we need to purchase a new PC for the project?
•	Does there already exist a physics engine of some type capable of the calculations and motion planning we need for this project?














Subproject One – PLC to PC Connection and Communications
Project Goal: Create a network socket on the Kinoped onboard PLC capable of reading and writing data (received and published to and from the Movement Management System (ROS node)) to and from the PLC controller onboard the Kinoped robot. 

Subsystem Flowchart

 

Subproject Description

Subsystem Inputs: Raw psi and encoder values from the PLC (read in via ethernet) and psi values to publish to the PLC (read in from the Movement Management Node).
Subsystem Outputs: Cylinder psi values (written to the PLC via ethernet) and joint angle values for each Kinoped joint relative and psi values for each cylinder (published to the ROS Movement Management System Node).

	The PLC network socket must be written to run on the Linux Debian 10 PC such that it is constantly updated by the PLC (data transferred via ethernet cable) as the Kinoped Legs move and the encoder and cylinder psi values change. Values read in from the PLC must be published to the Movement Management System Node at regular intervals (interval to be determined) to keep the rest of the command-and-control system up to date with position data. The Movement Management System Node will also be publishing new psi command data at regular intervals (interval to be determined) for the PLC network socket to read in and write out to the PLC.
	In doing so, the PLC network socket will need to handle all conversions to and from raw PLC data and ROS node data, such that each component can digest and use the data for their purposes. The socket may also need to handle the broadcasting of any error messages from the PLC, run checks on air pressure supplied to the PLC, and handle initial machine startup calibrations.
Note: To ensure encoders are calibrated to the correct zero-degree angle for each joint, a start-up procedure may need to be created (such that every time the command-and-control system starts up, the Movement Management Node runs the startup procedure (involving moving joints to their range of motion (ROM) limits) allowing the PLC socket to determine a zero-degree angle for each joint encoder).

Subproject Questions

•	How fast will information be able to be transferred from PLC to PC to ROS node and back?
•	What existing system/programs will be used to read in the data from the PLC?
o	FINS
o	OPC PLC base product 
o	Kepware 
•	How much of the HMI controller interface program written by Benoit Brisson can be used for the PLC socket?










Subproject Two – Movement Management System
Project Goal: Create a ROS node (written in C++ or Python) which handles the entirety of all control of the Kinoped Legs by communicating with the PLC Socket, the Unity Environments, and the Visual Controller Interface, and which carries out all modeling and calculations needed to command and control the movement of the Kinoped Legs. 

Subsystem Flowchart

 

Subproject Definitions

Environment Management Cycle: Management of all movement of the Kinoped Legs as the user interacts with the Unity Environment by using the device. Handles communication between PLC and Unity, Xsens and Unity, and all calculations needed to determine and execute motion of the Kinoped Legs.
The cycle consists of three phases:
1.	PLC Reception and Translation: Receives PLC encoder values, PLC cylinder Psi values, and Xsens sensor values when published to the node, then converts them to the correct reference frame and format for current Unity Environment and ROS.
2.	Unity Publishing and Receiving: The Movement Management Node publishes relevant position data (of Kinoped Footplate and User Leg) to the Unity Environment to keep visuals updated with the current position of the Kinoped Footplates and User Legs. Once this is received, the Unity Environment publishes a Nearest Object Bundle to the Movement Management Node for each Kinoped Leg (giving information about what object and forces are interacting with the users’ feet in the Unity Environment). The MM Node then publishes all relevant movement and position data to postgreSQL database (in association with the current user's profile).
3.	Movement Calculation and Cylinder Execution: Finally, the Movement Management Node uses the position data, Nearest Object Bundles, cylinder Psi values, and calculations from the previous Environment Management Cycle, as well as the position data, cylinder Psi values, and the current Nearest Object Bundles to extrapolate position, velocity, and acceleration vectors and the momentum (first converting the Nearest Object Bundle information to the correct reference frame and format) of the Kinoped Footplates. These calculations along with the Environment Launch Info are then used to calculate the next target position for the Kinoped Footplates and the Psi needed at each cylinder in the Kinoped Legs to achieve that position. These Psi values are then published to the PLC.
Subproject Description
Subsystem Inputs: PLC Socket values, Unity Environment specifications, Unity Environment interaction data, XSENS position data, and controller interface commands.
Subsystem Outputs: PLC Socket commands, PostgreSQL database data, XSENS and joint position data, environment activity information, and user use data for controller interface.
	The Movement Management System is a ROS node written in C++ or Python that is in charge of managing all movement of the Kinoped Legs and the calculations needed to move them. It has two primary components: General Management Handling and Environment Management Cycle. 
	The General Management Handling handles starting up the machine, error handling, setting up and adjusting the machine for a particular user, reading in Unity Environment information, and starting, pausing, and stopping the Environment Management Cycle with the correct information to handle movement correctly for the Unity Environment currently in use. 
	Once started with the configurations appropriate for the particular environment the user is currently using with the machine, the Environment Management Cycle runs the cycle (until stopped) of reading in position data, converting it to a format and reference frame capable of being read into Unity, publishing the data to the Unity Environment, reading in environmental data from the Unity Environment, determining the next position to move the cylinders, and then publishing psi values for each cylinder to the PLC Socket to achieve that position.

Subproject Questions

•	How will the physics of the movements be handled? 
o	MATLAB for modeling?
o	ROS plugin for modeling?
o	Some level of ML for predicting movements? 























 
Subproject Three - ROS Controller Hub
Project Goal: Create a central framework for communication with all elements of the Kinoped command and control system, functioning as a platform for all required nodes as well as motion planning and tracking.

Subproject Fundamentals Flowchart

 

Subproject Description 

	The ROS Controller Hub will serve as the main controller and communication organizer for the command-and-control system as a whole. While the visual controller interface will act as the user’s way to control the Kinoped device, it will primarily just serve as a way to interact with the ROS Controller hub from a graphical interface. The controller hub design will evolve along with and conform to the needs of the other subsystems within the project.
	The MoveIt! plugin may be used for extrapolating joint positions necessary for a certain Kinoped Footplate position in space or simplified tracking of the motion of the Kinoped Legs. It may also be used for kinematics solving if possible, but this is unlikely.
Note: As the project evolves there will need to more nodes added, and further functionalities explored. This is only a partial picture of what the ROS Controller Hub will be required to control, a set of functions that will be expanded as the project progresses.

Subproject Questions

•	Will we need to use a plugin kinematics solver or will all movements be predictive based on incoming PLC data?
•	Will MoveIt! play a role greater than simply speeding up the determination of joint positions based on end-effector location? 
o	If used for Kinematics solving:
	Will it be fast enough, or will MoveIt!2 be required?
	Will we need to use IKFast to develop our own custom kinematics solver?















 
Subproject Four – Unity Environments
Project Goal: Create Unity3D VR environments capable of representing the lower extremity movements of a user on the Kinoped as well as a VR environment with objects they can interact with during device use. 

Subproject Fundamentals Flowchart

 

Subproject Definitions

Unity Environment: The virtual environment that could consist of any number of Unity Environmental Objects, any physics, and any representation of the users’ movements. There are two types of environments for the two different modes of user visualization and kinematics. The type of kinematic environment the user has launched should be sent to the Movement Management Node upon launch of the environment.
Type A: Kinematically Correct Visualization
•	Despite the pelvis being fixed, all position (User Leg and Kinoped Footplate) data published to the Unity Environment has been transformed to represent the data that would have been published if the pelvis were not fixed
o	This means that all position data published from the Movement Manager Node to the Unity Environment will not be position relative to the pelvis, but rather the center of mass of the individual*
o	This also means that position data for the pelvis will also be published to the Unity Environment for visualization 
o	Additionally, all Environmental position data published from the Unity Environment will be transformed to a fixed pelvis perspective before undergoing any analysis 
Type B: Spatially Correct Visualization 
•	This type of environment will be a direct translation of the users’ movements in real space to the Unity Environment 
•	All positions published to the Unity Environment will be relative to the fixed hip of the user 
o	This means that if the user took off the VR headset mid-game, they would see their lower extremities in the same place they were visualized at in the VR Environment 
*This may change, these types of environments are crucial but 2nd priority to Type B Environments for the time being. Further discussion is required.
Unity Environment Object: Any visual in the Unity Environments which can be interacted with by the user’s lower extremity representation in the Unity Environment. Every object has at all times three fundamental characteristics: Environment Object Interaction Type, Environment Object Interaction Force, and position relative to the nearest user foot representation. Objects are at a fixed position relative to the fixed pelvis in Type B Unity Environments and are fixed in orientation to users’ center of mass but not fixed in distance in Type A Unity Environments.
Environment Object Interaction Type: There are three types of interaction a user can experience with a Unity Environment Object:  
Push: When the user is in range of the object, a force is exerted at the Kinoped Footplate to “push” them away from contact with the object. 
Pull: When the user is in range of the object, a force is exerted at the Kinoped Footplate to “pull” them towards contact with the object.
Collision: When the user is in contact with the object, a force is exerted at the Kinoped Footplate to simulate a "collision" from contact with the object. If the user pushes with a force greater than the object's force, then the object will start to move with the users’ foot representation.
Environment Object Interaction Force: The numerical force value that an object will exert upon the user. If the Environment Object Interaction Type is push, this will be the force against the user while moving towards the object, if the type is pull, this will be the force at which the users’ foot is pulled towards the object, and if the type is collision, this will be the maximum force the user will have to exert to move the object within the Unity Environment.
Nearest Object Bundle: Bundle of information describing the Unity Environment Object the current Kinoped Footplate (based on most recent position data) is closest to (one bundle per footplate). Includes position of the nearest point on the nearest object (relative to the footplate), the Environment Object Interaction Type, and the Environment Object Interaction Force for that object. 
Environment Management Cycle: Management of all movement of the Kinoped Legs as the user interacts with the Unity Environment using the device. Handles communication between PLC and Unity, Xsens and Unity, and all calculations needed to determine and execute motion of the Kinoped Legs.
The cycle consists of three phases:
1.	PLC Reception and Translation: Receives PLC encoder values PLC cylinder Psi values, and XSENS sensor values when published to the node, then converts them to the correct reference frame and format for current Unity Environment and ROS.
2.	Unity Publishing and Receiving: The Movement Management Node publishes relevant position data (of Kinoped Footplate and User Leg) to the Unity Environment to keep visuals updated with the current position of the Kinoped Footplates and User Legs. Once this is received, the Unity Environment publishes a Nearest Object Bundle to the Movement Management Node for each Kinoped Leg (giving information about what object and forces are interacting with the users’ feet in the Unity Environment). Finally, the MM Node publishes all relevant movement and position data to PostgreSQL database (in association with the current user’s profile).
3.	Movement Calculation and Cylinder Execution: Finally, the Movement Management Node uses the position data, Nearest Object Bundles, cylinder Psi values, and calculations from the previous Environment Management Cycle, as well as the position data, cylinder Psi values, and the current Nearest Object Bundles to extrapolate position, velocity, and acceleration vectors and the momentum (first converting the Nearest Object Bundle information to the correct reference frame and format) of the Kinoped Footplates. These calculations along with the Environment Launch Info are then used to calculate the next target position for the Kinoped Footplates and the Psi needed at each cylinder in the Kinoped Legs to achieve that position. These Psi values are then published to the PLC.
Environment Launch Info: Bundle of information pertaining to the type of environment that was launched, which tells the Movement Manager Node how to send and interpret the data passed between itself and ROS. Sent upon startup of every environment, the environment does not finish launching until it receives confirmation that Environment Launch Info was loaded by the Movement Manager Node.
Environment Status (Start/Stop/Pause): Message published to Movement Manager Node when the user (from within the VR system) changes the status of their activity in the Unity Environment. When the Unity Environment is launched initially or after the user pauses the environment, the user should be presented with a screen (within the VR system) to choose the setting and set up for the environment they are about to start. During this time, the Movement Manager Node will have the Kinoped legs in a position where the user can comfortably enter and exit the machine (Ingress/Egress Position). Once the user is ready to begin, they will select Start (from within the VR system).  At this point, the Environment Status (Start) message will be published to the Movement Manager Node causing the node to move the Kinoped Legs to Standing Position and begin controlling movement of the footplates as directed by the Unity Environment. If the user pauses the environment during use (using the Valve Index Controllers), the pause message will be published and the Movement Manager Node will return the legs to standing position, until the environment is un-paused, and the start message is published again. If the user pauses and then ends their use of the environment, the stop message will be published causing the Movement Manager Node to return the Kinoped Legs to (Ingress/Egress Position) and send a message to the ROS controller to shutdown the Unity Environment. 

Subproject Description

	The Unity Environments will serve as the visualization of user movements, the display through which they can interact with a virtual environment, and the subsystem that sends commands for movement (in the form of information about the current environment) to control the movement of the Kinoped Legs. The current plan is to transmit information between ROS and Unity using Unity Robotics Hub, though other methods may be used as the project evolves.
The Unity Environments should function as independent environments that can be launched at any time by the ROS Controller Hub (upon receiving the command to do so from the Visual Control Interface) but will always interact in the same way with the Movement Manager Node. They do not handle any physics or kinematic calculations; they only display visual representations of the user and their interactions in the environment and send information about the location of objects in the environment. 
Note: Each environment containing elements dependent on user dimensions must adjust environmental objects to be proportional. 

First Fundamental Environments

Environment 1: Free Floating
•	Type B Environment
•	An environment consisting of empty space with no Unity Environment Objects
•	Has two modes (chosen using the valve index controllers upon launch of the environment) 
1.	Only footplate positions are visualized: The user can see two glowing orbs representing their feet in the environment. These orbs are based upon the positioning of the Kinoped Footplates (published by the Movement Manager Node to the Unity Environment) and so will move exactly how the user moves their feet.
2.	 The Users Leg positions are visualized: Visual representations of the legs are shown in some capacity (stick figure, semi-anatomical, etc.) as they move using data fed in from the Movement Manager Node describing the position of the users' legs as they move.
Environment 2: Trace your Name
•	Type B Environment 
•	An environment consisting of empty space with no Unity Environment Objects
•	Users’ feet are visualized by two glowing orbs
•	When a user moves their feet (visualized as glowing orbs) they leave a glowing trail behind them as they move (which disappears after a few seconds) 
•	This allows you to draw (ex. sign your name) in 3D space with your feet

Environment 3: Basic Pattern Following 
•	Type B Environment 
•	Feet are represented as glowing orbs and glowing 3D patterns appear that you must trace with them
o	These patterns must be sized relative to the users’ dimensions (which will be published to the environment from the Movement Management Node upon launch of the Unity Environment)
•	Upon launch of the Environment, the user must select the configuration they want for the environment (selections are made using the valve index controllers)
o	User selects a basic pattern to trace, for example:
	Circle 
	Square 
	Loop
o	User selects a foot to trace the pattern with 
	Left foot
	Right foot
	Both 
o	User selects an interaction type for the pattern 
	Push
	Pull
	None
o	User selects a force amount for the interaction (in pounds (lbs))
	0 to 100
•	Environment launches and the correct patterns appear passing the correct Nearest Object Bundle to the Movement Management Node based on the users’ selections 
•	The Movement Management Node will handle applying forces to push or pull the user towards or away from the path, the Unity Environment just needs to display the users’ movements and transmit the relative position of the path objects and their traits 

Environment 4: Ambulatory Pattern Tracing -TBD
•	Type A Environment
•	Three modes 
1.	Left Leg
2.	Right Leg
3.	Both 

Subproject Questions

•	What format do the Unity Environments need the position data to be in?
•	How should an API be created so to allow for easy further development of Unity Environments in the future?
•	Should the Unity Environment (and SteamVR) be run on a separate OS or device?
o	If so should ROS-sharp be used and will it be fast enough?

















Subproject Five – Online Database Integration

Will be used to move collected data from the PostgreSQL database to cloud storage after the user ends their session in an environment. Will also be used to store user profiles and machine configuration for that user as well as an identifier linked to data from their previous sessions. 

TBD






















Subproject Six – Visual Controller Interface
TBD
  
