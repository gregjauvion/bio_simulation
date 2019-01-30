
/*
 * A Model represents a model which is built from the interface.
 */
class Model {

	/* Constructor */
	constructor() {
		// Name of the model
		this.name = "";
        // Some user-defined notes relative to this model
        this.notes = "";
		/* The list of reactions, molecules and parameters for this model. */
		this.reactions = [];
		this.molecules = [];
		this.parameters = [];
		/* Contains the number of reactions each molecule appears in, and the number of formulas each parameter appears in.
		It is needed to manage the auto-add/auto-delete of parameters. */
        this.molecules_number = {};
        this.parameters_number = {};
        /* Object representing the growth, exp by default */
        this.growth = {type: "exp", parameter: ""};
        /* The expressions reprenting the division trigger and the volume share when division. */
        this.division_trigger = "";
        this.volume_share = "";
        /* The settings */
        this.settings = new Settings();
	}

	/*
	 * Utilities methods needed.
	 */
    // Returns the molecules from a formula of type "A + B + C"
    getMolecules(s) { return s.replace(/ /g,"").split("+"); }
    // Returns the list of parameters from a user-defined formula
    getParameters(func) {
        if (func==null) return [];
        // Split with + - * / ^ ( ) > < ,
        var params = func.replace(/ /g,"").split(/[\+\-\*/^()><,]/);
        var molecules_names = this.molecules.map(function(m) {return m.name;});
        // Return all parts of the function which can not be parsed as floats and which are not reserved characters (N, V, V_birth, [*]) and which are not a molecule
        return params.filter(
            function(p) {return Model.reserved_characters.indexOf(p)==-1 
                && p.match(/\[.*\]/)==null
                && molecules_names.indexOf(p)==-1
                && isNaN(parseFloat(p));}
        );
    }

    /*
     * Methods used when updating the model.
     */
    // Update the parameters when adding an expression
    addExpression(expr) {
    	var parameters_to_add = this.getParameters(expr);
    	for (var i=0; i<parameters_to_add.length; i++) {
    		var param = parameters_to_add[i];
            if (typeof this.parameters.find(function(p) {return p.name==param}) == "undefined" && param!="") {
                this.parameters.push({name: param, value: undefined});
            }
            // Update dictionary
            if (!(param in this.parameters_number)) this.parameters_number[param] = 0;
            this.parameters_number[param] += 1;
    	}
    }
    // Update parameters when removing an expression
    removeExpression(expr) {
    	var parameters_to_remove = this.getParameters(expr);
    	for (var i=0; i<parameters_to_remove.length; i++) {
    		var param = parameters_to_remove[i];
            this.parameters_number[param] -= 1;
            var params = this.parameters.map(function(p) {return p.name;});
            if (this.parameters_number[param]==0) this.parameters.splice(params.indexOf(param),1);
		}
    }
    // Adds a reaction
    addReaction(left, right, rate){
        this.reactions.push([left, right, rate]);

        // Add the molecules which appear in the reaction
        var molecules_to_add = this.getMolecules(left).concat(this.getMolecules(right));
        for (var i=0; i<molecules_to_add.length; i++) {
        	var mol = molecules_to_add[i];
        	// Add in array
            if (typeof this.molecules.find(function(m) {return m.name==mol}) == "undefined" && mol!="") {
                this.molecules.push({name: mol, split: 0});
                this.settings.simulation.quantities_initialization.push("1");
                this.settings.simulation.is_quantity_init_conc.push(true);
                this.settings.simulation.zero_during_burn.push(false);
            }
            // Update dictionary
            if (!(mol in this.molecules_number)) this.molecules_number[mol] = 0;
            this.molecules_number[mol] += 1;
        }

        // Add the parameters which appear in the rate
        this.addExpression(rate);
    }
    // Deletes a reaction
    deleteReaction(r) {
    	// Deletes from reactions array
        var ind = -1;
        for (var i=0; i<this.reactions.length; i++)
            if (this.reactions[i][0]==r[0] && this.reactions[i][1]==r[1] && this.reactions[i][2]==r[2])
                ind = i;
    	if (ind==-1) {
    		console.log("The reaction should be find in the array.");
    	} else {
    		this.reactions.splice(ind,1);
    	}

    	// Update molecules
    	var molecules_to_delete = this.getMolecules(r[0]).concat(this.getMolecules(r[1]));
    	for (var i=0; i<molecules_to_delete.length; i++) {
    		var mol = molecules_to_delete[i];
			this.molecules_number[mol] -= 1;
			var mols = this.molecules.map(function(m) {return m.name;});
			if (this.molecules_number[mol]==0) 
            {
                this.molecules.splice(mols.indexOf(mol),1);
                this.settings.simulation.quantities_initialization.splice(mols.indexOf(mol),1);
                this.settings.simulation.is_quantity_init_conc.splice(mols.indexOf(mol),1);
                this.settings.simulation.zero_during_burn.splice(mols.indexOf(mol),1);
            }
    	}

    	// Update parameters
        this.removeExpression(r[2]);
    }

