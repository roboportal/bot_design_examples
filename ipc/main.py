import asyncio
import json
import zmq
from zmq.asyncio import Context

ctx = Context.instance()


async def receive():
    print("Receiving")
    socket = ctx.socket(zmq.ROUTER)
    socket.bind("tcp://*:5556")

    while True:
        req = await socket.recv_multipart()
        print(f"Robot received {req[1]}")


async def send():
    print("Sending")

    deal = ctx.socket(zmq.DEALER)
    deal.connect("tcp://localhost:5555")

    while True:
        telemetry = {
            "id": 0,
            "lat": 1,
            "lng": 1,
            "headingAngle": 180,
            "battery": {
                "min": 0,
                "max": 100,
                "value": 50,
                "uom": "%",
                "charging": False
            }
        }

        await deal.send_multipart([json.dumps(telemetry).encode('utf-8')])
        await asyncio.sleep(1)

asyncio.run(
    asyncio.wait(
        [
            receive(),
            send(),
        ]
    )
)
