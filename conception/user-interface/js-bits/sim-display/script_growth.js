


// load the model
model = JSON.parse('{"molecules":[{"name":"A","split":1},{"name":"B","split":1},{"name":"C","split":1}],"parameters":[{"name":"synth_A_max","value":"22.3"},{"name":"synth_B_max","value":"25.2"},{"name":"synth_C_max","value":"3.2"},{"name":"A_conc_rep","value":"3.4"},{"name":"B_conc_rep","value":"12.3"},{"name":"C_conc_rep","value":"4.1"},{"name":"mu_avg","value":"0.02"},{"name":"mu_std","value":"0.004"},{"name":"a","value":"1"},{"name":"b","value":"1"},{"name":"split","value":"0.4"},{"name":"lnm_sig_1","value":"0.1"}],"reactions":[["","A","synth_A_max / ( 1 + [C] / C_conc_rep )"],["","B","synth_B_max / ( 1 + [A] / A_conc_rep )"],["","C","synth_C_max / ( 1 + [B] / B_conc_rep )"]],"division_trigger":"V>a*V_birth+b+N(0,lnm_sig_1)","volume_share":"split","growth":{"type": "exp", "parameter": "N(mu_avg,mu_std)"}}')
console.log(model)

// load the data
console.log(data[5])

//default svg dimensions
var h = 300;
var w = 1000;

// create the svg for showing the mother machine
var svg_mother_machine = document.createElementNS("http://www.w3.org/2000/svg", 'svg');
document.getElementById('mother-machine-view').appendChild(svg_mother_machine);
svg_mother_machine.setAttribute("width",w);
svg_mother_machine.setAttribute("height",h);
svg_mother_machine.setAttribute("id","svg-mother-machine");

// create the svg to show the trace
var svg_trace = document.createElementNS("http://www.w3.org/2000/svg", 'svg');
document.getElementById('trace-view').appendChild(svg_trace);
svg_trace.setAttribute("width",w);
svg_trace.setAttribute("height",h);
svg_trace.setAttribute("id","svg-trace");
var svg_trace_rect = document.createElementNS("http://www.w3.org/2000/svg", 'rect');
document.getElementById('svg-trace').appendChild(svg_trace_rect);
svg_trace_rect.setAttribute("x",0.05*w);
svg_trace_rect.setAttribute("y",0.05*h);
svg_trace_rect.setAttribute("width",0.95*w);
svg_trace_rect.setAttribute("height",0.9*h);
svg_trace_rect.setAttribute("stroke","black");
svg_trace_rect.setAttribute("fill","none");
svg_trace_rect.setAttribute("id","svg-trace-rect");



// create the options to choose which molecule to display
model['molecules'].forEach(function (item,index)
	{
		console.log(item);
		var mol_opt = document.createElement('option');
		document.getElementById('select-molecule').appendChild(mol_opt);
		mol_opt.setAttribute('value',index);
		mol_opt.innerHTML=item['name'];
	} );

// current time step
var it = 0; var il=0; var num_div = 0;
var size = 0; var scale = 0.2; 
var imol=0; var conc = 0; var scale_conc = 1.2;
var scale_time = 0.95 * w / data[data.length-1]['time'];
var t=0; var path_str='';


function start()
{
	
	// initial size
	size = data[it]['population'][il]['volume'];
	// get molecule selection
	var selector = document.getElementById('select-molecule');
	imol = selector.options[selector.selectedIndex].value;
	// initial concentration
	conc = parseFloat(data[it]['population'][il]['quantities'][imol]) / parseFloat(size);
	// construct first cell
	var svg_cell = makeCellSvg();
	// construct beginning of path for trace
	var x = 0.05 * w; var y = 0.95*h - conc*scale_conc;
	path_str = 'M ' + x+ ' ' + y;
	var svg_trace_path = makeTraceSvg();

	console.log('starting anim');
	var id = setInterval(doStep,10);
	function doStep()
	{
		// console.log('step')
		if (it==data.length-1)
		{
			clearInterval(id);
			it = 0; il=0; num_div = 0;
			// document.getElementById('generation-counter').innerHTML = 'Number of divisions = 0';
			// size = data[it]['population'][il]['volume'];
			// t = data[it]['time'];
			// conc = parseFloat(data[it]['population'][il]['quantities'][imol]) / parseFloat(size);
			// makeCellSvg();
			// var x = 0.05 * w; var y = 0.95*h - conc*scale_conc;
			// path_str = 'M ' + x + ' ' + y;
			// makeTraceSvg();
		}
		else
		{
			var successor = findSuccessor(it,il);
			if (successor['has_divided'])
			{
				num_div = num_div + 1;
				document.getElementById('generation-counter').innerHTML = 'Number of divisions = ' + num_div;
			}
			// console.log(successor);
			il = successor['ic']; it = it + 1;
			size = data[it]['population'][il]['volume'];
			t = data[it]['time'];
			conc = parseFloat(data[it]['population'][il]['quantities'][imol]) / parseFloat(size);
			document.getElementById('conc-display').innerHTML = 'Molecule concentration = ' + conc.toFixed(2);
			makeCellSvg();
			makeTraceSvg();
		}
	}
}



function makeCellSvg()
{
	var svg_cell = document.getElementById("svg-cell");
	if (typeof(svg_cell) == 'undefined' || svg_cell == null)
	{
		svg_cell = document.createElementNS("http://www.w3.org/2000/svg", 'rect');
		document.getElementById("svg-mother-machine").appendChild(svg_cell);
	}
	svg_cell.setAttribute("x",w/50);
	svg_cell.setAttribute("y",h/5);
	svg_cell.setAttribute("rx",30);
	svg_cell.setAttribute("ry",30);
	svg_cell.setAttribute("width",w * size * scale);
	svg_cell.setAttribute("height",h/3);
	svg_cell.setAttribute("id","svg-cell");
	// compute fill color based on concentration
	color = 'rgba(' + Math.round(conc*scale_conc) + ',0,0,1.0)';
	// console.log(color)
	svg_cell.setAttribute("style","fill:"+color+";stroke:black;stroke-width:5;opacity:0.5");
	return svg_cell;
}

function makeTraceSvg()
{
	var svg_trace_path = document.getElementById("svg-trace-path");
	if (typeof(svg_trace_path) == 'undefined' || svg_trace_path == null)
	{
		svg_trace_path = document.createElementNS("http://www.w3.org/2000/svg", 'path');
		document.getElementById("svg-trace").appendChild(svg_trace_path);
	}
	svg_trace_path.setAttribute("id","svg-trace-path");
	svg_trace_path.setAttribute("stroke","red");
	svg_trace_path.setAttribute("stroke-width","1");
	svg_trace_path.setAttribute("fill","none");
	var x = t*scale_time + 0.05 * w;
	var y = - conc*scale_conc + 0.95 * h;
	path_str = path_str + ' L ' + x + ' ' + y;
	// console.log(path_str);
	svg_trace_path.setAttribute("d",path_str);
	return svg_trace_path;
}


function findSuccessor(it,ic)
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