    /*
     * Returns the model as a json
     */
    toJson() {
        return JSON.stringify({
            name: this.name, notes: this.notes, molecules: this.molecules, parameters: this.parameters, reactions: this.reactions,
            growth: this.growth, division_trigger: this.division_trigger, volume_share: this.volume_share,
            molecules_number: this.molecules_number, parameters_number: this.parameters_number
        });
    }

    /*
     * Reads a model from a json
     */
     fromJson(json) {
        this.name = json.name;
        this.notes = json.notes;
        this.reactions = json.reactions;
        this.molecules = json.molecules;
        this.molecules_number = json.molecules_number;
        this.parameters_number = json.parameters_number;
        this.parameters = json.parameters;
        this.growth = json.growth;
        this.division_trigger = json.division_trigger;
        this.volume_share = json.volume_share;
     }

     /*
      * Validate a model, i.e determine if the simulation should run normally.
      * Value associated to the key success is true or false.
      * If it is false, we have also some information about the remaining problems in the model.
      */
      validate() {
        var return_value = {};
        if (this.name=="") return_value.name = false;
        if (this.reactions.length==0) return_value.reactions = false;
        this.parameters.forEach(function(p) {if (p.value===undefined) return_value.parameters=false;});
        if (this.growth.parameter=="") return_value.growth_parameter = false;
        if (this.division_trigger=="") return_value.division_trigger = false;
        if (this.volume_share=="") return_value.volume_share = false;
        return_value.valid = Object.keys(return_value).length>0 ? false : true;
        return return_value;
      }
}

/*
 * Settings represents the settings of the simulation, built from the interface.
 */
class Settings {
    constructor() {
        /* this.simulation defines the settings needed to run a simulation. Some of the parameters are hardcoded here, others are set from the interface. */
        this.simulation = {
            // These parameters are hardcoded here
            "nb_lineages": 10,
            "nb_lineages_sample": 10,
            "volume_initialization": "1",
            // These parameters are set in the interface
            "mother_machine": true,
            "stochastic": true,
            "burn_duration": 0,
            "quantities_initialization": [],
            "is_quantity_init_conc": [],
            "zero_during_burn": []
        }
        /* this.stop_condition defines when to stop the simulation. It is completely hardcoded here. */
        this.stop_condition = {"time": 100000, "simulation_time": 60, "nb_timesteps": 1000, "nb_generations": 10};
    }

    toJson() {
        var stringified = JSON.stringify({
            simulation: this.simulation,
            stop_condition: this.stop_condition
        });
        return stringified;
    }

    fromJson(json) {
        this.simulation = json.simulation;
        this.stop_condition = json.stop_condition;
     }
}

/* Static variables */
Model.reserved_characters = ["", "U", "N", "Npos", "LN", "NB", "V", "V_birth"]

