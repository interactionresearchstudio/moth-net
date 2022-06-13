import { Form, Button } from 'react-bootstrap';

function NetworksForm(props) {
    return(
        <Form>
            <fieldset disabled={props.disabled}>
                <Form.Group className='mb-3'>
                    <Form.Label htmlFor='ssid'>Network Name</Form.Label>
                    <Form.Select onChange={props.onNetworkChange} id='ssid' placeholder='Select Network'>
                        {props.networks.map((network, index) => 
                            <option key={index}>{network}</option>
                        )}
                    </Form.Select>
                </Form.Group>
                <Form.Group className='mb-3'>
                    <Form.Label htmlFor='pass'>Password</Form.Label>
                    <Form.Control onChange={props.onPasswordChange} id='pass' type='password'/>
                </Form.Group>
                {props.connected ? <Button variant='success' disabled>Connected</Button> : <Button type='submit' onClick={props.onSubmit}>Connect</Button>}
            </fieldset>
        </Form>
    );
}

export default NetworksForm;