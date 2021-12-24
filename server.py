from flask import Flask, request
from flask_cors import CORS
from flask_restful import reqparse

app = Flask(__name__)
CORS(app)

temp_data = []
humi_data = []

# add a humidity log 
@app.route('/api/humidity', methods=['POST'])
def add_humidity():
	parser = reqparse.RequestParser()
	parser.add_argument('sensor_id', type=str, required=True)
	parser.add_argument('time', type=str, required=True, help='Rate cannot be converted')
	parser.add_argument('value', type=int, required=True)
	args = parser.parse_args()
	humi_data.append(args)
	return '', 204

# get all humidity data
@app.route('/api/humidity', methods=['GET'])
def get_humidity():
	return { 'data': humi_data }, 200

# add a temperature log
@app.route('/api/temperature', methods=['POST'])
def add_temperature():
	parser = reqparse.RequestParser()
	parser.add_argument('sensor_id', type=str, required=True)
	parser.add_argument('time', type=str, required=True, help='Rate cannot be converted')
	parser.add_argument('value', type=int, required=True)
	args = parser.parse_args()
	temp_data.append(args)
	return '', 204

# get all temperature data
@app.route('/api/temperature', methods=['GET'])
def get_temperature():
	return { 'data': temp_data }, 200

if __name__ == '__main__':
	app.run(host='0.0.0.0')  # run our Flask app, make available to network