/* Copy a model */
function copyModel(model) {
    copy = new Model();
    copy.name = model.name;
    copy.notes = model.notes;
    copy.division_trigger = model.division_trigger;
    copy.volume_share = model.volume_share;
    // Copy reactions, molecules, parameters
    copy.reactions = []
    copy.molecules = [];
    copy.parameters = [];
    model.reactions.forEach(function(r) {copy.reactions.push([r[0], r[1], r[2]]);});
    model.molecules.forEach(function(r) {copy.molecules.push({name:r.name,split: r.split});});
    model.parameters.forEach(function(r) {copy.parameters.push({name:r.name, value:r.value});});
    // Copy growth
    copy.growth = {type:model.growth.type, parameter:model.growth.parameter};
    // Copy the maps
    copy.molecules_number = {};
    copy.parameters_number = {};
    for (var p in model.molecules_number) { copy.molecules_number[p] = model.molecules_number[p]; }
    for (var p in model.parameters_number) { copy.parameters_number[p] = model.parameters_number[p]; }
    // Copy the settings
    copy.settings = new Settings();
    copy.settings.simulation = {
            "nb_lineages": model.settings.simulation.nb_lineages,
            "nb_lineages_sample": model.settings.simulation.nb_lineages_sample,
            "volume_initialization": model.settings.simulation.volume_initialization,
            "mother_machine": model.settings.simulation.mother_machine,
            "stochastic": model.settings.simulation.stochastic,
            "burn_duration": model.settings.simulation.burn_duration,
            "quantities_initialization": [],
            "is_quantity_init_conc": [],
            "zero_during_burn": []
    };
    model.settings.simulation.quantities_initialization.forEach(function(r) {copy.settings.simulation.quantities_initialization.push(r);});
    model.settings.simulation.is_quantity_init_conc.forEach(function(r) {copy.settings.simulation.is_quantity_init_conc.push(r);});
    model.settings.simulation.zero_during_burn.forEach(function(r) {copy.settings.simulation.zero_during_burn.push(r);});
    copy.settings.stop_condition = {
        "time": model.settings.stop_condition.time,
        "simulation_time": model.settings.stop_condition.simulation_time,
        "nb_timesteps": model.settings.stop_condition.nb_timesteps,
        "nb_generations": model.settings.stop_condition.nb_generations
    };
    return copy;
}


/* 
 * Some tests of the Model and Settings class. If you want to run it, execute the function in a javascript console 
 */
function testModelAndSettings() {
    console.log("--\nTests of Model and Settings classes.\n--");
    // Build a complex model for the tests
    model_json = {"name": "TestModel", "notes": "Hey!", "reactions": [], "molecules": [], "parameters": [],
        "molecules_number": {}, "parameters_number": {},
        "growth": {"type": "exp", "parameter": "mu"}, "division_trigger": "1", "volume_share": 0.5};
    model = new Model();
    model.fromJson(model_json);
    settings = new Settings();
    settings.fromJson(
        {
            "stop_condition": {"nb_timesteps": 3, "nb_generations": 10, "simulation_time": 60, "time": 100000},
            "simulation": {"burn_duration": 0, "volume_initialization": "1", "nb_lineages": 10, "stochastic": true, "quantities_initialization": ["1"], "is_quantity_init_conc": [false], "nb_lineages_sample": 10, "mother_machine": true, "zero_during_burn": [false]}
        }
    );
    model.settings = settings;
    console.log(model);
    model.addReaction("A+B","C+D","mu+sigma");
    model.addReaction("E","F","alpha^2");

    // Copy the model and modify the original model
    model_copy = copyModel(model);
    model.name = "ModifiedName";
    model.reactions = [];
    model.parameters = [];
    model.molecules_number = {};
    model.settings.stop_condition = {};
    // Log the model and its copy
    console.log("Model :\n" + model.toJson());
    console.log("Copy :\n" + model_copy.toJson());
    console.log("-- Tests finished. --");
}
