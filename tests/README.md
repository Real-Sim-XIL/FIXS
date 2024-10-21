# Description of all tests

## CoordMerge

  a controller that receives information from SUMO and controls a dummy simulink profile

## SumoIpg
  
  a coordMerge network that tests SUMO and IPG co-simulation

## TurnSignal

  a network based on IPG example highway interchange to test the vehicle turning signal inidcators are synchronized with IPG

## SumoDriver

  a SUMO and Matlab co-simulation that feedback actual dynamics to SUMO vehicles

## SpeedLimit

  a dummy network with different speed limits to verify that FIXS is able to transmit speed limit information

## SignalIpg

  use the Shallowford network to test the synchronization of traffic signal light of SUMO and IPG

## ShallowfordRdSCIL

  the scenario for testing signal controller in the loop 

## Elevation

  test the transmit of elevation data of SUMO and IPG

## DelayedConnection

  ability to start SUMO simulation and only launch simulink after XYZ seconds

## Tests that need further service or development

  AsyncConn, VissimIpg, VissimSpeedLimit
