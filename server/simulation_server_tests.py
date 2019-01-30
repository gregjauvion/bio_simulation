
from simulation_client import SimulationClient
import threading
import time

###
# Parameters of the tests
###

client = SimulationClient("www.turingbio.tech")

# A model working with some settings
model = {"division_trigger": "V>V_birth+1+N(0,0.1)", "reactions": [["A", "B", "k * A"], ["B", "A", "kk * B"], ["", "A", "synth_A * V"]], "name": "M2", "parameters": [{"name": "k", "value": "3.0"}, {"name": "kk", "value": "3.5"}, {"name": "synth_A", "value": "100.0"}], "notes": "A second test", "molecules_number": {"A": 3, "": 1, "B": 2}, "growth": {"type": "exp", "parameter": "1.0"}, "parameters_number": {"kk": 1, "k": 1, "synth_A": 1, "V_b": 0}, "volume_share": "0.5", "molecules": [{"name": "A", "split": 0}, {"name": "B", "split": 0}]}
settings = {"stop_condition": {"nb_timesteps": 1000, "nb_generations": 10, "simulation_time": 60, "time": 100000}, "simulation": {"burn_duration": 0, "nb_lineages": 10, "volume_initialization": 1, "quantities_initialization": [1, 1], "mother_machine": True, "zero_during_burn": [False, False], "stochastic": True, "nb_lineages_sample": 10}}

# A model for which the simulation fails
model_fails = {"division_trigger": "XXX", "reactions": [["A", "B", "k * A"], ["B", "A", "kk * B"], ["", "A", "synth_A * V"]], "name": "M2", "parameters": [{"name": "k", "value": "XXX"}, {"name": "kk", "value": "3.5"}, {"name": "synth_A", "value": "100.0"}], "notes": "A second test", "molecules_number": {"A": 3, "": 1, "B": 2}, "growth": {"type": "exp", "parameter": "1.0"}, "parameters_number": {"kk": 1, "k": 1, "synth_A": 1, "V_b": 0}, "volume_share": "0.5", "molecules": [{"name": "A", "split": 0}, {"name": "B", "split": 0}]}


###
# The tests
###

def simulation():
	"""Try to run the simulation on the working model and the not working model
	"""
	output = client.simulate(model, settings)
	print "Success for working model : ", output["success"]
	output = client.simulate(model_fails, settings)
	print "Success for non-working model : ", output["success"]

def simul(model, settings):
	"""Function to send a simulation, used to multi-thread them
	"""
	print "sending simulation"
	print "simulation result : ", client.simulate(model, settings)["success"]
	return

def simultaneous_simulation():
	"""Send simultaneously 10 simulations to the server
	"""
	for i in range(5):
		threading.Thread(target=simul, args=(model, settings), kwargs={}).start()
		threading.Thread(target=simul, args=(model_fails, settings), kwargs={}).start()

def lots_of_simulations(nb_simulations):
	"""Send a lot of simulations to the server.
	"""
	for i in range(nb_simulations):
		simul(model, settings)
		simul(model_fails, settings)


simulation()

simultaneous_simulation()

lots_of_simulations(15)
