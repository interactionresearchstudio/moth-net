import { Accordion, Form } from "react-bootstrap";
import { Plug, PlugFill } from 'react-bootstrap-icons';

function DevicesAccordion(props) {
    return(
        <Accordion className="mt-3">
            {props.devices.map((device, index) => 
                <Accordion.Item eventKey={index} key={index}>
                    <Accordion.Header>
                        {device.connected ? <PlugFill/> : <Plug/>}
                        <span className="ml-1">Device {index+1}: {device.name} / {device.sensorType}</span>
                    </Accordion.Header>
                    <Accordion.Body>
                        <Form.Group className="mb-3">
                            <Form.Label htmlFor='aioFeed'>Adafruit IO feed</Form.Label>
                            <Form.Control onChange={props.onFeedChange} key={index} id='aioFeed' placeholder='Feed name'/>
                        </Form.Group>
                    </Accordion.Body>
                </Accordion.Item>
            )}
        </Accordion>
    );
}

export default DevicesAccordion;