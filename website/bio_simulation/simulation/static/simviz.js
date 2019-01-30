
// like a namespace
var viz = {};

// make all views
viz.makeViews = function()
{
	// store model and sim data
	viz.model = data.model;
	viz.lineages = data.lineages;

	// remove previous views
	sim_view_1 = document.getElementById("sim-view-1");
	while (sim_view_1.firstChild) { sim_view_1.removeChild(sim_view_1.firstChild); }
	sim_view_2 = document.getElementById("sim-view-2");
	while (sim_view_2.firstChild) { sim_view_2.removeChild(sim_view_2.firstChild); }
	sim_view_3 = document.getElementById("sim-view-3");
	while (sim_view_3.firstChild) { sim_view_3.removeChild(sim_view_3.firstChild); }
	sim_view_4 = document.getElementById("sim-view-4");
	while (sim_view_4.firstChild) { sim_view_4.removeChild(sim_view_4.firstChild); }

	// make views
	viz.makeViewGrowthDivision(sim_view_1);
	viz.makeViewMolContLineageComp(sim_view_2);
	viz.makeViewFullMolCont(sim_view_3);
	viz.makeView2DTraj(sim_view_4);
}



// view-1: volume traces
viz.makeViewGrowthDivision = function(view)
{
	// create 2 rows, one for text-based info and one for the graph
	var txt = document.createElement('div');
	txt.setAttribute('class','grid');
	view.appendChild(txt);
	var graph = document.createElement('div');
	graph.setAttribute('class','grid');
	graph.setAttribute('id','view-1-graph');
	view.appendChild(graph);
	
	// get all interdivision times, show average as text
	// var div_data = viz.getDivisionData(sim);
	var avg_tdiv = 0; //math.mean( div_data.map(i => i.tdiv) );
	var txt_avgtdiv = document.createElement('div');
	txt_avgtdiv.setAttribute('class','grid results-text');
	txt_avgtdiv.innerHTML = 'Average inter-division time: <b>TO DO</b>'; // + avg_tdiv.toFixed(2);
	txt.appendChild(txt_avgtdiv);

	// selection button for the number of lineages to show
	var btn_num_lineages_div = document.createElement('div');
	btn_num_lineages_div.setAttribute('class','grid  results-text');
	btn_num_lineages_div.innerHTML = 'Number of lineages to show: ';
	var btn_num_lineages = document.createElement('input');
	btn_num_lineages.setAttribute('type','number');
	btn_num_lineages.setAttribute('id','view-1-btn');
	btn_num_lineages.setAttribute('value',2);
	btn_num_lineages.setAttribute('min',1);
	btn_num_lineages.setAttribute('max',viz.lineages.length);
	btn_num_lineages.setAttribute('onchange','viz.showVolumeTraces()');
	btn_num_lineages_div.appendChild(btn_num_lineages);
	txt.appendChild(btn_num_lineages_div);

	// tick box for showing average or not
	// var btn_show_avg_div = document.createElement('div');
	// btn_show_avg_div.setAttribute('class','grid');
	// btn_show_avg_div.innerHTML = 'Show/hide population average: ';
	// var btn_show_avg = document.createElement('input');
	// btn_show_avg.setAttribute('type','checkbox');
	// btn_show_avg.setAttribute('id','view-1-check');
	// btn_show_avg.setAttribute('onchange','viz.showVolumeTraces()');
	// btn_show_avg_div.appendChild(btn_show_avg);
	// txt.appendChild(btn_show_avg_div);

	// showing volume traces
	viz.view1 = view;
	viz.showVolumeTraces();
}

