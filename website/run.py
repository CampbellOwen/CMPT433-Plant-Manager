from flask import Flask, render_template, jsonify
from flask_cors import CORS
import threading
import time
from socket import *

clientSocket = socket(AF_INET, SOCK_DGRAM)
clientSocket.settimeout(3)



app = Flask( __name__,
            static_folder = "./dist/static",
            template_folder = "./dist" )
cors = CORS( app, resources={ r"/api/*": { "origins": "*" } } )

devices = { 
            1234: { 
                "status": "Offline" 
            }, 
            0: {
                "status": "Offline" 
            }, 
            12345: {
                "status":"Offline" 
            }, 
            12346: {
                "status":"Offline" 
            },
            12347: {
                "status":"Offline" 
            } 
        }

def update_devices():
    global devices
    devices[ 1234 ]["status"] = "Online" if devices[1234]["status"] == "Offline" else "Offline"

    addr = ("192.168.86.45", 12345)
    message = b"Apasdf"
    clientSocket.sendto(message, addr)
    try:
        data, server = clientSocket.recvfrom(1024)
        print('%s %d' % (data, elapsed))
    except timeout:
        print('REQUEST TIMED OUT')

def setInterval( val ):
  value = val[ 0 ]
  t = threading.Timer( value, setInterval, [ [ value ] ] )
  t.start()
  update_devices()

setInterval( [ 5 ] )

@app.route( '/' )
def index():
    return render_template( "index.html" )

@app.route( '/api/devices' )
def getDevices():
    global devices
    return jsonify( devices )

