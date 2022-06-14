import { Accordion, Form } from "react-bootstrap";
import { Plug, PlugFill } from 'react-bootstrap-icons';

function DevicesAccordion(props) {
    if (props.devices.length > 0)
    return(
        <Accordion className="mt-3">
            {props.devices.length ? <></> : <p>No devices</p>}
            {props.devices.map((device, index) => 
                <Accordion.Item eventKey={index} key={index}>
                    <Accordion.Header>
                        {device.connected ? <PlugFill/> : <Plug/>}
                        <span className="ml-1">Device {index+1}: {device.name} / {device.sensorType}</span>
                    </Accordion.Header>
                    <Accordion.Body>
                        <Form.Group className="mb-3">
                            <Form.Label htmlFor={'aioFeed' + index}>Adafruit IO feed</Form.Label>
                            <Form.Control 
                                onChange={event => props.onFeedChange(index, event)} 
                                key={index} 
                                id={'aioFeed' + index} 
                                placeholder='Feed name'
                                value={device.feed}
                            />
                        </Form.Group>
                    </Accordion.Body>
                </Accordion.Item>
            )}
        </Accordion>
    );
    else return(<p className="mt-3">No devices found.</p>)
}

export default DevicesAccordion;