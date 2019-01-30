


// load the model
model = JSON.parse('{"molecules":[{"name":"A","split":1},{"name":"B","split":1},{"name":"C","split":1}],"parameters":[{"name":"synth_A_max","value":"22.3"},{"name":"synth_B_max","value":"25.2"},{"name":"synth_C_max","value":"3.2"},{"name":"A_conc_rep","value":"3.4"},{"name":"B_conc_rep","value":"12.3"},{"name":"C_conc_rep","value":"4.1"},{"name":"mu_avg","value":"0.02"},{"name":"mu_std","value":"0.004"},{"name":"a","value":"1"},{"name":"b","value":"1"},{"name":"split","value":"0.4"},{"name":"lnm_sig_1","value":"0.1"}],"reactions":[["","A","synth_A_max / ( 1 + [C] / C_conc_rep )"],["","B","synth_B_max / ( 1 + [A] / A_conc_rep )"],["","C","synth_C_max / ( 1 + [B] / B_conc_rep )"]],"division_trigger":"V>a*V_birth+b+N(0,lnm_sig_1)","volume_share":"split","growth":{"type": "exp", "parameter": "N(mu_avg,mu_std)"}}')
console.log(model)

// load the data
console.log(data[5])

//default svg dimensions
var h = 400;
var w = 1000;

// create the svg for individual lineages
var svg_ind_lineages = document.createElementNS("http://www.w3.org/2000/svg", 'svg');
document.getElementById('indiv-lineages-plot').appendChild(svg_ind_lineages);
svg_ind_lineages.setAttribute("width",w);
svg_ind_lineages.setAttribute("height",h);
svg_ind_lineages.setAttribute("id","svg-indiv-lineages");

// add one line per lineage
var num_to_show = Math.min(5,data[0]['population'].length);
var step_h = h/(num_to_show+1);
var step_w = w/data.length;
for (var il=0; il<num_to_show; il++)
{
	var line = document.createElementNS("http://www.w3.org/2000/svg", 'rect');
	svg_ind_lineages.appendChild(line);
	line.setAttribute("y",(il+1)*step_h)
	line.setAttribute("x",0)
	line.setAttribute("width",w)
	line.setAttribute("height",5)
}

// show the lineages division events
for (var il=0; il<num_to_show; il++) // iterate on lineage
{
	var ic=il;
	// iterate on timepoints, backwards
	var it=data.length-1;
	while (it>0)
    {
        ancestor=findAncestor(it,ic,data);
        if (ancestor.is_mother) // means division occured 
        {
			// add a vertical 'line'
			var line = document.createElementNS("http://www.w3.org/2000/svg", 'rect');
			svg_ind_lineages.appendChild(line);
			line.setAttribute("y",(il+1)*step_h-8)
			line.setAttribute("x",it*step_w)
			line.setAttribute("width",3)
			line.setAttribute("height",20)        	
        }
        it--; ic=ancestor.ic;
    }
}

// show absolute time
time_ticks = [0,data[Math.round(data.length/2)]['time'],data[data.length-1]['time']]
console.log(time_ticks)
for (it in time_ticks)
{
	var t = document.createElementNS("http://www.w3.org/2000/svg", 'text');
	svg_ind_lineages.appendChild(t);
	t.setAttribute("y",2.7*step_h);
	t.setAttribute("x",time_ticks[it]*0.93*w/time_ticks[2]);
	t.setAttribute("fill","gray")
	t.innerHTML = "t = "+time_ticks[it].toFixed(2);
}


// create the svg for average over lineages
var svg_avg_lineages = document.createElementNS("http://www.w3.org/2000/svg", 'svg');
document.getElementById('avg-lineages-plot').appendChild(svg_avg_lineages);
svg_avg_lineages.setAttribute("width",w);
svg_avg_lineages.setAttribute("height",h);
svg_avg_lineages.setAttribute("id","svg-avg-lineages");

// we compute the average division time in four parts


function getAllDivisionTimes(data)
{
	// for (var il=0; il<data[0]['population'].length; il++) // iterate on lineage
	// {
	// 	var ic=il;
	// 	// iterate on timepoints, backwards
	// 	var it=data.length-1;
	// 	while (it>0)
	//     {
	//         ancestor=findAncestor(it,ic,data);
	//         if (ancestor.is_mother) // means division occured 
	// 	}
	// }
}




// function to find the index of the ancestor (or same cell) at previous time-point
function findAncestor(it,ic,data)
{
    var ID=data[it]["population"][ic]["ID"];
    var prev_ic=0; var is_mother = false;
    // search if still present at previous time point
    while ( prev_ic<data[it-1]["population"].length && data[it-1]["population"][prev_ic]["ID"] != ID ) {prev_ic++;}
    // if not the case, search for the mother
    if (prev_ic==data[it-1]["population"].length)
    {
        // console.log("cell does not exist in previous time point -> search mother");
        ID=data[it]["population"][ic]["motherID"];
        // console.log ("Mother ID = "+ID);
        prev_ic=0;
        while ( prev_ic<data[it-1]["population"].length && data[it-1]["population"][prev_ic]["ID"] != ID ) {prev_ic++;}
        // if still nothing, problem
        if (prev_ic==data[it-1]["population"].length)
        {
            console.log("Did not find the mother either ?! problem.");
        }
        is_mother=true;
    }
    return { "ic":prev_ic , "ID":ID , "is_mother":is_mother };
}

function findSuccessor(it,ic,data)
{
	var ID=data[it]["population"][ic]["ID"];
	var next_ic=0; var has_divided = false;
	// search if still present at next time point
	while (next_ic<data[it+1]['population'].length && data[it+1]['population'][next_ic]['ID'] != ID) {next_ic++;}
	// if not, search for children
	if (next_ic==data[it+1]['population'].length)
	{
		next_ic = 0;
		while (next_ic<data[it+1]['population'].length && data[it+1]['population'][next_ic]['motherID'] != ID) {next_ic++;}
		// if still nothing, problem
        if (next_ic==data[it+1]["population"].length)
        {
            console.log("Did not find the daughter either ?! problem.");
        }
		has_divided = true;
	}
	return { "ic":next_ic , "has_divided":has_divided };
}