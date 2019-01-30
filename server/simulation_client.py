
import json
import urllib, urllib2

HOST = 'localhost'
PORT = 8081

class SimulationClient():

	def __init__(self, host=HOST, port=PORT):
		"""Build the url to request the server.
		"""
		self.url = 'http://{h}:{p}'.format(h=host, p=str(port))

	def post(self, request, values=None):
		"""Sends a POST request to the server and returns the response.
		- request : the request to send
		- values : a dictionary with the data to send
		"""
		req = urllib2.Request(self.url + '/' + request, urllib.urlencode(values if values else {}))
		# The response is formed the following way : header which finished with \n, and then the response
		response = urllib2.urlopen(req).read()
		output = response.split('\n')[1]
		return json.loads(output)

	def simulate(self, model, settings):
		"""Run a simulation.
		model and settings are json objects.
		"""
		return self.post('simulation', values={'model': json.dumps(model), 'settings': json.dumps(settings)})

	def reload(self, lib_path):
		"""Reload the simulation api.
		"""
		return self.post('reload', values={'lib_path': json.dumps(lib_path)})


###
# In this test, we run a few simulations at the same time, to check that the server can handle them.
# - Note : to run this test, there must be a running server!
###
if __name__ == "__main__":
	client = SimulationClient()
	# Note : the model has a growth rate equal to 0 to be able to control the simulation time in the settings
	model = {"molecules":[{"name":"A","split":1},{"name":"B","split":1},{"name":"C","split":1}],"parameters":[{"name":"synth_A_max","value":"22.3"},{"name":"synth_B_max","value":"25.2"},{"name":"synth_C_max","value":"3.2"},{"name":"A_conc_rep","value":"3.4"},{"name":"B_conc_rep","value":"12.3"},{"name":"C_conc_rep","value":"4.1"},{"name":"mu","value":"0.02"},{"name":"a","value":"1"},{"name":"b","value":"0.1"},{"name":"split","value":"0.4"}],"reactions":[["","A","synth_A_max / ( 1 + [C] / C_conc_rep )"],["","B","synth_B_max / ( 1 + [A] / A_conc_rep )"],["","C","synth_C_max / ( 1 + [B] / B_conc_rep )"]],"division_trigger":"V>a+b","volume_share":"split","growth":{"type": "exp", "parameter": "mu"}}
	settings = {"simulation": {"nb_lineages": 100, "nb_lineages_sample": 3, "mother_machine": True, "stochastic": True, "volume_initialization": 1, "quantities_initialization": [1,1,1], "burn_duration": 12, "zero_during_burn": [False,False,False]}, "stop_condition": {"time": 100000, "simulation_time": 60, "nb_timesteps": 10, "nb_generations": 10}}
	# We run 6 simulations : some which succeed and some which crash
	client.simulate(model, settings)
	client.simulate("modele foireux", "settings foireux")
	client.simulate(model, settings)
	client.simulate(model, settings)
	print 'reloading API'
	client.reload('libsim-engine.1.0.0.dylib')
	client.simulate("modele foireux", "settings foireux")
	print client.simulate(model, settings)["success"]
	client.simulate(model, settings)
	print client.simulate("modele foireux", "settings foireux")["success"]