viz.showVolumeTraces = function()
{
	// store scroll position
	viz.scroll_pos = window.pageYOffset;
	
	// remove previous graph
	var graph = document.getElementById('view-1-graph');
	graph.remove();

	// get the number of lineages to use
	var num_to_show = document.getElementById('view-1-btn').value;

	// get the data
	var data = [];
	for (var i=0;i<num_to_show;i++)
	{
		// here change using new
		let trace = viz.getLineageData('V',i);
		// set the color
		trace['line']={'color':'rgb('+Math.floor((num_to_show-i)*255/num_to_show)+',0,'+Math.floor(i*255/num_to_show)+')','width':0.75};
		data.push(trace);
	}

	// get the average volume data if need
	// if (document.getElementById('view-1-check').checked)
	// {
		// here change using new
		// var trace_avg = viz.getPopulationData(viz.sim,'V');
		// trace_avg['line']={'color':'rgb(0,0,0)','width':1.5};
		// trace_avg['name']='population average';
		// data.push(trace_avg);
	// }

	// build the axis and layout object
	var axis_x = { 'showgrid':true, 'zeroline':false, 'nticks':20, 'showline':false, 'title':'Time' , 'mirror':'all'};
	var axis_y = { 'showgrid':true, 'zeroline':false, 'nticks':20, 'showline':false, 'title':'Cell size' , 'mirror':'all'};
	var layout = { 'showlegend':false , 'xaxis':axis_x , 'yaxis':axis_y };
	var fig = { 'data':data , 'layout':layout };
	var graph = document.createElement('div');
	graph.setAttribute('class','grid');
	graph.setAttribute('id','view-1-graph');
	viz.view1.appendChild(graph);
	Plotly.plot(document.getElementById('view-1-graph'),fig);	

	// restore position
	window.scrollTo(0, viz.scroll_pos);
}


// view-2: molecular content, lineage comparison
viz.makeViewMolContLineageComp = function(view)
{
	// create 2 rows, one for text-based info and one for the graph
	var txt = document.createElement('div');
	txt.setAttribute('class','grid  results-text');
	view.appendChild(txt);
	var graph = document.createElement('div');
	graph.setAttribute('class','grid');
	graph.setAttribute('id','view-2-graph');
	view.appendChild(graph);

	// selector for which molecular species
	var mol_select_div = document.createElement('div');
	mol_select_div.setAttribute('class','grid');
	mol_select_div.innerHTML = 'Molecular species to display: ';
	var mol_select = document.createElement('select');
	mol_select.setAttribute("id","view-2-mol");
	model['molecules'].forEach(function (item,index)
	{
		var mol_opt = document.createElement('option');
		mol_select.appendChild(mol_opt);
		mol_opt.setAttribute('value',index);
		mol_opt.innerHTML=item['name'];
	} );
	mol_select.setAttribute("onchange","viz.showMolContTraces()");
	mol_select_div.appendChild(mol_select);
	txt.appendChild(mol_select_div);

	// tick box for showing concentration or abs
	var btn_show_abs_div = document.createElement('div');
	btn_show_abs_div.setAttribute('class','grid');
	btn_show_abs_div.innerHTML = 'Show absolute numbers instead of concentration: ';
	var btn_show_abs = document.createElement('input');
	btn_show_abs.setAttribute('type','checkbox');
	btn_show_abs.setAttribute('id','view-2-check-abs');
	btn_show_abs.setAttribute('onchange','viz.showMolContTraces()');
	btn_show_abs_div.appendChild(btn_show_abs);
	txt.appendChild(btn_show_abs_div);

	// selection button for the number of lineages to show
	var btn_num_lineages_div = document.createElement('div');
	btn_num_lineages_div.setAttribute('class','grid');
	btn_num_lineages_div.innerHTML = 'Number of lineages to show: ';
	var btn_num_lineages = document.createElement('input');
	btn_num_lineages.setAttribute('type','number');
	btn_num_lineages.setAttribute('id','view-2-btn');
	btn_num_lineages.setAttribute('value',2);
	btn_num_lineages.setAttribute('min',1);
	btn_num_lineages.setAttribute('max',viz.lineages.length);
	btn_num_lineages.setAttribute('onchange','viz.showMolContTraces()');
	btn_num_lineages_div.appendChild(btn_num_lineages);
	txt.appendChild(btn_num_lineages_div);

	// tick box for showing average or not
	var btn_show_avg_div = document.createElement('div');
	btn_show_avg_div.setAttribute('class','grid');
	btn_show_avg_div.innerHTML = 'Show/hide population average: ';
	var btn_show_avg = document.createElement('input');
	btn_show_avg.setAttribute('type','checkbox');
	btn_show_avg.setAttribute('id','view-2-check-avg');
	btn_show_avg.setAttribute('onchange','viz.showMolContTraces()');
	// btn_show_avg_div.appendChild(btn_show_avg);
	// txt.appendChild(btn_show_avg_div);

	// showing traces
	viz.view2 = view;
	viz.showMolContTraces();
}

