import { useState } from 'react';
import './App.css';
import { Button, Spinner, Container, Row, Col } from 'react-bootstrap';
import AIOForm from './Components/AIOForm';
import NetworksForm from './Components/NetworksForm';
import DevicesAccordion from './Components/DevicesAccordion';

/*
TODO: 
[X] add wifi form into main page 
[X] controlled forms
[X] esp sends a json object containing "SSID", "mac", "name", "aio_connected"
[X] ui sends {"SSID": "MyNetwork", "PASS": "MyPassword"}
[X] ui sends {"aio_user": "myaiouser", "aio_key": "myaiokey1234"}
[X] ui sends manipulated devices object
[ ] remove device button (only on disconnected devices)
[ ] use dropdown device list to select which sensor an action should be connected to
[X] websockets with data: networks, devices, feeds, status
*/

const ws = new WebSocket("ws://192.168.0.48/ws");

function App() {
  const [isAioConnected, setAioConnected] = useState(false);
  const [isWifiConnected, setWifiConnected] = useState(false);
  const [isScanning, setIsScanning] = useState(false);
  const [isWifiScanning, setIsWifiScanning] = useState(false);
  const [scanIntervalId, setScanIntervalId] = useState(null);
  const [scanWifiIntervalId, setScanWifiIntervalId] = useState(null);
  const [networks, setNetworks] = useState([]);
  const [feeds, setFeeds] = useState([]);
  const [wifiSSID, setWifiSSID] = useState('');
  const [wifiPass, setWifiPass] = useState('');
  const [aioUsername, setAioUsername] = useState('');
  const [aioKey, setAioKey] = useState('');
  const [devices, setDevices] = useState(
    [
     /*{
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
      }*/
    ]
  );


  ws.onmessage = (e) => {
    const json = JSON.parse(e.data);
    try {
      //if (json[0].)
      // Distinguish between different ws messages from the hub
      // if networks, populate networks array
      // if status, update isAioConnected, isWifiConnected
      console.log(json);
      if(Object.prototype.toString.call(json) === '[object Array]') {
        console.log('Received array');
        if ('SSID' in json[0]) {
          console.log('Received networks!');
          setNetworks(json);
        } else if ('mac' in json[0]) {
          console.log('Received devices!');
          setDevices(json);
        } else if ('name' in json[0]) {
          console.log('Received feeds from aio!');
          setFeeds(json);
          console.log(feeds);
        } else {
          console.log('Array with unknown structure. No data was updated.');
        }
      }
      else {
        if ('aio_connected' in json) {
          console.log('Received status!');
          setAioConnected(json.aio_connected);
          setWifiConnected(json.wifi_connected);
        }
      }
    } catch(e) {
      // Handle error
    }
  };

  ws.onopen = (e) => {
    console.log('Connected to WebSocket');
    ws.send("status");
  }

  const onAioSubmit = (e) => {
    e.preventDefault();
    console.log('AIO credentials submitted');
    ws.send(JSON.stringify({aio_user: aioUsername, aio_key: aioKey}));
  };

  const onWifiSubmit = (e) => {
    e.preventDefault();
    console.log('Wifi credentials submitted.');
    ws.send(JSON.stringify({SSID: wifiSSID, PASS: wifiPass}));
  };

  const handleScan = (e) => {
    console.log('Start scan');
    setIsScanning(true);
    ws.send("devices");
    let id = setInterval(() => {
      console.log('Requested devices');
      ws.send("devices");
    }, 6000);
    setScanIntervalId(id);
  };

  const handleWifiScan = (e) => {
    console.log('Start wifi scan');
    setIsWifiScanning(true);
    setWifiConnected(false);
    ws.send("networks");
    let id = setInterval(() => {
      console.log('Requested networks');
      ws.send("networks");
    }, 6000);
    setScanWifiIntervalId(id);
  }

  const handleAioFormChange = (e) => {
    setAioConnected(false);
    if (e.target.id === 'aioUsername') {
      setAioUsername(e.target.value);
    } else if (e.target.id === 'aioKey') {
      setAioKey(e.target.value);
    }
  };

  const handleWifiFormChange = (e) => {
    setIsWifiScanning(false);
    setWifiConnected(false);
    clearInterval(scanWifiIntervalId);
    if (e.target.id === "ssid") {
      setWifiSSID(e.target.value);
    } else if (e.target.id === "pass") {
      setWifiPass(e.target.value);
    }
  };

  const handleFeedChange = (index, e) => {
    setIsScanning(false);
    clearInterval(scanIntervalId);
    let currentDevices = [...devices];
    currentDevices[index].feed = e.target.value;
    setDevices(currentDevices);
  };

  const handleSave = (e) => {
    console.log('Saved devices');
    ws.send(JSON.stringify(devices));
    // TODO send devices object to ESP
  };

  return (
    <div className="App mt-5">
      <Container>
        <h1>Configure Your Devices</h1>
        <Row>
          <Col md='6' className='mt-5'>
            <h3>WiFi Settings</h3>
            <Button
              variant='secondary'
              disabled={isWifiScanning}
              onClick={isWifiScanning ? null : handleWifiScan}
              className='mb-3'
            >
              {isWifiScanning ? <Spinner animation="border" size="sm" as="span" /> : null}
              {isWifiScanning ? ' Scanning...' : 'Scan for Networks'}
            </Button>
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
              variant='secondary'
              disabled={isScanning}
              onClick={isScanning ? null : handleScan}
            >
              {isScanning ? <Spinner animation="border" size="sm" as="span" /> : null}
              {isScanning ? ' Scanning...' : 'Scan for Devices'}
            </Button>
            <DevicesAccordion 
              devices={devices}
              onFeedChange={handleFeedChange}
            />
            <Button
              variant='primary'
              onClick={handleSave}
              className='mt-3'
            >Save</Button>
          </Col>
        </Row>

      </Container>
    </div>
  );
}

export default App;
