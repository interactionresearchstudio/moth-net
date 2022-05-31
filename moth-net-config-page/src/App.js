import { useState } from 'react';
import './App.css';
import { Button, Spinner, Container, Row, Col } from 'react-bootstrap';
import AIOForm from './Components/AIOForm';

function App() {
  const [isAioConnected, setAioConnected] = useState(false);
  const [isScanning, setIsScanning] = useState(false);

  const onAioSubmit = (e) => {
    e.preventDefault();
    console.log('AIO credentials submitted');
    //setAioConnected(true);
  };

  const handleScan = (e) => {
    console.log('Start scan');
    setIsScanning(true);
  };

  return (
    <div className="App mt-5">
      <Container>
        <h1>Configure Your Devices</h1>
        <Row className='mt-5'>
          <Col>
            <h3>Adafruit IO Settings</h3>
            <AIOForm
              onSubmit={onAioSubmit}
              connected={isAioConnected}
            />
          </Col>
          <Col>
            <h3>Devices in your network</h3>
            <Button
              variant='primary'
              disabled={isScanning}
              onClick={isScanning ? null : handleScan}
            >
              {isScanning ? <Spinner animation="border" size="sm" /> : null}
              {isScanning ? ' Scanning...' : 'Scan'}
            </Button>
          
          </Col>
        </Row>

      </Container>
    </div>
  );
}

export default App;
