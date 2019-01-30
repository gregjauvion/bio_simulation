
import ctypes
import json


class SimulationApi():
	"""This class is the simulation API. It enables to call all functions defined in interface.cpp in python.
	path is the full path to the cpp library.
	"""
	def __init__(self, path):
		self.lib = ctypes.CDLL(path)
		self.lib.simulate.restype = ctypes.c_char_p

	def simulate(self, model, settings):
		""" Call the simulate(.,.) method of interface.cpp (to run a simulation).
		- model is a json describing the model
		- settings is a json formed the following way : {"simulation": {}, "stop_condition": {}}
		"""
		return json.loads(self.lib.simulate(json.dumps(model), json.dumps(settings)))

	@staticmethod
	def get_lineages(outputs):
		""" outputs is a json returned by the function simulate.
		This function reads it and returns a list whose each element contains the cell states ([t]) for this lineage.
		Assumes that we have the mother machine setting.
		"""
		lineages = []
		for l in outputs["lineages"]:
			lineage = []
			for c in l:
				for d in outputs["cell_cycles"][c]:
					lineage.append(outputs["data"][d])
			lineages.append(lineage)
		return lineages


###
# Tests
###
if __name__=="__main__":
	api = SimulationApi("../code/build/src/libsim-engine.1.0.0.dylib")
	# Define a model and some settings
	model = {"molecules":[{"name":"A","split":1},{"name":"B","split":1},{"name":"C","split":1}],"parameters":[{"name":"synth_A_max","value":"22.3"},{"name":"synth_B_max","value":"25.2"},{"name":"synth_C_max","value":"3.2"},{"name":"A_conc_rep","value":"3.4"},{"name":"B_conc_rep","value":"12.3"},{"name":"C_conc_rep","value":"4.1"},{"name":"mu","value":"0.02"},{"name":"a","value":"1"},{"name":"b","value":"0.1"},{"name":"split","value":"0.4"}],"reactions":[["","A","synth_A_max / ( 1 + [C] / C_conc_rep )"],["","B","synth_B_max / ( 1 + [A] / A_conc_rep )"],["","C","synth_C_max / ( 1 + [B] / B_conc_rep )"]],"division_trigger":"V>a+b","volume_share":"split","growth":{"type": "exp", "parameter": "mu"}}
	settings = {"simulation": {"nb_lineages": 3, "nb_lineages_sample": 3, "mother_machine": True, "stochastic": True, "volume_initialization": 1, "quantities_initialization": [1,1,1], "burn_duration": 12, "zero_during_burn": [False,False,False]}, "stop_condition": {"time": 100000, "simulation_time": 60, "nb_timesteps": 3, "nb_generations": 10}}
	output = api.simulate(model, settings)
	print output
	# Build the lineages from the output
	output = json.loads('{"cell_cycles":[[0,3],[1,4],[2,5],[6,9,12,15,18,21,24,27,30,33,36,39,42,45,48,51,54,57],[7,10,13,16,19,22,25,28,31,34,37,40,43,46,49,52,55,58],[8,11,14,17,20,23,26,29,32,35,38,41,44,47,50,53,56,59]],"data":[[0.0,1,1,1],[0.0,1,1,1],[0.0,1,1,1],[0.005,1.01005016708417,1,1],[0.005,1.01005016708417,1,1],[0.005,1.01005016708417,1,2],[0.01,0.510100670013378,1,1],[0.01,0.510100670013378,0.0,1],[0.01,0.510100670013378,0.0,1],[0.015,0.515227266976758,1,1],[0.015,0.515227266976758,0.0,1],[0.015,0.515227266976758,0.0,1],[0.02,0.520405387096194,1,1],[0.02,0.520405387096194,2,1],[0.02,0.520405387096194,0.0,1],[0.025,0.525635548188012,1,1],[0.025,0.525635548188012,3,1],[0.025,0.525635548188012,0.0,1],[0.03,0.530918273272679,1,1],[0.03,0.530918273272679,3,1],[0.03,0.530918273272679,0.0,1],[0.035,0.536254090627108,1,1],[0.035,0.536254090627108,3,1],[0.035,0.536254090627108,0.0,1],[0.04,0.541643533837479,4,1],[0.04,0.541643533837479,5,3],[0.04,0.541643533837479,1,2],[0.045,0.547087141852605,5,1],[0.045,0.547087141852605,6,3],[0.045,0.547087141852605,2,2],[0.05,0.552585459037823,6,2],[0.05,0.552585459037823,6,4],[0.05,0.552585459037823,3,3],[0.055,0.558139035229435,6,2],[0.055,0.558139035229435,7,4],[0.055,0.558139035229435,3,3],[0.06,0.563748425789687,6,2],[0.06,0.563748425789687,7,5],[0.06,0.563748425789687,3,3],[0.065,0.56941419166231,6,3],[0.065,0.56941419166231,8,5],[0.065,0.56941419166231,2,4],[0.07,0.575136899428613,6,3],[0.07,0.575136899428613,7,5],[0.07,0.575136899428613,2,4],[0.075,0.580917121364141,7,3],[0.075,0.580917121364141,8,5],[0.075,0.580917121364141,3,4],[0.08,0.586755435495904,8,4],[0.08,0.586755435495904,8,6],[0.08,0.586755435495904,5,4],[0.085,0.592652425660182,8,4],[0.085,0.592652425660182,9,6],[0.085,0.592652425660182,6,4],[0.09,0.598608681560904,9,4],[0.09,0.598608681560904,9,6],[0.09,0.598608681560904,7,4],[0.095,0.604624798828625,10,6],[0.095,0.604624798828625,9,7],[0.095,0.604624798828625,8,4]],"lineages":[[0,3],[1,4],[2,5]],"structure":["t","V","q_0","q_1"]}')
	lineages = api.get_lineages(output)
	print lineages

