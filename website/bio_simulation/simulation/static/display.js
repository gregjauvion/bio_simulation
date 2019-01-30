


// what are the different possible states ?
	// no model, no edit (we start there)
	// no model, editing a first model (clicking results should not be possible)
	// model, description, no edit (clicking results should be possible)
	// model, description, edit (clicking results should not be possible)
	// model, results, not yet received data
	// model, results, simulation has failed
	// model, results, plots

// starting

	// create the Model object
	// non edit mode
	// if existing model -> display its description
	// if no model -> show the text explaining how to create a model, inactivate link to results


// interactive behavior

	// deleting a model
	// duplicating a model
	// displaying results
	// editing a model
	// saving a model


// The model being displayed on the dashboard with its id. 
// By default, the model is empty and the id is undefined
var model = new Model();
var model_id;
var model_id_before_new; // used to reload previous model when canceling a new model
var edit_mode = false;
var model_being_edited = new Model(); // used for editing (original model will be copied into that instance when editing a model)
// A variable to store simulation data
var data;
// a variable to tell us if we are loading a model and are not finished
var is_loading_model = false;
var is_loading_data = false;
var current_timeout;


// important functions: loadModel, createModel, displayModel, displayResults, displayModelEditMode...
// and the functions generating html from the model in JS
// and all the function handling change of model parameters during edit mode


// called when a model is clicked on the models navigation menu
function loadModel(id,link_element) {


	// do nothing if we are already loading a model or data
	if (is_loading_model) {return;}
	if (is_loading_data) {return;}
	clearTimeout(current_timeout);
	is_loading_model = true;

	// clean and update the JS state
	data = undefined;
	model_id = id;
	edit_mode = false;
	model = new Model();
	model_being_edited = new Model();

	// make the link bold (and others not)
	$('.model-link').removeClass('active-model-link');
	$('#link-new-model').removeClass('active-model-link');
	$(link_element).addClass('active-model-link');

	// update the status / link
	$("#other-model-view-link").attr('onClick','');
	$("#other-model-view-link").addClass('no-cursor');
	$("#other-model-view-link").text('loading...'); 


	// load the model and display it
	$.ajax({
		url : "load_model/",
		type : "POST",
		data : {model_id: id},
		success : function(json) {
			// store the model as JS object
			model.fromJson(json["model"]);
			model.settings.fromJson(json["settings"]);
			// Store the model name in the form to let the user download the outputs
			$("#model-name-in-form").val(JSON.stringify(json["model"]["name"]));
			$("#model-molecules-in-form").val(JSON.stringify(json["model"]["molecules"]));
			displayModel();
			// if (is_loading_data) { current_timeout = setTimeout(function(){displayModel();},2000); }
			is_loading_model = false;
		},
        error : function(xhr,errmsg,err) {}
    });

	// Load the simulation
	is_loading_data = true;
	$.ajax({
		url : "load_simulation/",
		type : "POST",
		data : {model_id: id},
		success : function(json) {
			// store
			data = json;
			// Store the results in the form to let the user download the outputs
			$("#simulation-output").val(JSON.stringify(json.lineages));
			// for state
			is_loading_data = false;
			// display model if we are finished with the model
			if (!is_loading_model) {displayModel();}
		},
        error : function(xhr,errmsg,err) {}
    });
}
// function reloadModel() { loadModel(model_id,document.getElementById('link-'+model_id)); }

// called when the '+' is clicked at the bottom of the models navigation menu
function createModel() {
	// remove highlight previously select model
	$('.model-link').removeClass('active-model-link');
	$('#link-new-model').addClass('active-model-link');
	$("#no-model").hide();
	model_id_before_new = model_id;
	model_id = undefined;
	model = new Model();
	model_being_edited = new Model();
	edit_mode = true;
	displayModelEditMode(true);
}


