from flask import Flask, render_template, jsonify
from flask_cors import CORS

app = Flask( __name__,
            static_folder = "./dist/static",
            template_folder = "./dist" )
cors = CORS( app, resources={ r"/api/*": { "origins": "*" } } )

@app.route( '/' )
def index():
    return render_template( "index.html" )

@app.route( '/api/devices' )
def getDevices():
    return jsonify({ 1234: { "status": "alive" }, 0: {"status": "alive" }, 12345: {"status":"alive" } } )
