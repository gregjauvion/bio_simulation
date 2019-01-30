
// assume that the model, settings and data variables are defined globally

function getCellsAtTime(t)
{
	var cell_cycles = getCellCyclesAtTime(t);
	var cells = [];
	cell_cycles.forEach(function(item)
	{
		var icell = 0;
		while (getTime(item[icell])<t) {icell++;}
		cells.push(item[icell]);
	});
	return cells;
}

function giveBirthCell(cell_cycle) { return cell_cycle[0]; }
function givePreDivCell(cell_cycle) { return cell_cycle[cell_cycle.length-1]; }

function getTime(cell) {return data.data[cell][data.structure['t']]; }
function getAmount(imol,cell) { return data.data[cell][data.structure['q_'+imol]]; }
function getVolume(cell) { return data.data[cell][data.structure['V']]; }
function getConc(imol,cell) { return getAmount(imol,cell)/getVolume(imol,cell); }

function getCellCyclesAtTime(t)
{
	return data.cell_cycles.filter(function(item)
	{
		return getTime(giveBirthCell(item)) < t & getTime(givePreDivCell(item)) > t;
	});
}




console.log(getCellsAtTime(3.2).map(function(cell){return getVolume(cell);}));

console.log(data.lineages[0].map(function(cell_cycle)
	{
		return { "t":data.cell_cycles[cell_cycle].map(function(cell){return getTime(cell);}) ,
				 "V":data.cell_cycles[cell_cycle].map(function(cell){return getVolume(cell);}) };
	}));
