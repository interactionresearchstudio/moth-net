import { Form, Button } from 'react-bootstrap';

function NetworksForm(props) {
    return(
        <Form>
            <fieldset disabled={props.disabled}>
                <Form.Group className='mb-3'>
                    <Form.Label htmlFor='ssid'>Network Name</Form.Label>
                    <Form.Select 
                        type='text'
                        id='ssid' 
                        value={props.ssid}
                        name='ssid'
                        onChange={props.onFormChange} 
                    >
                        <option value=''>Select a network</option>
                        {props.networks.map((network, index) => 
                            <option value={network.SSID} key={index}>{network.SSID}</option>
                        )}
                    </Form.Select>
                </Form.Group>
                <Form.Group className='mb-3'>
                    <Form.Label htmlFor='pass'>Password</Form.Label>
                    <Form.Control
                        onChange={props.onFormChange} 
                        id='pass' 
                        type='password' 
                        value={props.pass}
                    />
                </Form.Group>
                {props.connected ? <Button variant='success' disabled>Connected</Button> : <Button type='submit' onClick={props.onSubmit}>Connect</Button>}
            </fieldset>
        </Form>
    );
}

export default NetworksForm;