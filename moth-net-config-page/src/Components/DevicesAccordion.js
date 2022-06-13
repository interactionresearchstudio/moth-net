import { Accordion, Form } from "react-bootstrap";

function DevicesAccordion(props) {
    return(
        <Accordion className="mt-3">
            {props.devices.map((device, index) => 
                <Accordion.Item eventKey={index} key={index}>
                    <Accordion.Header><span>Device {index+1}: {device.name}</span> / <span className="text-end">{device.type}</span></Accordion.Header>
                    <Accordion.Body>
                        <Form.Group className="mb-3">
                            <Form.Label htmlFor='aioFeed'>Adafruit IO feed</Form.Label>
                            <Form.Control id='aioFeed' placeholder='Feed name'/>
                        </Form.Group>
                    </Accordion.Body>
                </Accordion.Item>
            )}
        </Accordion>
    );
}

export default DevicesAccordion;