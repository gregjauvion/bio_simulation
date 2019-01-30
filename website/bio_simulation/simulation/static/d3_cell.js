
/*
 * Functions to display cell with d3.js.
 */

var padding_default_value = 1;

/*
 * This function is used to display a cell inside a svg. data is an object which represents the cell to visualize.
 * A cell and the molecules inside it are ellipses.
 * The keys of the object are the following :
 * - data.volume is between 0 and 1 (1 means that the cell fills completely the svg)
 * - data.padding manages the space between the molecules in the cell (0 means that the molecules are as big as possible, the bigger it is the smaller are the molecules). Default value is 1.
 * - data.ellipse : object with two keys, alpha (rx/ry which must be >1) and rotation (the angle of rotation of the ellipse in degrees between 0 and 360). By default alpha=1 and rotation=0
 * - data.class is the html class of the cell (to be able to define CSS on it)
 * - data.x and data.y translate the center of the cell in the svg (0 by default, which means the center of the svg)
 * - data.children is a list with all the molecules in the cell, each molecule is an object with the following keys :
 * -- size : size of the molecule (this is a relative size, used to have some molecules bigger than others)
 * -- class : html class
 * -- text : the text displayed in the molecule
 * -- ellipse : see data.ellipse
 */
function displayCell(svg, data) {
	/* Shuffle the children array first, to shuffle the different kinds of molecules. */
	function shuffle(a) {
	    for (let i = a.length; i; i--) {
	        let j = Math.floor(Math.random() * i);
	        [a[i - 1], a[j]] = [a[j], a[i - 1]];
	    }
	}
	shuffle(data.children);

	/* The diameter of the cell */
	var volume = data.volume ? data.volume : 1;
	var x = data.x ? data.x : 0, y = data.y ? data.y : 0;
	var width = svg.attr("width"), height = svg.attr("height");
    var diameter = Math.min(width, height) * volume;
    // We put it at the middle of the svg, and translate by x and y
    var g = svg.append("g").attr("transform", "translate(" + ((width-diameter)/2 + x) + "," + ((height-diameter)/2 + y) + ")");

	/* Create the hierarchy. */
	var hierarchy = d3.hierarchy(data)
		.sum(function(d) { return d.size; })
		.sort(function(d) { return d.sort; });

    /* Create the packing, which is then used to generate the position and radius of all points. */
    var padding = data.padding ? data.padding : padding_default_value;
	var pack = d3.pack()
	    .size([diameter, diameter])
	    .padding(function(d) { return padding * d.value; });

	/* Plot the cell and the molecules */
	var node = g.selectAll(".node")
		.data(pack(hierarchy).descendants())
		.enter().append("g")
		.attr("class", function(d) { return d.data.class; })
		.attr("transform", function(d) { return "translate(" + d.x + "," + d.y + ")"; });

	function alpha(d) { return d.data.ellipse ? (d.data.ellipse.alpha ? d.data.ellipse.alpha : 1) : 1;};
	function rotation(d) { return d.data.ellipse ? (d.data.ellipse.rotation ? d.data.ellipse.rotation : 0) : 0;};

	node.append("ellipse")
		.attr("rx", function(d) { return d.r; })
		.attr("ry", function(d) { return d.r / alpha(d); })
		.attr("transform", function(d) { return "rotate(" + rotation(d) + ")"; });

	node.filter(function(d) { return d.data.text; })
		.append("text")
		.attr("dy", "0.3em")
		.text(function(d) { return d.data.text; });
}

function clear(svg) { svg.selectAll("*").remove(); }
