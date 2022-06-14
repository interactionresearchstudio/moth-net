import { Form, Button } from 'react-bootstrap';

function AIOForm(props) {
    return(
        <Form>
            <fieldset disabled={props.disabled}>
                <Form.Group className='mb-3'>
                    <Form.Label htmlFor='aioUsername'>Adafruit IO username</Form.Label>
                    <Form.Control 
                        onChange={props.onFormChange} 
                        id='aioUsername' 
                        placeholder='Your Adafruit IO username' 
                        value={props.username}
                    />
                </Form.Group>
                <Form.Group className='mb-3'>
                    <Form.Label htmlFor='aioKey'>Adafruit IO key</Form.Label>
                    <Form.Control
                        onChange={props.onFormChange} 
                        id='aioKey' 
                        placeholder='Your Adafruit IO key'
                        value={props.aioKey}
                    />
                </Form.Group>
                {props.connected ? <Button variant='success' disabled>Connected</Button> : <Button type='submit' onClick={props.onSubmit}>Connect</Button>}
            </fieldset>
        </Form>
    );
}

export default AIOForm;