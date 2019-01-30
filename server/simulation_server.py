
import json
import cgi
from BaseHTTPServer import BaseHTTPRequestHandler, HTTPServer
from simulation_api import SimulationApi
from multiprocessing import Process, Manager
import logging, time, argparse

# The server runs on port 8081
PORT = 8081

logging.basicConfig(filename='server.log', level=logging.INFO, format='%(asctime)s %(message)s')

class SimulationServer(HTTPServer):

    """The HTTP server used to run the simulations and return the results.
    """
    def __init__(self, server_address, request_handler_class, lib_path):
        logging.info("Building SimulationServer.")
        HTTPServer.__init__(self, server_address, request_handler_class)
        # The api to the cpp library, used to run the simulations
        self.api = SimulationApi(path=lib_path)
        # All the simulation outputs, obtained from python processes run at each request, are stored in this shared dictionary
        self.outputs = Manager().dict()
        # Each simulation will be identified by this index, incremented at each request
        self.nb_requests = 0


class SimulationServerHandler(BaseHTTPRequestHandler):

    """The handler used to run the simulation and return the result.
    """
    def do_POST(self):
        # Needs to write this in the request header : the outputs will be chunked
        self.send_header('Transfer-Encoding', 'chunked')
        # Get POST variables
        length = int(self.headers['content-length'])
        postvars = cgi.parse_qs(self.rfile.read(length), keep_blank_values=1)
        # Handle the request and build the response
        response = {}

        # This request performs a simulation
        if self.path=='/simulation':
            # Retrieve the POST variables
            model = json.loads(postvars['model'][0])
            settings = json.loads(postvars['settings'][0])
            name = str(self.server.nb_requests)
            self.server.nb_requests += 1
            logging.info("Simulation {n} to run : {s}".format(n=name, s=json.dumps({"model": model, "settings": settings})))
            # Create a python process running the cpp simulation
            process = Process(target=get_output, args=(self.server.outputs, name, self.server.api, model, settings))
            # Run the process and check every 0.1 second that it is still alive, otherwise either it has succeeded either it has crashed
            process.start()
            while process.is_alive():
                time.sleep(0.1)
            # Check if we have an output
            if name in self.server.outputs:
                response["success"] = True
                response["output"] = self.server.outputs[name]
                del self.server.outputs[name]
                logging.info("simulation {n} succeeded.".format(n=name))
            else:
                response["success"] = False
                logging.info("simulation {n} failed.".format(n=name))

        # This request reloads the simulation api (to use if the library has been updated)
        if self.path=='/reload':
            lib_path = json.loads(postvars['lib_path'][0])
            self.server.api = SimulationApi(path=lib_path)
            response["success"] = True

        # Return the response
        self.wfile.write(json.dumps(response))

def get_output(outputs, key, api, model, settings):
    # Run the simulation
    outputs[key] = api.simulate(model, settings)

def run(lib_path, host, port=PORT):
    """Function used to run the server.
    """
    http = SimulationServer((host, port), SimulationServerHandler, lib_path)
    http.serve_forever()


###
# Run the server
###

if __name__ == "__main__":
    # Define the command-line arguments parser
    parser = argparse.ArgumentParser()
    parser.add_argument("--host", default="localhost")
    parser.add_argument("--lib-path")
    args = parser.parse_args()
    # Run the server
    run(args.lib_path, args.host)