viz.showMolContTraces = function()
{
	// store scroll position
	viz.scroll_pos = window.pageYOffset;

	// remove previous graph
	var graph = document.getElementById('view-2-graph');
	graph.remove();

	// get the number of lineages to use
	var num_to_show = document.getElementById('view-2-btn').value;

	// get the molecule to display
	var imol = document.getElementById('view-2-mol').options[document.getElementById('view-2-mol').selectedIndex].value;
	var mol = imol;
	if (!document.getElementById('view-2-check-abs').checked)
	{
		mol = '[' + mol + ']';
	}

	// get the data
	var data = [];
	for (var i=0;i<num_to_show;i++)
	{
		// change here
		let trace = viz.getLineageData(mol,i);
		// set the color
		trace['line']={'color':'rgb('+Math.floor((num_to_show-i)*255/num_to_show)+',0,'+Math.floor(i*255/num_to_show)+')','width':0.75};
		data.push(trace);
	}

	// get the average volume data if need
	// if (document.getElementById('view-2-check-avg').checked)
	// {
	// 	var trace_avg = viz.getPopulationData(viz.sim,mol);
	// 	trace_avg['line']={'color':'rgb(0,0,0)','width':1.5};
	// 	trace_avg['name']='population average';
	// 	data.push(trace_avg);
	// }

	// build the axis and layout object
	var axis_x = { 'showgrid':true, 'zeroline':false, 'nticks':20, 'showline':false, 'title':'Time' , 'mirror':'all'};
	var title_y = model.molecules[imol].name;
	if (document.getElementById('view-2-check-abs').checked) {title_y += ' (# of molecules)';}
	else {title_y += ' (concentration)';}
	var axis_y = { 'showgrid':true, 'zeroline':false, 'nticks':20, 'showline':false, 'title':title_y , 'mirror':'all'};
	var layout = { 'showlegend':false , 'xaxis':axis_x , 'yaxis':axis_y };
	var fig = { 'data':data , 'layout':layout };
	var graph = document.createElement('div');
	graph.setAttribute('class','grid');
	graph.setAttribute('id','view-2-graph');
	viz.view2.appendChild(graph);
	Plotly.plot(document.getElementById('view-2-graph'),fig);

	// restore position
	window.scrollTo(0, viz.scroll_pos);
}





// view-3: full molecular content, for one lineage
viz.makeViewFullMolCont = function(view)
{
	// create 2 rows, one for text-based info and one for the graph
	var txt = document.createElement('div');
	txt.setAttribute('class','grid');
	view.appendChild(txt);
	var graph = document.createElement('div');
	graph.setAttribute('class','grid');
	graph.setAttribute('id','view-3-graph');
	view.appendChild(graph);

	// button to change the lineage
	var btn_chg_lin_div = document.createElement('div');
	btn_chg_lin_div.setAttribute('class','grid results-text');
	var btn_chg_lin = document.createElement('button');
	btn_chg_lin.innerHTML = 'change lineage';
	btn_chg_lin.setAttribute('onclick','viz.changeLineageView3()');
	btn_chg_lin_div.appendChild(btn_chg_lin);
	txt.appendChild(btn_chg_lin_div);

	// tick box for showing average or not
	var btn_show_avg_div = document.createElement('div');
	btn_show_avg_div.setAttribute('class','grid results-text');
	btn_show_avg_div.innerHTML = 'Show/hide population average: ';
	var btn_show_avg = document.createElement('input');
	btn_show_avg.setAttribute('type','checkbox');
	btn_show_avg.setAttribute('id','view-3-check-avg');
	btn_show_avg.setAttribute('onchange','viz.showFullMolCont()');
	// btn_show_avg_div.appendChild(btn_show_avg);
	// txt.appendChild(btn_show_avg_div);	

	// showing traces
	viz.view3 = view;
	viz.view3_il = 0;
	viz.showFullMolCont();

} 

viz.changeLineageView3 = function()
{
	viz.view3_il++;
	if (viz.view3_il>viz.lineages.length-1) {viz.view3_il=0;}
	viz.showFullMolCont();
}

