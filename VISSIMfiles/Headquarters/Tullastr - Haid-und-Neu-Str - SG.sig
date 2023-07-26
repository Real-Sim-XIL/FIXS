<?xml version="1.0" encoding="UTF-8"?>
<sc version="201503" id="1" name="" frequency="1" steps="0" defaultIntergreenMatrix="0" EPICSTimeHorizon="100000" EPICSLogLevel="3" interstagesUsingMinDurations="true" checkSum="1480433923">
  <signaldisplays>
    <display id="1" name="Red" state="RED">
      <patterns>
        <pattern pattern="MINUS" color="#FF0000" isBold="true" />
      </patterns>
    </display>
    <display id="2" name="Red/Amber" state="REDAMBER">
      <patterns>
        <pattern pattern="FRAME" color="#CCCC00" isBold="true" />
        <pattern pattern="SLASH" color="#CC0000" isBold="false" />
        <pattern pattern="MINUS" color="#CC0000" isBold="false" />
      </patterns>
    </display>
    <display id="3" name="Green" state="GREEN">
      <patterns>
        <pattern pattern="FRAME" color="#00CC00" isBold="true" />
        <pattern pattern="SOLID" color="#00CC00" isBold="false" />
      </patterns>
    </display>
    <display id="4" name="Amber" state="AMBER">
      <patterns>
        <pattern pattern="FRAME" color="#CCCC00" isBold="true" />
        <pattern pattern="SLASH" color="#CCCC00" isBold="false" />
      </patterns>
    </display>
  </signaldisplays>
  <signalsequences>
    <signalsequence id="3" name="Red-Red/Amber-Green-Amber">
      <state display="1" isFixedDuration="false" isClosed="true" defaultDuration="1000" />
      <state display="2" isFixedDuration="true" isClosed="true" defaultDuration="1000" />
      <state display="3" isFixedDuration="false" isClosed="false" defaultDuration="5000" />
      <state display="4" isFixedDuration="true" isClosed="true" defaultDuration="3000" />
    </signalsequence>
    <signalsequence id="4" name="Red-Green">
      <state display="1" isFixedDuration="false" isClosed="true" defaultDuration="1000" />
      <state display="3" isFixedDuration="false" isClosed="false" defaultDuration="5000" />
    </signalsequence>
  </signalsequences>
  <sgs>
    <sg id="1" name="V1" defaultSignalSequence="3" underEPICSControl="false">
      <defaultDurations>
        <defaultDuration display="1" duration="1000" />
        <defaultDuration display="2" duration="1000" />
        <defaultDuration display="3" duration="5000" />
        <defaultDuration display="4" duration="3000" />
      </defaultDurations>
      <EPICSTrafficDemands />
    </sg>
    <sg id="2" name="V2" defaultSignalSequence="3" underEPICSControl="false">
      <defaultDurations>
        <defaultDuration display="1" duration="1000" />
        <defaultDuration display="2" duration="1000" />
        <defaultDuration display="3" duration="5000" />
        <defaultDuration display="4" duration="3000" />
      </defaultDurations>
      <EPICSTrafficDemands />
    </sg>
    <sg id="3" name="V3" defaultSignalSequence="3" underEPICSControl="false">
      <defaultDurations>
        <defaultDuration display="1" duration="1000" />
        <defaultDuration display="2" duration="1000" />
        <defaultDuration display="3" duration="5000" />
        <defaultDuration display="4" duration="3000" />
      </defaultDurations>
      <EPICSTrafficDemands />
    </sg>
    <sg id="4" name="V4" defaultSignalSequence="3" underEPICSControl="false">
      <defaultDurations>
        <defaultDuration display="1" duration="1000" />
        <defaultDuration display="2" duration="1000" />
        <defaultDuration display="3" duration="5000" />
        <defaultDuration display="4" duration="3000" />
      </defaultDurations>
      <EPICSTrafficDemands />
    </sg>
    <sg id="5" name="V5" defaultSignalSequence="3" underEPICSControl="false">
      <defaultDurations>
        <defaultDuration display="1" duration="1000" />
        <defaultDuration display="2" duration="1000" />
        <defaultDuration display="3" duration="5000" />
        <defaultDuration display="4" duration="3000" />
      </defaultDurations>
      <EPICSTrafficDemands />
    </sg>
    <sg id="52" name="T52" defaultSignalSequence="4" underEPICSControl="false">
      <defaultDurations>
        <defaultDuration display="1" duration="1000" />
        <defaultDuration display="3" duration="5000" />
      </defaultDurations>
      <EPICSTrafficDemands />
    </sg>
    <sg id="53" name="T53" defaultSignalSequence="4" underEPICSControl="false">
      <defaultDurations>
        <defaultDuration display="1" duration="1000" />
        <defaultDuration display="3" duration="5000" />
      </defaultDurations>
      <EPICSTrafficDemands />
    </sg>
  </sgs>
  <dets />
  <callingPoints />
  <callingPointPairs />
  <intergreenmatrices>
    <intergreenmatrix id="1" name="Matrix 1">
      <intergreen clearingsg="1" enteringsg="3" value="6000" />
      <intergreen clearingsg="1" enteringsg="4" value="6000" />
      <intergreen clearingsg="1" enteringsg="5" value="6000" />
      <intergreen clearingsg="2" enteringsg="5" value="7000" />
      <intergreen clearingsg="3" enteringsg="1" value="6000" />
      <intergreen clearingsg="3" enteringsg="5" value="6000" />
      <intergreen clearingsg="3" enteringsg="52" value="7000" />
      <intergreen clearingsg="3" enteringsg="53" value="6000" />
      <intergreen clearingsg="4" enteringsg="1" value="6000" />
      <intergreen clearingsg="5" enteringsg="1" value="6000" />
      <intergreen clearingsg="5" enteringsg="2" value="7000" />
      <intergreen clearingsg="5" enteringsg="3" value="6000" />
      <intergreen clearingsg="5" enteringsg="52" value="6000" />
      <intergreen clearingsg="5" enteringsg="53" value="6000" />
      <intergreen clearingsg="52" enteringsg="3" value="11000" />
      <intergreen clearingsg="52" enteringsg="5" value="10000" />
      <intergreen clearingsg="53" enteringsg="3" value="6000" />
      <intergreen clearingsg="53" enteringsg="5" value="8000" />
    </intergreenmatrix>
  </intergreenmatrices>
  <progs>
    <prog id="1" cycletime="72000" switchpoint="0" offset="0" intergreens="1" fitness="0.000000" vehicleCount="0" name="Signal program 1">
      <sgs>
        <sg sg_id="1" signal_sequence="3">
          <cmds>
            <cmd display="1" begin="14000" />
            <cmd display="3" begin="68000" />
          </cmds>
          <fixedstates>
            <fixedstate display="4" duration="3000" />
            <fixedstate display="2" duration="1000" />
          </fixedstates>
        </sg>
        <sg sg_id="2" signal_sequence="3">
          <cmds>
            <cmd display="1" begin="17000" />
            <cmd display="3" begin="44000" />
          </cmds>
          <fixedstates>
            <fixedstate display="4" duration="3000" />
            <fixedstate display="2" duration="1000" />
          </fixedstates>
        </sg>
        <sg sg_id="3" signal_sequence="3">
          <cmds>
            <cmd display="3" begin="45000" />
            <cmd display="1" begin="64000" />
          </cmds>
          <fixedstates>
            <fixedstate display="2" duration="1000" />
            <fixedstate display="4" duration="3000" />
          </fixedstates>
        </sg>
        <sg sg_id="4" signal_sequence="3">
          <cmds>
            <cmd display="3" begin="17000" />
            <cmd display="1" begin="65000" />
          </cmds>
          <fixedstates>
            <fixedstate display="2" duration="1000" />
            <fixedstate display="4" duration="3000" />
          </fixedstates>
        </sg>
        <sg sg_id="5" signal_sequence="3">
          <cmds>
            <cmd display="3" begin="21000" />
            <cmd display="1" begin="40000" />
          </cmds>
          <fixedstates>
            <fixedstate display="2" duration="1000" />
            <fixedstate display="4" duration="3000" />
          </fixedstates>
        </sg>
        <sg sg_id="52" signal_sequence="4">
          <cmds>
            <cmd display="1" begin="11000" />
            <cmd display="3" begin="68000" />
          </cmds>
          <fixedstates />
        </sg>
        <sg sg_id="53" signal_sequence="4">
          <cmds>
            <cmd display="1" begin="11000" />
            <cmd display="3" begin="67000" />
          </cmds>
          <fixedstates />
        </sg>
      </sgs>
    </prog>
  </progs>
  <stages />
  <interstageProgs />
  <stageProgs />
  <dailyProgLists />
</sc>