// Html code corresponding to a reaction for display
function reactionHtml(left, right, rate) {return '<tr><td>'+left+'</td><td><p style="font-weight:bold">&#8594;</p></td><td>'+right+'</td><td>'+rate+'</td><td><button type="button" class="fa fa-trash-o" onclick="deleteReaction($(this));">x</button></td></tr>';}
// Html code corresponding to a molecule to display
function moleculeHtml(molecule, edit) {
	var opt0 = '<option value=0' + (molecule.split==0 ? ' selected' : '') + '>Split from share</option>';
	var opt1 = '<option value=1' + (molecule.split==1 ? ' selected' : '') + '>Split equal</option>';
	var opt2 = '<option value=2' + (molecule.split==2 ? ' selected' : '') + '>No split</option>';
	var disabled = edit ? '' : ' disabled';
	return '<tr>' +
		'<td>' + molecule.name + '</td>' +
		'<td>' + '<select onchange="changeMoleculeOption($(this));"' + disabled + '>' + opt0 + opt1 + opt2 + '</select>' + '</td>' +
		'</tr>';
}
// Html code corresponding to a parameter
function parameterHtml(parameter, edit) {
	if (edit)
		return '<tr><td>'+parameter.name+'</td><td><input type="text" placeholder="Parameter value..."'+(parameter.value ? ' value='+parameter.value : '')+' onchange="changeParameter($(this));"></td></tr>';
	else
		return '<tr><td>'+parameter.name+'</td><td>'+parameter.value+'</td></tr>';
}
// Html code corresponding to the initialization formula for a molecule
function quantityInitHtml(molecule , expr , init_type , edit) {
	var index;
	var init_expression;
	if (edit) {
		index = model_being_edited.molecules.map(function(m) {return m.name;}).indexOf(molecule.name);
		init_expression = '<input size=10 type="text" placeholder="Expression for initialization..." value='+expr+' onchange="changeQuantityInitExpression($(this));">';
	} else {
		index = model.molecules.map(function(m) {return m.name;}).indexOf(molecule.name);
		init_expression = expr;
	}
	var opt0 = '<option value=0' + (init_type ? ' selected' : '') + '>Number of molecules</option>';
	var opt1 = '<option value=1' + (init_type ? ' selected' : '') + '>Concentration</option>';
	var disabled = edit ? '' : ' disabled';
	var result = '<tr>' +
		'<td>' + molecule.name + '</td>' +
		'<td>' + init_expression + '</td>' +
		'<td>' + '<select onchange="changeQuantityInitType($(this));"' + disabled + '>' + opt0 + opt1 + '</select>' + '</td>' +
		'<td>' + '<input type="checkbox"'+ (model_being_edited.settings.simulation.zero_during_burn[index] ? ' checked' : '') + ' onchange="changeQuantityInitZeroDuringBurn($(this));"' + disabled + '>' + '</td>' +
		'</tr>';
	return result;
}

// Handle parameter change
function changeParameter(d) {
	// Retrieve the line of the table
	var parameter_line = d.parent().parent();
	var parameter_name = parameter_line.children().eq(0).html();
	// Change the parameter value
	var parameter_value = parameter_line.children().eq(1).find("input").val();
	var index = model_being_edited.parameters.map(function(p) {return p.name;}).indexOf(parameter_name);
	if (index>=0)
		model_being_edited.parameters[index].value = parameter_value;
	else
		console.log("The parameter has not been found.");
	displayModelEditMode(false);
}
// Add the reaction once the reaction builder has been filled
function addReaction() {
    // Add the reaction in the model
    model_being_edited.addReaction($("#left_part").val(), $("#right_part").val(), $("#rate").val());
    // Display the model again
    displayModelEditMode(false);
    // Empty the new reaction inputs
    $("#left_part").val("");
    $("#right_part").val("");
    $("#rate").val("");
}
// Delete a reaction
function deleteReaction(d) {
	// Retrieve the line of the table
	var reaction_line = d.parent().parent();
	// Remove the reaction from the model and the line from the table
	var columns = reaction_line.children();
	model_being_edited.deleteReaction([columns.eq(0).html(), columns.eq(2).html(), columns.eq(3).html()]);
	reaction_line.remove();
	// Display again
    displayModelEditMode(false);
}
// Handle molecule option change
function changeMoleculeOption(d) {
	// Get the name of the molecule
	var molecule_name = d.parent().parent().children().eq(0).html();
	var index = model_being_edited.molecules.map(function(m) {return m.name;}).indexOf(molecule_name);
	if (index>=0)
		model_being_edited.molecules[index].split = parseInt(d.val());
	else
		console.log("The molecule has not been found.");
}
// Handle initialization expression / init type / zero burn change
function changeQuantityInitExpression(d) {
	// Get the name of the molecule
	var molecule_name = d.parent().parent().children().eq(0).html();
	var index = model_being_edited.molecules.map(function(m) {return m.name;}).indexOf(molecule_name);
	if (index>=0)
		model_being_edited.settings.simulation.quantities_initialization[index] = d.val();
	else
		console.log("The molecule has not been found.");
	displayModelEditMode(false);
}
function changeQuantityInitType(d) {
	// Get the name of the molecule
	var molecule_name = d.parent().parent().children().eq(0).html();
	var index = model_being_edited.molecules.map(function(m) {return m.name;}).indexOf(molecule_name);
	if (index>=0)
		model_being_edited.settings.simulation.is_quantity_init_conc[index] = (d.val() == 1);
	else
		console.log("The molecule has not been found.");
	displayModelEditMode(false);		
}
function changeQuantityInitZeroDuringBurn(d) {
	// Get the name of the molecule
	var molecule_name = d.parent().parent().children().eq(0).html();
	var index = model_being_edited.molecules.map(function(m) {return m.name;}).indexOf(molecule_name);
	if (index>=0)
		model_being_edited.settings.simulation.zero_during_burn[index] = d.is(":checked");
	else
		console.log("The molecule has not been found.");
	displayModelEditMode(false);
}
function updateSettingsChoices() {
	model_being_edited.settings.simulation.mother_machine = $("#settings-mother-machine")[0].checked;
	model_being_edited.settings.simulation.stochastic = $("#settings-stochastic")[0].checked;
	model_being_edited.settings.simulation.burn_duration = parseFloat($("#settings-pre-simulation-edit").val());
	displayModelEditMode(false);
}



