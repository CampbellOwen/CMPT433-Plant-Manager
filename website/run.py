from flask import Flask, render_template, jsonify
from flask_cors import CORS
import sqlite3


sql_query = "SELECT d.id, d.status, t.value, h.value, m.value from devices d LEFT JOIN (( ( SELECT * FROM humidity h1 JOIN ( SELECT id, MAX(time) maxTime FROM humidity GROUP BY id) h2 ON h1.id = h2.id AND h1.time = h2.maxTime ) h JOIN ( SELECT * FROM temperature t1 JOIN ( SELECT id, MAX(time) maxTime FROM temperature GROUP BY id) t2 ON t1.id = t2.id AND t1.time = t2.maxTime ) t ON h.id = t.id) JOIN ( SELECT * FROM moisture m1 JOIN ( SELECT id, MAX(time) maxTime FROM moisture GROUP BY id) m2 ON m1.id = m2.id AND m1.time = m2.maxTime ) m ON t.id = m.id ) info ON d.id = info.id ; "


sql_query_easy = "SELECT * from moisture"

app = Flask( __name__,
            static_folder = "./dist/static",
            template_folder = "./dist" )
cors = CORS( app, resources={ r"/api/*": { "origins": "*" } } )

def update_devices():
    global devices
    devices[ 1234 ]["status"] = "Online" if devices[1234]["status"] == "Offline" else "Offline"

@app.route( '/' )
def index():
    return render_template( "index.html" )

@app.route( '/api/devices' )
def getDevices():
    devices = {}
    with sqlite3.connect( '/root/plants.db', check_same_thread=False ) as conn:
        r = conn.execute( sql_query )
        for row in r:
            filtered = [ elem if not elem is None else 0 for elem in list(row) ]
            devices[ int( filtered[ 0 ] ) ] = { "status": str( filtered[ 1 ] ), "temperature": int(filtered[ 2 ] ), "humidity": int( filtered[ 3 ] ), "moisture": int( filtered[ 4 ] ) }

    print( devices )
    

    return jsonify( devices )

