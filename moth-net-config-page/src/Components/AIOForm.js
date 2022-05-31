import { Form, Button } from 'react-bootstrap';

function AIOForm(props) {
    return(
        <Form>
            <fieldset disabled={props.disabled}>
                <Form.Group className='mb-3'>
                    <Form.Label htmlFor='aioUsername'>Adafruit IO username</Form.Label>
                    <Form.Control id='aioUsername' placeholder='Your Adafruit IO username'/>
                </Form.Group>
                <Form.Group className='mb-3'>
                    <Form.Label htmlFor='aioKey'>Adafruit IO key</Form.Label>
                    <Form.Control id='aioKey' placeholder='Your Adafruit IO key'/>
                </Form.Group>
                {props.connected ? <Button variant='success' disabled>Connected</Button> : <Button type='submit' onClick={props.onSubmit}>Connect</Button>}
            </fieldset>
        </Form>
    );
}

export default AIOForm;