// display a model in non-edit mode
function displayModel() {

	// hide results
	$("#simulation-results").hide();
	// hide validation from edit mode
	$("#model-validation-container").hide();

	// show the content header bar
	$("#other-model-view-link").attr('onClick','');
	$("#other-model-view-link").addClass('no-cursor');
	if (data===undefined) {
		$("#other-model-view-link").text('loading...'); 
	}
	else if (!data.hasOwnProperty('simulation')) {
		$("#other-model-view-link").text('simulation not available yet, reload regularly');
	}
	else if (!data.simulation.success) {
		$("#other-model-view-link").text('simulation has failed');
	}
	else {
		$("#other-model-view-link").text('show results'); 
		$("#other-model-view-link").attr('onClick','displayResults();');
		$("#other-model-view-link").removeClass('no-cursor');
	}
	$("#other-model-view-link").show();
	$("#current-model-view-name").text('Description');
	$("#current-model-view-name").show();
	$("#download-button").hide();
	$("#save-button").hide();
	$("#cancel-edit-button").hide();	
	$("#edit-button").show();
	$("#duplicate-button").show();
	$("#content-header").show();

	// show the model name
	$("#model-name").text(model.name);
	$("#model-name").show();
	$("#model-name-edit").hide();

	// show the model notes
	$("#model-notes").html(model.notes ? model.notes.replace(/\n/g,"<br>") : "");
	$("#model-notes").show();
	$("#model-notes-edit").hide();

    // show the reactions
    $("#all-reactions-table").empty();
	model.reactions.forEach(function(d) {
		$("#all-reactions-table").append(reactionHtml(d[0], d[1], d[2]));
	});	
    $("#new-reaction").hide();
    $("#all-reactions-table :button").hide();
    $("#reactions-container").show();

    // show the molecules
    if (model.molecules.length==0) {
    	$("#box-molecules").hide();
    } else {
    	$("#box-molecules").show();
        $("#all-molecules-table").empty();
        model.molecules.sort(function(a,b){ return a.name<b.name ? -1 : 1; }).forEach(function(d) {
        	$("#all-molecules-table").append(moleculeHtml(d, edit_mode));
        });
    }
    $("#molecules-container").show();

    // show the parameters
    if (model.parameters.length==0) {
    	$("#box-parameters").hide();
    } else {
    	$("#box-parameters").show();
        $("#all-parameters-table").empty();
        model.parameters.sort(function(a,b){ return a.name<b.name ? -1 : 1; }).forEach(function(d) {
        	$("#all-parameters-table").append(parameterHtml(d, edit_mode));
        });
    }
    $("#parameters-container").show();

    // show the growth
    $("#growth-type").prop("disabled", true);
    $("#growth-parameter-edit").hide();
    $("#growth-parameter").show();
    $("#growth-parameter").text(model.growth.parameter);
    // division
    $("#division-trigger-edit").hide();
	$("#division-trigger").show();
	$("#division-trigger").text(model.division_trigger);
    $("#volume-share-edit").hide();
	$("#volume-share").show();
	$("#volume-share").text(model.volume_share);
	// growth and division
	$("#cell-evolution-container").show();

	// show the settings: choices
	$("#settings-mother-machine").prop("disabled", true);
	$("#settings-mother-machine")[0].checked = model.settings.simulation.mother_machine;
	$("#settings-stochastic").prop("disabled", true);
	$("#settings-stochastic")[0].checked = model.settings.simulation.stochastic;
	$("#settings-pre-simulation-edit").hide();
	$("#settings-pre-simulation-text").show();
	$("#settings-pre-simulation-text").text(model.settings.simulation.burn_duration);

	// settings: volume initialization
	$("#volume-init-edit").hide();
	$("#volume-init").show();
	$("#volume-init").text(model.settings.simulation.volume_initialization);

	// settings: quantity initialization
	$("#quantities-init-table-head").empty(); 
	$("#quantities-init-table").empty();
	if (model.molecules.length>0) {
		$("#quantities-init-table-head").append("<tr><th>Molecule name</th><th>Init. expression</th><th>Init. type</th><th>Absent during pre-simulation ?</th></tr>");
    	model.molecules.sort(function(a,b){ return a.name<b.name ? -1 : 1; }).forEach(function(d,i) {
    		$("#quantities-init-table").append(quantityInitHtml(d,model.settings.simulation.quantities_initialization[i],model.settings.simulation.is_quantity_init_conc[i],edit_mode));
    	});
    }

    // show settings as a whole
    $("#settings-container").show();

	// show the whole description
	$("#model-description").show();
	$("#model-description-boxes").show();

	// show the whole content
	$("#content").show();
}

