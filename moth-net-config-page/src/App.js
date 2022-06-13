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
  const [wifiCredentials, setWifiCredentials] = useState({
    SSID: "",
    PASS: ""
  });
  const [aioCredentials, setAioCredentials] = useState({
    aio_user: "",
    aio_key: ""
  });
  const [devices, setDevices] = useState(
    [
      {
        name: "Capacitive sensor",
        mac: "FF:FF:FF:FF:FF",
        feed: "",
        sensorType: "Input",
        value: "",
        connected: true
      },
      {
        name: "Capacitive sensor",
        mac: "FF:FF:FF:FF:FF",
        feed: "",
        sensorType: 0,
        value: "",
        connected: true
      },
      {
        name: "Servo motor",
        mac: "FF:FF:FF:FF:FF",
        type: "Output",
        feed: "",
        optionLabel: "Swing (deg)",
        optionValue: 45,
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

  const handleAioUsernameChange = (e) => {
    console.log(e.target.value);
    let creds = aioCredentials;
    creds.aio_user = e.target.value;
    setAioCredentials(creds);
  };

  const handleAioKeyChange = (e) => {
    console.log(e.target.value);
    let creds = aioCredentials;
    creds.aio_key = e.target.value;
    setAioCredentials(creds);
  };

  const handleNetworkChange = (e) => {
    console.log(e.target.value);
    let creds = wifiCredentials;
    creds.SSID = e.target.value;
    setWifiCredentials(creds);
  };

  const handlePasswordChange = (e) => {
    console.log(e.target.value);
    let creds = wifiCredentials;
    creds.PASS = e.target.value;
    setWifiCredentials(creds);
  };

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
              onNetworkChange={handleNetworkChange}
              onPasswordChange={handlePasswordChange}
            />
            <div className='mt-5'></div>
            <h3>Adafruit IO Settings</h3>
            <AIOForm
              onSubmit={onAioSubmit}
              connected={isAioConnected}
              onUsernameChange={handleAioUsernameChange}
              onKeyChange={handleAioKeyChange}
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
            <DevicesAccordion devices={devices}/>
          </Col>
        </Row>

      </Container>
    </div>
  );
}

export default App;