viz.showFullMolCont = function()
{
	// store scroll position
	viz.scroll_pos = window.pageYOffset;

	// remove previous graph
	var graph = document.getElementById('view-3-graph');
	graph.remove();

	// get the data
	var data = [];
	for (var imol=0;imol<model.molecules.length;imol++)
	{
		let trace = viz.getLineageData('['+imol+']',viz.view3_il);
		// set the color
		trace['line']={'color':'rgb('+Math.floor((model.molecules.length-imol)*255/model.molecules.length)+',0,'+Math.floor(imol*255/model.molecules.length)+')','width':0.75};
		trace['name']=model.molecules[imol].name;
		data.push(trace);
		// if (document.getElementById("view-3-check-avg").checked)
		// {
		// 	let trace_avg =  viz.getPopulationData(viz.sim,'['+imol+']');
		// 	trace_avg['line']={'color':'rgb('+Math.floor((model.molecules.length-imol)*255/model.molecules.length)+',0,'+Math.floor(imol*255/model.molecules.length)+')','width':1.25};
		// 	trace_avg['name']=model.molecules[imol].name + ' (average)';
		// 	trace_avg['name']=model.molecules[imol].name + ' (average)';
		// 	data.push(trace_avg);
		// }
	}

	// build the axis and layout object
	var axis_x = { 'showgrid':true, 'zeroline':false, 'nticks':20, 'showline':false, 'title':'Time' , 'mirror':'all'};
	var axis_y = { 'showgrid':true, 'zeroline':false, 'nticks':20, 'showline':false, 'title':'Concentration' , 'mirror':'all'};
	var layout = { 'showlegend':true , 'xaxis':axis_x , 'yaxis':axis_y };
	var fig = { 'data':data , 'layout':layout };
	var graph = document.createElement('div');
	graph.setAttribute('class','grid');
	graph.setAttribute('id','view-3-graph');
	viz.view3.appendChild(graph);
	Plotly.plot(document.getElementById('view-3-graph'),fig);

	// restore position
	window.scrollTo(0, viz.scroll_pos);
}





// view 4: 2D traj
viz.makeView2DTraj = function(view)
{
	if (viz.model.molecules.length>1)
	{
		// create 2 rows, one for text-based info and one for the graph
		var txt = document.createElement('div');
		txt.setAttribute('class','grid results-text');
		view.appendChild(txt);
		var graph = document.createElement('div');
		graph.setAttribute('class','grid');
		graph.setAttribute('id','view-4-graph');
		view.appendChild(graph);

		// selector for which molecular species
		for (var i=0;i<2;i++)
		{
			var mol_select_div = document.createElement('div');
			mol_select_div.setAttribute('class','grid');
			if (i==0) {mol_select_div.innerHTML = 'First molecular species: ';}
			else {mol_select_div.innerHTML = 'Second molecular species: ';}
			var mol_select = document.createElement('select');
			mol_select.setAttribute("id","view-4-mol-"+(i+1));
			model['molecules'].forEach(function (item,index)
			{
				var mol_opt = document.createElement('option');
				mol_select.appendChild(mol_opt);
				mol_opt.setAttribute('value',index);
				mol_opt.innerHTML=item['name'];
			} );
			mol_select.setAttribute("onchange","viz.show2DTraj()");
			mol_select_div.appendChild(mol_select);
			txt.appendChild(mol_select_div);
		}
		document.getElementById('view-4-mol-2').selectedIndex=1;

		// showing traces
		viz.view4 = view;
		viz.show2DTraj();
	}
	else
	{
		var msg = document.createElement('div');
		msg.innerHTML='Your model has less than 2 types of molecules !';
		msg.setAttribute('class','grid results-text');
		msg.setAttribute('style','font-weight:bold');
		view.appendChild(msg);
		viz.view4 = view;
	}

}