// display results of a model (meant to be called only when a model is displayed in non-edit mode)
function displayResults() {

	// update the content header bar
	$("#other-model-view-link").attr('onClick', 'displayModel();');
	$("#other-model-view-link").removeClass('no-cursor');
	$("#other-model-view-link").text('show description');
	$("#current-model-view-name").text('Results');
	$("#edit-button").hide();
	$("#duplicate-button").hide();
	$("#download-button").show();

	// hide the description
	$("#model-description").hide();

	// empty previous results
	$("#sim-view-1").empty();
	$("#sim-view-2").empty();
	$("#sim-view-3").empty();
	$("#sim-view-4").empty();

	// make the plots and show
	$("#simulation-results").show();
	viz.makeViews();
}


// display model description in edit mode !
function displayModelEditMode(model_creation) {

	// hide results
	$("#simulation-results").hide();

	// show the content header bar
	$("#current-model-view-name").text('Description');
	$("#current-model-view-name").show();
	$("#other-model-view-link").attr('onClick','');
	$("#other-model-view-link").text('model editing mode');
	$("#other-model-view-link").addClass('no-cursor');
	$("#other-model-view-link").show();
	$("#download-button").hide();
	$("#edit-button").hide();
	$("#duplicate-button").hide();
	$("#save-button").show();
	if (model_creation) {
		$("#cancel-edit-button").hide();
	} else {
		$("#cancel-edit-button").show();
	}
	$("#content-header").show();

	// hide current description
	$("#model-description").hide();

	// show the model name
	$("#model-name").hide();
	$("#model-name-edit").val(model_being_edited.name);
	$("#model-name-edit").show();

	// Notes of the model
    $("#model-notes").hide();
    $("#model-notes-edit").val(model_being_edited.notes);
    $("#model-notes-edit").show();

    // show the reactions
    $("#all-reactions-table").empty();
	model_being_edited.reactions.forEach(function(d) {
		$("#all-reactions-table").append(reactionHtml(d[0], d[1], d[2]));
	});
    // Show the new reaction and the delete buttons
    $("#new-reaction").show();
    $("#all-reactions-table :button").show();

    // Display the molecules (if there are some only)
    if (model_being_edited.molecules.length==0) {
    	$("#molecules-container").hide();
    } else {
    	$("#molecules-container").show();
        $("#all-molecules-table").empty();
        model_being_edited.molecules.sort(function(a,b){ return a.name<b.name ? -1 : 1; }).forEach(function(d) {
        	$("#all-molecules-table").append(moleculeHtml(d, edit_mode));
        });
    }

    // Display the parameters
    if (model_being_edited.parameters.length==0) {

    	$("#parameters-container").hide();
    } else {
    	$("#parameters-container").show();
        $("#all-parameters-table").empty();
        model_being_edited.parameters.sort(function(a,b){ return a.name<b.name ? -1 : 1; }).forEach(function(d) {
        	$("#all-parameters-table").append(parameterHtml(d, edit_mode));
        });
    }

    // Display the growth part
    $("#growth-type").prop("disabled", false);
    $("#growth-parameter").hide();
    $("#growth-parameter-edit").show();
    $("#growth-parameter-edit").val(model_being_edited.growth.parameter);

    // Display the division part
    $("#division-trigger-edit").show();
    $("#division-trigger-edit").val(model_being_edited.division_trigger);
	$("#division-trigger").hide();
    $("#volume-share-edit").show();
    $("#volume-share-edit").val(model_being_edited.volume_share);
	$("#volume-share").hide();

	// Display the settings part: choices
	$("#settings-mother-machine").prop("disabled", true);
	$("#settings-mother-machine")[0].checked = model_being_edited.settings.simulation.mother_machine;
	$("#settings-stochastic").prop("disabled", false);
	$("#settings-stochastic")[0].checked = model_being_edited.settings.simulation.stochastic;
	$("#settings-pre-simulation-text").hide();
	$("#settings-pre-simulation-edit").show();
	$("#settings-pre-simulation-edit").val(model_being_edited.settings.simulation.burn_duration);

	// Display the volume initialization part
	$("#volume-init-edit").show();
	$("#volume-init-edit").val(model_being_edited.settings.simulation.volume_initialization);
	$("#volume-init").hide();

	// Display the settings part: table
	$("#quantities-init-table-head").empty(); 
	$("#quantities-init-table").empty();
	if (model_being_edited.molecules.length>0) {
		$("#quantities-init-table-head").append("<tr><th>Molecule name</th><th>Init. expression</th><th>Init. type</th><th>Absent during pre-simulation ?</th></tr>");
    	model_being_edited.molecules.sort(function(a,b){ return a.name<b.name ? -1 : 1; }).forEach(function(d,i) {
    		$("#quantities-init-table").append(quantityInitHtml(d,model_being_edited.settings.simulation.quantities_initialization[i],
    															model_being_edited.settings.simulation.is_quantity_init_conc[i],edit_mode));
    	});
    }

    // Display the model validation
    $("#model-validation-content").empty();
    model_validation = model_being_edited.validate();
    for (var key in model_validation) {
    	if (key=="name") $("#model-validation-content").append("<div class='grid'>The model has no name.<br></div>");
    	if (key=="reactions") $("#model-validation-content").append("<div class='grid'>The model has no reactions.</div>");
    	if (key=="parameters") $("#model-validation-content").append("<div class='grid'>At least one parameter has no value.</div>");
    	if (key=="growth_parameter") $("#model-validation-content").append("<div class='grid'>The cell growth rate is undefined.</div>");
    	if (key=="division_trigger") $("#model-validation-content").append("<div class='grid'>The rule triggering the division is undefined.</div>");
    	if (key=="volume_share") $("#model-validation-content").append("<div class='grid'>The inherited volume fraction after division is undefined.</div>");
    }
    if (model_validation.valid) { $("#model-validation-container").hide(); } else { $("#model-validation-container").show(); }
    // Disable the save button if the model is not valid
    $("#save-button").prop('disabled', !model_validation.valid);

	// show the whole description
	$("#model-description").show();
	$("#model-description-boxes").show();

	// show the whole content
	$("#content").show();
}


function editModel() {
	edit_mode = true;
	model_being_edited = copyModel(model);
	displayModelEditMode(false);
}

function cancelEdit() {
	edit_mode = false;
	// special case: the model was a new one
	if (model_id===undefined) {
		// if no other model, refresh the page
		if (model_id_before_new===undefined) {
			location.reload();
		}
		// reload previously selected model
		else {
			model_id = model_id_before_new;
			reloadModel();
		}
	}
	else {
		displayModel();
	}
}

function saveModel() {
	if (model_id!==undefined) $("#form-save-model-model-id").val(model_id);
	$("#form-save-model-model").val(model_being_edited.toJson());
	$("#form-save-model-settings").val(model_being_edited.settings.toJson());
	$("#form-save-model").submit();
}

function deleteModel(id) {
	if (confirm("Do you really want to delete the model ?")) {
		// The id is like delete-{id}, retrieve the id first
		var id_ = id.split("-")[1];
		$("#form-delete-model-model-id").val(id_);
		$("#form-delete-model").submit();
	}
}

function duplicateModel() {
	if (model_id!==undefined) $("#form-copy-model-model-id").val(model_id);
	$("#form-copy-model").submit();
}
