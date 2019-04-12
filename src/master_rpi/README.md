# Mesh REST-API

## Usage

This REST-API can be reached by `http://[device_ip]:8080/api/v1/`.
All the routes documented here are based on this address.


#### Responses  
All response will have the format

````json
{
  "request_id": "[internally used request_id for this request]",
  "some type of value": "[value]"
}
````

#### Errors
if a incorrect response (`400 Bad Request`) is sent or if there are internal problems 
the response will come in the format
````json
{
  "error": {
    "message": "[short explanation of the error]",
    "request_id": "[internally used request_id for this request]",
    "...":"..."
  }
}
````


### List all Nodes
**Definition**   
`GET /nodes`

Returns all nodes which are registered in the network  

**Responses**
- `200 OK`
````json
{"nodes":
    [
      {
        "node": 1
      },
      {
        "node": 6
      },
      {
        "..." : "..."
      }      
    ]
}
````

### Battery Value
**Definition**  
`GET /nodes/<node>/battery`

**Responses**
- `200 OK`
````json
{
  "request_id": "[requestID]",
  "battery": "[Battery in %]"
}
````


### Sensor Value
**Definition**  
`GET /nodes/<node>/sensors`  
Optional: If more than one sensor on one device:  
`GET /nodes/<node>/sensors/<index>`  

**Responses**
- `200 OK`
````json
{
  "request_id": "[requestID]",
  "value": "[Value]"
}
````


### Valve Control (ON/OFF)
**Definition**   
**_ON_**:  
`POST /nodes/<node>/valves/<index>/ON`   
**_OFF_**:  
`POST /nodes/<node>/valves/<index>/OFF`  

**Responses**
- `200 OK`
````json
{
  "request_id": "[requestID]",
  "message": "OK"
}
````


### Valve State
**Definition**   
`GET /nodes/<node>/valve/<index>`  

**Responses**
- `200 OK`
````json
{
  "request_id": "[requestID]",
  "state": "[state: ON/OFF]"
}
````


### Subscribe to Registrations
**Definition**  
`POST /subscriptions/registrations`  

**Arguments**
- `"callback_url":string"` the url where the callbacks should be send to

**Response**
- `201 Created`

## Typical Error Responses
Here are the typical error responses described which can appear at each 
request.

#### "The given node is not valid"
The server/mesh-master checked if the given node registered over DHCP 
in this network. This error message occurs if this node was not found 
in the lookup.

**Example**
````json
{
    "error": {
        "internalMessage": "The given node is not valid #100",
        "request_id": 100
    }
}
````

#### "Could not write the given payload. [...]"
The server/mesh-master could not send this payload. 
That may be because the node is currently not listening or unavailable.
Also the payload which should be send could be incorrect


**Example**
````json
{
    "error": {
        "internalMessage": "Couldn't write the given payload. The node may be not available #100",
        "request_id": 100
    }
}
````

#### "Could not get the response [...]"
The payload may have been send correctly, but for some reason the expected 
response did not come. This response usually comes after a specific time, because the
server waited for the incoming.


**Example**
````json
{
    "error": {
        "internalMessage": "Could not get the response #100",
        "request_id": 100
    }
}
````
