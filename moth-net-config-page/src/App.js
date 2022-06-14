import { useState } from 'react';
import './App.css';
import { Button, Spinner, Container, Row, Col } from 'react-bootstrap';
import AIOForm from './Components/AIOForm';
import NetworksForm from './Components/NetworksForm';
import DevicesAccordion from './Components/DevicesAccordion';

/*
TODO: 
[X] add wifi form into main page 
[ ] controlled forms
[ ] esp sends a json object containing "SSID", "mac", "name", "aio_connected"
[ ] ui sends {"SSID": "MyNetwork", "PASS": "MyPassword"}
[ ] ui sends {"aio_user": "myaiouser", "aio_key": "myaiokey1234"}
[ ] ui sends manipulated devices object
[ ] delete device button
[ ] websockets with data: networks, devices, feeds, status
*/

function App() {
  const [isAioConnected, setAioConnected] = useState(false);
  const [isWifiConnected, setWifiConnected] = useState(false);
  const [isScanning, setIsScanning] = useState(false);
  const [networks, setNetworks] = useState(['one', 'two', 'three']);
  const [wifiSSID, setWifiSSID] = useState('');
  const [wifiPass, setWifiPass] = useState('');
  const [aioUsername, setAioUsername] = useState('');
  const [aioKey, setAioKey] = useState('');
  const [devices, setDevices] = useState(
    [
      {
        name: "Capacitive sensor",
        mac: "FF:FF:FF:FF:FF",
        feed: "",
        sensorType: "Input",
        value: "",
        connected: false
      },
      {
        name: "Capacitive sensor",
        mac: "FF:FF:FF:FF:FF",
        feed: "",
        sensorType: "Input",
        value: "",
        connected: true
      },
      {
        name: "Servo motor",
        mac: "FF:FF:FF:FF:FF",
        sensorType: "Input",
        feed: "",
        connected: true
      }
    ]
  );

  const ws = new WebSocket("ws://localhost/ws");

  ws.onmessage = (e) => {
    const json = JSON.parse(e.data);
    try {
      //if (json[0].)
      // Distinguish between different ws messages from the hub
    } catch(e) {
      // Handle error
    }
  };

  const onAioSubmit = (e) => {
    e.preventDefault();
    console.log('AIO credentials submitted');
    //setAioConnected(true);
  };

  const onWifiSubmit = (e) => {
    e.preventDefault();
    console.log('Wifi credentials submitted.');
    //setAioConnected(true);
  };

  const handleScan = (e) => {
    console.log('Start scan');
    setIsScanning(true);
  };

  const handleAioFormChange = (e) => {
    if (e.target.id === 'aioUsername') {
      setAioUsername(e.target.value);
    } else if (e.target.id === 'aioKey') {
      setAioKey(e.target.value);
    }
  };

  const handleWifiFormChange = (e) => {
    if (e.target.id === "ssid") {
      setWifiSSID(e.target.value);
    } else if (e.target.id === "pass") {
      setWifiPass(e.target.value);
    }
  };

  const handleFeedChange = (e) => {
    console.log(e);
  }

  return (
    <div className="App mt-5">
      <Container>
        <h1>Configure Your Devices</h1>
        <Row>
          <Col md='6' className='mt-5'>
            <h3>WiFi Settings</h3>
            <NetworksForm
              connected={isWifiConnected}
              onSubmit={onWifiSubmit}
              networks={networks}
              onFormChange={handleWifiFormChange}
              ssid={wifiSSID}
              pass={wifiPass}
            />
            <div className='mt-5'></div>
            <h3>Adafruit IO Settings</h3>
            <AIOForm
              onSubmit={onAioSubmit}
              connected={isAioConnected}
              onFormChange={handleAioFormChange}
              username={aioUsername}
              aioKey={aioKey}
            />
          </Col>
          <Col md='6' className='mt-5'>
            <h3>Devices in your network</h3>
            <Button
              variant='primary'
              disabled={isScanning}
              onClick={isScanning ? null : handleScan}
            >
              {isScanning ? <Spinner animation="border" size="sm" as="span" /> : null}
              {isScanning ? ' Scanning...' : 'Scan'}
            </Button>
            <DevicesAccordion 
              devices={devices}
              onFeedChange={handleFeedChange}
            />
          </Col>
        </Row>

      </Container>
    </div>
  );
}

export default App;
