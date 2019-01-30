

// create the question placeholder
var question = {};
question.div = document.createElement('div');
question.div.setAttribute('id','question-div');
document.body.appendChild(question.div);

// create the answer place holder
var answer = {};
answer.div = document.createElement('div');
answer.div.setAttribute('id','answer-div');
document.body.appendChild(answer.div);

// create the validation place holder
var validation = {};
validation.div = document.createElement('div');
validation.div.setAttribute('id','validation-div');
document.body.appendChild(validation.div);

// global variable to collect the last choice
current_placeholder = -1;
choice = -1;

// test
showText(question,'How do cells grow ?');
showRadioSelect(answer,['Exponentially','They don\'t grow']);








function createRawDisplay(type,father)
{
	var raw = document.createElement(type);
	father.appendChild(raw);
	raw.setAttribute("style","border-radius: 5px; color:black;"+
	 						 "background:rgb(245, 245, 245); padding: 8px 8px 10px 10px;"+
	 						 "text-decoration: none; margin-top:10px; margin-bottom:10px;"+
	 						 "margin-left:40px; margin-right:40px; text-align:center");
	return raw;
}

function showText(placeholder,txt)
{
	if (placeholder.hasOwnProperty('disp')) {placeholder.disp.remove();}
	var raw = createRawDisplay('div',placeholder.div);
	raw.innerHTML = txt;
	placeholder.disp = raw;
	return raw;
}

function showDropDownSelect(placeholder,choices)
{
	var raw = createRawDisplay('select',answer.div);
	var opt = document.createElement('option');
	opt.setAttribute('value',-1);
	opt.innerHTML='...';
	raw.appendChild(opt);
	for (var i=0;i<choices.length;i++)
	{
		opt = document.createElement('option');
		opt.setAttribute('value',i);
		opt.innerHTML=choices[i];
		raw.appendChild(opt);
	}
	raw.setAttribute('onchange','checkChoiceDropDown()');
	placeholder.disp = raw;
	current_placeholder = placeholder;
	return raw;
}

function showRadioSelect(placeholder,choices)
{
	var raw = createRawDisplay('form',answer.div);
	for (var i=0;i<choices.length;i++)
	{
		var label = document.createElement("label");
    	var radio = document.createElement("input");
    	radio.type = "radio";
    	radio.name = "choice";
    	radio.value = i;
    	label.appendChild(radio);
		label.appendChild(document.createTextNode(choices[i]));
    	raw.appendChild(label);
    	raw.appendChild(document.createElement('br'));
	}
	raw.setAttribute('onchange','checkChoiceRadio()');
	placeholder.disp = raw;
	current_placeholder = placeholder;
	return raw;
}

function checkChoiceDropDown()
{
	choice = current_placeholder.disp.options[current_placeholder.disp.selectedIndex].value;
	if (choice <0)
		showText(validation,'Incorrect selection.');	
	else
	{
		showText(validation,'Very good, you have chosen option = '+choice);	
	}
}

function checkChoiceRadio()
{
	choice = -1;
	buttons = document.getElementsByName("choice");
	for(var i = 0; i < buttons.length; i++)
	{
	    if(buttons[i].checked)
	    {
	        choice = buttons[i].value;
	    }
	    if (choice <0)
			showText(validation,'Incorrect selection.');	
		else
		{
			showText(validation,'Very good, you have chosen option = '+choice);	
		}
	}
}


