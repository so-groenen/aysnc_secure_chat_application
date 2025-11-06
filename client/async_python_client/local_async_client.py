import sys
import asyncio
from prompt_toolkit import PromptSession
from prompt_toolkit.patch_stdout import patch_stdout
 

MAX_DATA_RECIEVE = 1024
IS_RUNNING       = True
OTHER_NAME       = "SERVER"
ENDING_CHAR      = '\r\n'


async def connect(ip, port) -> tuple[asyncio.StreamReader, asyncio.StreamWriter]:
    reader, writer = await asyncio.open_connection(ip, port)
    return reader, writer

async def handle_output(writer: asyncio.StreamWriter):
    global IS_RUNNING
    session = PromptSession()
    while IS_RUNNING:
        with patch_stdout():  
            try:
                msg: str = await session.prompt_async("msg: ")
                msg += ENDING_CHAR
                writer.write(msg.encode())
                await writer.drain()
            except Exception as e:
                print("connection closed")
                IS_RUNNING = False

 
async def handle_input(reader: asyncio.StreamReader) -> None:
    global IS_RUNNING
    while IS_RUNNING:
        try:
            data = await reader.read(MAX_DATA_RECIEVE)
            data = data.decode("utf-8")
            if data:
                print(f"{OTHER_NAME}: {data}")
        except Exception as e:
            print("connection closed")
            IS_RUNNING = False
            sys.exit(1)
        

    
async def main(ip, port):
    reader, writer = await connect(ip, port)
    return await asyncio.gather( handle_input(reader), handle_output(writer) )

DEFAULT_PORT = 6970
if __name__ == "__main__":

    port = DEFAULT_PORT
    if len(sys.argv) > 1:
        port = int(sys.argv[1])
        
    print("Local client start")
    print(f"listening on port {port}")
    
    asyncio.run(main(ip="127.0.0.1", port=port))