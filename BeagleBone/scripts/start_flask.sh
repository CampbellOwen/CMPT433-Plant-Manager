#! /bin/bash
source backend/venv/bin/activate
FLASK_APP=run.py FLASK_DEBUG=0 flask run --host 0.0.0.0