viz.show2DTraj = function()
{
	// store scroll position
	viz.scroll_pos = window.pageYOffset;

	// remove previous graph or message
	viz.view4.lastChild.remove();

	// get the molecule to display
	var imol_1 = document.getElementById('view-4-mol-1').options[document.getElementById('view-4-mol-1').selectedIndex].value;
	var imol_2 = document.getElementById('view-4-mol-2').options[document.getElementById('view-4-mol-2').selectedIndex].value;
	var mol_1 = imol_1; var mol_2 = imol_2;
	if (true)
	{
		mol_1 = '[' + mol_1 + ']';
		mol_2 = '[' + mol_2 + ']';
	}

	// if imol1 == imol2, say to change
	if (imol_1==imol_2)
	{
		var msg = document.createElement('grid');
		msg.innerHTML='Choose different species !';
		msg.setAttribute('style','font-weight:bold');
		viz.view4.appendChild(msg);		
		return;
	}

	// get the data 
	var data = [];
	var num_to_show = 1;
	for (var i=0;i<num_to_show;i++)
	{
		let trace_1 = viz.getLineageData(mol_1,i);
		let trace_2 = viz.getLineageData(mol_2,i);
		let trace = { 'x':trace_1.y , 'y':trace_2.y };
		trace['type']='scatter';
		trace['mode']='markers';
		// set the color
		trace['marker']={'size':8,'color':[]};
		let t_max = trace_1.x[trace_1.x.length-1];
		for (var j=0;j<trace_1.x.length;j++)
		{
			trace['marker']['color'].push('rgb('+Math.floor(trace_1.x[j]*255/t_max)+',0,'+Math.floor((t_max-trace_1.x[j])*255/t_max)+')');
			// trace['marker']['color'].push('rgb(255,0,0)');
		}
		data.push(trace);
	}

	// build the axis and layout object
	var axis_x = { 'showgrid':true, 'zeroline':false, 'nticks':20, 'showline':false, 'title':model.molecules[imol_1].name , 'mirror':'all'};
	var axis_y = { 'showgrid':true, 'zeroline':false, 'nticks':20, 'showline':false, 'title':model.molecules[imol_2].name , 'mirror':'all'};
	var layout = { 'showlegend':false , 'xaxis':axis_x , 'yaxis':axis_y };
	var fig = { 'data':data , 'layout':layout };
	var graph = document.createElement('div');
	graph.setAttribute('class','grid');
	graph.setAttribute('id','view-4-graph');
	viz.view4.appendChild(graph);
	Plotly.plot(document.getElementById('view-4-graph'),fig);	

	// restore position
	window.scrollTo(0, viz.scroll_pos);
}




viz.getValue = function(sim,it,ic,imol,abs_number)
{
	if (abs_number==1) {return parseFloat(sim[it]['population'][ic]['quantities'][imol]);}
	var size = sim[it]['population'][ic]['volume'];
	return parseFloat(sim[it]['population'][ic]['quantities'][imol])/parseFloat(size);
}

viz.getDivisionData = function(sim)
{
	divisions = [];
	// iterate on cells at time 0
	for (var ic0=0;ic0<1;ic0++)
	{
		var ic = ic0; 
		var tbirth = sim[0].time;
		// iterate on time points
		var it=0;
		while (it<sim.length-1)
		{ 
			var next_cell = viz.findSuccessor(sim,it,ic);
			if (next_cell.has_divided)
			{
				divisions.push({'tbirth':tbirth,'tdiv':sim[it].time-tbirth});
				tbirth = sim[it+1].time;
			}
			ic = next_cell.ic;
			it = it+1;
		}
	}
	return divisions;
}

viz.getLineageData = function(prop,il)
{
	var x = []; var y = [];
	var lineage = viz.lineages[il];
	for(var i=0;i<lineage.length-1;i++)
	{
		x.push(lineage[i][0]);
		if (prop=='V')
		{
			y.push(lineage[i][1]);
		}
		else
		{
			if(prop[0]=='[')
			{
				var iprop = parseFloat(prop.slice(1,prop.length-1));
				var amount = lineage[i][2+iprop];
				var volume = lineage[i][1];
				y.push(amount/volume);	
			}
			else
			{
				y.push(lineage[i][2+parseFloat(prop)]);
			}
		}
	}
	return { 'x':x , 'y':y};
}

viz.getPopulationData = function(prop)
{
	var x = []; var y = [];
	for(var i=0;i<viz.lineages[0].length-1;i++)
	{
		x.push(viz.lineages[0]['time']);
		if (prop=='V')
		{
			y.push( math.mean( viz.lineages[0][i].map(ic => ic.volume) ) );
		}
		else
		{
			if(prop[0]=='[')
			{
				var iprop = prop.slice(1,prop.length-1);
				y.push( math.mean( sim[i]['population'].map(ic => parseFloat(ic.quantities[iprop])/parseFloat(ic.volume)) ) );
			}
			else
			{
				y.push( math.mean( sim[i]['population'].map(ic => parseFloat(ic.quantities[prop])) ) );
			}
		}
	}
	return { 'x':x , 'y':y};
}

