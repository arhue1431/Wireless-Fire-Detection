# Wireless-Fire-Detection
# Wireless forest fire detection system for senior design project
### Abigail Rhue, Tate Ichiki 

## Motivation
On average, there are more than 100,000 wildfires that occur in the United States every year. Due to climate change/global warming, wildfires tend to occur more frequently and last longer. The Wireless Fire Detection System was inspired by the Mendocino Complex Fire (July 2018). This fire lasted 23 days and burned over 450,000 acres of land. 

## Goal 
The goal of this project was to build a wireless system using multiple modules and a variety of sensors to detect forest fires. The modules would talk to each other and notify the fire department in order to provide necessary procedures to contain and put out the fire.

## Naterials
- Arduino uno
- NR24L01 Transceiver
- MQ 2 Gas Sensor
- MPL3115A2 Altitude/Pressure Sensor
- DHT22 Humifity and Temperature Sensor

## Methods
A mesh wriless sensor network was set up using the transceivers mentioned above. Three modules were made, each containing the same sensors and sending data through the network ending up outputting to a monitor for data collection and monitoring. The first module (node 11) is farthest away from the monitor. This node has the sensors to monitor its surroundings and a transmitter that sends all of its data to its direct parent node (node 10). Node 10 will then collect its own sensor data and the data it receives from node 11 and sends it to the last module (node 00). Node 00 receieves all of the data from node 10 and from its own sensors, so it ends up with three separate data sets. Node 00 displays its data to the monitor and once thresholds of values are determined for when a fire is started, it would alert the monitor of the problem. Because it is a mesh network, there is security built in if one of the nodes fails. If node 10 fails, node 11 will instead directly send to nodw 00. With more modules, node 11 would send its data to another module the same distance away from it as node 10 was instead of directly to node 00.  The devices are shown below.

Node 11
![Node11]()

Node 10
![Node10]()

Node 00
![Node00]()


## Code
The code for each module is linked below:
- [Transmitter Only (node 11)]
- [Transmitter and Receiver (node 10)]
- [Receiver only (node 00)]

## Results
The wireless mesh network was built and tested. Each module successfully sent data to each other. As expected, when one node failed, the system was still able to communicate to the parent node. The system was tested by lighting a match. In the serial monitor, it showed an increase of temperature and increase of smoke. This showed that the sensors were properly wired and could detect when a fire would start.

## Youtube Video
A [video](https://www.youtube.com/watch?v=4ruEdpTAvp0&feature=youtu.be) created for the Capstone Competition giving an overview of the project and showing the modules working.


## Further considerations
In the future, testing the modules in larger scale would be ideal. Through these tests, threshold to determine the instance a fire start will help aid in early detection. A GPS system would also be integrated to get accurate location readings if a fire occurs. Finally, moving from detection to prediction of forest fires would be the next step, whether predicting when a fire may start or where a fire is going while in prograss.
## Resources
